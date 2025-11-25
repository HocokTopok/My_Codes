#include <iostream>
#include <vector>

class SparseTable {
 public:
  SparseTable(const std::vector<int>& array) {
    size_t size = array.size();
    size_t bin_size = 0;
    while ((1 << bin_size) <= size) {
      ++bin_size;
    }

    sparse_.resize(size, std::vector<int>(bin_size));

    for (size_t i = 0; i < size; ++i) {
      sparse_[i][0] = array[i];
    }

    for (size_t l = 1; l < bin_size; ++l) {
      for (size_t i = 0; i + (1 << l) <= size; ++i) {
        sparse_[i][l] = std::min(sparse_[i][l - 1], sparse_[i + (1 << (l - 1))][l - 1]);
      }
    }
  }
 
  int Query(size_t left, size_t right) {
    size_t bin_size = 0;
    size_t length = right - left + 1;
    while ((1 << (bin_size + 1)) <= length) {
      ++bin_size;
    }
    return std::min(sparse_[left][bin_size], sparse_[right - (1 << bin_size) + 1][bin_size]);
  }

 private:
  std::vector<std::vector<int>> sparse_;
};