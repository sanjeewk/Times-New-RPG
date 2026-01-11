import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F
import numpy as np
from PIL import Image
import torchvision.transforms as T
import random
from collections import namedtuple, deque
import os
import time

# Define the action space
ACTIONS = ['UP', 'DOWN', 'LEFT', 'RIGHT', 'ATTACK']
NUM_ACTIONS = len(ACTIONS)

# Define experience tuple for replay buffer
Transition = namedtuple('Transition', ('state', 'action', 'next_state', 'reward'))

class ReplayBuffer:
    """Experience replay buffer for DQN"""

    def __init__(self, capacity=10000):
        self.capacity = capacity
        self.memory = deque([], maxlen=capacity)

    def push(self, *args):
        """Save a transition"""
        self.memory.append(Transition(*args))

    def sample(self, batch_size):
        """Sample a batch of transitions"""
        return random.sample(self.memory, batch_size)

    def __len__(self):
        return len(self.memory)

class DQN(nn.Module):
    """Deep Q-Network for image-based reinforcement learning"""

    def __init__(self, h, w, outputs):
        super(DQN, self).__init__()
        self.conv1 = nn.Conv2d(1, 32, kernel_size=8, stride=4)  # Changed from 3 to 1 channel (grayscale)
        self.bn1 = nn.BatchNorm2d(32)
        self.conv2 = nn.Conv2d(32, 64, kernel_size=4, stride=2)
        self.bn2 = nn.BatchNorm2d(64)
        self.conv3 = nn.Conv2d(64, 64, kernel_size=3, stride=1)
        self.bn3 = nn.BatchNorm2d(64)

        # With adaptive_avg_pool2d(1,1), final spatial dims are always 1x1
        # So linear_input_size = num_channels_after_conv3 = 64
        linear_input_size = 64

        self.head = nn.Linear(linear_input_size, 512)
        self.output = nn.Linear(512, outputs)

    def forward(self, x):
        x = x.float() / 255.0
        x = F.relu(self.bn1(self.conv1(x)))
        x = F.relu(self.bn2(self.conv2(x)))
        x = F.relu(self.bn3(self.conv3(x)))
        x = F.adaptive_avg_pool2d(x, (1, 1))
        x = x.view(x.size(0), -1)
        x = F.relu(self.head(x))
        return self.output(x)

