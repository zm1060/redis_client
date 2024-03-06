# Find required packages
# FindPackage(cpp_redis REQUIRED) # Uncomment if cpp_redis is installed system-wide

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/cpp_redis/includes)

add_library(redis_client
    redis_push_client.cc
    redis_pull_client.cc
    redis_push_client.h
    redis_pull_client.h
)

# Link against cpp_redis and potentially other libraries
target_link_libraries(${PROJECT_NAME} cpp_redis)
