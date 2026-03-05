#include <algorithm>
#include <iostream>
#include <vector>

using Graph = std::vector<std::vector<int>>;

bool ModifiedDFS(Graph& graph, size_t vertex, std::vector<size_t>& colors,
                 std::vector<size_t>& result) {
  colors[vertex] = 1;

  for (size_t neighbour : graph[vertex]) {
    if (colors[neighbour] == 1) {
      return true;
    }
    if (colors[neighbour] == 0 &&
        ModifiedDFS(graph, neighbour, colors, result)) {
      return true;
    }
  }

  colors[vertex] = 2;
  result.push_back(vertex);
  return false;
}

int main() {
  size_t vertex_count;
  size_t edge_count;
  std::cin >> vertex_count;
  std::cin >> edge_count;

  Graph graph(vertex_count, std::vector<int>());

  for (; edge_count > 0; --edge_count) {
    size_t u_num;
    size_t v_num;
    std::cin >> u_num >> v_num;
    --u_num;
    --v_num;
    graph[u_num].push_back(v_num);
  }

  std::vector<size_t> colors(vertex_count, 0);
  std::vector<size_t> result;

  bool win = true;

  for (size_t index = 0; index < vertex_count; ++index) {
    if (colors[index] == 0 && ModifiedDFS(graph, index, colors, result)) {
      std::cout << -1;
      win = false;
      break;
    }
  }

  if (win) {
    std::reverse(result.begin(), result.end());
    for (size_t vertex : result) {
      std::cout << vertex + 1 << " ";
    }
  }
}