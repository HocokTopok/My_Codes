#include <functional>
#include <iostream>
#include <string>
#include <vector>

template <typename T, typename Compare = std::less<T>>
class Heap {
 public:
  explicit Heap(size_t query_number, Compare comp = Compare())
      : positions_(query_number, 0), comp_(comp) {}

  Heap(const std::vector<T>& elements, Compare comp = Compare())
      : queryCount_(elements.size()),
        comp_(comp) {  // Реализовал типа куча создана запросами Insert
                       // (queryCount_ подсчитано как количество Insert и
                       // positions_ реализован как при запросах Insert)
    heap_.reserve(elements.size());
    positions_.reserve(elements.size());

    for (size_t i = 0; i < heap_.size(); ++i) {
      heap_[i] = std::pair<T, size_t>(elements[i], i + 1);
      positions_[i] = i;
    }

    for (size_t i = (heap_.size() - 1) / 2; i >= 0; --i) {
      SiftDown(i);
    }
  }

  void Insert(const T& new_elem) {
    ++queryCount_;

    heap_.push_back(std::pair<T, size_t>(new_elem, queryCount_));
    positions_[queryCount_ - 1] = heap_.size() - 1;
    SiftUp(heap_.size() - 1);
  }

  T GetMin() {
    ++queryCount_;

    return heap_[0].first;
  }

  void ExtractMin() {
    ++queryCount_;

    HeapSwap(0, heap_.size() - 1);
    heap_.pop_back();
    if (heap_.size() > 1) {
      SiftDown(0);
    }
  }

  void DecreaseKey(size_t query_index, const T& delta) {
    ++queryCount_;

    size_t index = positions_[query_index - 1];
    heap_[index].first -= delta;
    SiftUp(index);
  }

 private:
  std::vector<std::pair<T, size_t>> heap_;
  std::vector<size_t> positions_;
  size_t queryCount_ = 0;
  Compare comp_;

  void HeapSwap(size_t index, size_t jndex) {
    std::swap(heap_[index], heap_[jndex]);
    std::swap(positions_[heap_[index].second - 1],
              positions_[heap_[jndex].second - 1]);
  }

  void SiftUp(size_t index) {
    while (index > 0) {
      size_t parent = (index - 1) / 2;

      if (comp_(heap_[index].first, heap_[parent].first)) {
        HeapSwap(index, parent);
        index = parent;
      } else {
        break;
      }
    }
  }

  void SiftDown(size_t index) {
    while (true) {
      size_t left = (index * 2) + 1;
      size_t right = (index * 2) + 2;
      size_t minimum = index;

      if (left < heap_.size() &&
          comp_(heap_[left].first, heap_[minimum].first)) {
        minimum = left;
      }
      if (right < heap_.size() &&
          comp_(heap_[right].first, heap_[minimum].first)) {
        minimum = right;
      }
      if (index == minimum) {
        break;
      }

      HeapSwap(index, minimum);
      index = minimum;
    }
  }
};

enum class CommandType { Insert, DecreaseKey, GetMin, ExtractMin };

struct Query {
  CommandType command;
  int64_t number;
  int64_t delta;

  Query(CommandType command_name, int64_t number_or_index = 0,
        int64_t delta_number = 0)
      : command(command_name), number(number_or_index), delta(delta_number) {}
};

struct Data {
  std::vector<Query> queries;
};

Data Input() {
  Data data;
  size_t query_number;
  std::cin >> query_number;
  while (query_number != 0) {
    std::string query;
    std::cin >> query;
    if (query == "insert") {
      int64_t number;
      std::cin >> number;
      data.queries.push_back(Query(CommandType::Insert, number));
    } else if (query == "decreaseKey") {
      int64_t number_1;
      std::cin >> number_1;
      int64_t number_2;
      std::cin >> number_2;
      data.queries.push_back(
          Query(CommandType::DecreaseKey, number_1, number_2));
    } else if (query == "getMin") {
      data.queries.push_back(Query(CommandType::GetMin));
    } else if (query == "extractMin") {
      data.queries.push_back(Query(CommandType::ExtractMin));
    }
    --query_number;
  }
  return data;
}

std::vector<int64_t> Process(Data& data) {
  std::vector<int64_t> results;
  Heap<int64_t> min_heap(data.queries.size());
  for (Query& query : data.queries) {
    if (query.command == CommandType::Insert) {
      min_heap.Insert(query.number);
    } else if (query.command == CommandType::DecreaseKey) {
      min_heap.DecreaseKey(query.number, query.delta);
    } else if (query.command == CommandType::GetMin) {
      results.push_back(min_heap.GetMin());
    } else {
      min_heap.ExtractMin();
    }
  }

  return results;
}

void Output(std::vector<int64_t> results) {
  for (int64_t& result : results) {
    std::cout << result << "\n";
  }
}

int main() {
  Data data = Input();
  std::vector<int64_t> results_for_get_min = Process(data);
  Output(results_for_get_min);
}