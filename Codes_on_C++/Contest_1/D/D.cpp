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
int main() {
  const int kMaxsize = 1e5 + 5;
  int vec[kMaxsize];
  int num_c;
  std::cin >> num_c;
  for (int i = 0; i < num_c; i++) {
    std::cin >> vec[i];
  }

  int tasks_c;
  std::cin >> tasks_c;
  for (int i = 0; i < tasks_c; i++) {
    int begin;
    int end;
    int target;
    std::cin >> begin >> end >> target;
    if (BinarySearch(vec + begin, vec + end, target)) {
      std::cout << "YES\n";
    } else {
      std::cout << "NO\n";
    }
  }
}