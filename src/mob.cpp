#include "Mob.hpp"

// #include 
// Attack function

Projectile Mob::attack(float target_x, float target_y)  
{
    //TraceLog(LOG_INFO, "Mob attack: x=%f, y=%f", x, y);

    Vector2 direction = Vector2Normalize(Vector2Subtract(Vector2{ target_x + 8, target_y + 8 }, Vector2{ x,y }));

    // Create new projectile
    Projectile newProjectile = { Vector2{x,y}, direction,  projectileSpeed, true };

    return newProjectile;
    //return 3;
}

// Move function
void Mob::random_move() {
    int movement = GetRandomValue(1, 4);
    int mob_move_x = 0;            
    int mob_move_y = 0;
    
    switch (movement) {
        case 1: mob_move_x = -TILE_WIDTH; break;
        case 2: mob_move_x = TILE_WIDTH; break;
        case 3: mob_move_y = -TILE_HEIGHT; break;
        case 4: mob_move_y = TILE_HEIGHT; break;
    }
    x += mob_move_x;
    y += mob_move_y;

}

// Getters for position
int Mob::getX()  { return x; }
int Mob::getY()  { return y; }

// Reduce health
void Mob::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}