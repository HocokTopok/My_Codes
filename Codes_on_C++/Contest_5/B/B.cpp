#include <iostream>
#include <vector>

using Graph = std::vector<std::vector<int>>;

void DFS(Graph& graph, size_t vertex, std::vector<bool>& used,
         std::vector<size_t>& claster) {
  used[vertex] = true;
  claster.push_back(vertex);
  for (size_t neighbour : graph[vertex]) {
    if (!used[neighbour]) {
      DFS(graph, neighbour, used, claster);
    }
  }
}

int main() {
  size_t vertex_count;
  size_t edge_count;
  std::cin >> vertex_count;
  std::cin >> edge_count;

  Graph graph(vertex_count, std::vector<int>());

  for (; edge_count != 0; --edge_count) {
    size_t u_num;
    size_t v_num;
    std::cin >> u_num >> v_num;
    --u_num;
    --v_num;
    graph[u_num].push_back(v_num);
    graph[v_num].push_back(u_num);
  }

  std::vector<bool> used(vertex_count, false);

  std::vector<std::vector<size_t>> clasters;

  for (size_t vertex = 0; vertex < vertex_count; ++vertex) {
    if (!used[vertex]) {
      std::vector<size_t> claster;
      DFS(graph, vertex, used, claster);
      clasters.push_back(claster);
    }
  }

  std::cout << clasters.size() << "\n";
  for (std::vector<size_t>& claster : clasters) {
    std::cout << claster.size() << "\n";
    for (size_t vertex : claster) {
      std::cout << vertex + 1 << " ";
    }
    std::cout << "\n";
  }
}