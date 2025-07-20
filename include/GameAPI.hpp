#pragma once
#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "Qlearning.hpp"
#include "zmq.hpp"
#include <zmq.h> 

using json = nlohmann::json;

struct PlayerState {
    int x;
    int y;
    int health;

    json to_json() const {
        return {
            {"x", x},
            {"y", y},
            {"health", health}
        };
    }
};

struct GameAction {
    Action action;
    int intensity;  // For movement speed or attack strength
    
    static GameAction from_json(const json& j) {
        std::string actionStr = j.at("action").get<std::string>();
        Action type;
        
        if (actionStr == "UP") type = Action::MOVE_UP;
        else if (actionStr == "DOWN") type = Action::MOVE_DOWN;
        else if (actionStr == "LEFT") type = Action::MOVE_LEFT;
        else if (actionStr == "RIGHT") type = Action::MOVE_RIGHT;
        else type = Action::FIRE_PROJECTILE;

        return {type, j.at("intensity").get<int>()};
    }
};

class GameAPI {
public:
    GameAPI(const std::string& python_address = "tcp://127.0.0.1:5555");
    ~GameAPI();
    
    // Synchronous action request (blocks until response received)
    GameAction getNextAction(const PlayerState& currentState);

private:
    void* context;
    void* requester;
};