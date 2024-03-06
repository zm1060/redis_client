# redis_client

## How to build?

### Step 1:
Run a Redis Server:
https://redis.io/docs/install/install-redis/install-redis-on-linux/
<br>

On Ubuntu
```shell
sudo apt install lsb-release curl gpg
curl -fsSL https://packages.redis.io/gpg | sudo gpg --dearmor -o /usr/share/keyrings/redis-archive-keyring.gpg

echo "deb [signed-by=/usr/share/keyrings/redis-archive-keyring.gpg] https://packages.redis.io/deb $(lsb_release -cs) main" | sudo tee /etc/apt/sources.list.d/redis.list

sudo apt-get update
sudo apt-get install redis
```
After installation, run it!
```code
sudo systemctl start redis
```
<br>

```code
redis-cli
```
<br>

```code
127.0.0.1:6379>ping
```
it will output like this:
```code
127.0.0.1:6379> ping
PONG
```
### Step 2:
```shell
git clone https://github.com/zm1060/redis_client
```


```shell
cd redis_client && git submodule update --init --recursive
```

```shell
mkdir build && cd build
cmake ..
```

It may have error:
```code
-- Configuring done
CMake Error: install(EXPORT "cpp_redis" ...) includes target "cpp_redis" which requires target "tacopie" that is not in any export set.
-- Generating done
CMake Generate step failed.  Build files cannot be regenerated correctly.
```

We need to moditify the CMakeLists.txt file in cpp_redis/tacopie
<br>
Go to the line 165:
```code
###
# install
###
```
Add the following lines at then end of the install part to fix the problems.
```
# Export target "tacopie"
install(TARGETS tacopie
        EXPORT tacopieTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)

# Export the export set
install(EXPORT tacopieTargets
        FILE tacopieTargets.cmake
        NAMESPACE tacopie::
        DESTINATION lib/cmake/tacopie
)
```

## Step 2
From build directory
```shell
./tests/test_redis_client 
```

some output:
```code
Trying to pull data from queue: testQueue
Data pulled successfully.
Push operations took 146 milliseconds.
Pull operations took 131 milliseconds.
[       OK ] RedisClientTest.PerformanceTest (278 ms)
Queue cleared successfully. Items removed: 0
[----------] 4 tests from RedisClientTest (281 ms total)

[----------] Global test environment tear-down
[==========] 4 tests from 1 test suite ran. (281 ms total)
[  PASSED  ] 4 tests.
```
