#pragma once
#include <vector>
#include <array>

#include "raylib.h"
#include "raymath.h"

#include "display.hpp"
#include "audio.hpp"
#include "projectile.hpp"
#include "player.hpp"


enum class GameState {Pause, Game};
enum class TextureAsset { Tilemap };

// function to get if coordinates are outside the bound
// enemy class

class Timer {
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


class Game {
    private:
        static constexpr int MAX_TEXTURES = 1;
        static constexpr int WORLD_WIDTH = 25;
        static constexpr int WORLD_HEIGHT = 20;
    
        std::array<Texture2D, MAX_TEXTURES> textures;
    
    // world stored in this 2D arry
        Tile world[WORLD_WIDTH][WORLD_HEIGHT];
        Tile dungeon[WORLD_WIDTH][WORLD_HEIGHT];
        Camera2D camera;
    
        Entity player;
        Entity dungeon_gate;
        Entity orc;
        Entity chest;
    
        Timer combatTextTimer;
        Timer playerTimer;
    
        Audio audio;
        int player_sprite_toggle = 0;
        int bound = 8;
        int wall_width = 3;
    
        std::vector<Projectile> projectiles;
        const float projectileSpeed = 5.0f;
        const float projectileRadius = 2.0f;
    
    
    public:
        Game();
    
        void Startup();
    
        void Update();
    
        void Render();
    
        void Shutdown();    
    // private:
        void DrawTile(int pos_x, int pos_y, int texture_index_x, int texture_index_y, int flip=0);
    };