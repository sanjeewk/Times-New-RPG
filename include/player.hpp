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

        // Stamina mechanic
        float stamina;
        static constexpr float STAMINA_MAX = 60.0f;
        static constexpr float STAMINA_REGEN = 0.05f;
        static constexpr float STAMINA_MOVE_COST = 2.0f;
        static constexpr float STAMINA_ATTACK_COST = 5.0f;

        // constructor
        Player(int health, int attackPower, int startX, int startY, Zone zone) : 
            Entity(health, attackPower, startX, startY), zone(zone), money(0), experience(0)
        {
            TraceLog(LOG_INFO, "Player constructor called");
            isAlive = true;
            stamina = STAMINA_MAX; // Initialize stamina to max
        }

        // Regenerate stamina (call every time step)
        void regenerate_stamina();

        // Attack function
        Projectile attack(float player_x, float player_y);
        
        // Move function
        void random_move(Tile world[20][18]);
        bool move(Action action, Tile world[20][18]);
};

#endif