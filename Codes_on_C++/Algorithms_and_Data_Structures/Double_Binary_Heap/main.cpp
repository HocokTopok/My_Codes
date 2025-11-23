#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class MinMaxHeap {
 public:
  MinMaxHeap() : size_(0) {}

  void Insert(int number) {
    ++size_;
    ++data_[number];

    min_heap_.push_back(number);
    MinSiftUp(min_heap_.size() - 1);

    max_heap_.push_back(number);
    MaxSiftUp(max_heap_.size() - 1);
  }

  std::string GetMin() {
    while (size_ != 0 && data_[min_heap_[0]] == 0) {
      std::swap(min_heap_[0], min_heap_[min_heap_.size() - 1]);
      min_heap_.pop_back();
      if (min_heap_.size() > 1) {
        MinSiftDown(0);
      }
    }

    if (size_ == 0) {
      return "error";
    }

    return std::to_string(min_heap_[0]);
  }

  std::string GetMax() {
    while (size_ != 0 && data_[max_heap_[0]] == 0) {
      std::swap(max_heap_[0], max_heap_[max_heap_.size() - 1]);
      max_heap_.pop_back();
      if (max_heap_.size() > 1) {
        MaxSiftDown(0);
      }
    }

    if (size_ == 0) {
      return "error";
    }

    return std::to_string(max_heap_[0]);
  }

  std::string ExtractMin() {
    while (size_ != 0 && data_[min_heap_[0]] == 0) {
      std::swap(min_heap_[0], min_heap_[min_heap_.size() - 1]);
      min_heap_.pop_back();
      if (min_heap_.size() > 1) {
        MinSiftDown(0);
      }
    }

    if (size_ == 0) {
      return "error";
    }

    --size_;
    --data_[min_heap_[0]];
    std::string result = std::to_string(min_heap_[0]);

    std::swap(min_heap_[0], min_heap_[min_heap_.size() - 1]);
    min_heap_.pop_back();
    if (min_heap_.size() > 1) {
      MinSiftDown(0);
    }

    return result;
  }

  std::string ExtractMax() {
    while (size_ != 0 && data_[max_heap_[0]] == 0) {
      std::swap(max_heap_[0], max_heap_[max_heap_.size() - 1]);
      max_heap_.pop_back();
      if (max_heap_.size() > 1) {
        MaxSiftDown(0);
      }
    }

    if (size_ == 0) {
      return "error";
    }

    --size_;
    --data_[max_heap_[0]];
    std::string result = std::to_string(max_heap_[0]);

    std::swap(max_heap_[0], max_heap_[max_heap_.size() - 1]);
    max_heap_.pop_back();
    if (max_heap_.size() > 1) {
      MaxSiftDown(0);
    }

    return result;
  }

  int Size() const { return size_; }

  void Clear() {
    size_ = 0;
    data_.clear();
    min_heap_.clear();
    max_heap_.clear();
  }

 private:
  std::vector<int> max_heap_;
  std::vector<int> min_heap_;
  std::unordered_map<int, int> data_;
  int size_;

  void MinSiftUp(size_t index) {
    while (index != 0) {
      size_t parent = (index - 1) / 2;

      if (min_heap_[parent] <= min_heap_[index]) {
        break;
      }

      std::swap(min_heap_[parent], min_heap_[index]);
      index = parent;
    }
  }

  void MaxSiftUp(size_t index) {
    while (index != 0) {
      size_t parent = (index - 1) / 2;

      if (max_heap_[parent] >= max_heap_[index]) {
        break;
      }

      std::swap(max_heap_[parent], max_heap_[index]);
      index = parent;
    }
  }

  void MinSiftDown(size_t index) {
    while (true) {
      size_t left = 2 * index + 1;
      size_t right = 2 * index + 2;
      size_t minimum = index;

      if (left < min_heap_.size() && min_heap_[minimum] > min_heap_[left]) {
        minimum = left;
      }

      if (right < min_heap_.size() && min_heap_[minimum] > min_heap_[right]) {
        minimum = right;
      }

      if (minimum == index) {
        break;
      }

      std::swap(min_heap_[index], min_heap_[minimum]);
      index = minimum;
    }
  }

  void MaxSiftDown(size_t index) {
    while (true) {
      size_t left = 2 * index + 1;
      size_t right = 2 * index + 2;
      size_t maximum = index;

      if (left < max_heap_.size() && max_heap_[maximum] < max_heap_[left]) {
        maximum = left;
      }

      if (right < max_heap_.size() && max_heap_[maximum] < max_heap_[right]) {
        maximum = right;
      }

      if (maximum == index) {
        break;
      }

      std::swap(max_heap_[index], max_heap_[maximum]);
      index = maximum;
    }
  }
};