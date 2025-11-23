#include <cmath>
#include <iostream>
#include <vector>

int IndexOrder(std::vector<int>& vec, int left, int right, int k_index) {
  // std::cout << left << " " << right << "\n";
  if (left >= right) {
    return vec[k_index];
  }

  int i_index = left;
  int j_index = right;
  int pivot = vec[(left + right) / 2];  // Добавить медиану медиан

  while (i_index <= j_index) {
    while (vec[i_index] < pivot) {
      ++i_index;
    }
    while (vec[j_index] > pivot) {
      --j_index;
    }
    if (i_index <= j_index) {
      std::swap(vec[i_index], vec[j_index]);
      ++i_index;
      --j_index;
    }
  }
  // for (size_t i = 0; i < vec.size(); ++i) {
  //   std::cout << vec[i] << " ";
  // }
  // std::cout << "\n";
  if (k_index <= j_index) {
    return IndexOrder(vec, left, j_index, k_index);
  }
  return IndexOrder(vec, i_index, right, k_index);
}

int main() {
  const int kNum1 = 123;
  const int kNum2 = 45;
  const int kNum3 = 10'000'000;
  const int kNum4 = 4321;

  int count;
  std::cin >> count;
  int k_index;
  std::cin >> k_index;
  int a0;
  std::cin >> a0;
  int a1;
  std::cin >> a1;

  std::vector<int> vec(count);
  vec[0] = a0;
  vec[1] = a1;
  for (int i = 2; i < count; ++i) {
    vec[i] = (vec[i - 1] * kNum1 + vec[i - 2] * kNum2) % (kNum3 + kNum4);
  }

  // for (size_t i = 0; i < n; ++i) {
  //   std::cout << vec[i] << " ";
  // }
  // std::cout << "\n";

  std::cout << IndexOrder(vec, 0, count - 1, k_index - 1) << "\n";

  // for (size_t i = 0; i < n; ++i) {
  //   std::cout << vec[i] << " ";
  // }
}