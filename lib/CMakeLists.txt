
add_library(connection STATIC 
  src/connection/connection.cpp 
  src/connection/poll_manager.cpp
)
target_include_directories(connection PUBLIC .)

add_library(request STATIC src/protocol/request.cpp)
target_include_directories(request PUBLIC .)

add_library(listener STATIC src/connection/listener.cpp)
target_link_libraries(listener connection request)
target_include_directories(listener PUBLIC .)

add_library(client_conn STATIC src/connection/client.cpp)
target_link_libraries(client_conn connection request)
target_include_directories(client_conn PUBLIC .)
