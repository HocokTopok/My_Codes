#include <iostream>
#include <random>

class Treap {
 public:
  Treap() : root_(nullptr) {}

  ~Treap() { Delete(root_); }

  void AddElement(size_t index, int value) {
    if (root_ == nullptr) {
      root_ = new Node(value);
      return;
    }
    auto [left, right] = Split(root_, index);
    root_ = Merge(Merge(left, new Node(value)), right);
  }

  void Cut(Treap& other, size_t start, size_t end) {
    auto [tmp, right] = Split(root_, end / 2);
    auto [left, change] = Split(tmp, (start - 1) / 2);

    auto [tmp_other, right_other] = Split(other.root_, (end + 1) / 2);
    auto [left_other, change_other] = Split(tmp_other, start / 2);

    root_ = Merge(left, Merge(change_other, right));
    other.root_ = Merge(left_other, Merge(change, right_other));
  }

  long long GetSum(size_t left, size_t right) {
    auto [tmp, second] = Split(root_, right / 2);
    auto [first, main] = Split(tmp, left / 2);
    long long answer = Summa(main);
    root_ = Merge(first, Merge(main, second));
    return answer;
  }

 private:
  struct Node {
    int value;
    int priority;
    size_t size;
    long long summa;
    Node* left;
    Node* right;

    Node(int value)
        : value(value),
          priority(rand()),
          size(1),
          summa(value),
          left(nullptr),
          right(nullptr) {}
  };

  Node* root_;

  static size_t Size(Node* node) { return (node == nullptr ? 0 : node->size); }

  static long long Summa(Node* node) {
    return (node == nullptr ? 0 : node->summa);
  }

  static bool IsEmpty(Node* node) { return (node == nullptr); }

  static void Update(Node* node) {
    if (node == nullptr) {
      return;
    }
    node->size = 1 + Size(node->left) + Size(node->right);
    node->summa = +node->value + Summa(node->left) + Summa(node->right);
  }

  void Delete(Node* node) {
    if (node == nullptr) {
      return;
    }
    Delete(node->left);
    Delete(node->right);
    delete node;
  }

  Node* Merge(Node* left, Node* right) {
    if (left == nullptr) {
      return right;
    }
    if (right == nullptr) {
      return left;
    }
    if (left->priority < right->priority) {
      right->left = Merge(left, right->left);
      Update(right);
      return right;
    }
    left->right = Merge(left->right, right);
    Update(left);
    return left;
  }

  std::pair<Node*, Node*> Split(Node* node, size_t index) {
    if (node == nullptr) {
      return {nullptr, nullptr};
    }
    if (Size(node->left) + 1 <= index) {
      auto [left, right] = Split(node->right, index - Size(node->left) - 1);
      node->right = left;
      Update(node);
      return {node, right};
    }
    auto [left, right] = Split(node->left, index);
    node->left = right;
    Update(node);
    return {left, node};
  }
};

int main() {
  size_t size;
  int query;
  int cnt = 1;
  std::cin >> size >> query;
  while (size != 0 || query != 0) {
    Treap even_indexes;
    Treap uneven_indexes;
    std::cout << "Swapper " << cnt << ":\n";
    for (size_t ix = 0; ix < size; ++ix) {
      int value;
      std::cin >> value;
      if (ix % 2 == 0) {
        uneven_indexes.AddElement(ix, value);
      } else {
        even_indexes.AddElement(ix, value);
      }
    }
    for (int i = 0; i < query; ++i) {
      int type;
      size_t left;
      size_t right;
      std::cin >> type >> left >> right;
      if (type == 2) {
        std::cout << uneven_indexes.GetSum(left, right + 1) +
                         even_indexes.GetSum(left - 1, right)
                  << '\n';
      }
      if (type == 1) {
        even_indexes.Cut(uneven_indexes, left, right);
      }
    }
    std::cin >> size >> query;
    ++cnt;
  }
}