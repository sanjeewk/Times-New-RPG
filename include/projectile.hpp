#pragma once
#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <vector>

#include "raylib.h"
#include "raymath.h"

//#include "mob.hpp"
//#include "player.hpp"

class Mob;

const float projectileSpeed = 5.0f;

enum class ProjectileType
{
    BULLET,
    FIREBALL,
    ARROW,
    EXPLOSION
};

struct Projectile
{
    Vector2 position;
    Vector2 direction;
    //ProjectileType type;
    float speed;
    bool active;
};

void collisions(std::vector<Projectile>& projectiles, Mob& enemy);
void update_projectile(Projectile& projectile);
void remove_projectiles(std::vector<Projectile>& projectiles);

#endif