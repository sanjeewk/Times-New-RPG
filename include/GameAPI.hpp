#pragma once
#pragma once
#include <string>
#include <nlohmann/json.hpp>
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

class GameAPI {
public:
    GameAPI(const std::string& python_address = "tcp://127.0.0.1:5555");
    //~GameAPI();
    
    // Synchronous action request (blocks until response received)
    void getNextAction(const PlayerState& currentState);

private:
    void* context;
    void* requester;
};