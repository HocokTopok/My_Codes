#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

const size_t kMax = 2009000999;

using Pair = std::pair<size_t, size_t>;
using Graph = std::vector<std::vector<Pair>>;

struct Data {
  Data(size_t vertex_count)
      : graph(vertex_count, std::vector<Pair>()),
        used(vertex_count, false),
        dist(vertex_count, kMax),
        vertex_count(vertex_count) {}

  Graph graph;
  std::vector<bool> used;
  std::vector<size_t> dist;
  size_t vertex_count;
};

void Dijkstra(Data& data, size_t vertex) {
  std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> min_heap;
  data.dist[vertex] = 0;
  min_heap.push({0, vertex});

  while (!min_heap.empty()) {
    auto [min_dist, min_index] = min_heap.top();
    min_heap.pop();

    if (data.used[min_index]) {
      continue;
    }

    data.used[min_index] = true;

    for (Pair neighbour : data.graph[min_index]) {
      if (data.dist[neighbour.first] >
          data.dist[min_index] + neighbour.second) {
        data.dist[neighbour.first] = data.dist[min_index] + neighbour.second;
        min_heap.push({data.dist[neighbour.first], neighbour.first});
      }
    }
  }
}

int main() {
  size_t maps_count;
  std::cin >> maps_count;

  for (; maps_count > 0; --maps_count) {
    size_t vertex_count;
    size_t edges_count;
    std::cin >> vertex_count >> edges_count;

    Data data(vertex_count);

    for (; edges_count > 0; --edges_count) {
      size_t u_num;
      size_t v_num;
      size_t w_num;
      std::cin >> u_num >> v_num >> w_num;
      data.graph[u_num].push_back({v_num, w_num});
      data.graph[v_num].push_back({u_num, w_num});
    }

    size_t start_vertex;
    std::cin >> start_vertex;

    Dijkstra(data, start_vertex);

    for (size_t dist : data.dist) {
      std::cout << dist << " ";
    }

    std::cout << "\n";
  }
}