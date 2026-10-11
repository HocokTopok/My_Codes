#include <algorithm>
#include <iostream>
#include <vector>

const int64_t kMax = 201;

using Pair = std::pair<size_t, size_t>;
using Graph = std::vector<std::vector<std::pair<int64_t, size_t>>>;

struct Edge {
  int64_t weight;
  size_t first;
  size_t second;
};

class DSU {
 public:
  DSU(size_t vertex_count) : roots_(vertex_count) {
    for (size_t index = 0; index < vertex_count; ++index) {
      roots_[index].ptr = &roots_[index];
    }
  }

  void Merge(size_t first_index, size_t second_index) {
    Node* first_leader = GetLeader(&roots_[first_index]);
    Node* second_leader = GetLeader(&roots_[second_index]);

    if (first_leader == second_leader) {
      return;
    }

    second_leader->ptr = first_leader;
  }

  bool Connected(size_t first_index, size_t second_index) noexcept {
    Node* first_leader = GetLeader(&roots_[first_index]);
    Node* second_leader = GetLeader(&roots_[second_index]);

    return first_leader == second_leader;
  }

 private:
  struct Node {
    Node* ptr;
  };

  static Node* GetLeader(Node* node) noexcept {
    Node* root = node;

    while (root->ptr != root) {
      root = root->ptr;
    }

    while (node->ptr != root) {
      Node* next = node->ptr;
      node->ptr = root;
      node = next;
    }

    return root;
  }

  std::vector<Node> roots_;
};

struct Data {
  Data(size_t vertex_count, size_t edge_count)
      : edges(edge_count),
        max_tree(vertex_count),
        used(vertex_count, false),
        vertex_count(vertex_count) {}

  void BuildMaxTree() {
    DSU dsu(vertex_count);

    std::sort(edges.begin(), edges.end(),
              [](const Edge& first, const Edge& second) {
                return first.weight > second.weight;
              });

    for (const Edge& edge : edges) {
      if (!dsu.Connected(edge.first, edge.second)) {
        dsu.Merge(edge.first, edge.second);
        max_tree[edge.first].push_back({edge.weight, edge.second});
        max_tree[edge.second].push_back({edge.weight, edge.first});
      }
    }
  }

  std::vector<Edge> edges;
  Graph max_tree;
  std::vector<bool> used;
  std::vector<Pair> queries;
  std::vector<int64_t> results;
  size_t vertex_count;
};

int64_t MaxDFS(Data& data, size_t current_vertex, size_t end_vertex,
               int64_t minimum = kMax) {
  if (current_vertex == end_vertex) {
    return minimum;
  }

  data.used[current_vertex] = true;

  for (auto [min_weight, min_vertex] : data.max_tree[current_vertex]) {
    if (!data.used[min_vertex]) {
      int64_t result =
          MaxDFS(data, min_vertex, end_vertex, std::min(minimum, min_weight));
      if (result != -1) {
        return result;
      }
    }
  }

  return -1;
}

Data Input() {
  size_t vertex_count;
  size_t edge_count;
  size_t queries_count;
  std::cin >> vertex_count >> edge_count >> queries_count;

  Data data(vertex_count, edge_count);

  for (size_t index = 0; index < edge_count; ++index) {
    size_t u_num;
    size_t v_num;
    int64_t w_num;
    std::cin >> u_num >> v_num >> w_num;
    --u_num;
    --v_num;
    data.edges[index] = {w_num, u_num, v_num};
  }

  for (; queries_count > 0; --queries_count) {
    size_t u_num;
    size_t v_num;
    std::cin >> u_num >> v_num;
    --u_num;
    --v_num;
    data.queries.push_back({u_num, v_num});
  }

  return data;
}

void Output(const std::vector<int64_t> kResults) {
  for (int64_t result : kResults) {
    std::cout << result << "\n";
  }
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  Data data = Input();
  data.BuildMaxTree();
  for (auto [u_num, v_num] : data.queries) {
    std::fill(data.used.begin(), data.used.end(), false);
    data.results.push_back(MaxDFS(data, u_num, v_num));
  }
  Output(data.results);
}