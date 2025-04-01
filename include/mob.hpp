#ifndef MOB_HPP
#define MOB_HPP

#include <string>
#include <raylib.h>

#include "display.hpp"
#include "projectile.hpp"
#include "entity.hpp"

class Mob : public Entity {
public:
    float x, y; // Position of the mob
    std::string name;
    int health;
    int attackPower;
    bool isAlive;

    // constructor
    Mob(int health, int attackPower, int startX, int startY) : health(health), attackPower(attackPower), x(startX), y(startY)
    {
        isAlive = true;
    }

    // Attack function
    Projectile attack(float target_x, float target_y);
    
    // Move function
    void random_move();
    
    // Getters for position
    int getX();
    int getY();
    
    // Reduce health
    void takeDamage(int damage);
};

#endif // MOB_HPP