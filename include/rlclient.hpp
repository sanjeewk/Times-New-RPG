#pragma once

#include <zmq.hpp>
#include <nlohmann/json.hpp>
#include <functional>
#include <thread>
#include <atomic>
#include <queue>
#include <mutex>
#include <condition_variable>

using json = nlohmann::json;

class AsyncGameClient {
public:
    AsyncGameClient(const std::string& server_addr = "tcp://127.0.0.1:5555");
    ~AsyncGameClient();

    void start();
    void stop();
    
    void sendRequest(const json& request);
    void setResponseCallback(std::function<void(const json&)> callback);
    bool request_in_flight_{false};

private:
    void workerThread();

    zmq::context_t context_;
    zmq::socket_t socket_;
    std::thread worker_thread_;
    std::atomic<bool> running_{false};
    
    std::queue<json> request_queue_;
    std::mutex queue_mutex_;
    std::condition_variable queue_cv_;
    
    std::mutex in_flight_mutex_;
    std::condition_variable response_cv_;

    
    std::function<void(const json&)> response_callback_;
};