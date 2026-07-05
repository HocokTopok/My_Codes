#include <algorithm>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <vector>

template <typename T>
class Deque {
 private:
  template <bool IsConst>
  class BaseIterator;

 public:
  using iterator = BaseIterator<false>;
  using const_iterator = BaseIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  Deque() = default;

  explicit Deque(size_t count);

  Deque(size_t count, const T& value);

  Deque(const Deque& other);

  Deque& operator=(const Deque& other);

  ~Deque();

  size_t size() const noexcept;

  bool empty() const noexcept { return bucket_count_ == 0; }

  T& operator[](size_t index) noexcept;

  const T& operator[](size_t index) const noexcept;

  T& at(size_t index);

  const T& at(size_t index) const;

  void push_back(const T& value);

  void pop_back();

  void push_front(const T& value);

  void pop_front();

  iterator insert(const_iterator iter, const T& value);

  iterator erase(const_iterator iter);

  void swap(Deque& other) noexcept;

  iterator begin() noexcept { return iterator(this, 0); }

  const_iterator begin() const noexcept { return const_iterator(this, 0); }

  const_iterator cbegin() const noexcept { return begin(); }

  iterator end() noexcept { return iterator(this, size()); }

  const_iterator end() const noexcept { return const_iterator(this, size()); }

  const_iterator cend() const noexcept { return end(); }

  reverse_iterator rbegin() noexcept {
    return std::make_reverse_iterator(end());
  }

  const_reverse_iterator rbegin() const noexcept {
    return std::make_reverse_iterator(end());
  }

  const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  reverse_iterator rend() noexcept {
    return std::make_reverse_iterator(begin());
  }

  const_reverse_iterator rend() const noexcept {
    return std::make_reverse_iterator(begin());
  }

  const_reverse_iterator crend() const noexcept { return rend(); }

 private:
  struct Bucket {
    Bucket()
        : elements(static_cast<T*>(::operator new(sizeof(T) * kBucketSize))) {}

    Bucket(size_t count);

    Bucket(size_t count, const T& value);

    Bucket(const Bucket& other);

    ~Bucket();

    void push_back(const T& value);

    void pop_back();

    void push_front(const T& value);

    void pop_front();

    T& operator[](size_t index) noexcept { return elements[index]; }

    const T& operator[](size_t index) const noexcept { return elements[index]; }

    size_t start_index = 0;
    size_t end_index = 0;
    T* elements = nullptr;
    Bucket* prev = nullptr;
    Bucket* next = nullptr;
  };

  void reallocate();

  size_t buckets_capacity() const noexcept { return data_.size(); }

  size_t find_element_index(size_t index) const noexcept {
    return (start_index_ + index) % kBucketSize;
  }

  size_t find_bucket_index(size_t index) const noexcept {
    return (first_bucket_ + ((start_index_ + index) / kBucketSize)) %
           buckets_capacity();
  }

  size_t find_last_bucket_index() const noexcept {
    return (first_bucket_ + bucket_count_ - 1) % buckets_capacity();
  }

  size_t find_begin_bucket_index() const noexcept {
    return (first_bucket_ + buckets_capacity() - 1) % buckets_capacity();
  }

  size_t find_end_bucket_index() const noexcept {
    return (first_bucket_ + bucket_count_) % buckets_capacity();
  }

  size_t bucket_count_ = 0;
  size_t first_bucket_ = 0;
  size_t start_index_ = 0;
  size_t end_index_ = 0;
  std::vector<Bucket*> data_;
  static const size_t kBucketSize = 64;
};

template <typename T>
template <bool IsConst>
class Deque<T>::BaseIterator {
 public:
  using pointer = std::conditional_t<IsConst, const T*, T*>;
  using reference = std::conditional_t<IsConst, const T&, T&>;
  using value_type = T;
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = std::ptrdiff_t;

  using deque_ptr = std::conditional_t<IsConst, const Deque*, Deque*>;

  BaseIterator() noexcept = default;
  BaseIterator(deque_ptr ptr, size_t index) noexcept;

  BaseIterator(const BaseIterator& other) noexcept = default;
  BaseIterator& operator=(const BaseIterator& other) noexcept = default;

  pointer operator->() const noexcept { return current_; }
  reference operator*() const noexcept { return *current_; }

