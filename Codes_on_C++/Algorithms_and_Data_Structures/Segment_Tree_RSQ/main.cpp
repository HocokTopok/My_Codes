#include <iostream>
#include <vector>

class SegmentTreeSum {
 public:
  SegmentTreeSum(size_t size) {
    bin_size_ = 1;
    while (size > bin_size_) {
      bin_size_ *= 2;
    }
    data_.assign(2 * bin_size_, 0);
  }

  void Update(size_t index, int new_value) {
    index += bin_size_;
    data_[index] = new_value;
    index /= 2;

    while (index != 0) {
      data_[index] = data_[index * 2] + data_[index * 2 + 1];
      index /= 2;
    }
  }

  int Query(size_t left, size_t right) {
    int result = 0;

    left += bin_size_;
    right += bin_size_;

    while (left <= right) {
      if (left % 2 == 1) {
        result += data_[left];
        ++left;
      }

      if (right % 2 == 0) {
        result += data_[right];
        --right;
      }

      left /= 2;
      right /= 2;
    }

    return result;
  }

 private:
  size_t bin_size_;
  std::vector<int> data_;
};