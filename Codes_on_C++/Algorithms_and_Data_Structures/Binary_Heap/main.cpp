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