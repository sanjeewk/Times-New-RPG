#include <vector>
#include "raylib.h"
#include "raymath.h"
#include "projectile.hpp"

constexpr int scWidth = 1100;
constexpr int scHeight = 950;

// Projectile structure


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

// void collisions() {
//     // projectile collisions
//     for (auto& projectile : projectiles) {
//         if (CheckCollisionCircles(projectile.position, projectileRadius,
//             enemy.position, enemy.radius)) {
//             projectile.active = false;
//             // Handle enemy hit
//         }
//     }
// }

//void remove_projectiles(){
//    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
//    [](const Projectile& p) { return !p.active; }), projectiles.end());
//}