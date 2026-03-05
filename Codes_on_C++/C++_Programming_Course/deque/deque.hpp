#include <algorithm>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <vector>

template <typename T>
class Deque {
 public:
  template <bool IsConst>
  class BaseIterator {
   public:
    using pointer = std::conditional_t<IsConst, const T*, T*>;
    using reference = std::conditional_t<IsConst, const T&, T&>;
    using value_type = T;
    using iterator_category = std::random_access_iterator_tag;
    using difference_type = std::ptrdiff_t;

    using deque_ptr = std::conditional_t<IsConst, const Deque*, Deque*>;

    BaseIterator() = default;

    BaseIterator(deque_ptr deque_ptr, size_t index)
        : deque_ptr_(deque_ptr), index_(index) {}

    BaseIterator(const BaseIterator& other) = default;

    BaseIterator& operator=(const BaseIterator& other) = default;

    pointer operator->() const { return &(*deque_ptr_)[index_]; }

    reference operator*() const { return (*deque_ptr_)[index_]; }

    BaseIterator& operator++() {
      ++index_;
      return *this;
    }

    BaseIterator operator++(int) {
      BaseIterator tmp(deque_ptr_, index_);
      ++index_;
      return tmp;
    }

    BaseIterator& operator--() {
      --index_;
      return *this;
    }

    BaseIterator operator--(int) {
      BaseIterator tmp(deque_ptr_, index_);
      --index_;
      return tmp;
    }

    BaseIterator& operator+=(difference_type count) {
      index_ += count;
      return *this;
    }

    BaseIterator& operator-=(difference_type count) {
      index_ -= count;
      return *this;
    }

    BaseIterator operator+(difference_type count) const {
      return BaseIterator(deque_ptr_, index_ + count);
    }

    BaseIterator operator-(difference_type count) const {
      return BaseIterator(deque_ptr_, index_ - count);
    }

    difference_type operator-(const BaseIterator& other) const {
      return static_cast<difference_type>(index_ - other.index_);
    }

    bool operator==(const BaseIterator& other) const {
      return deque_ptr_ == other.deque_ptr_ && index_ == other.index_;
    }

    bool operator!=(const BaseIterator& other) const {
      return deque_ptr_ != other.deque_ptr_ || index_ != other.index_;
    }

    bool operator>=(const BaseIterator& other) const {
      return deque_ptr_ == other.deque_ptr_ && index_ >= other.index_;
    }

    bool operator>(const BaseIterator& other) const {
      return deque_ptr_ == other.deque_ptr_ && index_ > other.index_;
    }

    bool operator<=(const BaseIterator& other) const {
      return deque_ptr_ == other.deque_ptr_ && index_ <= other.index_;
    }

    bool operator<(const BaseIterator& other) const {
      return deque_ptr_ == other.deque_ptr_ && index_ < other.index_;
    }

    operator BaseIterator<true>() const {
      return BaseIterator<true>(deque_ptr_, index_);
    }

   private:
    deque_ptr deque_ptr_ = nullptr;
    size_t index_ = 0;
  };

  using iterator = BaseIterator<false>;
  using const_iterator = BaseIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  Deque()
      : bucket_count_(0), first_bucket_(0), start_index_(0), end_index_(0) {}

  explicit Deque(size_t count) : first_bucket_(0), start_index_(0) {
    bucket_count_ = (count + kBucketSize - 1) / kBucketSize;
    end_index_ = count - (bucket_count_ - 1) * kBucketSize;

    data_.resize(2 * bucket_count_);

    size_t iter = 0;

    for (; iter < bucket_count_ - 1; ++iter) {
      data_[iter] = new Bucket(kBucketSize);
    }

    data_[iter] = new Bucket(end_index_);
  }

  Deque(size_t count, const T& value) : first_bucket_(0), start_index_(0) {
    bucket_count_ = (count + kBucketSize - 1) / kBucketSize;
    end_index_ = count - (bucket_count_ - 1) * kBucketSize;

    data_.resize(2 * bucket_count_);

    size_t iter = 0;

    for (; iter < bucket_count_ - 1; ++iter) {
      data_[iter] = new Bucket(kBucketSize, value);
    }

    data_[iter] = new Bucket(end_index_, value);
  }

