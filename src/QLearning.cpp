#include "Qlearning.hpp"

#include <iostream>
#include <fstream>
#include <map>
#include <array>
#include <tuple>


// Save to binary file
void QLearningAgent::savetoBinary(const std::map<State, std::array<float, 5>>& q_table,
    const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file for writing");
    }

    // Write number of entries
    size_t size = q_table.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));

    // Write each entry
    for (const auto& [state, values] : q_table) {
        file.write(reinterpret_cast<const char*>(&state.agent_x), sizeof(state.agent_x));
        file.write(reinterpret_cast<const char*>(&state.agent_y), sizeof(state.agent_y));
        file.write(reinterpret_cast<const char*>(&state.enemy_x), sizeof(state.enemy_x));
        file.write(reinterpret_cast<const char*>(&state.enemy_y), sizeof(state.enemy_y));
        file.write(reinterpret_cast<const char*>(&state.enemy_health), sizeof(state.enemy_health));
        file.write(reinterpret_cast<const char*>(values.data()),
            values.size() * sizeof(float));
    }
}

// Load from binary file
std::map<State, std::array<float, 5>> QLearningAgent::loadQTable(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file for reading");
    }

    std::map<State, std::array<float, 5>> q_table;

    // Read number of entries
    size_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));

    // Read each entry
    for (size_t i = 0; i < size; ++i) {
        State state;
        std::array<float, 5> values;

        file.read(reinterpret_cast<char*>(&state.agent_x), sizeof(state.agent_x));
        file.read(reinterpret_cast<char*>(&state.agent_y), sizeof(state.agent_y));
        file.read(reinterpret_cast<char*>(&state.enemy_x), sizeof(state.enemy_x));
        file.read(reinterpret_cast<char*>(&state.enemy_y), sizeof(state.enemy_y));
        file.read(reinterpret_cast<char*>(&state.enemy_health), sizeof(state.enemy_health));
        file.read(reinterpret_cast<char*>(values.data()),
            values.size() * sizeof(float));

        q_table[state] = values;
    }

    return q_table;
}

QLearningAgent::QLearningAgent() : rng(std::random_device{}()) {
    // Initialize Q-table with zeros
    q_table = loadQTable("assets/qtable.txt");
    std::cout << "Q-learning agent initialized with empty Q-table." << std::endl;

}