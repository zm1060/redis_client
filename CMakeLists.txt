cmake_minimum_required(VERSION 3.10)
project(redis_client VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include subdirectories
add_subdirectory(cpp_redis)
add_subdirectory(src)
add_subdirectory(tests)
