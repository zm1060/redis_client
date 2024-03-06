#ifndef PUSHCLIENT_H
#define PUSHCLIENT_H

#include <cpp_redis/cpp_redis>
#include <iostream>
#include <stdexcept>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <string>

class PushClient {
public:
    PushClient(const std::string& host, int port);
    ~PushClient();
    void push(const std::string& queueName, const std::string& data);
    void pushBatch(const std::string& queueName, const std::vector<std::string>& data);
    void disconnect();
    bool connected() const;

private:
    mutable std::mutex mutex_; // Protects access to client_ and isConnected_
    cpp_redis::client client_;
    std::atomic<bool> isConnected_;
};

#endif // PUSHCLIENT_H
