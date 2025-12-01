#include <iostream>
#include <vector>

const int kINF = 1e9;

class SegmentTreeMin {
 public:
  SegmentTreeMin(size_t size) {
    bin_size_ = 1;
    while (bin_size_ < size) {
      bin_size_ *= 2;
    }
    data_.assign(2 * bin_size_, kINF);
  }

  void Update(size_t index, int value) {
    index += bin_size_;
    data_[index] = value;
    index /= 2;

    while (index != 0) {
      data_[index] = std::min(data_[index * 2], data_[index * 2 + 1]);
      index /= 2;
    }
  }

  int Query(size_t left, size_t right) {
    if (left > right) {
      return kINF;
    }

    left += bin_size_;
    right += bin_size_;
    int res = kINF;

    while (left <= right) {
      if (left % 2 == 1) {
        res = std::min(res, data_[left]);
        ++left;
      }

      if (right % 2 == 0) {
        res = std::min(res, data_[right]);
        --right;
      }

      left /= 2;
      right /= 2;
    }
    return res;
  }

 private:
  size_t bin_size_;
  std::vector<int> data_;
};

int main() {
    std::cout << "Segment Tree RMQ Test" << "\n";
    return 0;
}