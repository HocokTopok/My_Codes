#include <iostream>
#include <vector>

class FenwickTree {
 public:
  FenwickTree(size_t size) : fenwick_(size + 1, 0) {}

  FenwickTree(std::vector<int> numbers) : fenwick_(numbers.size() + 1, 0) {
    for (size_t i = 0; i < numbers.size(); ++i) {
      Update(i + 1, numbers[i]);
    }
  }

  int RangeSum(size_t left, size_t right) {
    if (left > right) {
      return 0;
    }
    return Query(right) - Query(left - 1);
  }

 private:
  std::vector<int> fenwick_;

  void Update(size_t index, int delta) {
    while (index < fenwick_.size()) {
      fenwick_[index] += delta;
      index += index & -index;
    }
  }

  int Query(size_t index) {
    int sum = 0;
    while (index > 0) {
      sum += fenwick_[index];
      index -= index & -index;
    }
    return sum;
  }
};