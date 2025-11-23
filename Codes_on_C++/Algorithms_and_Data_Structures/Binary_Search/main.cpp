#include <iostream>
#include <vector>

bool BinarySearch(const int* begin, const int* end, int target) {
  const int* left = begin;
  const int* right = end - 1;
  while (left <= right) {
    const int* mid = left + (right - left) / 2;
    if (*mid == target) {
      return true;
    }
    if (*mid < target) {
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }
  return false;
}