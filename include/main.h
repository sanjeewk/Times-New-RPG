#pragma once
#pragma once

#include "raylib.h"
#include "raymath.h"
#include <array>

constexpr int TILE_WIDTH = 16;
constexpr int TILE_HEIGHT = 16;
constexpr int screenWidth = 1100;
constexpr int screenHeight = 950;

enum class TextureAsset { Tilemap };
enum class SoundAsset { FootGrass, FootStone, Attack, Death, Coins };
enum class MusicAsset { LightAmbience, DarkAmbience };
enum class Zone { All, World, Dungeon };
enum class TileType { Dirt, Grass, Tree, Boundary };

struct Tile {
    int x;
    int y;
    TileType type;
};

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