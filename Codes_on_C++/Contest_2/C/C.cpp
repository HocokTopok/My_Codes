#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

const int kMod = 1 << 30;

class MaxHeap {
 public:
  MaxHeap() = default;

  void Insert(int number) {
    heap_.push_back(number);
    SiftUp(heap_.size() - 1);
  }

  void Pop() {
    if (heap_.empty()) {
      return;
    }
    std::swap(heap_[0], heap_[heap_.size() - 1]);
    heap_.pop_back();
    if (!heap_.empty()) {
      SiftDown(0);
    }
  }

  int Top() const {
    if (heap_.empty()) {
      return -1;
    }
    return heap_[0];
  }

  int Size() const { return heap_.size(); }

  const std::vector<int>& GetVector() const { return heap_; }

 private:
  std::vector<int> heap_;

  void SiftUp(size_t index) {
    while (index != 0) {
      size_t parent = (index - 1) / 2;

      if (heap_[parent] >= heap_[index]) {
        break;
      }

      std::swap(heap_[parent], heap_[index]);
      index = parent;
    }
  }

  void SiftDown(size_t index) {
    size_t size = heap_.size();
    while (true) {
      size_t left = index * 2 + 1;
      size_t right = index * 2 + 2;
      size_t maximum = index;

      if (left < size && heap_[left] > heap_[maximum]) {
        maximum = left;
      }
      if (right < size && heap_[right] > heap_[maximum]) {
        maximum = right;
      }
      if (index == maximum) {
        break;
      }

      std::swap(heap_[index], heap_[maximum]);
      index = maximum;
    }
  }
};

int main() {
  int count;
  int k_elements;
  std::cin >> count >> k_elements;
  MaxHeap max_heap;

  int x_num;
  int y_num;
  long long current;
  std::cin >> current >> x_num >> y_num;

  while (count != 0) {
    current = (x_num * current + y_num) % kMod;
    int number = static_cast<int>(current);

    if (max_heap.Size() < k_elements) {
      max_heap.Insert(number);
    } else if (number < max_heap.Top()) {
      max_heap.Pop();
      max_heap.Insert(number);
    }

    --count;
  }

  std::vector<int> results = max_heap.GetVector();
  std::sort(results.begin(), results.end());

  for (int i = 0; i < k_elements; ++i) {
    if (i > 0) {
      std::cout << " ";
    }
    std::cout << results[i];
  }
}