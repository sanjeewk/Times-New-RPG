#pragma once
#include <vector>
#include <array>

#include "raylib.h"
#include "raymath.h"

#include "tilemap.hpp"
#include "audio.hpp"
#include "projectile.hpp"
#include "player.hpp"

constexpr int TILE_WIDTH = 16;
constexpr int TILE_HEIGHT = 16;
constexpr int screenWidth = 1100;
constexpr int screenHeight = 950;

enum class GameState {Pause, Game};
enum class TextureAsset { Tilemap };

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