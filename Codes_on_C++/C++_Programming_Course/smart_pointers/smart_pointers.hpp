#include <memory>

namespace details {

class BaseControlBlock {
 public:
  BaseControlBlock() = default;

  virtual ~BaseControlBlock() = default;

  virtual void* get_pointer() noexcept = 0;

  virtual void destroy_object() noexcept = 0;

  virtual void destroy_block() noexcept = 0;

  void increase_shared() noexcept { ++shared_count_; }

  void decrease_shared() noexcept { --shared_count_; }

  void increase_weak() noexcept { ++weak_count_; }

  void decrease_weak() noexcept { --weak_count_; }

  void release_shared() noexcept;

  void release_weak() noexcept;

  size_t shared_count() const noexcept { return shared_count_; }

  size_t weak_count() const noexcept { return weak_count_; }

 protected:
  size_t shared_count_ = 1;
  size_t weak_count_ = 0;
};

template <typename T, typename Deleter, typename Allocator>
class PointerControlBlock : public BaseControlBlock {
 public:
  using AllocTraits = std::allocator_traits<Allocator>;
  using Block = PointerControlBlock<T, Deleter, Allocator>;
  using BlockAllocator = AllocTraits::template rebind_alloc<Block>;
  using BlockTraits = std::allocator_traits<BlockAllocator>;

  PointerControlBlock(T* ptr, Deleter deleter, Allocator alloc)
      : ptr_(ptr), deleter_(std::move(deleter)), alloc_(std::move(alloc)) {}

  void destroy_object() noexcept override { deleter_(ptr_); }

  void destroy_block() noexcept override;

  T* get_t_pointer() noexcept { return ptr_; }

  void* get_pointer() noexcept override { return get_t_pointer(); }

 private:
  T* ptr_;
  [[no_unique_address]] Deleter deleter_;
  [[no_unique_address]] Allocator alloc_;
};

template <typename T, typename Allocator>
class AllocateSharedControlBlock : public BaseControlBlock {
 public:
  using AllocTraits = std::allocator_traits<Allocator>;
  using Block = AllocateSharedControlBlock<T, Allocator>;
  using BlockAllocator = AllocTraits::template rebind_alloc<Block>;
  using BlockTraits = std::allocator_traits<BlockAllocator>;

  template <typename... Args>
  AllocateSharedControlBlock(Allocator alloc, Args&&... args)
      : alloc_(std::move(alloc)) {
    AllocTraits::construct(alloc_, get_t_pointer(),
                           std::forward<Args>(args)...);
  }

  void destroy_object() noexcept override {
    AllocTraits::destroy(alloc_, get_t_pointer());
  }

  void destroy_block() noexcept override;

  T* get_t_pointer() noexcept { return reinterpret_cast<T*>(&storage_); }

  void* get_pointer() noexcept override { return get_t_pointer(); }

 private:
  [[no_unique_address]] Allocator alloc_;
  alignas(T) std::byte storage_[sizeof(T)];
};

}  // namespace details

template <typename T>
class SharedPtr {
 public:
  SharedPtr() = default;

  ~SharedPtr();

  SharedPtr(std::nullptr_t) noexcept {}

  SharedPtr& operator=(std::nullptr_t) noexcept;

  template <typename U>
  SharedPtr(U* ptr);

  template <typename U, typename Deleter>
  SharedPtr(U* ptr, Deleter deleter);

  template <typename U, typename Deleter, typename Allocator>
  SharedPtr(U* ptr, Deleter deleter, Allocator alloc);

  SharedPtr(const SharedPtr& other);

  SharedPtr(SharedPtr&& other);

  SharedPtr& operator=(const SharedPtr& other);

  SharedPtr& operator=(SharedPtr&& other);

  template <typename U>
  SharedPtr(const SharedPtr<U>& other);

  template <typename U>
  SharedPtr(SharedPtr<U>&& other);

  template <typename U>
  SharedPtr& operator=(const SharedPtr<U>& other);

  template <typename U>
  SharedPtr& operator=(SharedPtr<U>&& other);

  size_t use_count() const noexcept {
    return block_ == nullptr ? 0 : block_->shared_count();
  }

  T* get() const noexcept { return ptr_; }

  T& operator*() const noexcept { return *ptr_; }

  T* operator->() const noexcept { return ptr_; }

  void reset() noexcept;

 private:
  template <typename U, typename Deleter = std::default_delete<U>,
            typename Allocator = std::allocator<U>>
  static details::PointerControlBlock<U, Deleter, Allocator>*
  create_pointer_block(U* ptr, Deleter deleter = Deleter(),
                       Allocator alloc = Allocator());

  SharedPtr(T* ptr, details::BaseControlBlock* block_ptr)
      : ptr_(ptr), block_(block_ptr) {}

