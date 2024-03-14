
add_library(data_types STATIC
  src/data_types/redis_nil.cpp
  src/data_types/redis_string.cpp
  src/data_types/redis_int.cpp
  src/data_types/redis_array.cpp
  src/data_types/registry.cpp
  src/data_types/redis_double.cpp
)
target_include_directories(data_types PUBLIC .)

add_library(connection STATIC 
  src/connection/connection.cpp 
  src/connection/poll_manager.cpp
)
target_link_libraries(connection data_types)
target_include_directories(connection PUBLIC .)

add_library(request STATIC src/protocol/request.cpp)
target_include_directories(request PUBLIC .)

add_library(response STATIC src/protocol/response.cpp)
target_include_directories(response PUBLIC .)

add_library(listener STATIC src/connection/listener.cpp)
target_link_libraries(listener connection request response)
target_include_directories(listener PUBLIC .)

add_library(client_conn STATIC src/connection/client.cpp)
target_link_libraries(client_conn connection request response)
target_include_directories(client_conn PUBLIC .)
