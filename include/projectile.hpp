#pragma once
#include <vector>
#include "raylib.h"
#include "raymath.h"

struct Projectile
{
    Vector2 position;
    Vector2 direction;
    float speed;
    bool active;
};

void update_projectile(Projectile& projectile);
void test();
void collisions(std::vector<Projectile>& projectiles);