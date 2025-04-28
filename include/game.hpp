#pragma once
#include <vector>
#include <array>

#include "raylib.h"
#include "raymath.h"

#include "display.hpp"
#include "audio.hpp"
#include "projectile.hpp"
#include "player.hpp"
#include "mob.hpp"
#include "qlearning.hpp"


enum class GameState {Pause, Game};
enum class TextureAsset { Tilemap, Player, Dungeon };

// function to get if coordinates are outside the bound
// enemy class

class Timer 
{
    public:
        double startTime = 0;
        double lifeTime = 0;
        bool isActive = false;

        void Start(double lifetime) {
            startTime = GetTime();
            lifeTime = lifetime;
            isActive = true;
        }

        bool IsDone() const {
            return GetTime() - startTime >= lifeTime;
        }

        double GetElapsed() const {
            return GetTime() - startTime;
        }

};

class Game 
{
    private:
        // static constexpr int MAX_TEXTURES = 3;
        // static constexpr int WORLD_WIDTH = 20;
        // static constexpr int WORLD_HEIGHT = 18;

    
        std::array<Texture2D, MAX_TEXTURES> textures;
    
    // world stored in this 2D array
        Tile world[WORLD_WIDTH][WORLD_HEIGHT];
        Tile dungeon[WORLD_WIDTH][WORLD_HEIGHT];
        Camera2D camera;
    
        //Entity player;
        Object dungeon_gate;
        Object chest;

        
        Player protagonist;
        QLearningAgent agent;
    
        Timer combatTextTimer;
        Timer playerTimer;
    
        Audio audio;
        int player_sprite_toggle = 0;
        int bound = 8;
        int wall_width = 3;
    
        std::vector<Projectile> player_projectiles;
        std::vector<Projectile> enemy_projectiles;
        std::vector<Mob> enemies;    
    
    public:
        bool training;
        int count;
        Mob enemy;
        Game();
    
        void Startup();
    
        void Update();
        void update_qlearning();
        void reset();
        void render();
    
        void Shutdown();    
    // private:
        void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y, int flip=0);
        void DrawPlayerTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y, int flip=0);
    };