  void increase_shared() const noexcept;

  void decrease_shared() noexcept;

  template <typename U>
  friend class WeakPtr;

  template <typename U, typename... Args>
  friend SharedPtr<U> MakeShared(Args&&... args);

  template <typename U, typename Allocator, typename... Args>
  friend SharedPtr<U> AllocateShared(const Allocator& alloc, Args&&... args);

  template <typename U>
  friend class SharedPtr;

  T* ptr_ = nullptr;
  details::BaseControlBlock* block_ = nullptr;
};

template <typename T>
class WeakPtr {
 public:
  WeakPtr() = default;

  ~WeakPtr() { decrease_weak(); }

  WeakPtr(const WeakPtr& other) : block_(other.block_) { increase_weak(); }

  WeakPtr(WeakPtr&& other);

  WeakPtr& operator=(const WeakPtr& other);

  WeakPtr& operator=(WeakPtr&& other);

  WeakPtr(const SharedPtr<T>& other) : block_(other.block_) { increase_weak(); }

  WeakPtr& operator=(const SharedPtr<T>& other);

  bool expired() const noexcept {
    return block_ == nullptr || block_->shared_count() == 0;
  }

  SharedPtr<T> lock();

 private:
  void increase_weak() const noexcept;

  void decrease_weak() noexcept;

  void increase_shared() const noexcept;

  details::BaseControlBlock* block_ = nullptr;
};

template <typename T, typename Allocator, typename... Args>
SharedPtr<T> AllocateShared(const Allocator& alloc, Args&&... args);

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
  return AllocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}

void details::BaseControlBlock::release_shared() noexcept {
  decrease_shared();

  if (shared_count() == 0) {
    destroy_object();

    if (weak_count() == 0) {
      destroy_block();
    }
  }
}

void details::BaseControlBlock::release_weak() noexcept {
  decrease_weak();

  if (shared_count() == 0 && weak_count() == 0) {
    destroy_block();
  }
}

template <typename T, typename Deleter, typename Allocator>
void details::PointerControlBlock<T, Deleter,
                                  Allocator>::destroy_block() noexcept {
  BlockAllocator block_alloc(alloc_);

  this->~PointerControlBlock();
  BlockTraits::deallocate(block_alloc, this, 1);
}

template <typename T, typename Allocator>
void details::AllocateSharedControlBlock<T,
                                         Allocator>::destroy_block() noexcept {
  BlockAllocator block_alloc(alloc_);

  this->~AllocateSharedControlBlock();
  BlockTraits::deallocate(block_alloc, this, 1);
}

template <typename T>
template <typename U, typename Deleter, typename Allocator>
details::PointerControlBlock<U, Deleter, Allocator>*
SharedPtr<T>::create_pointer_block(U* ptr, Deleter deleter, Allocator alloc) {
  using AllocTraits = std::allocator_traits<Allocator>;
  using Block = details::PointerControlBlock<U, Deleter, Allocator>;
  using BlockAllocator = AllocTraits::template rebind_alloc<Block>;
  using BlockTraits = std::allocator_traits<BlockAllocator>;

  BlockAllocator block_alloc(alloc);

  Block* block_ptr = BlockTraits::allocate(block_alloc, 1);
  try {
    BlockTraits::construct(block_alloc, block_ptr, ptr, deleter, alloc);
  } catch (...) {
    BlockTraits::deallocate(block_alloc, block_ptr, 1);
    throw;
  }

  return block_ptr;
}

template <typename T>
void SharedPtr<T>::increase_shared() const noexcept {
  if (block_ != nullptr) {
    block_->increase_shared();
  }
}

template <typename T>
void SharedPtr<T>::decrease_shared() noexcept {
  if (block_ == nullptr) {
    return;
  }

  details::BaseControlBlock* old_block = block_;

  block_ = nullptr;
  ptr_ = nullptr;

  old_block->release_shared();
}

