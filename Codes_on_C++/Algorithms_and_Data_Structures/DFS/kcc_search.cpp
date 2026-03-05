#include <algorithm>
#include <iostream>
#include <vector>

using Graph = std::vector<std::vector<int>>;

void ModifiedDFS(Graph& graph, size_t vertex, std::vector<size_t>& colors,
                 std::vector<size_t>& out) {
  colors[vertex] = 1;

  for (size_t neighbour : graph[vertex]) {
    if (colors[neighbour] == 0) {
      ModifiedDFS(graph, neighbour, colors, out);
    }
  }

  colors[vertex] = 2;
  out.push_back(vertex);
}

void ModifiedDFS(Graph& graph, size_t vertex, std::vector<size_t>& colors,
                 std::vector<size_t>& kcc, size_t count) {
  colors[vertex] = 1;

  for (size_t neighbour : graph[vertex]) {
    if (colors[neighbour] == 0) {
      ModifiedDFS(graph, neighbour, colors, kcc, count);
    }
  }

  colors[vertex] = 2;
  kcc[vertex] = count;
}

int main() {
  size_t vertex_count;
  size_t edge_count;
  std::cin >> vertex_count;
  std::cin >> edge_count;

  Graph graph(vertex_count, std::vector<int>());
  Graph transponed_graph(vertex_count, std::vector<int>());

  for (; edge_count > 0; --edge_count) {
    size_t u_num;
    size_t v_num;
    std::cin >> u_num >> v_num;
    --u_num;
    --v_num;
    graph[u_num].push_back(v_num);
    transponed_graph[v_num].push_back(u_num);
  }

  std::vector<size_t> colors1(vertex_count, 0);
  std::vector<size_t> colors2(vertex_count, 0);
  std::vector<size_t> out;

  for (size_t index = 0; index < vertex_count; ++index) {
    if (colors1[index] == 0) {
      ModifiedDFS(graph, index, colors1, out);
    }
  }
  std::reverse(out.begin(), out.end());

  std::vector<size_t> kcc(vertex_count, 0);

  size_t count = 1;
  for (size_t index : out) {
    if (colors2[index] == 0) {
      ModifiedDFS(transponed_graph, index, colors2, kcc, count);
      ++count;
    }
  }

  std::cout << count - 1 << "\n";

  for (size_t vertex : kcc) {
    std::cout << vertex << " ";
  }
}