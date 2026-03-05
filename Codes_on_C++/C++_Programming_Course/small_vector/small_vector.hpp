#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <type_traits>

template <typename T, size_t N>
class SmallVector {
 public:
  template <bool IsConst>
  class BasicIterator {
   public:
    using pointer = std::conditional_t<IsConst, const T*, T*>;
    using reference = std::conditional_t<IsConst, const T&, T&>;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::contiguous_iterator_tag;

    BasicIterator() = default;

    explicit BasicIterator(pointer ptr) : ptr_(ptr) {}

    reference operator*() const { return *ptr_; }

    pointer operator->() const { return ptr_; }

    BasicIterator& operator++() {
      ++ptr_;
      return *this;
    }
    BasicIterator operator++(int) {
      BasicIterator tmp(*this);
      ++ptr_;
      return tmp;
    }
    BasicIterator& operator--() {
      --ptr_;
      return *this;
    }
    BasicIterator operator--(int) {
      BasicIterator tmp(*this);
      --ptr_;
      return tmp;
    }
    BasicIterator& operator+=(difference_type n) {
      ptr_ += n;
      return *this;
    }
    BasicIterator& operator-=(difference_type n) {
      ptr_ -= n;
      return *this;
    }

    difference_type operator-(const BasicIterator& other) const {
      return ptr_ - other.ptr_;
    }

    bool operator==(const BasicIterator& other) const {
      return ptr_ == other.ptr_;
    }
    bool operator!=(const BasicIterator& other) const {
      return ptr_ != other.ptr_;
    }
    bool operator<(const BasicIterator& other) const {
      return ptr_ < other.ptr_;
    }
    bool operator>(const BasicIterator& other) const {
      return ptr_ > other.ptr_;
    }
    bool operator<=(const BasicIterator& other) const {
      return ptr_ <= other.ptr_;
    }
    bool operator>=(const BasicIterator& other) const {
      return ptr_ >= other.ptr_;
    }

    operator BasicIterator<true>() const { return BasicIterator<true>(ptr_); }

   private:
    pointer ptr_ = nullptr;
  };

  using Iterator = BasicIterator<false>;
  using constIterator = BasicIterator<true>;

  SmallVector() : size_(0), dynamic_data_(nullptr), capacity_(N) {}

  SmallVector(size_t size) : SmallVector() { resize(size); }

  SmallVector(size_t size, const T& obj) : SmallVector() { resize(size, obj); }

  SmallVector(const SmallVector& other) : SmallVector() {
    reserve(other.size_);
    size_t i = 0;
    try {
      for (; i < other.size_; ++i) {
        new (Data() + i) T(other[i]);
      }
      size_ = other.size_;
    } catch (...) {
      for (size_t j = 0; j < i; ++j) {
        (Data() + j)->~T();
      }
    }
  }

  SmallVector& operator=(const SmallVector& other) {
    if (this != &other) {
      SmallVector tmp(other);
      Swap(tmp);
    }
    return *this;
  }

  ~SmallVector() {
    DestroyAll();
    if (UsingHeap()) {
      delete[] reinterpret_cast<int8_t*>(dynamic_data_);
    }
  }

  void push_back(const T& value) {
    if (size_ == capacity_) {
      size_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
      reserve(new_capacity);
    }
    new (Data() + size_) T(value);
    ++size_;
  }

  void pop_back() {
    assert(size_ > 0);
    --size_;
    (Data() + size_)->~T();
  }

  void reserve(size_t new_capacity) {
    if (new_capacity <= capacity_) {
      return;
    }
    T* new_data = reinterpret_cast<T*>(new int8_t[new_capacity * sizeof(T)]);
    size_t i = 0;
    try {
      for (; i < size_; ++i) {
        new (new_data + i) T(Data()[i]);
      }
    } catch (...) {
      for (size_t j = 0; j < i; ++j) {
        (new_data + j)->~T();
      }
      delete[] reinterpret_cast<int8_t*>(new_data);
      throw;
    }

    DestroyAll();
    if (UsingHeap()) {
      delete[] reinterpret_cast<int8_t*>(dynamic_data_);
    }
    dynamic_data_ = new_data;
    capacity_ = new_capacity;
  }

  void resize(size_t new_size) {
    if (new_size < size_) {
      for (size_t i = new_size; i < size_; ++i) {
        (Data() + i)->~T();
      }
    } else {
      reserve(new_size);
      for (size_t i = size_; i < new_size; ++i) {
        new (Data() + i) T();
      }
    }
    size_ = new_size;
  }

  void resize(size_t new_size, const T& value) {
    if (new_size < size_) {
      for (size_t i = new_size; i < size_; ++i) {
        (Data() + i)->~T();
      }
    } else {
      reserve(new_size);
      for (size_t i = size_; i < new_size; ++i) {
        new (Data() + i) T(value);
      }
    }
    size_ = new_size;
  }

  size_t Size() const { return size_; }

  T* Data() {
    return UsingHeap() ? dynamic_data_ : reinterpret_cast<T*>(stack_buffer_);
  }
  const T* Data() const {
    return UsingHeap() ? dynamic_data_
                       : reinterpret_cast<const T*>(stack_buffer_);
  }

  T& operator[](size_t index) { return Data()[index]; }
  const T& operator[](size_t index) const { return Data()[index]; }

  void Swap(SmallVector& other) {
    if (this == &other) {
      return;
    }
    std::swap(dynamic_data_, other.dynamic_data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);

    for (size_t i = 0; i < sizeof(stack_buffer_); ++i) {
      std::swap(stack_buffer_[i], other.stack_buffer_[i]);
    }
  }

  Iterator begin() { return Iterator(Data()); }
  constIterator begin() const { return constIterator(Data()); }
  Iterator end() { return Iterator(Data() + size_); }
  constIterator end() const { return constIterator(Data() + size_); }
  constIterator cbegin() const { return begin(); }
  constIterator cend() const { return end(); }

 private:
  bool UsingHeap() const { return dynamic_data_ != nullptr; }

  void DestroyAll() {
    for (size_t i = 0; i < size_; ++i) {
      (Data() + i)->~T();
    }
  }

  size_t size_ = 0;
  size_t capacity_ = N;
  T* dynamic_data_ = nullptr;
  alignas(T) int8_t stack_buffer_[sizeof(T) * N];
};