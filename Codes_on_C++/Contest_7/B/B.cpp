#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

using Pair = std::pair<size_t, size_t>;
using Graph = std::vector<std::vector<Pair>>;

struct Data {
  Data(size_t vertex_count)
      : graph(vertex_count),
        used(vertex_count, false),
        vertex_count(vertex_count) {}

  Graph graph;
  std::vector<bool> used;
  size_t vertex_count;
};

Data Input() {
  size_t vertex_count;
  size_t edge_count;
  std::cin >> vertex_count >> edge_count;
  Data data(vertex_count);
  for (; edge_count > 0; --edge_count) {
    size_t u_num;
    size_t v_num;
    size_t w_num;
    std::cin >> u_num >> v_num >> w_num;
    --u_num;
    --v_num;
    data.graph[u_num].push_back({w_num, v_num});
    data.graph[v_num].push_back({w_num, u_num});
  }

  return data;
}

void Output(size_t result) { std::cout << result; }

size_t Prima(Data& data, size_t start_vertex = 0) {
  size_t min_ostov = 0;

  std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> min_heap;
  min_heap.push({0, start_vertex});

  while (!min_heap.empty()) {
    auto [min_weight, min_vertex] = min_heap.top();
    min_heap.pop();

    if (data.used[min_vertex]) {
      continue;
    }

    data.used[min_vertex] = true;
    min_ostov += min_weight;

    for (auto [new_weight, new_vertex] : data.graph[min_vertex]) {
      if (!data.used[new_vertex]) {
        min_heap.push({new_weight, new_vertex});
      }
    }
  }

  return min_ostov;
}

int main() {
  Data data = Input();
  size_t result = Prima(data);
  Output(result);
}
