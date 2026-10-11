#include <algorithm>
#include <iostream>
#include <vector>

const int kMax = 1'000'000'000;

class SparseTable {
 public:
  SparseTable(const std::vector<int>& array) {
    size_t size = array.size();
    size_t bin_size = 0;
    while ((1 << bin_size) <= size) {
      ++bin_size;
    }

    sparse_.resize(size, std::vector<std::pair<int, int>>(bin_size));

    for (size_t i = 0; i < size; ++i) {
      sparse_[i][0] = {array[i], kMax};
    }

    for (size_t length = 1; length < bin_size; ++length) {
      for (size_t i = 0; i + (1 << length) <= size; ++i) {
        sparse_[i][length] =
            Combine(sparse_[i][length - 1],
                    sparse_[i + (1 << (length - 1))][length - 1]);
      }
    }
  }

  int Query(size_t left, size_t right) {
    size_t length = right - left + 1;
    size_t bin_size = 0;

    while ((1 << (bin_size + 1)) <= length) {
      ++bin_size;
    }

    return Combine(sparse_[left][bin_size],
                   sparse_[right - (1 << bin_size) + 1][bin_size])
        .second;
  }

 private:
  std::vector<std::vector<std::pair<int, int>>> sparse_;

  static std::pair<int, int> Combine(std::pair<int, int> left,
                                     std::pair<int, int> right) {
    std::vector<int> candidates;

    candidates.push_back(left.first);
    if (left.second != kMax) {
      candidates.push_back(left.second);
    }
    candidates.push_back(right.first);
    if (right.second != kMax) {
      candidates.push_back(right.second);
    }

    std::sort(candidates.begin(), candidates.end());

    candidates.erase(std::unique(candidates.begin(), candidates.end()),
                     candidates.end());

    if (candidates.size() >= 2) {
      return {candidates[0], candidates[1]};
    }
    if (candidates.size() == 1) {
      return {candidates[0], kMax};
    }
    return {kMax, kMax};
  }
};

struct Data {
  std::vector<int> elements;
  std::vector<std::pair<int, int>> queries;
};

Data Input() {
  Data data;

  size_t elements_count;
  size_t queries_count;
  std::cin >> elements_count >> queries_count;

  data.elements.resize(elements_count);
  for (size_t i = 0; i < elements_count; ++i) {
    std::cin >> data.elements[i];
  }

  data.queries.resize(queries_count);
  for (size_t i = 0; i < queries_count; ++i) {
    size_t left;
    size_t right;
    std::cin >> left >> right;
    data.queries[i] = std::pair<int, int>(left, right);
  }

  return data;
}

std::vector<int> Process(Data& data) {
  std::vector<int> results;

  SparseTable sparse(data.elements);
  for (std::pair<int, int>& query : data.queries) {
    results.push_back(sparse.Query(query.first - 1, query.second - 1));
  }

  return results;
}

void Output(std::vector<int>& results) {
  for (int& result : results) {
    std::cout << result << "\n";
  }
}

int main() {
  Data data = Input();
  std::vector<int> results = Process(data);
  Output(results);
}