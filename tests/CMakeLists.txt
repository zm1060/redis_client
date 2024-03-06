# Enable testing with CTest
enable_testing()

# Find GoogleTest package
find_package(GTest REQUIRED)

# Add your test executable
add_executable(test_redis_client test_redis_client.cc)

# If RTTI is disabled project-wide and needs to be enabled
target_compile_options(test_redis_client PRIVATE "-frtti")

# Or if RTTI needs to be disabled for consistency with Google Test
target_compile_options(test_redis_client PRIVATE "-fno-rtti")

# Include directories
target_include_directories(test_redis_client PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    ${GTEST_INCLUDE_DIRS} # This might be redundant with target_link_libraries if using GTest::gtest
)

# Link with GoogleTest and the libraries to test
# Note: The order might matter in some cases, ensure GoogleTest libraries are specified last
target_link_libraries(test_redis_client 
    cpp_redis
    redis_client
    GTest::gtest 
    GTest::gtest_main
)

# Add the tests to be run
add_test(NAME test_redis_client COMMAND test_redis_client)
