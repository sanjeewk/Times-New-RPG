#include "raylib.h"
#include "game.hpp"
#include "player.hpp"


// Call this every time step to regenerate stamina
void Player::regenerate_stamina() {
    this->stamina += STAMINA_REGEN; // Use member variable
    if (this->stamina > STAMINA_MAX) this->stamina = STAMINA_MAX;
}

Projectile Player::attack(float target_x, float target_y)  
{
    if (stamina < STAMINA_ATTACK_COST) {
        TraceLog(LOG_INFO, "Not enough stamina to attack!");
        return Projectile{}; // Return an invalid projectile
    }
    this->stamina -= STAMINA_ATTACK_COST; // Use member variable
    if (stamina < 0) stamina = 0;

    Vector2 direction = Vector2Normalize(Vector2Subtract(Vector2{ target_x + 8, target_y + 8 }, Vector2{ x + 8, y + 8 }));

    // Create new projectile
    Projectile newProjectile = { Vector2{x,y}, direction,  projectileSpeed, true, ProjectileType::MAGIC};

    return newProjectile;
}

// Move function
void Player::random_move(Tile world[20][18]) 
{
    int movement = GetRandomValue(1, 4);
    int move_x = 0;            
    int move_y = 0;
    
    switch (movement) {
        case 1: move_x = -TILE_WIDTH; break;
        case 2: move_x = TILE_WIDTH; break;
        case 3: move_y = -TILE_HEIGHT; break;
        case 4: move_y = TILE_HEIGHT; break;
    }
    x += move_x;
    y += move_y;

    int wx = x / TILE_WIDTH;
    int wy = y / TILE_HEIGHT;

    Tile target_tile = world[wx][wy]; 

    // do not allow players to move to pass the boundary
    if (target_tile.type == TileType::Boundary) {
        
        TraceLog(LOG_INFO, "move not allowed x=%f, y=%f", x, y);
        x -= move_x;
        y -= move_y;
    }
}

// execute action or return false if not possible
bool Player::move(Action action, Tile world[20][18])
{
    //TraceLog(LOG_INFO, "Player::move called with action: %d", static_cast<int>(action));

    if (stamina < STAMINA_MOVE_COST) 
    {
        TraceLog(LOG_INFO, "Not enough stamina to move!");
        return false;
    }

    int move_x = 0;
    int move_y = 0;
    switch (action) 
    {
        case Action::MOVE_UP: 
            move_y = -TILE_HEIGHT;
            // TraceLog(LOG_INFO, "Moving up");
            break;
        case Action::MOVE_DOWN: 
            move_y = +TILE_HEIGHT;
            // TraceLog(LOG_INFO, "Moving down");
            break;
        case Action::MOVE_LEFT: 
            move_x = -TILE_WIDTH;  
            // TraceLog(LOG_INFO, "Moving left");
            break;
        case Action::MOVE_RIGHT:
            move_x = TILE_WIDTH;
            // TraceLog(LOG_INFO, "Moving right");
            break;
    }
    x += move_x;
    y += move_y;


    int wx = x / TILE_WIDTH;
    int wy = y / TILE_HEIGHT;

    Tile target_tile = world[wx][wy]; 

    //do not allow players to move to pass the boundary
    if (target_tile.type == TileType::Boundary) 
    {
        TraceLog(LOG_INFO, "move not allowed x=%f, y=%f", x, y);
        if (move_x != 0)
        {
            x -= move_x*2;
        }
        else
        {
            y -= move_y*2;
        }
        return false;
    }
    else
    {
        this->stamina -= STAMINA_MOVE_COST; // Use member variable
        if (stamina < 0) stamina = 0;
    }
    return true;
}
