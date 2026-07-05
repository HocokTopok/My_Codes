#include <iostream>
#include <iterator>
#include <memory>
#include <type_traits>

template <typename T, typename Allocator = std::allocator<T>>
class List {
 public:
  List() : fake_node_(&fake_node_, &fake_node_), alloc_(Allocator()) {}

  explicit List(size_t count, const Allocator& alloc = Allocator());

  List(size_t count, const T& value, const Allocator& alloc = Allocator());

  List(std::initializer_list<T> init, const Allocator& alloc = Allocator());

  List(const List& other)
      : List(other, allocator_traits::select_on_container_copy_construction(
                        other.alloc_)) {}

  List& operator=(const List& other);

  ~List();

  T& front() noexcept { return static_cast<Node*>(fake_node_.right)->value; }

  const T& front() const noexcept {
    return static_cast<Node*>(fake_node_.right)->value;
  }

  T& back() noexcept { return static_cast<Node*>(fake_node_.left)->value; }

  const T& back() const noexcept {
    return static_cast<Node*>(fake_node_.left)->value;
  }

  bool empty() const noexcept { return fake_node_.right == &fake_node_; }

  size_t size() const noexcept { return size_; }

  void push_back(const T& value);

  void pop_back() noexcept;

  void push_front(const T& value);

  void pop_front() noexcept;

  void swap(List& other) noexcept;

 private:
  struct BaseNode {
    BaseNode(BaseNode* first_ptr, BaseNode* second_ptr)
        : left(first_ptr), right(second_ptr) {}

    BaseNode* left = nullptr;
    BaseNode* right = nullptr;
  };

  struct Node : BaseNode {
    Node() : BaseNode(nullptr, nullptr) {}
    Node(const T& value) : BaseNode(nullptr, nullptr), value(value) {}

    T value;
  };

  template <bool IsConst>
  class BaseIterator;

 public:
  using value_type = T;
  using allocator_type = Allocator;
  using iterator = BaseIterator<false>;
  using const_iterator = BaseIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  iterator begin() noexcept { return iterator(fake_node_.right); }

  const_iterator begin() const noexcept {
    return const_iterator(const_cast<BaseNode*>(fake_node_.right));
  }

  const_iterator cbegin() const noexcept { return begin(); }

  iterator end() noexcept { return iterator(&fake_node_); }

  const_iterator end() const noexcept {
    return const_iterator(const_cast<BaseNode*>(&fake_node_));
  }

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

  Allocator get_allocator() const noexcept { return alloc_; }

 private:
  using allocator_traits = std::allocator_traits<Allocator>;
  using node_allocator = allocator_traits::template rebind_alloc<Node>;
  using node_allocator_traits = std::allocator_traits<node_allocator>;

  Node* create_node();

  Node* create_node(const T& value);

  void delete_node(Node* temp_node);

  List(const List& other, const Allocator& alloc);

  BaseNode fake_node_;
  node_allocator alloc_;
  size_t size_ = 0;
};

template <typename T, typename Allocator>
template <bool IsConst>
class List<T, Allocator>::BaseIterator {
 public:
  using pointer = std::conditional_t<IsConst, const T*, T*>;
  using reference = std::conditional_t<IsConst, const T&, T&>;
  using value_type = T;
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;

  BaseIterator() = default;

  BaseIterator(BaseNode* ptr) : ptr_(ptr) {}

  BaseIterator(const BaseIterator& other) = default;

  BaseIterator& operator=(const BaseIterator& other) = default;

  pointer operator->() const noexcept {
    return &(static_cast<Node*>(ptr_)->value);
  }

  reference operator*() const noexcept {
    return static_cast<Node*>(ptr_)->value;
  }

  BaseIterator& operator++() noexcept;

  BaseIterator operator++(int) noexcept;

  BaseIterator& operator--() noexcept;

  BaseIterator operator--(int) noexcept;

  bool operator==(const BaseIterator& other) const noexcept {
    return ptr_ == other.ptr_;
  }

  bool operator!=(const BaseIterator& other) const noexcept {
    return ptr_ != other.ptr_;
  }