  Deque(const Deque& other)
      : bucket_count_(other.bucket_count_),
        first_bucket_(other.first_bucket_),
        start_index_(other.start_index_),
        end_index_(other.end_index_),
        data_(other.data_.size()) {
    if (!other.data_.empty()) {
      for (size_t i = first_bucket_;
           i != (first_bucket_ + bucket_count_) % other.data_.size();
           i = (i + 1) % other.data_.size()) {
        data_[i] = new Bucket(*other.data_[i]);
      }
    }
  }

  Deque& operator=(const Deque& other) {
    if (&other != this) {
      Deque tmp(other);
      swap(tmp);
    }
    return *this;
  }

  ~Deque() {
    for (auto& bucket : data_) {
      delete bucket;
    }
  }

  size_t size() const {
    if (bucket_count_ == 0) {
      return 0;
    }
    return ((bucket_count_ - 1) * kBucketSize) - start_index_ + end_index_;
  }

  bool empty() const { return size() == 0; }

  T& operator[](size_t index) {
    size_t bucket_index = find_bucket_index(index);
    size_t element_index = find_element_index(index);
    return (*data_[bucket_index])[element_index];
  }

  const T& operator[](size_t index) const {
    size_t bucket_index = find_bucket_index(index);
    size_t element_index = find_element_index(index);
    return (*data_[bucket_index])[element_index];
  }

  T& at(size_t index) {
    if (index >= size()) {
      throw std::out_of_range("Index out of range");
    }

    size_t bucket_index = find_bucket_index(index);
    size_t element_index = find_element_index(index);
    return (*data_[bucket_index])[element_index];
  }

  const T& at(size_t index) const {
    if (index >= size()) {
      throw std::out_of_range("Index out of range");
    }

    size_t bucket_index = find_bucket_index(index);
    size_t element_index = find_element_index(index);
    return (*data_[bucket_index])[element_index];
  }

  void push_back(const T& value) {
    if (bucket_count_ == 0 || end_index_ == kBucketSize) {
      if (bucket_count_ == data_.size()) {
        reallocate();
      }

      size_t new_bucket = (first_bucket_ + bucket_count_) % data_.size();

      delete data_[new_bucket];
      data_[new_bucket] = new Bucket();
      data_[new_bucket]->start_index = 0;
      data_[new_bucket]->end_index = 0;
      data_[new_bucket]->push_back(value);

      ++bucket_count_;
      end_index_ = 1;

      return;
    }

    size_t last_bucket = find_last_bucket_index();
    data_[last_bucket]->push_back(value);
    ++end_index_;
  }

  void pop_back() {
    size_t last_bucket = find_last_bucket_index();
    data_[last_bucket]->pop_back();

    if (data_[last_bucket]->end_index == 0) {
      --bucket_count_;
      end_index_ = kBucketSize;
    } else {
      --end_index_;
    }
  }

  void push_front(const T& value) {
    if (start_index_ == 0 || bucket_count_ == 0) {
      if (bucket_count_ == data_.size()) {
        reallocate();
      }

      size_t new_bucket = (first_bucket_ + data_.size() - 1) % data_.size();

      delete data_[new_bucket];
      data_[new_bucket] = new Bucket();
      data_[new_bucket]->start_index = kBucketSize;
      data_[new_bucket]->end_index = kBucketSize;
      data_[new_bucket]->push_front(value);

      ++bucket_count_;
      first_bucket_ = new_bucket;

      start_index_ = kBucketSize - 1;
      return;
    }

    data_[first_bucket_]->push_front(value);
    --start_index_;
  }

  void pop_front() {
    data_[first_bucket_]->pop_front();

    if (data_[first_bucket_]->start_index == data_[first_bucket_]->end_index) {
      --bucket_count_;
      first_bucket_ = (first_bucket_ + 1) % data_.size();
      start_index_ = 0;
    } else {
      ++start_index_;
    }
  }

  void insert(iterator iter, const T& value) {
    size_t real_index = static_cast<size_t>(iter - begin());
    push_back(value);

    for (size_t index = size() - 1; index > real_index; --index) {
      (*this)[index] = (*this)[index - 1];
    }
    (*this)[real_index] = value;
  }

  void erase(iterator iter) {
    for (iterator index = iter + 1; index != end(); ++index) {
      *(index - 1) = *index;
    }

    pop_back();
  }

