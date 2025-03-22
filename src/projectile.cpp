#include <vector>
#include "raylib.h"
#include "raymath.h"
#include "projectile.hpp"
#include <string>

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

// projectile collisions
void collisions(std::vector<Projectile>& projectiles) {

    for (Projectile& projectile : projectiles) {

        if (CheckCollisionCircles(projectile.position, 8.0f,
            Vector2{80.0, 80.0}, 8.0f)) {
            projectile.active = false;
            // Handle enemy hit
            TraceLog(LOG_INFO, "Collisions");
        }
        // if (!projectile.active) {
        //     TraceLog(LOG_INFO, "NOT ACTIVE");
        // }
    }


}

void test(){
    TraceLog(LOG_INFO, "Test");
}
//void remove_projectiles(){
//    projectiles.erase(std::remove_if(projectiles.begin(), projectiles.end(),
//    [](const Projectile& p) { return !p.active; }), projectiles.end());
//}