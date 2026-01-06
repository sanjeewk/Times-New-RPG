import time
import zmq
import json
import random
import os
from typing import Dict, Any

class GameAIServer:
    def __init__(self, port=5555):
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.REP)
        self.socket.bind(f"tcp://*:{port}")
        print(f"Game AI server listening on port {port}...")
    
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

        frame_count = 0
        while True:
            # Wait for request from C++
            message = self.socket.recv()
            print(f"Received image data of size: {len(message)} bytes")

            # # Save the image
            # filename = f"screenshots/frame_{frame_count:06d}.png"
            # with open(filename, 'wb') as f:
            #     f.write(message)
            # print(f"Saved frame to {filename}")
            # frame_count += 1


            action = self.decide_action({"health":20})
            self.socket.send(json.dumps(action).encode('utf-8'))

            # Log the action
            print(f"Action sent: {action}")


if __name__ == "__main__":
    server = GameAIServer()
    server.run()
