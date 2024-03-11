#include "include/server.h"

int main() {
  mini_redis::server server{"localhost", "5000"};
  server.run();
}