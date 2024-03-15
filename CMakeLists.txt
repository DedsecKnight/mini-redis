cmake_minimum_required(VERSION 3.20)
project(redis_cli 
        VERSION 1 
        DESCRIPTION "Mini redis client and server implementation"
        LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_subdirectory(client)
add_subdirectory(lib)
add_subdirectory(server)
add_subdirectory(examples)