  iterator begin() { return iterator(this, 0); }

  const_iterator begin() const { return const_iterator(this, 0); }

  const_iterator cbegin() const { return begin(); }

  iterator end() { return iterator(this, size()); }

  const_iterator end() const { return const_iterator(this, size()); }

  const_iterator cend() const { return end(); }

  reverse_iterator rbegin() { return reverse_iterator(end()); }

  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }

  const_reverse_iterator crbegin() const { return rbegin(); }

  reverse_iterator rend() { return reverse_iterator(begin()); }

  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  const_reverse_iterator crend() const { return rend(); }

 private:
  struct Bucket {
    Bucket() : start_index(0), end_index(0) {
      elements = static_cast<T*>(malloc(sizeof(T) * kBucketSize));
    }

    Bucket(const Bucket& other)
        : start_index(other.start_index), end_index(other.end_index) {
      elements = static_cast<T*>(malloc(sizeof(T) * kBucketSize));

      size_t iter = start_index;
      try {
        for (; iter < end_index; ++iter) {
          new (elements + iter) T(other.elements[iter]);
        }
      } catch (...) {
        for (size_t j = start_index; j < iter; ++j) {
          (elements + j)->~T();
        }

        free(elements);
        throw;
      }
    }

    Bucket(size_t count) : start_index(0), end_index(count) {
      elements = static_cast<T*>(malloc(sizeof(T) * kBucketSize));

      size_t iter = 0;
      try {
        for (; iter < count; ++iter) {
          new (elements + iter) T();
        }
      } catch (...) {
        for (size_t j = 0; j < iter; ++j) {
          (elements + j)->~T();
        }

        free(elements);
        throw;
      }
    }

    Bucket(size_t count, const T& value) : start_index(0), end_index(count) {
      elements = static_cast<T*>(malloc(sizeof(T) * kBucketSize));

      size_t iter = 0;
      try {
        for (; iter < count; ++iter) {
          new (elements + iter) T(value);
        }
      } catch (...) {
        for (size_t j = 0; j < iter; ++j) {
          (elements + j)->~T();
        }

        free(elements);
        throw;
      }
    };

    ~Bucket() {
      for (size_t index = start_index; index < end_index; ++index) {
        elements[index].~T();
      }
      free(elements);
    }

    void push_back(const T& value) {
      new (elements + end_index) T(value);
      ++end_index;
    }

    void push_front(const T& value) {
      --start_index;
      new (elements + start_index) T(value);
    }

    void pop_back() {
      --end_index;
      (elements + end_index)->~T();
    }

    void pop_front() {
      (elements + start_index)->~T();
      ++start_index;
    }

    T& operator[](size_t index) { return elements[index]; }

    const T& operator[](size_t index) const { return elements[index]; }

    size_t start_index = 0;
    size_t end_index = 0;
    T* elements = nullptr;
  };

  void swap(Deque& other) noexcept {
    std::swap(bucket_count_, other.bucket_count_);
    std::swap(first_bucket_, other.first_bucket_);
    std::swap(start_index_, other.start_index_);
    std::swap(end_index_, other.end_index_);
    std::swap(data_, other.data_);
  }

  void reallocate() {
    if (data_.empty()) {
      data_.resize(2);
      return;
    }

    std::vector<Bucket*> temp(data_);

    data_.resize(2 * data_.size());

    size_t iter = 0;
    for (size_t index = first_bucket_;
         index != (first_bucket_ + bucket_count_) % temp.size();
         index = (index + 1) % temp.size()) {
      data_[iter] = temp[index];
      ++iter;
    }

    first_bucket_ = 0;
  }

  size_t find_bucket_index(size_t index) const {
    return (first_bucket_ + (start_index_ + index) / kBucketSize) %
           data_.size();
  }

  size_t find_element_index(size_t index) const {
    return (start_index_ + index) % kBucketSize;
  }

  size_t find_last_bucket_index() const {
    return (first_bucket_ + bucket_count_ + data_.size() - 1) % data_.size();
  }

  size_t bucket_count_ = 0;
  size_t first_bucket_ = 0;
  size_t start_index_ = 0;
  size_t end_index_ = 0;
  std::vector<Bucket*> data_;
  static inline const size_t kBucketSize = 64;
};