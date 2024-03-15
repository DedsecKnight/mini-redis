add_library(client_main OBJECT client.cpp)
target_link_libraries(client_main client_obj)
add_executable(client)
target_link_libraries(client PRIVATE client_main)

add_library(server_main OBJECT server.cpp)
target_link_libraries(server_main server_obj)
add_executable(server)
target_link_libraries(server PRIVATE server_main)