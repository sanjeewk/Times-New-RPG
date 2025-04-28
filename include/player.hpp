#ifndef PLAYER_H
#define PLAYER_H

#include "entity.hpp"

enum class Zone { All, World, Dungeon };

struct Object 
{
    float x;
    float y;
    Zone zone;
    bool isAlive;
    bool isPassable;
    int health;
    int damage;
    int money;
    int experience;
};


class Player : public Entity
{
    public:
        Vector2 position;
        Zone zone;

        int damage;
        int money;
        int experience;

        // costructor
        Player(int health, int attackPower, int startX, int startY, Zone zone) : 
            Entity(health, attackPower, startX, startY), zone(zone), money(0), experience(0)
        {
            TraceLog(LOG_INFO, "Player constructor called");
            // Entity(health, attackPower, startX, startY)
            isAlive = true;
        }
    
        // Attack function
        Projectile attack(float player_x, float player_y);
        
        // Move function
        void random_move(Tile world[20][18]);
        bool move(Action action, Tile world[20][18]);
             
};

#endif