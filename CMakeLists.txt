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

add_executable(server)
set_target_properties(server 
  PROPERTIES 
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
target_link_libraries(server PRIVATE server_main)

add_executable(client)
set_target_properties(client
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
target_link_libraries(client PRIVATE client_main)