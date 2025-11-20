#include <cstddef>
#include <vector>

class RingBuffer {
 public:
  explicit RingBuffer(size_t capacity) : buffer_(capacity) {}

  size_t Size() const { return size_; }

  bool Empty() const { return size_ == 0; }

  bool TryPush(int element) {
    if (size_ == buffer_.capacity()) {
      return false;
    }
    size_t end = (start_ + size_) % buffer_.capacity();
    buffer_[end] = element;
    ++size_;
    return true;
  }

  bool TryPop(int* element) {
    if (size_ == 0) {
      return false;
    }
    if (element != nullptr) {
      *element = buffer_[start_];
    }
    start_ = (start_ + 1) % buffer_.capacity();
    --size_;
    return true;
  }

 private:
  std::vector<int> buffer_;
  size_t size_ = 0;
  size_t start_ = 0;
};