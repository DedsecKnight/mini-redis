#include "include/server.h"

int main() {
  auto server = mini_redis::server{"localhost", "5000"};
  server.enable_nonblocking_io();
  server.run();
}