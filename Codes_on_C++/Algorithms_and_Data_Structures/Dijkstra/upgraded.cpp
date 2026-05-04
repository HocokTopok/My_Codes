#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

const size_t kMax = 999'999'999;

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