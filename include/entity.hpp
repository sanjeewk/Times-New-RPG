#pragma once
#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "projectile.hpp"
#include "display.hpp"
#include "Qlearning.hpp"

class Entity {
public:
    float x, y; 
    bool isAlive;
    int health;
    int attackPower;
    int state_x, state_y;

    Entity() = default; // Default constructor
   Entity(int health, int attackPower, float startX, float startY)
       : health(health), attackPower(attackPower), x(startX), y(startY), isAlive(true) {};


    //virtual Projectile attack(float target_x, float target_y) = 0;

    virtual void random_move(Tile world[20][18]) = 0;
};

#endif