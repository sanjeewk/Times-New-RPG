#pragma once
#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include <vector>

#include "raylib.h"
#include "raymath.h"
#include "entity.hpp"

//#include "mob.hpp"
//#include "player.hpp"

class Mob;

const float projectileSpeed = 6.0f;
const float projectileRadius = 2.0f;

enum class ProjectileType
{
    BULLET,
    FIREBALL,
    ARROW,
    MAGIC,
    EXPLOSION
};

struct Projectile
{
    Vector2 position;
    Vector2 direction;
    float speed;
    bool active;
    ProjectileType type;
};

bool collisions(std::vector<Projectile>& projectiles, Entity& enemy);
void update_projectile(Projectile& projectile);
void remove_projectiles(std::vector<Projectile>& projectiles);
void draw_projectiles(std::vector<Projectile>& projectiles, Texture2D* textures);

#endif
