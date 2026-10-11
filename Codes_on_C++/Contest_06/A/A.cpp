#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

using Pair = std::pair<int64_t, int64_t>;
using Graph = std::vector<std::vector<Pair>>;

const int64_t kStartVertex = 0;
const int64_t kMax = 2e18L;

struct Data {
  Data(int64_t vertex_count, int64_t edge_count, int64_t query_count)
      : graph(vertex_count),
        edge_weight(edge_count),
        potential(vertex_count, kMax),
        dist(vertex_count, kMax),
        used(vertex_count, false),
        vertex_count(vertex_count),
        query_count(query_count) {}

  Graph graph;
  std::vector<int64_t> edge_weight;
  std::vector<int64_t> potential;
  std::vector<int64_t> dist;
  std::vector<bool> used;
  int64_t vertex_count;
  int64_t query_count;
};

void Johnson(Data& data) {
  std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> min_heap;
  data.potential[kStartVertex] = 0;
  min_heap.push({0, kStartVertex});

  while (!min_heap.empty()) {
    auto [min_dist, min_vertex] = min_heap.top();
    min_heap.pop();

    if (min_dist > data.potential[min_vertex]) {
      continue;
    }

    for (auto [n_vertex, n_edge_index] : data.graph[min_vertex]) {
      int64_t nd = data.potential[min_vertex] + data.edge_weight[n_edge_index];
      if (nd < data.potential[n_vertex]) {
        data.potential[n_vertex] = nd;
        min_heap.push({nd, n_vertex});
      }
    }
  }
}

void UpdatePotentials(Data& data) {
  for (int64_t vertex = 0; vertex < data.vertex_count; ++vertex) {
    if (data.dist[vertex] != kMax && data.potential[vertex] != kMax) {
      data.potential[vertex] += data.dist[vertex];
    }
  }
}

void Dijkstra(Data& data, int64_t update_count,
              std::vector<std::vector<int64_t>>& buckets) {
  std::fill(data.dist.begin(), data.dist.end(), kMax);
  std::fill(data.used.begin(), data.used.end(), false);

  if ((int64_t)buckets.size() < update_count + 1) {
    buckets.resize(update_count + 1);
  }

  data.dist[kStartVertex] = 0;
  buckets[0].push_back(kStartVertex);

  for (int64_t current_dist = 0; current_dist <= update_count; ++current_dist) {
    for (int64_t index = 0; index < (int64_t)buckets[current_dist].size();
         ++index) {
      int64_t min_vertex = buckets[current_dist][index];
      if (data.used[min_vertex]) {
        continue;
      }
      data.used[min_vertex] = true;

      for (auto [n_vertex, n_edge_index] : data.graph[min_vertex]) {
        if (data.potential[min_vertex] == kMax ||
            data.potential[n_vertex] == kMax) {
          continue;
        }

        int64_t reduced_weight =
            data.edge_weight[n_edge_index] + data.potential[min_vertex] -
            data.potential[n_vertex];  // w'(u,v) = w(u,v) + h[u] - h[v] >= 0
        int64_t n_dist = data.dist[min_vertex] + reduced_weight;
        if (n_dist < data.dist[n_vertex]) {
          data.dist[n_vertex] = n_dist;
          if (n_dist <= update_count) {
            buckets[n_dist].push_back(n_vertex);
          }
        }
      }
    }
    buckets[current_dist].clear();
  }

  UpdatePotentials(data);
}

Data Input() {
  int64_t vertex_count;
  int64_t edge_count;
  int64_t query_count;
  std::cin >> vertex_count >> edge_count >> query_count;

  Data data(vertex_count, edge_count, query_count);

  for (int64_t edge_index = 0; edge_index < edge_count; ++edge_index) {
    int64_t v_from;
    int64_t v_to;
    int64_t weight;
    std::cin >> v_from >> v_to >> weight;
    --v_from;
    --v_to;
    data.graph[v_from].push_back({v_to, edge_index});
    data.edge_weight[edge_index] = weight;
  }

  return data;
}

int main() {
  Data data = Input();
  std::vector<std::vector<int64_t>> buckets;
  int64_t total_update_count = 0;

  Johnson(data);

  while (data.query_count-- != 0) {
    int64_t type;
    std::cin >> type;

    if (type == 1) {
      int64_t vertex;
      std::cin >> vertex;
      --vertex;

      if (total_update_count > 0) {
        Dijkstra(data, total_update_count, buckets);
        total_update_count = 0;
      }

      std::cout << (data.potential[vertex] != kMax ? data.potential[vertex]
                                                   : -1)
                << "\n";

    } else {
      int64_t update_count;
      std::cin >> update_count;
      total_update_count += update_count;

      for (int64_t j = 0; j < update_count; ++j) {
        int64_t edge;
        std::cin >> edge;
        --edge;
        data.edge_weight[edge] += 1;
      }
    }
  }
}