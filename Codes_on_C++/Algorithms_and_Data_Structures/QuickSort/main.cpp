#include <iostream>
#include <vector>

void QuickSort(std::vector<int>& vec, int left, int right) {
  if (left >= right) {
    return;
  }
  int i_index = left;
  int j_index = right;
  int pivot = vec[(left + right) / 2];
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
  QuickSort(vec, i_index, right);
  QuickSort(vec, left, j_index);
}