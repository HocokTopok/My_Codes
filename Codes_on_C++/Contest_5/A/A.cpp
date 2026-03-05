#include <algorithm>
#include <iostream>
#include <vector>

using Graph = std::vector<std::vector<size_t>>;

bool ModifiedDFS(Graph& graph, size_t vertex, std::vector<size_t>& colors,
                 std::vector<size_t>& parents, std::vector<size_t>& cycle) {
  colors[vertex] = 1;

  for (size_t neighbour : graph[vertex]) {
    if (colors[neighbour] == 0) {
      parents[neighbour] = vertex;
      if (ModifiedDFS(graph, neighbour, colors, parents, cycle)) {
        return true;
      }

    } else if (colors[neighbour] == 1) {
      size_t current = vertex;
      while (current != neighbour) {
        cycle.push_back(current);
        current = parents[current];
      }
      cycle.push_back(neighbour);
      std::reverse(cycle.begin(), cycle.end());
      return true;
    }
  }

  colors[vertex] = 2;

  return false;
}

int main() {
  size_t vertex_count;
  size_t edge_count;
  std::cin >> vertex_count;
  std::cin >> edge_count;

  Graph graph(vertex_count, std::vector<size_t>());

  for (; edge_count > 0; --edge_count) {
    size_t u_num;
    size_t v_num;
    std::cin >> u_num >> v_num;
    --u_num;
    --v_num;
    graph[u_num].push_back(v_num);
  }

  std::vector<size_t> colors(vertex_count, 0);
  std::vector<size_t> parents(vertex_count, 0);
  std::vector<size_t> cycle;

  for (size_t index = 0; index < vertex_count; ++index) {
    if (colors[index] == 0 &&
        ModifiedDFS(graph, index, colors, parents, cycle)) {
      std::cout << "YES" << "\n";
      for (size_t vertex : cycle) {
        std::cout << vertex + 1 << " ";
      }
      break;
    }
  }
  if (cycle.empty()) {
    std::cout << "NO";
  }
}