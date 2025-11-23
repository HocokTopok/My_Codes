#include <iostream>
#include <string>
#include <vector>

struct Node {
  int mood;
  Node* left;
  Node* right;
  int height;
  int size;

  Node(int number)
      : mood(number), left(nullptr), right(nullptr), height(1), size(1) {}
};

class AVLtree {
 public:
  AVLtree() : root_(nullptr) {}

  ~AVLtree() { Clear(root_); }

  static void Clear(Node* node) {
    if (node != nullptr) {
      Clear(node->left);
      Clear(node->right);
      delete node;
    }
  }

  void Insert(int mood) { root_ = Insert(root_, mood); }

  void Delete(int mood) { root_ = Delete(root_, mood); }

  std::string Exists(int mood) {
    return Exists(root_, mood) ? "true" : "false";
  }

  std::string Next(int mood) {
    int result = Next(root_, mood);
    return result == -1 ? "none" : std::to_string(result);
  }

  std::string Prev(int mood) {
    int result = Prev(root_, mood);
    return result == -1 ? "none" : std::to_string(result);
  }

  std::string Kth(int mood) {
    int result = Kth(root_, mood);
    return result == -1 ? "none" : std::to_string(result);
  }

 private:
  Node* root_;

  static int GetHeight(Node* node) {
    return node != nullptr ? node->height : 0;
  }

  static int GetSize(Node* node) { return node != nullptr ? node->size : 0; }

  static void Update(Node* node) {
    if (node != nullptr) {
      node->height =
          1 + std::max(GetHeight(node->left), GetHeight(node->right));
      node->size = 1 + GetSize(node->left) + GetSize(node->right);
    }
  }

  static int BalanceNumber(Node* node) {
    return node != nullptr ? GetHeight(node->left) - GetHeight(node->right) : 0;
  }

  static Node* LeftRotate(Node* node) {
    Node* x_node = node->right;
    Node* y_node = x_node->left;

    x_node->left = node;
    node->right = y_node;

    Update(node);
    Update(x_node);

    return x_node;
  }

  static Node* RightRotate(Node* node) {
    Node* x_node = node->left;
    Node* y_node = x_node->right;

    x_node->right = node;
    node->left = y_node;

    Update(node);
    Update(x_node);

    return x_node;
  }

  static Node* Balance(Node* node) {
    if (node == nullptr) {
      return node;
    }

    int balance_number = BalanceNumber(node);

    if (balance_number > 1 && BalanceNumber(node->left) >= 0) {
      return RightRotate(node);
    }

    if (balance_number > 1 && BalanceNumber(node->left) < 0) {
      node->left = LeftRotate(node->left);
      return RightRotate(node);
    }

    if (balance_number < -1 && BalanceNumber(node->right) <= 0) {
      return LeftRotate(node);
    }

    if (balance_number < -1 && BalanceNumber(node->right) > 0) {
      node->right = RightRotate(node->right);
      return LeftRotate(node);
    }

    return node;
  }

  static Node* FindMin(Node* node) {
    while (node != nullptr && node->left != nullptr) {
      node = node->left;
    }
    return node;
  }

  Node* Insert(Node* node, int mood) {
    if (node == nullptr) {
      return new Node(mood);
    }

    if (mood < node->mood) {
      node->left = Insert(node->left, mood);
    } else if (mood > node->mood) {
      node->right = Insert(node->right, mood);
    } else {
      return node;
    }

    Update(node);

    return Balance(node);
  }

  Node* Delete(Node* node, int mood) {
    if (node == nullptr) {
      return node;
    }

    if (mood < node->mood) {
      node->left = Delete(node->left, mood);
    } else if (mood > node->mood) {
      node->right = Delete(node->right, mood);
    } else {
      if (node->left == nullptr || node->right == nullptr) {  // 0/1
        Node* temp = node->left != nullptr ? node->left : node->right;
        if (temp == nullptr) {  // 0
          delete node;
          return nullptr;
        }
        Node* to_delete = node;
        node = temp;
        delete to_delete;
      } else {  // 2
        Node* delim = FindMin(node->right);
        node->mood = delim->mood;
        node->right = Delete(node->right, delim->mood);
      }
    }

    Update(node);

    return Balance(node);
  }

  bool Exists(Node* node, int mood) {
    if (node == nullptr) {
      return false;
    }

    if (node->mood > mood) {
      return Exists(node->left, mood);
    }
    if (node->mood < mood) {
      return Exists(node->right, mood);
    }
    return true;
  }

  static int Next(Node* node, int mood) {
    int result = -1;
    Node* current = node;

    while (current != nullptr) {
      if (current->mood > mood) {
        result = current->mood;
        current = current->left;
      } else {
        current = current->right;
      }
    }

    return result;
  }

  static int Prev(Node* node, int mood) {
    int result = -1;
    Node* current = node;

    while (current != nullptr) {
      if (current->mood < mood) {
        result = current->mood;
        current = current->right;
      } else {
        current = current->left;
      }
    }

    return result;
  }

  int Kth(Node* node, int order) {
    if (node == nullptr || order < 0 || order >= node->size) {
      return -1;
    }

    int left = GetSize(node->left);

    if (order < left) {
      return Kth(node->left, order);
    }
    if (order > left) {
      return Kth(node->right, order - left - 1);
    }
    return node->mood;
  }
};