# Times new RPG
![screen](readme/time-new.jpg)

Times new RPG is a retro top down RPG with a plot centered around time loop

Exploration of C++ and game design! I built this game as a way to explore game dev and explore the use of AI for improved gameplay. 

The goal of this project is to create a package that allows a plug and play method to train game bosses using Reinforcement Learning methods

Built with C++ and [raylib](https://www.raylib.com/). 

## Features
- Top-down tile-based world and dungeon
- Player and enemy movement
- Combat with projectiles
- Collectible items and chests
- Multiple sound effects and background music
- Camera following the player
- Simple AI for enemies

## Screenshots
![Menu Screen](assets/menu_screen.jpg)

## Getting Started

### Prerequisites
- C++17 compatible compiler (e.g., MinGW-w64, MSVC)
- [raylib](https://www.raylib.com/) (included as submodule/external)
- CMake or Make (for building)

### Building
#### Windows (Visual Studio)
1. Open `rpgtest.sln` in Visual Studio 2022
2. Build the solution

#### Windows (MinGW-w64)
1. Run `build-MinGW-W64.bat`

#### Cross-platform (Makefile)
1. Run `make` in the project root

### Running
- The executable will be in `bin/Debug/Times-New-RPG.exe` or `bin/Debug/rpgtest.exe`
- Run the game from the terminal or by double-clicking the executable

### Controls
- **WASD**: Move player
- **Mouse Left Click**: Shoot projectile
- **E**: Enter/exit dungeon
- **G**: Collect chest

## Project Structure
- `src/` - C++ source files
- `include/` - Header files
- `assets/` - Sprites, music, sound effects
- `bin/` - Compiled binaries
- `build/` - Build scripts and files
- `resources/` - Duplicate assets for packaging

## Credits
- [raylib](https://www.raylib.com/) for graphics/audio
- OpenGameArt for some sprites and sounds
- [Sprites](https://totuslotus.itch.io/characterpack)
- [Background](https://totuslotus.itch.io/characterpack)

## License
This project is for educational and personal use. See individual asset files for their respective licenses.


## Game Structure

The game is built around a top-down tile-based map. The player, enemies, and interactive objects  are all represented as entities on this grid. Each iteration the follwoing get updated:

- Player and enemy movement and actions
- Camera position to follow the player
- Projectile updates and collision detection
- Sound and music playback based on game events
- Tile map rendering for the current zone

## Reinforcement Learning 

Basic explanation of the RL used:

1. **State Representation**: The boss observes the game state, including player position, health, and environmental factors.
2. **Action Space**: The boss can choose from a set of actions (move, attack, defend, use abilities, etc.).
3. **Reward Function**: The boss receives rewards for effective actions that hurt the player and penalties for poor performance.
4. **Training Loop**: Using RL algorithms, the boss is trained over many simulated episodes to maximize its cumulative reward.

The goal is for the AI to learn over time from the player's movement patterns.