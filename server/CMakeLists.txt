add_library(server_obj STATIC src/server.cpp)
target_link_libraries(server_obj listener)
target_include_directories(server_obj PUBLIC .)

add_library(server_main OBJECT main.cpp)
target_link_libraries(server_main server_obj)
target_include_directories(server_main PUBLIC .)