#include <cstdlib>
#include <ctime>
#include <iostream>

int main() {
  std::srand(std::time(0));
  std::cout << rand() << std::endl;
  std::cout << rand() << std::endl;
  return 0;
}
