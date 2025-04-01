#ifndef PLAYER_H
#define PLAYER_H

enum class Zone { All, World, Dungeon };

struct Entity {
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


class Player {
    public:
        int health;
        Vector2 position;
        int x;
        int y;
        Zone zone;

        bool isAlive;
        bool isPassable;

        int damage;
        int money;
        int experience;
        int attackPower;


        // costructor
        Player(int health, int attackPower, int startX, int startY, Zone zone) : health(health), attackPower(attackPower), x(startX), y(startY), zone(zone)
        {
            isAlive = true;
        }
    
        // Attack function
        Projectile attack(float player_x, float player_y);
        
        // Move function
        void random_move();
        
        // Getters for position
        int getX();
        int getY();
        
        // Reduce health
        void takeDamage(int damage);
            
};

#endif