  BaseIterator& operator++() noexcept;
  BaseIterator operator++(int) noexcept;
  BaseIterator& operator--() noexcept;
  BaseIterator operator--(int) noexcept;

  BaseIterator& operator+=(difference_type count) noexcept;
  BaseIterator& operator-=(difference_type count) noexcept;

  BaseIterator operator+(difference_type count) const noexcept {
    return BaseIterator(deque_ptr_, index_ + count);
  }
  BaseIterator operator-(difference_type count) const noexcept {
    return BaseIterator(deque_ptr_, index_ - count);
  }

  difference_type operator-(const BaseIterator& other) const noexcept {
    return static_cast<difference_type>(index_ - other.index_);
  }

  bool operator==(const BaseIterator& other) const noexcept {
    return deque_ptr_ == other.deque_ptr_ && index_ == other.index_;
  }
  bool operator!=(const BaseIterator& other) const noexcept {
    return deque_ptr_ != other.deque_ptr_ || index_ != other.index_;
  }
  bool operator>=(const BaseIterator& other) const noexcept {
    return deque_ptr_ == other.deque_ptr_ && index_ >= other.index_;
  }
  bool operator>(const BaseIterator& other) const noexcept {
    return deque_ptr_ == other.deque_ptr_ && index_ > other.index_;
  }
  bool operator<=(const BaseIterator& other) const noexcept {
    return deque_ptr_ == other.deque_ptr_ && index_ <= other.index_;
  }
  bool operator<(const BaseIterator& other) const noexcept {
    return deque_ptr_ == other.deque_ptr_ && index_ < other.index_;
  }

  operator BaseIterator<true>() const noexcept {
    return BaseIterator<true>(deque_ptr_, index_);
  }

 private:
  void update_current() noexcept;

  deque_ptr deque_ptr_ = nullptr;
  Bucket* bucket_ = nullptr;
  size_t element_index_ = 0;
  pointer current_ = nullptr;
  size_t index_ = 0;
};

template <typename T>
template <bool IsConst>
void Deque<T>::BaseIterator<IsConst>::update_current() noexcept {
  if (bucket_ == nullptr || element_index_ == bucket_->end_index) {
    current_ = nullptr;
  } else {
    current_ = &(*bucket_)[element_index_];
  }
}

