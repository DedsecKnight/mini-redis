#include <include/test.h>

#include <iostream>

int main() {
  Test test{};
  int x = test.get_x();
  std::cout << x << std::endl;
  std::cout << "server app" << std::endl;
}