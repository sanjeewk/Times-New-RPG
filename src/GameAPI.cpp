#include "GameAPI.hpp"
#include <zmq.hpp>
#include <iostream>

GameAPI::GameAPI(const std::string& python_address) {
    context = zmq_ctx_new();
    requester = zmq_socket(context, ZMQ_REQ);
    zmq_connect(requester, python_address.c_str());
}

// GameAPI::~GameAPI() {
//     zmq_close(requester);
//     zmq_ctx_destroy(context);
// }

void GameAPI::getNextAction(const PlayerState& currentState) {
    // Serialize state to JSON
    json state_json = currentState.to_json();
    std::string request = state_json.dump();
    
    // Send to Python
    zmq_send(requester, request.c_str(), request.size(), 0);
    
    // Wait for and receive response
    char buffer[1024];
    int size = zmq_recv(requester, buffer, 1024, 0);
    std::string response_str(buffer, size);
    
    // Parse response
    json response_json = json::parse(response_str);
    //json::print(response_json);
}