#pragma once
#include <vector>

#include "raylib.h"
#include "raymath.h"

#include "player.hpp"

struct Projectile
{
    Vector2 position;
    Vector2 direction;
    float speed;
    bool active;
};

void collisions(std::vector<Projectile>& projectiles, Entity& enemy);
void update_projectile(Projectile& projectile);
void test();