template <typename T>
template <bool IsConst>
Deque<T>::BaseIterator<IsConst>::BaseIterator(deque_ptr ptr,
                                              size_t index) noexcept
    : deque_ptr_(ptr), index_(index) {
  if (deque_ptr_ == nullptr || deque_ptr_->empty()) {
    return;
  }

  if (index == deque_ptr_->size()) {
    size_t bucket_index = deque_ptr_->find_last_bucket_index();
    bucket_ = deque_ptr_->data_[bucket_index];
    element_index_ = bucket_->end_index;
    return;
  }

  size_t bucket_index = deque_ptr_->find_bucket_index(index_);
  bucket_ = deque_ptr_->data_[bucket_index];
  element_index_ = deque_ptr_->find_element_index(index_);
  update_current();
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template BaseIterator<IsConst>&
Deque<T>::BaseIterator<IsConst>::operator++() noexcept {
  ++element_index_;
  ++index_;

  if (element_index_ == bucket_->end_index && bucket_->next != nullptr) {
    bucket_ = bucket_->next;
    element_index_ = bucket_->start_index;
  }

  update_current();

  return *this;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template BaseIterator<IsConst>
Deque<T>::BaseIterator<IsConst>::operator++(int) noexcept {
  BaseIterator tmp(*this);
  ++(*this);
  return tmp;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template BaseIterator<IsConst>&
Deque<T>::BaseIterator<IsConst>::operator--() noexcept {
  --index_;

  if (element_index_ == bucket_->start_index && bucket_->prev != nullptr) {
    bucket_ = bucket_->prev;
    element_index_ = bucket_->end_index;
  }

  --element_index_;

  update_current();

  return *this;
}

template <typename T>
template <bool IsConst>
typename Deque<T>::template BaseIterator<IsConst>
Deque<T>::BaseIterator<IsConst>::operator--(int) noexcept {
  BaseIterator tmp(*this);
  --(*this);
  return tmp;
}

template <typename T>
Deque<T>::Bucket::Bucket(size_t count)
    : start_index(0),
      end_index(count),
      elements(static_cast<T*>(::operator new(sizeof(T) * kBucketSize))) {
  size_t index = start_index;

  try {
    for (; index < end_index; ++index) {
      new (elements + index) T();
    }
  } catch (...) {
    for (size_t j = start_index; j < index; ++j) {
      (elements + j)->~T();
    }
    ::operator delete(elements);
    throw;
  }
}

template <typename T>
Deque<T>::Bucket::Bucket(size_t count, const T& value)
    : start_index(0),
      end_index(count),
      elements(static_cast<T*>(::operator new(sizeof(T) * kBucketSize))) {
  size_t index = start_index;

  try {
    for (; index < end_index; ++index) {
      new (elements + index) T(value);
    }
  } catch (...) {
    for (size_t j = start_index; j < index; ++j) {
      (elements + j)->~T();
    }
    ::operator delete(elements);
    throw;
  }
}

template <typename T>
Deque<T>::Bucket::Bucket(const Bucket& other)
    : start_index(other.start_index),
      end_index(other.end_index),
      elements(static_cast<T*>(::operator new(sizeof(T) * kBucketSize))) {
  size_t index = start_index;

  try {
    for (; index < end_index; ++index) {
      new (elements + index) T(other[index]);
    }
  } catch (...) {
    for (size_t j = start_index; j < index; ++j) {
      (elements + j)->~T();
    }
    ::operator delete(elements);
    throw;
  }
}

template <typename T>
Deque<T>::Bucket::~Bucket() {
  for (size_t index = start_index; index < end_index; ++index) {
    elements[index].~T();
  }
  ::operator delete(elements);
}

template <typename T>
void Deque<T>::Bucket::push_back(const T& value) {
  new (elements + end_index) T(value);
  ++end_index;
}

template <typename T>
void Deque<T>::Bucket::pop_back() {
  --end_index;
  (elements + end_index)->~T();
}

template <typename T>
void Deque<T>::Bucket::push_front(const T& value) {
  --start_index;
  new (elements + start_index) T(value);
}

template <typename T>
void Deque<T>::Bucket::pop_front() {
  (elements + start_index)->~T();
  ++start_index;
}

template <typename T>
Deque<T>::Deque(size_t count) : first_bucket_(0), start_index_(0) {
  bucket_count_ = (count + kBucketSize - 1) / kBucketSize;
  end_index_ = count - (bucket_count_ - 1) * kBucketSize;

  data_.resize(2 * bucket_count_, nullptr);

  size_t index = 0;

  try {
    for (; index < bucket_count_ - 1; ++index) {
      data_[index] = new Bucket(kBucketSize);
    }
    data_[index] = new Bucket(end_index_);
  } catch (...) {
    for (size_t j = 0; j < index; ++j) {
      delete data_[j];
    }
    throw;
  }

  for (size_t i = 0; i < bucket_count_; ++i) {
    if (i > 0) {
      data_[i]->prev = data_[i - 1];
    }
    if (i + 1 < bucket_count_) {
      data_[i]->next = data_[i + 1];
    }
  }
}

template <typename T>
Deque<T>::Deque(size_t count, const T& value)
    : first_bucket_(0), start_index_(0) {
  bucket_count_ = (count + kBucketSize - 1) / kBucketSize;
  end_index_ = count - (bucket_count_ - 1) * kBucketSize;

  data_.resize(2 * bucket_count_, nullptr);

  size_t index = 0;

  try {
    for (; index < bucket_count_ - 1; ++index) {
      data_[index] = new Bucket(kBucketSize, value);
    }
    data_[index] = new Bucket(end_index_, value);
  } catch (...) {
    for (size_t j = 0; j < index; ++j) {
      delete data_[j];
    }
    throw;
  }

  for (size_t i = 0; i < bucket_count_; ++i) {
    if (i > 0) {
      data_[i]->prev = data_[i - 1];
    }
    if (i + 1 < bucket_count_) {
      data_[i]->next = data_[i + 1];
    }
  }
}

template <typename T>
Deque<T>::Deque(const Deque& other)
    : bucket_count_(other.bucket_count_),
      first_bucket_(other.first_bucket_),
      start_index_(other.start_index_),
      end_index_(other.end_index_),
      data_(other.data_.size(), nullptr) {
  size_t copied = 0;

  try {
    for (; copied < bucket_count_; ++copied) {
      size_t index = (first_bucket_ + copied) % buckets_capacity();
      data_[index] = new Bucket(*other.data_[index]);
    }
  } catch (...) {
    for (size_t j = 0; j < copied; ++j) {
      size_t index = (first_bucket_ + j) % buckets_capacity();
      delete data_[index];
    }
    throw;
  }

  for (size_t i = 0; i < bucket_count_; ++i) {
    size_t current = (first_bucket_ + i) % buckets_capacity();

    if (i > 0) {
      size_t prev = (first_bucket_ + i - 1) % buckets_capacity();
      data_[current]->prev = data_[prev];
    }

    if (i + 1 < bucket_count_) {
      size_t next = (first_bucket_ + i + 1) % buckets_capacity();
      data_[current]->next = data_[next];
    }
  }
}

template <typename T>
Deque<T>& Deque<T>::operator=(const Deque<T>& other) {
  Deque tmp(other);
  swap(tmp);
  return *this;
}

template <typename T>
Deque<T>::~Deque() {
  for (Bucket* ptr : data_) {
    delete ptr;
  }
}

template <typename T>
size_t Deque<T>::size() const noexcept {
  if (bucket_count_ == 0) {
    return 0;
  }
  return ((bucket_count_ - 1) * kBucketSize) - start_index_ + end_index_;
}

template <typename T>
T& Deque<T>::operator[](size_t index) noexcept {
  size_t bucket_index = find_bucket_index(index);
  size_t element_index = find_element_index(index);
  return (*data_[bucket_index])[element_index];
}

template <typename T>
const T& Deque<T>::operator[](size_t index) const noexcept {
  size_t bucket_index = find_bucket_index(index);
  size_t element_index = find_element_index(index);
  return (*data_[bucket_index])[element_index];
}

template <typename T>
T& Deque<T>::at(size_t index) {
  if (index >= size()) {
    throw std::out_of_range("Index out of range");
  }

  size_t bucket_index = find_bucket_index(index);
  size_t element_index = find_element_index(index);
  return (*data_[bucket_index])[element_index];
}

template <typename T>
const T& Deque<T>::at(size_t index) const {
  if (index >= size()) {
    throw std::out_of_range("Index out of range");
  }

  size_t bucket_index = find_bucket_index(index);
  size_t element_index = find_element_index(index);
  return (*data_[bucket_index])[element_index];
}

template <typename T>
void Deque<T>::push_back(const T& value) {
  if (empty() || end_index_ == kBucketSize) {
    if (bucket_count_ == buckets_capacity()) {
      reallocate();
    }

    size_t new_bucket = find_end_bucket_index();

    if (data_[new_bucket] == nullptr) {
      data_[new_bucket] = new Bucket();
    }

    Bucket* new_bucket_ptr = data_[new_bucket];

    if (!empty()) {
      Bucket* last_bucket = data_[find_last_bucket_index()];
      last_bucket->next = new_bucket_ptr;
      new_bucket_ptr->prev = last_bucket;
    } else {
      new_bucket_ptr->prev = nullptr;
    }

    new_bucket_ptr->next = nullptr;

    new_bucket_ptr->push_back(value);

    ++bucket_count_;
    end_index_ = 1;

    return;
  }

  size_t last_bucket = find_last_bucket_index();
  data_[last_bucket]->push_back(value);
  ++end_index_;
}

template <typename T>
void Deque<T>::pop_back() {
  size_t last_bucket = find_last_bucket_index();
  data_[last_bucket]->pop_back();
  --end_index_;

  if (end_index_ == 0) {
    Bucket* last_bucket_ptr = data_[last_bucket];
    if (last_bucket_ptr->prev != nullptr) {
      last_bucket_ptr->prev->next = nullptr;
    }
    last_bucket_ptr->next = nullptr;
    last_bucket_ptr->prev = nullptr;

    --bucket_count_;

    if (bucket_count_ == 0) {
      start_index_ = 0;
      end_index_ = 0;
      first_bucket_ = 0;
    } else {
      end_index_ = kBucketSize;
    }
  }
}

template <typename T>
void Deque<T>::push_front(const T& value) {
  if (empty() || start_index_ == 0) {
    if (bucket_count_ == buckets_capacity()) {
      reallocate();
    }

    size_t new_bucket = find_begin_bucket_index();

    if (data_[new_bucket] == nullptr) {
      data_[new_bucket] = new Bucket();
    }

    Bucket* new_block_ptr = data_[new_bucket];

    if (!empty()) {
      Bucket* first_bucket = data_[first_bucket_];
      first_bucket->prev = new_block_ptr;
      new_block_ptr->next = first_bucket;
    } else {
      new_block_ptr->next = nullptr;
    }

    new_block_ptr->prev = nullptr;

    data_[new_bucket]->start_index = kBucketSize;
    data_[new_bucket]->end_index = kBucketSize;
    data_[new_bucket]->push_front(value);

    ++bucket_count_;
    start_index_ = kBucketSize - 1;
    first_bucket_ = new_bucket;

    return;
  }

  data_[first_bucket_]->push_front(value);
  --start_index_;
}

template <typename T>
void Deque<T>::pop_front() {
  data_[first_bucket_]->pop_front();
  ++start_index_;

  if (start_index_ == kBucketSize) {
    Bucket* first_bucket_ptr = data_[first_bucket_];
    if (first_bucket_ptr->next != nullptr) {
      first_bucket_ptr->next->prev = nullptr;
    }
    first_bucket_ptr->next = nullptr;
    first_bucket_ptr->prev = nullptr;

    data_[first_bucket_]->start_index = 0;
    data_[first_bucket_]->end_index = 0;
    --bucket_count_;

    if (bucket_count_ == 0) {
      first_bucket_ = 0;
      start_index_ = 0;
      end_index_ = 0;
    } else {
      first_bucket_ = (first_bucket_ + 1) % buckets_capacity();
      start_index_ = 0;
    }
  }
}

template <typename T>
Deque<T>::iterator Deque<T>::insert(const_iterator iter, const T& value) {
  size_t insert_index = static_cast<size_t>(iter - cbegin());
  size_t size_n = size();

  if (insert_index == size_n) {
    push_back(value);
    return end() - 1;
  }

  if (insert_index == 0) {
    push_front(value);
    return begin();
  }

  if (insert_index < size_n - insert_index) {
    push_front((*this)[0]);

    for (size_t index = 0; index < insert_index; ++index) {
      std::swap((*this)[index], (*this)[index + 1]);
    }

  } else {
    push_back((*this)[size_n - 1]);

    for (size_t index = size_n; index > insert_index; --index) {
      std::swap((*this)[index], (*this)[index - 1]);
    }
  }

  (*this)[insert_index] = value;

  return begin() + insert_index;
}

template <typename T>
Deque<T>::iterator Deque<T>::erase(const_iterator iter) {
  size_t erase_index = static_cast<size_t>(iter - cbegin());
  size_t size_n = size();

  if (erase_index == size_n - 1) {
    pop_back();
    return end();
  }

  if (erase_index == 0) {
    pop_front();
    return begin();
  }

  if (erase_index < size_n - erase_index) {
    for (size_t index = erase_index; index > 0; --index) {
      std::swap((*this)[index], (*this)[index - 1]);
    }

    pop_front();

  } else {
    for (size_t index = erase_index; index < size_n - 1; ++index) {
      std::swap((*this)[index], (*this)[index + 1]);
    }

    pop_back();
  }

  return begin() + erase_index;
}

template <typename T>
void Deque<T>::swap(Deque& other) noexcept {
  std::swap(bucket_count_, other.bucket_count_);
  std::swap(first_bucket_, other.first_bucket_);
  std::swap(start_index_, other.start_index_);
  std::swap(end_index_, other.end_index_);
  std::swap(data_, other.data_);
}

template <typename T>
void Deque<T>::reallocate() {
  if (empty()) {
    data_.resize(2, nullptr);
    return;
  }

  std::vector<Bucket*> old_data(data_);
  data_.resize(2 * old_data.size(), nullptr);

  for (size_t i = 0; i < bucket_count_; ++i) {
    size_t old_index = (first_bucket_ + i) % old_data.size();
    data_[i] = old_data[old_index];
  }

  first_bucket_ = 0;

  for (size_t i = 0; i < bucket_count_; ++i) {
    data_[i]->prev = (i > 0) ? data_[i - 1] : nullptr;
    data_[i]->next = (i + 1 < bucket_count_) ? data_[i + 1] : nullptr;
  }
}