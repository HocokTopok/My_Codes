#include <algorithm>
#include <iostream>
#include <vector>

int main() {
  int num_c;
  std::cin >> num_c;
  std::vector<int> vec(num_c);
  for (int i = 0; i < num_c; i++) {
    std::cin >> vec[i];
  }
  std::vector<int> l_min(num_c);
  std::vector<int> r_min(num_c);
  l_min[0] = vec.front();
  r_min[num_c - 1] = vec.back();
  for (int li = 1; li < num_c; li++) {
    int ri = num_c - li - 1;
    l_min[li] = std::min(l_min[li - 1], vec[li]);
    r_min[ri] = std::min(r_min[ri + 1], vec[ri]);
  }
  int tasks;
  std::cin >> tasks;
  for (int i = 0; i < tasks; i++) {
    int left;
    int right;
    std::cin >> left >> right;
    int res = std::min(l_min[left - 1], r_min[right - 1]);
    std::cout << res << std::endl;
  }
}