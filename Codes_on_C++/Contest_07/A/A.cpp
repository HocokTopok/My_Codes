#include <algorithm>
#include <iostream>
#include <vector>

class DSU {
 public:
  DSU(std::vector<size_t> values) : roots_(values.size()), max_value_(0) {
    for (size_t i = 0; i < values.size(); ++i) {
      roots_[i].value = values[i];
      roots_[i].ptr = &roots_[i];
      max_value_ = std::max(values[i], max_value_);
    }
  }

  size_t GetLeader(size_t index) noexcept {
    return GetLeader(&roots_[index - 1])->value;
  }

  void Merge(size_t dst, size_t src) {
    Node* dst_root = GetLeader(&roots_[dst - 1]);
    Node* src_root = GetLeader(&roots_[src - 1]);

    if (dst_root == src_root) {
      return;
    }

    dst_root->value += src_root->value;
    src_root->value = 0;
    src_root->ptr = dst_root;

    max_value_ = std::max(dst_root->value, max_value_);
  }

  size_t GetMaxValue() const noexcept { return max_value_; }

 private:
  struct Node {
    Node* ptr;
    size_t value;
  };

  static Node* GetLeader(Node* node) noexcept {
    Node* root = node;
    while (root != root->ptr) {
      root = root->ptr;
    }

    while (node->ptr != node) {
      Node* next = node->ptr;
      node->ptr = root;
      node = next;
    }
    return root;
  }

  std::vector<Node> roots_;
  size_t max_value_;
};

int main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(nullptr);

  size_t tables_count;
  size_t queries_count;
  std::cin >> tables_count >> queries_count;

  std::vector<size_t> data(tables_count);

  for (size_t i = 0; i < tables_count; ++i) {
    std::cin >> data[i];
  }

  DSU dsu(data);

  for (size_t i = 0; i < queries_count; ++i) {
    size_t dst;
    size_t src;
    std::cin >> dst;
    std::cin >> src;
    dsu.Merge(dst, src);
    std::cout << dsu.GetMaxValue() << "\n";
  }
}
