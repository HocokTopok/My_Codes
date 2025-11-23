#include <cmath>
#include <iostream>
#include <vector>

int IndexOrder(std::vector<int>& vec, int left, int right, int k_index) {
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

  if (k_index <= j_index) {
    return IndexOrder(vec, left, j_index, k_index);
  }
  return IndexOrder(vec, i_index, right, k_index);
}