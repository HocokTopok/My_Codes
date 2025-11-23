#include <iostream>
#include <string>
#include <vector>

class Heap {
 public:
  Heap(size_t query_number) : positions_(query_number, 0) {}

  void Insert(int64_t new_elem) {
    ++queryCount_;

    heap_.push_back(std::pair<int64_t, size_t>(new_elem, queryCount_));
    positions_[queryCount_ - 1] = heap_.size() - 1;
    SiftUp(heap_.size() - 1);
  }

  int64_t GetMin() {
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

  void DecreaseKey(size_t query_index, int64_t delta) {
    ++queryCount_;

    size_t index = positions_[query_index - 1];
    heap_[index].first -= delta;
    SiftUp(index);
  }

 private:
  std::vector<std::pair<int64_t, size_t>> heap_;
  std::vector<size_t> positions_;
  size_t queryCount_ = 0;

  void HeapSwap(size_t index, size_t jndex) {
    std::swap(heap_[index], heap_[jndex]);
    std::swap(positions_[heap_[index].second - 1],
              positions_[heap_[jndex].second - 1]);
  }

  void SiftUp(size_t index) {
    while (index > 0) {
      size_t parent = (index - 1) / 2;

      if (heap_[index].first < heap_[parent].first) {
        HeapSwap(index, parent);
        index = parent;
      } else {
        break;
      }
    }
  }

  void SiftDown(size_t index) {
    while (true) {
      size_t left = index * 2 + 1;
      size_t right = index * 2 + 2;
      size_t minimum = index;

      if (left < heap_.size() && heap_[left].first < heap_[minimum].first) {
        minimum = left;
      }
      if (right < heap_.size() && heap_[right].first < heap_[minimum].first) {
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

struct Query {
  std::string command;
  int64_t number;
  int64_t delta;

  Query(std::string command_name, int64_t number_or_index = 0,
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
      data.queries.push_back(Query("insert", number));
    } else if (query == "decreaseKey") {
      int64_t number_1;
      std::cin >> number_1;
      int64_t number_2;
      std::cin >> number_2;
      data.queries.push_back(Query("decreaseKey", number_1, number_2));
    } else if (query == "getMin") {
      data.queries.push_back(Query("getMin"));
    } else if (query == "extractMin") {
      data.queries.push_back(Query("extractMin"));
    }
    --query_number;
  }
  return data;
}

std::vector<int64_t> Process(Data& data) {
  std::vector<int64_t> results;
  Heap min_heap(data.queries.size());
  for (Query& query : data.queries) {
    if (query.command == "insert") {
      min_heap.Insert(query.number);
    } else if (query.command == "decreaseKey") {
      min_heap.DecreaseKey(query.number, query.delta);
    } else if (query.command == "getMin") {
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