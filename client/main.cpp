#include <cstdio>
#include <cstdlib>

#include "include/client.h"

int main() {
  mini_redis::client client;
  if (client.connect_to("localhost", "5000") == -1) {
    exit(1);
  }
  std::printf("successfully connected to localhost:5000\n");
  client.run();
}