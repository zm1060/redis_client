#ifndef PULLCLIENT_H
#define PULLCLIENT_H

#include <cpp_redis/cpp_redis>
#include <iostream>
#include <stdexcept>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <string>

class PullClient {
public:
    PullClient(const std::string& host, int port);
    ~PullClient();
    std::string pull(const std::string& queueName);
    void clearQueue(const std::string& queueName);
    void disconnect();
    bool isConnected() const;

private:
    mutable std::mutex mutex_; // Protects access to client_ and isConnected_
    cpp_redis::client client_;
    std::atomic<bool> isConnected_;
};

#endif // PULLCLIENT_H
