#include "Mob.hpp"

// #include 
// Attack function

Projectile Mob::attack(float target_x, float target_y)  
{
    //TraceLog(LOG_INFO, "Mob attack: x=%f, y=%f", x, y);

    Vector2 direction = Vector2Normalize(Vector2Subtract(Vector2{ target_x + 8, target_y + 8 }, Vector2{ x,y }));

    // Create new projectile
    Projectile newProjectile = { Vector2{x,y}, direction,  projectileSpeed, true, ProjectileType::FIREBALL };

    return newProjectile;
    //return 3;
}

// Move radnomly in 1 of 4 directions
void Mob::random_move(Tile world[20][18]) 
{
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

    int wx = x / TILE_WIDTH;
    int wy = y / TILE_HEIGHT;

    Tile target_tile = world[wx][wy]; 

    // do not allow players to move to pass the boundary
    if (target_tile.type == TileType::Boundary) {
        
        //TraceLog(LOG_INFO, "move not allowed x=%f, y=%f", x, y);
        x -= mob_move_x;
        y -= mob_move_y;
    }

}

// execute action or return false if not possible
bool Mob::move(Action action, Tile world[20][18])
{
    int move_x = 0;
    int move_y = 0;
    switch (action) 
    {
        case MOVE_UP: 
            move_y = TILE_HEIGHT;
            break;
        case MOVE_DOWN: 
            move_y = -TILE_HEIGHT;
            break;
        case MOVE_LEFT: 
            move_x = -TILE_WIDTH;  
            break;
        case MOVE_RIGHT:
            move_x = TILE_WIDTH;
            break;
    }
    x += move_x;
    y += move_y;

    int wx = x / TILE_WIDTH;
    int wy = y / TILE_HEIGHT;

    Tile target_tile = world[wx][wy]; 

    // do not allow players to move to pass the boundary
    if (target_tile.type == TileType::Boundary) {
        
        // TraceLog(LOG_INFO, "move not allowed x=%f, y=%f", x, y);
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

    return true;
    //case FIRE_PROJECTILE:

}

// Getters for position
int Mob::getX()  { return x; }
int Mob::getY()  { return y; }

// Reduce health
void Mob::takeDamage(int damage) 
{
    health -= damage;
    if (health < 0) health = 0;
}
