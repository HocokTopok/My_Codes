#include <iostream>
#include <vector>

int main() {
  int size1;
  int size2;
  std::cin >> size1;
  std::cin >> size2;

  std::vector<int64_t> numbers1(size1);
  std::vector<int64_t> numbers2(size2);

  for (int64_t& number : numbers1) {
    std::cin >> number;
  }
  for (int64_t& number : numbers2) {
    std::cin >> number;
  }

  std::vector<int> data(size2, 0);
  int result = 0;

  for (int i = 0; i < size1; ++i) {
    int current = 0;
    for (int j = 0; j < size2; ++j) {
      if (numbers1[i] == numbers2[j]) {
        if (data[j] < current + 1) {
          data[j] = current + 1;
          if (data[j] > result) {
            result = data[j];
          }
        }
      } else if (numbers1[i] > numbers2[j]) {
        if (data[j] > current) {
          current = data[j];
        }
      }
    }
  }

  std::cout << result;
}