template <typename T>
SharedPtr<T>::~SharedPtr() {
  decrease_shared();
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(std::nullptr_t) noexcept {
  reset();
  return *this;
}

template <typename T>
template <typename U>
SharedPtr<T>::SharedPtr(U* ptr) : ptr_(ptr) {
  if (ptr_ != nullptr) {
    block_ = create_pointer_block(ptr);
  }
}

template <typename T>
template <typename U, typename Deleter>
SharedPtr<T>::SharedPtr(U* ptr, Deleter deleter) : ptr_(ptr) {
  if (ptr_ != nullptr) {
    block_ = create_pointer_block(ptr, std::move(deleter));
  }
}

template <typename T>
template <typename U, typename Deleter, typename Allocator>
SharedPtr<T>::SharedPtr(U* ptr, Deleter deleter, Allocator alloc) : ptr_(ptr) {
  if (ptr_ != nullptr) {
    block_ = create_pointer_block(ptr, std::move(deleter), std::move(alloc));
  }
}

template <typename T>
SharedPtr<T>::SharedPtr(const SharedPtr& other)
    : ptr_(other.ptr_), block_(other.block_) {
  increase_shared();
}

template <typename T>
SharedPtr<T>::SharedPtr(SharedPtr&& other)
    : ptr_(other.ptr_), block_(other.block_) {
  other.ptr_ = nullptr;
  other.block_ = nullptr;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr& other) {
  if (this == &other) {
    return *this;
  }

  decrease_shared();

  ptr_ = other.ptr_;
  block_ = other.block_;

  increase_shared();

  return *this;
}

template <typename T>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr&& other) {
  if (this == &other) {
    return *this;
  }

  decrease_shared();

  ptr_ = other.ptr_;
  block_ = other.block_;

  other.ptr_ = nullptr;
  other.block_ = nullptr;

  return *this;
}

template <typename T>
template <typename U>
SharedPtr<T>::SharedPtr(const SharedPtr<U>& other)
    : ptr_(other.ptr_), block_(other.block_) {
  increase_shared();
}

template <typename T>
template <typename U>
SharedPtr<T>::SharedPtr(SharedPtr<U>&& other)
    : ptr_(other.ptr_), block_(other.block_) {
  other.ptr_ = nullptr;
  other.block_ = nullptr;
}

template <typename T>
template <typename U>
SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<U>& other) {
  if constexpr (std::is_same_v<T, U>) {
    if (this == &other) {
      return *this;
    }
  }

  decrease_shared();

  ptr_ = other.ptr_;
  block_ = other.block_;

  increase_shared();

  return *this;
}

template <typename T>
template <typename U>
SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<U>&& other) {
  if constexpr (std::is_same_v<T, U>) {
    if (this == &other) {
      return *this;
    }
  }

  decrease_shared();

  ptr_ = other.ptr_;
  block_ = other.block_;

  other.ptr_ = nullptr;
  other.block_ = nullptr;

  return *this;
}

template <typename T>
void SharedPtr<T>::reset() noexcept {
  decrease_shared();
}

template <typename T>
void WeakPtr<T>::increase_weak() const noexcept {
  if (block_ != nullptr) {
    block_->increase_weak();
  }
}

template <typename T>
void WeakPtr<T>::decrease_weak() noexcept {
  if (block_ == nullptr) {
    return;
  }

  details::BaseControlBlock* old_block = block_;

  block_ = nullptr;

  old_block->release_weak();
}

template <typename T>
void WeakPtr<T>::increase_shared() const noexcept {
  if (block_ != nullptr) {
    block_->increase_shared();
  }
}

template <typename T>
WeakPtr<T>::WeakPtr(WeakPtr<T>&& other) : block_(other.block_) {
  other.block_ = nullptr;
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr<T>& other) {
  if (this == &other) {
    return *this;
  }

  decrease_weak();

  block_ = other.block_;

  increase_weak();

  return *this;
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr<T>&& other) {
  if (this == &other) {
    return *this;
  }

  decrease_weak();

  block_ = other.block_;

  other.block_ = nullptr;

  return *this;
}

template <typename T>
WeakPtr<T>& WeakPtr<T>::operator=(const SharedPtr<T>& other) {
  decrease_weak();

  block_ = other.block_;

  increase_weak();

  return *this;
}

template <typename T>
SharedPtr<T> WeakPtr<T>::lock() {
  if (expired()) {
    return SharedPtr<T>();
  }

  increase_shared();
  return SharedPtr<T>(static_cast<T*>(block_->get_pointer()), block_);
}

template <typename T, typename Allocator, typename... Args>
SharedPtr<T> AllocateShared(const Allocator& alloc, Args&&... args) {
  using AllocTraits = std::allocator_traits<Allocator>;
  using Block = details::AllocateSharedControlBlock<T, Allocator>;
  using BlockAllocator = AllocTraits::template rebind_alloc<Block>;
  using BlockTraits = std::allocator_traits<BlockAllocator>;

  BlockAllocator block_alloc(alloc);

  Block* block_ptr = BlockTraits::allocate(block_alloc, 1);
  try {
    ::new (static_cast<void*>(block_ptr))
        Block(alloc, std::forward<Args>(args)...);
  } catch (...) {
    BlockTraits::deallocate(block_alloc, block_ptr, 1);
    throw;
  }

  return SharedPtr<T>(block_ptr->get_t_pointer(),
                      static_cast<details::BaseControlBlock*>(block_ptr));
}
