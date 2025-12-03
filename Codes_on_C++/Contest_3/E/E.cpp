// Нерабочий код
#include <iostream>
#include <vector>

class FenwickTree {
 public:
  FenwickTree(size_t size) : fenwick_(size + 1, 0) {}

  int64_t RangeSum(size_t left, size_t right) const {
    if (left > right) {
      return 0;
    }
    return Query(right) - Query(left - 1);
  }

  void Update(size_t index, int64_t delta) {
    while (index < fenwick_.size()) {
      fenwick_[index] += delta;
      index += index & -index;
    }
  }

 private:
  std::vector<int64_t> fenwick_;

  int64_t Query(size_t index) const {
    int64_t result = 0;
    while (index > 0) {
      result += fenwick_[index];
      index -= index & -index;
    }
    return result;
  }
};

class DoubleFenwickTree {
 public:
  DoubleFenwickTree(size_t size)
      : first_elements_((size + 1) / 2), second_elements_(size / 2) {}

  void SetValue(size_t index, int64_t new_value) {
    if (index % 2 == 1) {
      size_t local_index = index / 2 + 1;
      int64_t old_value = first_elements_.RangeSum(local_index, local_index);
      first_elements_.Update(local_index, new_value - old_value);
    } else {
      size_t local_index = index / 2;
      int64_t old_value = second_elements_.RangeSum(local_index, local_index);
      second_elements_.Update(local_index, new_value - old_value);
    }
  }

  int64_t GetSum(size_t left, size_t right) const {
    size_t first_left = left / 2 + 1;
    size_t first_right = (right + 1) / 2;

    size_t second_left = (left + 1) / 2;
    size_t second_right = right / 2;

    return first_elements_.RangeSum(first_left, first_right) +
           second_elements_.RangeSum(second_left, second_right);
  }

  void SwapPairs(size_t left, size_t right) {
    size_t pairs_count = (right - left + 1) / 2;

    for (size_t i = 0; i < pairs_count; ++i) {
      size_t first_index = left + 2 * i;
      size_t second_index = first_index + 1;

      int64_t first_value = GetValue(first_index);
      int64_t second_value = GetValue(second_index);

      SetValue(first_index, second_value);
      SetValue(second_index, first_value);
    }
  }

 private:
  FenwickTree first_elements_;
  FenwickTree second_elements_;

  int64_t GetValue(size_t index) const {
    if (index % 2 == 1) {
      return first_elements_.RangeSum(index / 2 + 1, index / 2 + 1);
    }
    return second_elements_.RangeSum(index / 2, index / 2);
  }
};

int main() {
  size_t count = 1;
  size_t elements_count;
  size_t queries_count;
  std::cin >> elements_count >> queries_count;

  while (elements_count != 0) {
    std::cout << "Swapper " << count << ":\n";
    DoubleFenwickTree tree(elements_count);

    for (size_t i = 1; i <= elements_count; ++i) {
      int64_t new_element;
      std::cin >> new_element;
      tree.SetValue(i, new_element);
    }

    while (queries_count != 0) {
      size_t type;
      size_t left;
      size_t right;

      std::cin >> type >> left >> right;

      if (type == 1) {
        tree.SwapPairs(left, right);
      } else {
        std::cout << tree.GetSum(left, right) << "\n";
      }
      --queries_count;
    }

    ++count;
    std::cin >> elements_count >> queries_count;
  }
}