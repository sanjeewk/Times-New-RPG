import time
import zmq
import json
import random
import os
import signal
import sys
import argparse
from typing import Dict, Any
from PIL import Image
import torch
from RL import DQNAgent

class GameAIServer:
    def __init__(self, port=5555, model_path="models/dqn_model.pth", training=True):
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.REP)
        self.socket.bind(f"tcp://*:{port}")
        print(f"Game AI server listening on port {port}...")

        # Initialize RL agent
        self.rl_agent = DQNAgent(height=64, width=64, device='cuda' if torch.cuda.is_available() else 'cpu')
        self.training = True
        self.model_path = model_path

        # Create models directory if it doesn't exist
        os.makedirs(os.path.dirname(model_path), exist_ok=True)

        # Load existing model if available
        self.rl_agent.load_model(model_path)
        print(f"RL Agent initialized. Training mode: {self.training}")
    
    def decide_action(self, state: Dict[str, Any]) -> Dict[str, Any]:
        """Determine action based on game state"""
        # x, y, health = state["x"], state["y"], state["health"]
        # print(f"Received state: x={x}, y={y}, health={health}")

        # Strategic decision making
        # if health < 25:
        #     # Retreat when health is low
        #  action = "UP" if y < 8 else "LEFT"
        #     return {"action": action, "intensity": 3}
        # elif health > 75 and random.random() < 0.7:
        #     # Attack when health is high (70% chance)
        #     return {"action": "ATTACK", "intensity": 15}
        # else:
        #     # Random movement otherwise
        action = random.choice(["UP", "DOWN", "LEFT", "RIGHT", "ATTACK"])
        return {"action": action, "intensity": 1}
    
    def run(self):
        # Create screenshots folder if not exists
        if not os.path.exists("screenshots"):
            os.makedirs("screenshots")

        # Setup signal handler for graceful shutdown
        def signal_handler(sig, frame):
            print("Received Ctrl+C, shutting down server...")
            if self.training:
                print("Saving model before shutdown...")
                self.rl_agent.save_model(self.model_path)
            self.socket.close()
            self.context.term()
            sys.exit(0)

        signal.signal(signal.SIGINT, signal_handler)
        print("Press Ctrl+C to quit")

        frame_count = 0
        while True:
            # Wait for request from C++
            message = self.socket.recv()
            print(f"Received image data of size: {len(message)} bytes")

            # Convert raw RGBA pixel data to image
            # Screen dimensions: 1100x950 pixels, RGBA format (4 bytes per pixel)
            width, height = 1100, 950
            expected_size = width * height * 4

            if len(message) == expected_size:
                # Create image from raw RGBA data
                img = Image.frombytes('RGBA', (width, height), message)
                # save image for debugging
                # filename = f"screenshots/frame_{frame_count:06d}.png"
                # img.save(filename, 'PNG')
                # print(f"Saved frame to {filename}")

                # Get action from RL agent using the image
                action_name = self.rl_agent.get_action_from_image(img, training=self.training)
                action = {"action": action_name, "intensity": 1}

            else:
                print(f"Warning: Received data size {len(message)} doesn't match expected {expected_size}")
                # Fallback to random action if image processing fails
                action = {"action": random.choice(["UP", "DOWN", "LEFT", "RIGHT", "ATTACK"]), "intensity": 1}

            frame_count += 1

            self.socket.send(json.dumps(action).encode('utf-8'))

            # Log the action
            print(f"Action sent: {action}")

            # Save model periodically during training
            if self.training and frame_count % 1000 == 0:
                self.rl_agent.save_model(self.model_path)
                print(f"Model saved at frame {frame_count}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Game AI Server with DQN')
    parser.add_argument('--port', type=int, default=5555, help='Server port (default: 5555)')
    parser.add_argument('--model-path', type=str, default='models/dqn_model.pth', help='Path to save/load model')
    parser.add_argument('--training', action='store_true', help='Enable training mode')

    args = parser.parse_args()

    server = GameAIServer(port=args.port, model_path=args.model_path, training=args.training)
    server.run()
