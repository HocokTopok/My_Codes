#include <iostream>
#include <vector>

int main() {
  std::vector<int> a{1};
  std::vector<bool> b{true};

  auto x = a[0];
  auto y = b[0];

  a[0] = 0;
  b[0] = false;

  std::cout << x << ' ' << y << '\n';
}