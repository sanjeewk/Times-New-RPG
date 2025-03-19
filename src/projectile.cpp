#include <vector>
#include "raylib.h"

// Projectile structure
struct Projectile {
    Vector2 position;
    Vector2 direction;
    float speed;
    bool active;
};