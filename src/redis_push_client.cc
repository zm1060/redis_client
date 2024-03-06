#include "redis_push_client.h"

PushClient::PushClient(const std::string& host, int port) : isConnected_(false) {
    std::condition_variable cv;
    bool ready = false;

    client_.connect(host, port, [&ready, &cv, this](const std::string& host, std::size_t port, cpp_redis::connect_state status) {
        if (status == cpp_redis::connect_state::ok) {
            std::cout << "Successfully connected to Redis Push Client at: " << host << ":" << port << std::endl;
            isConnected_.store(true);
        } else {
            std::cerr << "Failed to connect to Redis Push Client at: " << host << ":" << port << std::endl;
            isConnected_.store(false);
        }
        {
            std::lock_guard<std::mutex> lk(mutex_);
            ready = true;
        }
        cv.notify_one();
    });

    // Wait for the connection to be established
    std::unique_lock<std::mutex> lk(mutex_);
    cv.wait(lk, [&ready] { return ready; });

    if (!isConnected_.load()) {
        throw std::runtime_error("Failed to connect to Redis server.");
    }
}

PushClient::~PushClient() {
    disconnect();
}

void PushClient::push(const std::string& queueName, const std::string& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isConnected_.load()) {
        throw std::runtime_error("Not connected to Redis server.");
    }

    if (queueName.empty()) {
        throw std::invalid_argument("Invalid queue name provided.");
    }

    client_.rpush(queueName, {data}, [this](const cpp_redis::reply& reply) {
        if (reply.is_error()) {
            std::cerr << "Error pushing data: " << reply.error() << std::endl;
            // Throw or handle error appropriately
        } else {
            std::cout << "Data pushed successfully. " << std::endl;
        }
    });

    client_.sync_commit(); // Consider client_.commit() for asynchronous commit if appropriate
}

void PushClient::pushBatch(const std::string& queueName, const std::vector<std::string>& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isConnected_.load()) {
        throw std::runtime_error("Not connected to Redis server.");
    }
    client_.rpush(queueName, data, nullptr); // nullptr as callback for batch operation


    client_.sync_commit(); // Commit all the batched commands at once
}


void PushClient::disconnect() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (isConnected_) {
        client_.disconnect();
        isConnected_.store(false);
    }
}

bool PushClient::connected() const {
    return isConnected_.load();
}