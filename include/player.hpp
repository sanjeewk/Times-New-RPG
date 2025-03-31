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

        // int move(int x, int y);
};

#endif