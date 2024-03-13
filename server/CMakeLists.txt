add_library(commands STATIC 
  src/data/global_data.cpp
  src/commands/get.cpp
  src/commands/set.cpp
  src/commands/del.cpp
  src/commands/expire.cpp
  src/commands/ttl.cpp
)
target_link_libraries(commands response request)
target_include_directories(commands PUBLIC .)

add_library(server_obj STATIC 
  src/server.cpp 
)
target_link_libraries(server_obj listener commands)
target_include_directories(server_obj PUBLIC .)

add_library(server_main OBJECT main.cpp)
target_link_libraries(server_main server_obj)
target_include_directories(server_main PUBLIC .)