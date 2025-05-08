#pragma once

#include <raylib.h>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <map>
#include <array>

// #include "load_qtable.hpp"

const int GRID_SIZE = 20;
const int CELL_SIZE = 30;
const int SCREEN_WIDTH = GRID_SIZE * CELL_SIZE;
const int SCREEN_HEIGHT = GRID_SIZE * CELL_SIZE;

struct State 
{
    int agent_x, agent_y;
    int enemy_x, enemy_y;
    int enemy_health;

    bool operator==(const State& other) const {
        return agent_x == other.agent_x && agent_y == other.agent_y &&
            enemy_x == other.enemy_x && enemy_y == other.enemy_y &&
            enemy_health == other.enemy_health;
    }

    // Add this for std::map compatibility
    bool operator<(const State& other) const {
        // Compare fields in order of importance
        if (agent_x != other.agent_x) return agent_x < other.agent_x;
        if (agent_y != other.agent_y) return agent_y < other.agent_y;
        if (enemy_x != other.enemy_x) return enemy_x < other.enemy_x;
        if (enemy_y != other.enemy_y) return enemy_y < other.enemy_y;
        return enemy_health < other.enemy_health;
    }

};

enum Action 
{
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    FIRE_PROJECTILE,
    ACTION_COUNT
};

class QLearningAgent 
{
    private:
        
        float alpha = 0.4f; // Learning rate
        float gamma = 0.9f; // Discount factor
        float epsilon = 0.3f; // Exploration rate
        std::mt19937 rng;

    public:
        std::map<State, std::array<float, 5>> q_table;
        int episode = 0;
        // QLearningAgent() : rng(std::random_device{}()) {}
        QLearningAgent();
        Action chooseAction(const State& state) 
        {
            std::uniform_real_distribution<float> dist(0.0f, 1.0f);

            // Explore (random action)
            if (dist(rng) < epsilon) 
            {
                //TraceLog(LOG_INFO, "explore");
                return static_cast<Action>(std::uniform_int_distribution<int>(0, ACTION_COUNT - 1)(rng));
            }

            // Exploit (best known action)
            auto& actions = q_table[state];
            return static_cast<Action>(std::distance(actions.begin(),
                std::max_element(actions.begin(), actions.end())));
        }

        void updateQValue(const State& state, Action action, float reward, const State& new_state) {
            float& old_value = q_table[state][action];
            float max_future_value = *std::max_element(q_table[new_state].begin(), q_table[new_state].end());
            old_value = old_value + alpha * (reward + gamma * max_future_value - old_value);
        }

};