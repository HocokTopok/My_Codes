#include <algorithm>
#include <iostream>
#include <vector>

bool Check(std::vector<int>& vec, int num_c, int kolvo, long long length) {
  int used = 1;
  long long max_light = vec[0] + length;

  for (int i = 1; i < num_c; i++) {
    if (vec[i] > max_light) {
      used++;
      if (used > kolvo) {
        return false;
      }
      max_light = vec[i] + length;
    }
  }

  return true;
}

int main() {
  int num_c;
  int kolvo;
  std::cin >> num_c >> kolvo;
  std::vector<int> vec(num_c);
  for (int i = 0; i < num_c; i++) {
    std::cin >> vec[i];
  }
  std::sort(vec.begin(), vec.end());

  long long left = 0;
  long long right = vec.back() - vec[0];

  while (left < right) {
    long long mid = (right + left) / 2;
    if (Check(vec, num_c, kolvo, mid)) {
      right = mid;
    } else {
      left = mid + 1;
    }
  }

  std::cout << left;
}