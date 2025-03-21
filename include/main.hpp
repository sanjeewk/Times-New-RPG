#pragma once
#include <vector>
#include "raylib.h"
#include "raymath.h"
#include <array>
#include "tilemap.hpp"
#include "audio.hpp"
#include "projectile.hpp"


constexpr int TILE_WIDTH = 16;
constexpr int TILE_HEIGHT = 16;
constexpr int screenWidth = 1100;
constexpr int screenHeight = 950;


enum class GameState {Pause, Game};
enum class TextureAsset { Tilemap };
enum class Zone { All, World, Dungeon };


struct Entity {
    int x;
    int y;
    Zone zone;
    bool isAlive;
    bool isPassable;
    int health;
    int damage;
    int money;
    int experience;
};

class Timer {
public:
    double startTime;
    double lifeTime;
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