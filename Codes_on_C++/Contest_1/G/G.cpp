#include <algorithm>
#include <iostream>
#include <vector>

int main() {
  int num_c;
  int kratno;
  std::cin >> num_c >> kratno;
  std::vector<int> vec(num_c);
  for (int i = 0; i < num_c; i++) {
    std::cin >> vec[i];
  }
  for (int i = 0; i < num_c; i++) {
    vec[i] %= kratno;
    if (vec[i] < 0) {
      vec[i] += kratno;
    }
  }
  int previous = 0;
  std::vector<int> prefix(kratno, 0);
  for (int i = 0; i < num_c; i++) {
    previous = (previous + vec[i]) % kratno;
    prefix[previous]++;
  }
  int count = 0;
  count += prefix[0];
  for (int i = 0; i < kratno; i++) {
    count += (prefix[i] * (prefix[i] - 1)) / 2;
  }
  std::cout << count;
}