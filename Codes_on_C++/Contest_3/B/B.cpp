#include <algorithm>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

class FenwickTree {
 public:
  FenwickTree(int size) : fenwick_(size + 1, 0) {}

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

  void Update(size_t index, int delta) {
    while (index < fenwick_.size()) {
      fenwick_[index] += delta;
      index += index & -index;
    }
  }

 private:
  std::vector<int> fenwick_;

  int Query(size_t index) {
    int sum = 0;
    while (index > 0) {
      sum += fenwick_[index];
      index -= index & -index;
    }
    return sum;
  }
};

int main() {
  size_t count;
  std::cin >> count;
  std::vector<std::pair<size_t, int>> segments;
  std::vector<int> all_right;

  while (count != 0) {
    int left;
    int right;
    std::cin >> left >> right;
    segments.push_back({left, right});
    all_right.push_back(right);
    --count;
  }

  std::sort(
      segments.begin(), segments.end(),
      [](const std::pair<int, int>& left, const std::pair<int, int>& right) {
        if (left.first == right.first) {
          return left.second > right.second;
        }
        return left.first < right.first;
      });

  std::sort(all_right.begin(), all_right.end());
  all_right.erase(std::unique(all_right.begin(), all_right.end()),
                  all_right.end());

  std::unordered_map<int, int> right_to_index;
  for (size_t i = 0; i < all_right.size(); ++i) {
    right_to_index[all_right[i]] = i + 1;
  }

  int64_t answer = 0;

  int fen_size = all_right.size();
  FenwickTree fenwick(fen_size);
  std::map<std::pair<size_t, int>, int> same_segments;

  for (std::pair<size_t, int>& segment : segments) {
    int right = segment.second;
    int index = right_to_index[right];

    int result = fenwick.RangeSum(index, fen_size);
    int same = same_segments[segment];

    answer += (result - same);

    same_segments[segment] = same + 1;
    fenwick.Update(index, 1);
  }

  std::cout << answer;
}