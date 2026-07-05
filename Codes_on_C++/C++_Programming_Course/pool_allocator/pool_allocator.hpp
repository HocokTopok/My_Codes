#include <algorithm>
#include <array>
#include <bit>
#include <exception>
#include <memory_resource>
#include <string>
#include <vector>

namespace constants {
static constexpr size_t kMinBlockSize = 8;
static constexpr size_t kMinBlockLog2 = 3;
static constexpr size_t kNoPool = std::numeric_limits<size_t>::max();
}  // namespace constants

class DeallocateError : public std::exception {
 public:
  explicit DeallocateError(
      const std::string kMsg = "Error: Failed to deallocate")
      : msg_(kMsg) {};

  const char* what() const noexcept override { return msg_.c_str(); }

 private:
  std::string msg_;
};

class DeallocateAlignmentError : public DeallocateError {
 public:
  DeallocateAlignmentError(
      const std::string kMsg =
          "Error: pointer is not aligned to the requested alignment")
      : DeallocateError(kMsg) {}
};

class DeallocateDoubleFreeError : public DeallocateError {
 public:
  DeallocateDoubleFreeError(
      const std::string kMsg = "Error: double free or block is not allocated")
      : DeallocateError(kMsg) {}
};

class DeallocateSizeMismatchError : public DeallocateError {
 public:
  DeallocateSizeMismatchError(
      const std::string kMsg =
          "Error: allocated size is not equal to deallocated size")
      : DeallocateError(kMsg) {}
};

template <std::pmr::pool_options pool_options>
class PoolAllocator : public std::pmr::memory_resource {
 public:
  PoolAllocator(
      std::pmr::memory_resource* upstream = std::pmr::get_default_resource());

  ~PoolAllocator();

  PoolAllocator(const PoolAllocator& other) = delete;

  PoolAllocator& operator=(const PoolAllocator& other) = delete;

  PoolAllocator(PoolAllocator&& other) = delete;

  PoolAllocator& operator=(PoolAllocator&& other) = delete;

 private:
  static constexpr size_t CountPools();

  static constexpr size_t kCountPools = CountPools();

  struct BaseHeader {
    size_t pool_index = constants::kNoPool;
    bool is_used = false;
    size_t size_used = 0;
  };

  struct PoolHeader {
    BaseHeader base;
  };

  struct UpstreamHeader {
    size_t alignment_used = 0;
    UpstreamHeader* next = nullptr;
    UpstreamHeader* prev = nullptr;
    BaseHeader base;
  };

  class Pool {
   public:
    Pool() = default;

    Pool(std::pmr::memory_resource* upstream, size_t block_size,
         size_t pool_index)
        : upstream_(upstream),
          block_size_(block_size),
          pool_index_(pool_index) {}

    ~Pool();

    struct FreeBlock {
      FreeBlock* next = nullptr;
    };

    struct ChunkHeader {
      ChunkHeader* next;
      size_t total_bytes;
    };

    void* GetEmptyBlock(size_t bytes);

    void ReturnBlock(void* ptr);

    void AddChunk();

   private:
    void AddNewBlocks(void* new_chunk, size_t full_block_size,
                      size_t pool_index);

    std::pmr::memory_resource* upstream_;
    ChunkHeader* chunks_head_ = nullptr;
    FreeBlock* free_list_ = nullptr;
    size_t block_size_;
    size_t pool_index_;
  };

  void* do_allocate(size_t bytes, size_t alignment);

  void do_deallocate(void* ptr, size_t bytes, size_t alignment);

  bool do_is_equal(const std::pmr::memory_resource& other) const noexcept {
    return this == &other;
  }

  std::pmr::memory_resource* upstream_;
  UpstreamHeader* upstream_head_ = nullptr;
  std::array<Pool, kCountPools> pools_;
};

template <std::pmr::pool_options pool_options>
constexpr size_t PoolAllocator<pool_options>::CountPools() {
  size_t pool_count = 1;
  size_t size_of_block = constants::kMinBlockSize;

  while (size_of_block != pool_options.largest_required_pool_block) {
    ++pool_count;
    size_of_block *= 2;
  }

  return pool_count;
}

template <std::pmr::pool_options pool_options>
PoolAllocator<pool_options>::Pool::~Pool() {
  ChunkHeader* node = chunks_head_;

  while (node != nullptr) {
    ChunkHeader* next = node->next;
    upstream_->deallocate(node, node->total_bytes, alignof(void*));
    node = next;
  }
}

template <std::pmr::pool_options pool_options>
void PoolAllocator<pool_options>::Pool::AddNewBlocks(void* new_chunk,
                                                     size_t full_block_size,
                                                     size_t pool_index) {
  std::byte* blocks_start = static_cast<std::byte*>(new_chunk);

  for (size_t index = 0; index < pool_options.max_blocks_per_chunk; ++index) {
    std::byte* mem = blocks_start + (index * full_block_size);

    PoolHeader* header = reinterpret_cast<PoolHeader*>(mem);
    header->base.is_used = false;
    header->base.pool_index = pool_index;

    FreeBlock* new_block =
        reinterpret_cast<FreeBlock*>(mem + sizeof(PoolHeader));
    new_block->next = free_list_;
    free_list_ = new_block;
  }
}

