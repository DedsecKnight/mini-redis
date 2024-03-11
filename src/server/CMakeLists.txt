# add_executable(server main.cpp)
add_library(server_obj OBJECT main.cpp)
target_link_libraries(server_obj test)
target_include_directories(server_obj PUBLIC .)