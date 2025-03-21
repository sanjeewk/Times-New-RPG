#pragma once

enum class TileType { Dirt, Grass, Tree, Boundary };

struct Tile {
    int x;
    int y;
    TileType type;
};

