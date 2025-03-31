#ifndef MOB_HPP
#define MOB_HPP

#include <string>
#include <raylib.h>

#include "display.hpp"
#include "projectile.hpp"

class Mob{
public:
    std::string name;
    int health;
    int attackPower;
    float x, y; // Position of the mob

    bool isAlive;
    // costructor
    Mob(int health, int attackPower, int startX, int startY) : health(health), attackPower(attackPower), x(startX), y(startY)
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

#endif // MOB_HPP