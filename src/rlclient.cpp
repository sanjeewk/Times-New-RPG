// async_client.cpp
#include "rlclient.hpp"
#include <iostream>
#include <vector>

AsyncGameClient::AsyncGameClient(const std::string& server_addr)
    : context_(1), socket_(context_, ZMQ_REQ) {
    socket_.connect(server_addr);
    socket_.set(zmq::sockopt::rcvtimeo, 2500);  // 2000ms timeout
}

AsyncGameClient::~AsyncGameClient() {
    stop();
}

void AsyncGameClient::start() {
    if (!running_) {
        running_ = true;
        worker_thread_ = std::thread(&AsyncGameClient::workerThread, this);
    }
}

void AsyncGameClient::stop() {
    if (running_) {
        running_ = false;
        queue_cv_.notify_all();
        response_cv_.notify_all();
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
    }
}

void AsyncGameClient::sendRequest(const std::vector<char>& data) {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        request_queue_.push(data);
    }
    queue_cv_.notify_one();
}

void AsyncGameClient::setResponseCallback(std::function<void(const json&)> callback) {
    response_callback_ = callback;
}

std::string AsyncGameClient::getNextResponse() {
    std::lock_guard<std::mutex> lock(response_mutex_);
    if (response_queue_.empty()) {
        return ""; // Return empty string if no response
    }
    std::string action = response_queue_.front();
    response_queue_.pop();
    return action;
}

bool AsyncGameClient::hasResponse() {
    std::lock_guard<std::mutex> lock(response_mutex_);
    return !response_queue_.empty();
}

void AsyncGameClient::workerThread() {
    while (running_) {
        std::vector<char> request_data;

        // Wait for requests or response completion
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_cv_.wait(lock, [this]() {
                return !running_ ||
                      (!request_in_flight_ && !request_queue_.empty());
            });

            if (!running_) break;

            request_data = request_queue_.front();
            request_queue_.pop();

            // Mark request as in flight
            std::lock_guard<std::mutex> flight_lock(in_flight_mutex_);
            request_in_flight_ = true;
        }

        // Send request
        try {
            zmq::message_t req(request_data.data(), request_data.size());
            socket_.send(req, zmq::send_flags::none);

            // Receive response
            zmq::message_t reply;
            if (socket_.recv(reply)) {
                json response = json::parse(reply.to_string());
                std::string action = response["action"];
                {
                    std::lock_guard<std::mutex> lock(response_mutex_);
                    response_queue_.push(action);
                }
                if (response_callback_) {
                    response_callback_(response);
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Network error: " << e.what() << std::endl;
        }

        // Clear in-flight status
        {
            std::lock_guard<std::mutex> flight_lock(in_flight_mutex_);
            request_in_flight_ = false;
        }
        response_cv_.notify_one();
    }
}
