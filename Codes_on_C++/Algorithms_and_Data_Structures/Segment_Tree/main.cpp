#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

const int kINF = 1e9;

class SegmentTreeMin {
 public:
  SegmentTreeMin(int size) {
    bin_size_ = 1;
    while (bin_size_ < size) {
      bin_size_ *= 2;
    }
    data_.assign(2 * bin_size_, kINF);
  }

  void Update(int index, int state) {
    int station_index = index + bin_size_;
    if (state == 1) {
      data_[station_index] = index;
    } else {
      data_[station_index] = kINF;
    }

    station_index /= 2;
    while (station_index != 0) {
      data_[station_index] =
          std::min(data_[station_index * 2], data_[station_index * 2 + 1]);
      station_index /= 2;
    }
  }

  int Query(int left, int right) {
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
  int bin_size_;
  std::vector<int> data_;
};