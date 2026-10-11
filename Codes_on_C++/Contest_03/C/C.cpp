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

struct Query {
  bool is_insert;
  int number_1;
  int number_2;

  Query(bool type, int num_1, int num_2)
      : is_insert(type), number_1(num_1), number_2(num_2) {}
};

std::vector<Query> Input() {
  std::vector<Query> queries;

  int count;
  std::cin >> count;

  while (count != 0) {
    char query_type;
    std::cin >> query_type;
    bool is_insert = (query_type == '+');

    int number_1;
    int number_2;
    std::cin >> number_1 >> number_2;

    queries.push_back(Query(is_insert, number_1, number_2));

    --count;
  }

  return queries;
}

std::vector<int> Process(std::vector<Query>& queries) {
  std::vector<int> results;

  ImplicitTreap treap;

  for (Query& query : queries) {
    if (query.is_insert) {
      treap.Insert(query.number_1, query.number_2);
    } else {
      results.push_back(treap.RangeMin(query.number_1, query.number_2));
    }
  }

  return results;
}

void Output(std::vector<int> results) {
  for (int result : results) {
    std::cout << result << "\n";
  }
}

int main() {
  std::vector<Query> data = Input();
  std::vector<int> results = Process(data);
  Output(results);
}