template <std::pmr::pool_options pool_options>
void PoolAllocator<pool_options>::Pool::AddChunk() {
  size_t total =
      sizeof(ChunkHeader) + (block_size_ * pool_options.max_blocks_per_chunk);
  void* mem = upstream_->allocate(total, alignof(void*));

  ChunkHeader* node = reinterpret_cast<ChunkHeader*>(mem);
  node->total_bytes = total;
  node->next = chunks_head_;
  chunks_head_ = node;

  void* blocks_start = reinterpret_cast<std::byte*>(mem) + sizeof(ChunkHeader);
  AddNewBlocks(blocks_start, block_size_, pool_index_);
}

template <std::pmr::pool_options pool_options>
void* PoolAllocator<pool_options>::Pool::GetEmptyBlock(size_t bytes) {
  if (free_list_ == nullptr) {
    AddChunk();
  }

  FreeBlock* free_block = free_list_;
  free_list_ = free_block->next;

  PoolHeader* header = reinterpret_cast<PoolHeader*>(
      reinterpret_cast<std::byte*>(free_block) - sizeof(PoolHeader));
  header->base.is_used = true;
  header->base.size_used = bytes;

  return free_block;
}

template <std::pmr::pool_options pool_options>
void PoolAllocator<pool_options>::Pool::ReturnBlock(void* ptr) {
  PoolHeader* header = reinterpret_cast<PoolHeader*>(
      reinterpret_cast<std::byte*>(ptr) - sizeof(PoolHeader));
  header->base.is_used = false;

  FreeBlock* free_block = reinterpret_cast<FreeBlock*>(ptr);
  free_block->next = free_list_;
  free_list_ = free_block;
}

template <std::pmr::pool_options pool_options>
PoolAllocator<pool_options>::PoolAllocator(std::pmr::memory_resource* upstream)
    : upstream_(upstream) {
  size_t size_of_block = constants::kMinBlockSize;

  for (size_t pool_index = 0; pool_index < kCountPools; ++pool_index) {
    pools_[pool_index] =
        Pool(upstream, size_of_block + sizeof(PoolHeader), pool_index);
    size_of_block *= 2;
  }
}

template <std::pmr::pool_options pool_options>
PoolAllocator<pool_options>::~PoolAllocator() {
  UpstreamHeader* node = upstream_head_;
  while (node != nullptr) {
    UpstreamHeader* next = node->next;
    upstream_->deallocate(node, node->base.size_used + sizeof(UpstreamHeader),
                          node->alignment_used);
    node = next;
  }
}

template <std::pmr::pool_options pool_options>
void* PoolAllocator<pool_options>::do_allocate(size_t bytes, size_t alignment) {
  size_t need_bytes = std::max(bytes, alignment);
  size_t block_size =
      std::max(std::bit_ceil(need_bytes), constants::kMinBlockSize);
  size_t pool_index = std::countr_zero(block_size) - constants::kMinBlockLog2;

  if (pool_index < kCountPools) {
    return pools_[pool_index].GetEmptyBlock(bytes);
  }

  size_t min_alignment = std::max(alignment, alignof(UpstreamHeader));

  void* mem =
      upstream_->allocate(bytes + sizeof(UpstreamHeader), min_alignment);
  UpstreamHeader* header = reinterpret_cast<UpstreamHeader*>(mem);
  header->base.is_used = true;
  header->base.size_used = bytes;
  header->alignment_used = min_alignment;
  header->next = upstream_head_;
  header->prev = nullptr;
  if (upstream_head_) {
    upstream_head_->prev = header;
  }
  upstream_head_ = header;

  return static_cast<std::byte*>(mem) + sizeof(UpstreamHeader);
}

template <std::pmr::pool_options pool_options>
void PoolAllocator<pool_options>::do_deallocate(void* ptr, size_t bytes,
                                                size_t alignment) {
  if (reinterpret_cast<std::uintptr_t>(ptr) % alignment != 0) {
    throw DeallocateAlignmentError();
  }
  BaseHeader* base_header = reinterpret_cast<BaseHeader*>(
      static_cast<std::byte*>(ptr) - sizeof(BaseHeader));
  if (!base_header->is_used) {
    throw DeallocateDoubleFreeError();
  }
  if (base_header->size_used != bytes) {
    throw DeallocateSizeMismatchError();
  }
  if (base_header->pool_index < kCountPools) {
    pools_[base_header->pool_index].ReturnBlock(ptr);
    return;
  }
  UpstreamHeader* upstream_header = reinterpret_cast<UpstreamHeader*>(
      static_cast<std::byte*>(ptr) - sizeof(UpstreamHeader));
  if (upstream_header->prev != nullptr) {
    upstream_header->prev->next = upstream_header->next;
  } else {
    upstream_head_ = upstream_header->next;
  }
  if (upstream_header->next != nullptr) {
    upstream_header->next->prev = upstream_header->prev;
  }
  upstream_->deallocate(upstream_header, bytes + sizeof(UpstreamHeader),
                        upstream_header->alignment_used);
}