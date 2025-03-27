#pragma once
constexpr int TILE_WIDTH = 16;
constexpr int TILE_HEIGHT = 16;
constexpr int screenWidth = 1100;
constexpr int screenHeight = 950;

enum class TileType { Dirt, Grass, Tree, Boundary };

struct Tile {
    int x;
    int y;
    TileType type;
};

