#include <iostream>

int main() {
  int res = 0;
  int num_c;
  std::cin >> num_c;
  for (int i = 0; i < 2 * num_c - 1; i++) {
    int num;
    std::cin >> num;
    res ^= num;
  }
  std::cout << res;
}