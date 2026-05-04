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
  data.dist[vertex] = 0;

  for (size_t _ = 0; _ < data.vertex_count; ++_) {
    size_t min_index = data.vertex_count;
    size_t min_dist = kMax;

    for (size_t index = 0; index < data.vertex_count; ++index) {
      if (!data.used[index] && data.dist[index] < min_dist) {
        min_index = index;
        min_dist = data.dist[index];
      }
    }

    if (min_index == data.vertex_count) {
      break;
    }

    data.used[min_index] = true;

    for (Pair neighbour : data.graph[min_index]) {
      data.dist[neighbour.first] = std::min(
          data.dist[neighbour.first], data.dist[min_index] + neighbour.second);
    }
  }
}