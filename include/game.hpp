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
#include "GameAPI.hpp"
#include "rlclient.hpp"
#include <thread>
#include <atomic>
#include <queue>


enum class GameState {Menu, Game, Pause};
enum class TextureAsset { Tilemap, Player, Dungeon, Hearts };

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
        static constexpr int MAX_TEXTURES = 4;
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

        QLearningAgent agent;
        GameAPI api;

    
        Timer combatTextTimer;
        Timer playerTimer;
        GameState game_state;
    
        Audio audio;
        int player_sprite_toggle = 0;
        int bound = 8;
        int wall_width = 3;
    
        std::vector<Projectile> player_projectiles;
        std::vector<Projectile> enemy_projectiles;
        std::vector<Mob> enemies;

        // Image capture thread
        std::thread image_capture_thread;
        std::atomic<bool> image_capture_running;
        std::queue<std::vector<unsigned char>> image_queue;

        void imageCaptureThread();

    public:
        AsyncGameClient client;
        bool training;
        int count;
        Mob enemy;
        Player protagonist;
        float current_reward;
        bool episode_done;
        int frame_count;
        Game();
    
        void Startup();
        void menu();
        void Update();
        void update_qlearning();
        void reset();
        void render();
        void check_state();
        // void SaveFrameAsImage(const char* filename);
    
        void Shutdown();    
    // private:
        void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y, int flip=0);
        void DrawPlayerTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y, int flip=0);
        void DrawUITile(int pos_x, int pos_y, int texture_index_x, int texture_index_y, int flip=0, float scale=0.0);

    };
