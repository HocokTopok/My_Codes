#include <ctime>
#include <iostream>
#include <vector>

class Treap {
 public:
  Treap() : root_(nullptr) { std::srand(std::time(0)); }

  ~Treap() { Clear(root_); }

  void Insert(int number) {
    if (Contains(root_, number)) {
      return;
    }

    Node* right;
    Node* left;
    Split(root_, number, left, right);

    Node* new_node = new Node(number);

    root_ = Merge(Merge(left, new_node), right);
  }

  int64_t Sum(int start, int end) {
    Node* left;
    Node* mid;
    Node* right;

    Split(root_, start - 1, left, mid);
    Split(mid, end, mid, right);

    int64_t result = mid != nullptr ? mid->sum : 0;

    root_ = Merge(Merge(left, mid), right);

    return result;
  }

 private:
  struct Node {
    int mood;
    int priority;
    int size;
    int64_t sum;
    Node* left;
    Node* right;

    Node(int number)
        : mood(number),
          priority(std::rand()),
          size(1),
          sum(number),
          left(nullptr),
          right(nullptr) {}
  };
  
  Node* root_;

  static void Update(Node* node) {
    if (node == nullptr) {
      return;
    }
    node->size = 1;
    node->sum = node->mood;
    if (node->left != nullptr) {
      node->size += node->left->size;
      node->sum += node->left->sum;
    }
    if (node->right != nullptr) {
      node->size += node->right->size;
      node->sum += node->right->sum;
    }
  }

  void Split(Node* node, int delimiter, Node*& left, Node*& right) {
    if (node == nullptr) {
      left = nullptr;
      right = nullptr;
      return;
    }

    if (node->mood <= delimiter) {
      Split(node->right, delimiter, node->right, right);
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

  bool Contains(Node* node, int target) {
    if (node == nullptr) {
      return false;
    }
    if (node->mood == target) {
      return true;
    }
    if (target < node->mood) {
      return Contains(node->left, target);
    }
    return Contains(node->right, target);
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