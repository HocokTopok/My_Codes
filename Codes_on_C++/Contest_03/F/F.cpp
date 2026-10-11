#include <iostream>
#include <vector>

class SegmentTree {
 public:
  SegmentTree(size_t size) {
    bin_size_ = 1;
    while (size > bin_size_) {
      bin_size_ *= 2;
    }
    data_.assign(2 * bin_size_, 0);
  }

  void Update(size_t index, size_t new_value) {
    if (index % 2 == 1) {
      new_value = -new_value;
    }

    index += bin_size_;
    data_[index] = new_value;
    index /= 2;

    while (index != 0) {
      data_[index] = data_[index * 2] + data_[index * 2 + 1];
      index /= 2;
    }
  }

  int Query(size_t left, size_t right) {
    int extra = left % 2 == 0 ? 1 : -1;

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

    return extra * result;
  }

 private:
  size_t bin_size_;
  std::vector<int> data_;
};

int main() {
  size_t elements_count;
  std::cin >> elements_count;

  SegmentTree tree(elements_count);

  for (size_t i = 0; i < elements_count; ++i) {
    int element;
    std::cin >> element;
    tree.Update(i, element);
  }

  size_t queries_count;
  std::cin >> queries_count;
  while (queries_count != 0) {
    size_t type;
    size_t num1;
    size_t num2;
    std::cin >> type >> num1 >> num2;
    if (type == 0) {
      tree.Update(num1 - 1, num2);
    } else {
      std::cout << tree.Query(num1 - 1, num2 - 1) << "\n";
    }
    --queries_count;
  }
}