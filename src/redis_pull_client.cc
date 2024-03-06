#include "redis_pull_client.h"

PullClient::PullClient(const std::string& host, int port) : isConnected_(false) {
    std::condition_variable cv;
    bool ready = false;

    client_.connect(host, port, [&ready, &cv, this](const std::string& host, std::size_t port, cpp_redis::connect_state status) {
        if (status == cpp_redis::connect_state::ok) {
            std::cout << "Successfully connected to Pull Client at: " << host << ":" << port << std::endl;
            isConnected_.store(true);
        } else {
            std::cerr << "Failed to connect to Pull Client at: " << host << ":" << port << std::endl;
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

PullClient::~PullClient() {
    disconnect();
}

std::string PullClient::pull(const std::string& queueName) {
    std::cout << "Trying to pull data from queue: " << queueName << std::endl;
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isConnected_.load()) {
        throw std::runtime_error("Not connected to Redis server.");
    }

    if (queueName.empty()) {
        throw std::invalid_argument("Invalid queue name provided.");
    }

    // 使用 std::future<reply> rpop(const std::string& key); 版本
    auto future_reply = client_.lpop(queueName);
    client_.sync_commit();  // 确保命令被发送到服务器
    auto reply = future_reply.get(); // 获取回复

    if (reply.is_error()) {
        std::cerr << "Error pulling data: " << reply.error() << std::endl;
        return "";
    }

    if (!reply.is_null()) { // 检查回复是否非空
        std::cout << "Data pulled successfully." << std::endl;
        return reply.as_string();
    } else {
        std::cout << "No data to pull from queue." << std::endl;
    }

    return "";
}


void PullClient::clearQueue(const std::string& queueName) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!isConnected_.load()) {
        throw std::runtime_error("Attempted to clear queue while not connected to Redis.");
    }

    client_.del({queueName}, [](const cpp_redis::reply& reply) {
        if (!reply.is_integer()) {
            std::cerr << "Error clearing queue: " << reply.error() << std::endl;
        } else {
            std::cout << "Queue cleared successfully. Items removed: " << reply.as_integer() << std::endl;
        }
    });

    client_.sync_commit();
}

void PullClient::disconnect() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (isConnected_) {
        client_.disconnect();
        isConnected_.store(false);
    }
}

bool PullClient::isConnected() const {
    return isConnected_.load();
}