class DQNAgent:
    """DQN Agent for game control"""

    def __init__(self, height=64, width=64, device='cpu'):
        self.device = torch.device(device)
        self.height = height
        self.width = width

        # Initialize networks
        self.policy_net = DQN(height, width, NUM_ACTIONS).to(self.device)
        self.target_net = DQN(height, width, NUM_ACTIONS).to(self.device)
        self.target_net.load_state_dict(self.policy_net.state_dict())
        self.target_net.eval()

        # Training parameters
        self.optimizer = optim.Adam(self.policy_net.parameters(), lr=1e-4)
        self.memory = ReplayBuffer(100000)

        # Hyperparameters
        self.batch_size = 32
        self.gamma = 0.99
        self.eps_start = 1.0
        self.eps_end = 0.1
        self.eps_decay = 100000
        self.target_update = 1000

        # Training state
        self.steps_done = 0
        self.episode_reward = 0
        self.last_state = None

        # Image preprocessing
        self.transform = T.Compose([
            T.Resize((height, width)),
            T.Grayscale(num_output_channels=1),  # Convert to grayscale
            T.ToTensor()
        ])

    def preprocess_image(self, image):
        """Preprocess PIL image for neural network input"""
        # Convert RGBA to RGB if needed
        if image.mode == 'RGBA':
            # Create a white background
            background = Image.new('RGB', image.size, (255, 255, 255))
            background.paste(image, mask=image.split()[-1])  # Use alpha channel as mask
            image = background

        # Convert to RGB if not already
        if image.mode != 'RGB':
            image = image.convert('RGB')

        return self.transform(image).unsqueeze(0).to(self.device)

    def select_action(self, state, training=True):
        """Select action using epsilon-greedy policy"""
        sample = random.random()

        if training:
            # print("training")
            eps_threshold = self.eps_end + (self.eps_start - self.eps_end) * \
                           np.exp(-1. * self.steps_done / self.eps_decay)
        else:
            eps_threshold = 0.05  # Small epsilon for inference

        self.steps_done += 1

        if sample > eps_threshold:
            with torch.no_grad():
                # Use policy network for action selection
                print("inference")
                q_values = self.policy_net(state)
                action_idx = q_values.max(1)[1].item()
        else:
            action_idx = random.randrange(NUM_ACTIONS)

        return action_idx, ACTIONS[action_idx]

    def optimize_model(self):
        """Perform one step of optimization"""
        if len(self.memory) < self.batch_size:
            return

        # Sample batch from replay buffer
        transitions = self.memory.sample(self.batch_size)
        batch = Transition(*zip(*transitions))

        # Create batch tensors
        state_batch = torch.cat(batch.state)
        action_batch = torch.tensor(batch.action, device=self.device).unsqueeze(1)
        reward_batch = torch.tensor(batch.reward, device=self.device, dtype=torch.float32)
        next_state_batch = torch.cat([s for s in batch.next_state if s is not None])

        # Compute Q(s_t, a)
        state_action_values = self.policy_net(state_batch).gather(1, action_batch)

        # Compute V(s_{t+1}) for all next states
        next_state_values = torch.zeros(self.batch_size, device=self.device)
        if len(next_state_batch) > 0:
            with torch.no_grad():
                next_state_values = self.target_net(next_state_batch).max(1)[0]

        # Compute expected Q values
        expected_state_action_values = (next_state_values * self.gamma) + reward_batch

        # Compute Huber loss
        criterion = nn.SmoothL1Loss()
        loss = criterion(state_action_values.squeeze(), expected_state_action_values)

        # Optimize the model
        self.optimizer.zero_grad()
        loss.backward()
        torch.nn.utils.clip_grad_value_(self.policy_net.parameters(), 100)
        self.optimizer.step()

        return loss.item()

    def update_target_network(self):
        """Update target network with policy network weights"""
        self.target_net.load_state_dict(self.policy_net.state_dict())

    def save_model(self, path):
        """Save model weights"""
        torch.save({
            'policy_net': self.policy_net.state_dict(),
            'target_net': self.target_net.state_dict(),
            'optimizer': self.optimizer.state_dict(),
            'steps_done': self.steps_done,
        }, path)

    def load_model(self, path):
        """Load model weights"""
        if os.path.isfile(path):
            checkpoint = torch.load(path, map_location=self.device)
            self.policy_net.load_state_dict(checkpoint['policy_net'])
            self.target_net.load_state_dict(checkpoint['target_net'])
            self.optimizer.load_state_dict(checkpoint['optimizer'])
            self.steps_done = checkpoint['steps_done']
            print(f"Loaded model from {path}")
        else:
            print(f"No model found at {path}")

    def get_action_from_image(self, image, training=False):
        """Get action from image input (inference mode)"""
        state = self.preprocess_image(image)
        action_idx, action_name = self.select_action(state, training=training)
        return action_name

    def train_step(self, image, reward, done):
        """Training step with image and reward"""
        current_state = self.preprocess_image(image)

        if self.last_state is not None:
            # Store transition in replay buffer
            self.memory.push(self.last_state, self.last_action_idx,
                           current_state if not done else None, reward)

            # Perform optimization
            loss = self.optimize_model()
        else:
            loss = None

        # Select action for next step
        self.last_action_idx, action = self.select_action(current_state, training=True)
        self.last_state = current_state

        # Update target network periodically
        if self.steps_done % self.target_update == 0:
            self.update_target_network()

        return action, loss

    def reset_episode(self):
        """Reset episode state"""
        self.last_state = None
        self.last_action_idx = None
