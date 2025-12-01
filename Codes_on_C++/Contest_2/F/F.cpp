#include <functional>
#include <iostream>
#include <string>
#include <vector>

template <typename Key, typename Compare = std::less<Key>>
class AVLtree {
 public:
  AVLtree() : root_(nullptr) {}

  AVLtree(const AVLtree& other) : root_(Copy(other.root_)) {}

  AVLtree& operator=(const AVLtree& other) {
    if (this != &other) {
      Clear(root_);
      root_ = Copy(other.root_);
    }
    return *this;
  }

  ~AVLtree() { Clear(root_); }

  void Insert(const Key& mood) { root_ = Insert(root_, mood); }

  void Delete(const Key& mood) { root_ = Delete(root_, mood); }

  std::string Exists(const Key& mood) {
    return Exists(root_, mood) ? "true" : "false";
  }

  std::string Next(const Key& mood) { return Next(root_, mood); }

  std::string Prev(const Key& mood) { return Prev(root_, mood); }

  std::string Kth(const int kOrder) { return Kth(root_, kOrder); }

 private:
  struct Node {
    int mood;
    Node* left;
    Node* right;
    int height;
    int size;

    Node(int number)
        : mood(number), left(nullptr), right(nullptr), height(1), size(1) {}
  };

  Node* root_;
  Compare comp_;

  static Node* Copy(Node* node) {
    if (node == nullptr) {
      return nullptr;
    }
    Node* new_node = new Node(node->mood);
    new_node->height = node->height;
    new_node->size = node->size;
    new_node->left = Copy(node->left);
    new_node->right = Copy(node->right);
    return new_node;
  }

  static void Clear(Node* node) {
    if (node != nullptr) {
      Clear(node->left);
      Clear(node->right);
      delete node;
    }
  }

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

  static Node* RebalanceSubtree(Node* node) {
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

    if (comp_(mood, node->mood)) {
      node->left = Insert(node->left, mood);
    } else if (comp_(node->mood, mood)) {
      node->right = Insert(node->right, mood);
    } else {
      return node;
    }

    Update(node);
    return RebalanceSubtree(node);
  }

  Node* Delete(Node* node, int mood) {
    if (node == nullptr) {
      return node;
    }

    if (comp_(mood, node->mood)) {
      node->left = Delete(node->left, mood);
    } else if (comp_(node->mood, mood)) {
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
    return RebalanceSubtree(node);
  }

  bool Exists(Node* node, int mood) {
    if (node == nullptr) {
      return false;
    }

    if (comp_(mood, node->mood)) {
      return Exists(node->left, mood);
    }
    if (comp_(node->mood, mood)) {
      return Exists(node->right, mood);
    }
    return true;
  }

  std::string Next(Node* node, int mood) {
    Node* result = nullptr;
    Node* current = node;

    while (current != nullptr) {
      if (comp_(mood, current->mood)) {
        result = current;
        current = current->left;
      } else {
        current = current->right;
      }
    }

    return result == nullptr ? "none" : std::to_string(result->mood);
  }

  std::string Prev(Node* node, int mood) {
    Node* result = nullptr;
    Node* current = node;

    while (current != nullptr) {
      if (comp_(current->mood, mood)) {
        result = current;
        current = current->right;
      } else {
        current = current->left;
      }
    }

    return result == nullptr ? "none" : std::to_string(result->mood);
  }

  std::string Kth(Node* node, int k_order) {
    if (node == nullptr || k_order < 0 || k_order >= node->size) {
      return "none";
    }

    int left = GetSize(node->left);

    if (k_order < left) {
      return Kth(node->left, k_order);
    }
    if (k_order > left) {
      return Kth(node->right, k_order - left - 1);
    }
    return std::to_string(node->mood);
  }
};

struct Data {
  std::vector<std::pair<std::string, int>> queries;
};

Data Input() {
  Data data;
  std::string command;
  int number;
  while (std::cin >> command >> number) {
    data.queries.push_back({command, number});
  }

  return data;
}

std::vector<std::string> Process(Data& data) {
  AVLtree<int> avl;
  std::vector<std::string> results;

  for (std::pair<std::string, int>& pair : data.queries) {
    if (pair.first == "insert") {
      avl.Insert(pair.second);
    } else if (pair.first == "delete") {
      avl.Delete(pair.second);
    } else if (pair.first == "exists") {
      results.push_back(avl.Exists(pair.second));
    } else if (pair.first == "next") {
      results.push_back(avl.Next(pair.second));
    } else if (pair.first == "prev") {
      results.push_back(avl.Prev(pair.second));
    } else if (pair.first == "kth") {
      results.push_back(avl.Kth(pair.second));
    } else {
      std::cout << pair.first << " - wrong command";
    }
  }

  return results;
}

void Output(std::vector<std::string>& results) {
  for (std::string& line : results) {
    std::cout << line << "\n";
  }
}

int main() {
  Data data = Input();
  std::vector<std::string> results = Process(data);
  Output(results);
}