  operator BaseIterator<true>() const noexcept {
    return BaseIterator<true>(ptr_);
  }

 private:
  BaseNode* ptr_ = nullptr;
};

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template BaseIterator<IsConst>&
List<T, Allocator>::BaseIterator<IsConst>::operator++() noexcept {
  ptr_ = ptr_->right;
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template BaseIterator<IsConst>
List<T, Allocator>::BaseIterator<IsConst>::operator++(int) noexcept {
  BaseIterator tmp(*this);
  ptr_ = ptr_->right;
  return tmp;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template BaseIterator<IsConst>&
List<T, Allocator>::BaseIterator<IsConst>::operator--() noexcept {
  ptr_ = ptr_->left;
  return *this;
}

template <typename T, typename Allocator>
template <bool IsConst>
typename List<T, Allocator>::template BaseIterator<IsConst>
List<T, Allocator>::BaseIterator<IsConst>::operator--(int) noexcept {
  BaseIterator tmp(*this);
  ptr_ = ptr_->left;
  return tmp;
}

template <typename T, typename Allocator>
List<T, Allocator>::Node* List<T, Allocator>::create_node() {
  Node* new_node = node_allocator_traits::allocate(alloc_, 1);

  try {
    node_allocator_traits::construct(alloc_, new_node);
  } catch (...) {
    node_allocator_traits::deallocate(alloc_, new_node, 1);
    throw;
  }

  return new_node;
}

template <typename T, typename Allocator>
List<T, Allocator>::Node* List<T, Allocator>::create_node(const T& value) {
  Node* new_node = node_allocator_traits::allocate(alloc_, 1);

  try {
    node_allocator_traits::construct(alloc_, new_node, value);
  } catch (...) {
    node_allocator_traits::deallocate(alloc_, new_node, 1);
    throw;
  }

  return new_node;
}

template <typename T, typename Allocator>
void List<T, Allocator>::delete_node(Node* temp_node) {
  node_allocator_traits::destroy(alloc_, temp_node);
  node_allocator_traits::deallocate(alloc_, temp_node, 1);
}

template <typename T, typename Allocator>
void List<T, Allocator>::swap(List& other) noexcept {
  if (node_allocator_traits::propagate_on_container_swap::value) {
    std::swap(alloc_, other.alloc_);
  }

  std::swap(size_, other.size_);

  bool this_empty = empty();
  bool other_empty = other.empty();

  std::swap(fake_node_, other.fake_node_);

  if (other_empty) {
    fake_node_.left = &fake_node_;
    fake_node_.right = &fake_node_;
  } else {
    fake_node_.left->right = &fake_node_;
    fake_node_.right->left = &fake_node_;
  }

  if (this_empty) {
    other.fake_node_.left = &other.fake_node_;
    other.fake_node_.right = &other.fake_node_;
  } else {
    other.fake_node_.left->right = &other.fake_node_;
    other.fake_node_.right->left = &other.fake_node_;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const Allocator& alloc)
    : fake_node_(&fake_node_, &fake_node_), alloc_(alloc) {
  size_t index = 0;
  try {
    for (; index < count; ++index) {
      Node* new_node = create_node();
      ++size_;

      new_node->left = fake_node_.left;
      fake_node_.left->right = new_node;
      new_node->right = &fake_node_;
      fake_node_.left = new_node;
    }
  } catch (...) {
    for (size_t j = 0; j < index; ++j) {
      Node* temp_node = static_cast<Node*>(fake_node_.left);
      fake_node_.left = temp_node->left;
      delete_node(temp_node);
    }

    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const T& value, const Allocator& alloc)
    : fake_node_(&fake_node_, &fake_node_), alloc_(alloc) {
  size_t index = 0;
  try {
    for (; index < count; ++index) {
      Node* new_node = create_node(value);
      ++size_;

      new_node->left = fake_node_.left;
      fake_node_.left->right = new_node;
      new_node->right = &fake_node_;
      fake_node_.left = new_node;
    }
  } catch (...) {
    for (size_t j = 0; j < index; ++j) {
      Node* temp_node = static_cast<Node*>(fake_node_.left);
      fake_node_.left = temp_node->left;
      delete_node(temp_node);
    }

    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(std::initializer_list<T> init, const Allocator& alloc)
    : fake_node_(&fake_node_, &fake_node_), alloc_(alloc) {
  auto iter = init.begin();
  try {
    for (; iter != init.end(); ++iter) {
      Node* new_node = create_node(*iter);
      ++size_;

      new_node->left = fake_node_.left;
      fake_node_.left->right = new_node;
      new_node->right = &fake_node_;
      fake_node_.left = new_node;
    }
  } catch (...) {
    for (auto j = init.begin(); j != iter; ++j) {
      Node* temp_node = static_cast<Node*>(fake_node_.left);
      fake_node_.left = temp_node->left;
      delete_node(temp_node);
    }

    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const List& other, const Allocator& alloc)
    : fake_node_(&fake_node_, &fake_node_), alloc_(alloc), size_(other.size()) {
  BaseNode* current_node = other.fake_node_.right;
  try {
    while (current_node != &other.fake_node_) {
      Node* new_node = create_node(static_cast<Node*>(current_node)->value);

      new_node->left = fake_node_.left;
      fake_node_.left->right = new_node;
      new_node->right = &fake_node_;
      fake_node_.left = new_node;

      current_node = current_node->right;
    }
  } catch (...) {
    for (BaseNode* j = other.fake_node_.right; j != current_node;
         j = j->right) {
      Node* temp_node = static_cast<Node*>(fake_node_.left);
      fake_node_.left = temp_node->left;
      delete_node(temp_node);
    }

    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& other) {
  if (this == &other) {
    return *this;
  }

  if (node_allocator_traits::propagate_on_container_copy_assignment::value &&
      alloc_ != other.alloc_) {
    List tmp(other, other.alloc_);
    swap(tmp);

    if (!node_allocator_traits::propagate_on_container_swap::value) {
      std::swap(alloc_, tmp.alloc_);
    }

    return *this;
  }

  BaseNode* this_current_node = fake_node_.right;
  BaseNode* other_current_node = other.fake_node_.right;

  for (; this_current_node != &fake_node_ &&
         other_current_node != &other.fake_node_;
       this_current_node = this_current_node->right,
       other_current_node = other_current_node->right) {
    static_cast<Node*>(this_current_node)->value =
        static_cast<const Node*>(other_current_node)->value;
  }

  while (other_current_node != &other.fake_node_) {
    push_back(static_cast<const Node*>(other_current_node)->value);
    other_current_node = other_current_node->right;
  }

  size_t nodes_to_delete = 0;
  while (this_current_node != &fake_node_) {
    ++nodes_to_delete;
    this_current_node = this_current_node->right;
  }

  while (nodes_to_delete-- > 0) {
    pop_back();
  }

  if (node_allocator_traits::propagate_on_container_copy_assignment::value) {
    alloc_ = other.alloc_;
  }

  return *this;
}

template <typename T, typename Allocator>
List<T, Allocator>::~List() {
  BaseNode* current_node = fake_node_.left;

  while (current_node != &fake_node_) {
    current_node->left->right = &fake_node_;
    fake_node_.left = current_node->left;

    delete_node(static_cast<Node*>(current_node));

    current_node = fake_node_.left;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_back(const T& value) {
  Node* new_node = create_node(value);
  ++size_;

  new_node->left = fake_node_.left;
  fake_node_.left->right = new_node;
  new_node->right = &fake_node_;
  fake_node_.left = new_node;
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_back() noexcept {
  Node* temp_node = static_cast<Node*>(fake_node_.left);
  --size_;

  temp_node->left->right = &fake_node_;
  fake_node_.left = temp_node->left;

  delete_node(temp_node);
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& value) {
  Node* new_node = create_node(value);
  ++size_;

  new_node->right = fake_node_.right;
  fake_node_.right->left = new_node;
  new_node->left = &fake_node_;
  fake_node_.right = new_node;
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_front() noexcept {
  Node* temp_node = static_cast<Node*>(fake_node_.right);
  --size_;

  temp_node->right->left = &fake_node_;
  fake_node_.right = temp_node->right;

  delete_node(temp_node);
}