#include <iostream>
#include <vector>

using Graph = std::vector<std::vector<size_t>>;

struct Data {
  Data(size_t vertex_count)
      : graph(vertex_count),
        used(vertex_count, false) {}

  Graph graph;
  std::vector<bool> used;
};

void DFS(Data& data, size_t current) {
  data.used[current] = true;

  // Здесь можно обработать вершину current
  std::cout << current + 1 << " ";

  for (size_t neighbour : data.graph[current]) {
    if (!data.used[neighbour]) {
      DFS(data, neighbour);
    }
  }
}

int main() {
  size_t vertex_count;
  size_t edge_count;
  std::cin >> vertex_count >> edge_count;

  Data data(vertex_count);

  for (size_t i = 0; i < edge_count; ++i) {
    size_t u_num;
    size_t v_num;
    std::cin >> u_num >> v_num;

    --u_num;
    --v_num;

    data.graph[u_num].push_back(v_num);
    data.graph[v_num].push_back(u_num);
  }

  for (size_t vertex = 0; vertex < vertex_count; ++vertex) {
    if (!data.used[vertex]) {
      DFS(data, vertex);
    }
  }

  std::cout << "\n";
}