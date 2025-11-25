#include <iostream>
#include <vector>

class FenwickTree {
 public:
  FenwickTree(std::vector<int> numbers) : fenwick_(numbers.size() + 1, 0) {
    for (int i = 0; i < numbers.size(); ++i) {
      Update(i + 1, numbers[i]);
    }
  }

  int RangeSum(int left, int right) {
    if (left > right) {
      return 0;
    }
    return Query(right) - Query(left - 1);
  }

 private:
  std::vector<int> fenwick_;

  void Update(int index, int delta) {
    while (index < fenwick_.size()) {
      fenwick_[index] += delta;
      index += index & -index;
    }
  }

  int Query(int index) {
    int sum = 0;
    while (index > 0) {
      sum += fenwick_[index];
      index -= index & -index;
    }
    return sum;
  }
};