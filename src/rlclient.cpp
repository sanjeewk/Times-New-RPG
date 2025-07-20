// async_client.cpp
#include "rlclient.hpp"
#include <iostream>

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

void AsyncGameClient::sendRequest(const json& request) {
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        request_queue_.push(request);
    }
    queue_cv_.notify_one();
}

void AsyncGameClient::setResponseCallback(std::function<void(const json&)> callback) {
    response_callback_ = callback;
}

void AsyncGameClient::workerThread() {
    while (running_) {
        json request;
        
        // Wait for requests or response completion
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_cv_.wait(lock, [this]() { 
                return !running_ || 
                      (!request_in_flight_ && !request_queue_.empty()); 
            });
            
            if (!running_) break;
            
            request = request_queue_.front();
            request_queue_.pop();
            
            // Mark request as in flight
            std::lock_guard<std::mutex> flight_lock(in_flight_mutex_);
            request_in_flight_ = true;
        }

        // Send request
        try {
            zmq::message_t req(request.dump());
            socket_.send(req, zmq::send_flags::none);

            // Receive response
            zmq::message_t reply;
            if (socket_.recv(reply)) {
                json response = json::parse(reply.to_string());
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