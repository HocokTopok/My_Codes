#include <algorithm>
#include <iostream>
#include <vector>

using Graph = std::vector<std::vector<size_t>>;

struct Data {
  Data(size_t vertex_count)
      : graph(vertex_count, std::vector<size_t>()),
        transponed_graph(vertex_count, std::vector<size_t>()),
        used(vertex_count, false),
        scc_number(vertex_count, 0) {}

  Graph graph;
  Graph transponed_graph;
  std::vector<bool> used;
  std::vector<size_t> t_out_order;
  std::vector<size_t> scc_number;
};

void DfsForOrdering(Data& data, size_t vertex) {
  data.used[vertex] = true;

  for (size_t neighbour : data.graph[vertex]) {
    if (!data.used[neighbour]) {
      DfsForOrdering(data, neighbour);
    }
  }

  data.t_out_order.push_back(vertex);
}

void DfsForSCC(Data& data, size_t vertex,
               size_t count) {  // false = true, true = false
  data.used[vertex] = false;

  for (size_t neighbour : data.transponed_graph[vertex]) {
    if (data.used[neighbour]) {
      DfsForSCC(data, neighbour, count);
    }
  }

  data.scc_number[vertex] = count;
}

int main() {
  size_t vertex_count;
  size_t edge_count;
  std::cin >> vertex_count >> edge_count;

  Data data(vertex_count);

  for (; edge_count > 0; --edge_count) {
    size_t start_vertex;
    size_t end_vertex;
    std::cin >> start_vertex >> end_vertex;
    --start_vertex;
    --end_vertex;
    data.graph[start_vertex].push_back(end_vertex);
    data.transponed_graph[end_vertex].push_back(start_vertex);
  }

  for (size_t index = 0; index < vertex_count; ++index) {
    if (!data.used[index]) {
      DfsForOrdering(data, index);
    }
  }
  std::reverse(data.t_out_order.begin(), data.t_out_order.end());

  size_t scc_index = 1;
  for (size_t index : data.t_out_order) {
    if (data.used[index]) {
      DfsForSCC(data, index, scc_index);
      ++scc_index;
    }
  }

  std::cout << scc_index - 1 << "\n";

  for (size_t vertex : data.scc_number) {
    std::cout << vertex << " ";
  }
}