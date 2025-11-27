#include <ctime>
#include <iostream>
#include <vector>

const int kMax = 1'000'000'000;

class ImplicitTreap {
 public:
  ImplicitTreap() : root_(nullptr) { std::srand(std::time(0)); }

  ~ImplicitTreap() { Clear(root_); }

  void Insert(size_t index, int number) {
    Node* new_node = new Node(number);

    Node* left;
    Node* right;
    Split(root_, index, left, right);

    root_ = Merge(Merge(left, new_node), right);
  }

  int RangeMin(size_t left_index, size_t right_index) {
    --left_index;
    --right_index;

    Node* left;
    Node* mid;
    Node* right;

    Split(root_, left_index, left, mid);
    Split(mid, right_index - left_index + 1, mid, right);

    int minimum = GetMin(mid);

    root_ = Merge(left, Merge(mid, right));

    return minimum;
  }

 private:
  struct Node {
    int value;
    int priority;
    size_t size;
    int minimum;
    Node* left;
    Node* right;

    Node(int number)
        : value(number),
          priority(std::rand()),
          size(1),
          minimum(number),
          left(nullptr),
          right(nullptr) {}
  };

  Node* root_;

  static size_t GetSize(Node* node) { return node == nullptr ? 0 : node->size; }

  static int GetMin(Node* node) {
    return node == nullptr ? kMax : node->minimum;
  }

  static void Update(Node* node) {
    if (node == nullptr) {
      return;
    }
    node->size = 1 + GetSize(node->left) + GetSize(node->right);
    node->minimum = std::min(node->value,
                             std::min(GetMin(node->left), GetMin(node->right)));
  }

  void Split(Node* node, size_t delimiter, Node*& left, Node*& right) {
    if (node == nullptr) {
      left = nullptr;
      right = nullptr;
      return;
    }

    size_t left_size = GetSize(node->left);
    if (left_size < delimiter) {
      Split(node->right, delimiter - 1 - left_size, node->right, right);
      left = node;
    } else {
      Split(node->left, delimiter, left, node->left);
      right = node;
    }

    Update(node);
  }

  Node* Merge(Node* left, Node* right) {
    if (left == nullptr) {
      return right;
    }
    if (right == nullptr) {
      return left;
    }

    if (left->priority <= right->priority) {
      right->left = Merge(left, right->left);
      Update(right);
      return right;
    }
    left->right = Merge(left->right, right);
    Update(left);
    return left;
  }

  void Clear(Node* node) {
    if (node == nullptr) {
      return;
    }
    Clear(node->left);
    Clear(node->right);
    delete node;
  }
};