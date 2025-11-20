#include <iostream>
#include <vector>

int main() {
  const int kMod = 1000000007;
  const int kBites = 32;
  long long res = 0;
  int num_c;
  std::cin >> num_c;
  std::vector<int> vec(num_c);
  for (int i = 0; i < num_c; i++) {
    std::cin >> vec[i];
  }
  for (int k = 0; k < kBites; k++) {
    long long one = 0;
    for (int i = 0; i < num_c; i++) {
      one += (vec[i] >> k) & 1;
    }
    long long temp = (1LL << k) % kMod;
    temp = temp * one % kMod;
    temp = temp * (num_c - one) % kMod;
    res = (res + temp) % kMod;
  }
  std::cout << res % kMod;
}