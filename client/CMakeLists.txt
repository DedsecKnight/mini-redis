add_library(client_obj STATIC src/client.cpp)
target_link_libraries(client_obj client_conn)
target_include_directories(client_obj PUBLIC .)