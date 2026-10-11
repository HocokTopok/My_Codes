#include <iostream>
#include <vector>

int main() {
  int res = 0;
  int num_c;
  std::cin >> num_c;
  std::vector<int> vec = {};

  for (int i = 0; i < 2 * num_c - 2; i++) {
    int num;
    std::cin >> num;
    vec.push_back(num);
    res ^= num;
  }
  int mask = res & -res;
  int res1 = 0;
  int res2 = 0;
  for (int num : vec) {
    if ((num & mask) != 0) {
      res1 ^= num;
    } else {
      res2 ^= num;
    }
  }
  if (res1 > res2) {
    std::cout << res2 << " " << res1;
  } else {
    std::cout << res1 << " " << res2;
  }
}