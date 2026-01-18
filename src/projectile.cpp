
#include "projectile.hpp"

#include <string>
#include <mob.hpp>

constexpr int scWidth = 1100;
constexpr int scHeight = 950;

// update projectile position based on speed and direction 
void update_projectile(Projectile& projectile) {
    if (projectile.active) {
        projectile.position = Vector2Add(projectile.position,
            Vector2Scale(projectile.direction, projectile.speed));

        // Deactivate projectiles that go off-screen
        if (projectile.position.x < 0 || projectile.position.x > scWidth ||
            projectile.position.y < 0 || projectile.position.y > scHeight) {
            projectile.active = false;
        }
    }
}

// projectile collisions
bool collisions(std::vector<Projectile>& projectiles, Entity& target) {
    bool collision = false;
    for (Projectile& projectile : projectiles) {
        //static_cast<float>(target.x), static_cast<float>(target.x)

        if (CheckCollisionCircles(projectile.position, 8.0f, Vector2{ target.x, target.y}, 8.0f)) {
            projectile.active = false;
            // Handle target hit
            TraceLog(LOG_INFO, "Collisions");
            // TraceLog(LOG_INFO, "health: %d", target.health);
            target.health -= 10;
            collision = true;

        }
    }
    return collision;
}

// remove inactive projectiles
void remove_projectiles(std::vector<Projectile>& projectiles)
{
   projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
   [](const Projectile& p) { return !p.active; }), projectiles.end());
}

// Draw projectiles on tile map based on type
void draw_projectiles(std::vector<Projectile>& projectiles)
{
    for (const auto& projectile : projectiles)
    {
        if (projectile.active)
        {
            if (projectile.type == ProjectileType::BULLET)
            {
                DrawCircleV(projectile.position, projectileRadius, BLUE);
            }
            else
            {
                DrawCircleV(projectile.position, projectileRadius, RED);
            }
        }
    }
}