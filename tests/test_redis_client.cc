#include <gtest/gtest.h>
#include "redis_push_client.h"
#include "redis_pull_client.h"
#include <chrono>
#include <string>
#include <memory>

class RedisClientTest : public ::testing::Test {
protected:
    static std::unique_ptr<PushClient> pushClient;
    static std::unique_ptr<PullClient> pullClient;
    static std::string host;
    static int port;
    static std::string queueName;

    static void SetUpTestSuite() {
        host = "127.0.0.1";
        port = 6379;
        queueName = "testQueue";
        pushClient = std::make_unique<PushClient>(host, port);
        pullClient = std::make_unique<PullClient>(host, port);

        while (!pushClient->connected() || !pullClient->isConnected()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    static void TearDownTestSuite() {
        if (pullClient->isConnected()) {
            pullClient->clearQueue(queueName);
        }
        pushClient->disconnect();
        pullClient->disconnect();
        pushClient.reset();
        pullClient.reset();
    }
};

std::unique_ptr<PushClient> RedisClientTest::pushClient = nullptr;
std::unique_ptr<PullClient> RedisClientTest::pullClient = nullptr;
std::string RedisClientTest::host;
int RedisClientTest::port;
std::string RedisClientTest::queueName;

TEST_F(RedisClientTest, PushAndPullData) {
    std::string testData = "Hello, World!";
    EXPECT_NO_THROW(pushClient->push(queueName, testData));

    std::string result;
    EXPECT_NO_THROW(result = pullClient->pull(queueName));
    EXPECT_EQ(testData, result);
}

TEST_F(RedisClientTest, ClearQueue) {
    std::string testData = "Data for clearing";
    EXPECT_NO_THROW(pushClient->push(queueName, testData));
    EXPECT_NO_THROW(pullClient->clearQueue(queueName));

    std::string result = pullClient->pull(queueName);
    EXPECT_TRUE(result.empty());
}

TEST_F(RedisClientTest, BatchPushAndPullData) {
    std::vector<std::string> testData = {"Data 1", "Data 2", "Data 3"};
    EXPECT_NO_THROW(pushClient->pushBatch(queueName, testData));

    for (const auto& data : testData) {
        std::string result;
        EXPECT_NO_THROW(result = pullClient->pull(queueName));
        EXPECT_EQ(data, result);
    }
}

TEST_F(RedisClientTest, PerformanceTest) {
    const int numOperations = 1000;
    std::string testData = "Performance Test Data";

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numOperations; ++i) {
        pushClient->push(queueName, testData);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto pushDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numOperations; ++i) {
        pullClient->pull(queueName);
    }
    end = std::chrono::high_resolution_clock::now();
    auto pullDuration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Push operations took " << pushDuration << " milliseconds." << std::endl;
    std::cout << "Pull operations took " << pullDuration << " milliseconds." << std::endl;
}
