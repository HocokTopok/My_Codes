#include <algorithm>
#include <iostream>
#include <vector>

const int64_t kMax = 30'000;
const int64_t kStartVertex = 0;

struct Data {
  Data(int64_t vertex_count)
      : dist(vertex_count, kMax),
        used(vertex_count, false),
        vertex_count(vertex_count) {}

  std::vector<std::tuple<int64_t, int64_t, int64_t>> edges;
  std::vector<int64_t> dist;
  std::vector<bool> used;
  int64_t vertex_count;
};

void FordBellman(Data& data, int64_t start_vertex) {
  data.dist[start_vertex] = 0;
  data.used[start_vertex] = true;

  for (int64_t i = 0; i < data.vertex_count; ++i) {
    for (auto [v_from, v_to, weight] : data.edges) {
      if (data.used[v_from]) {
        data.dist[v_to] = std::min(data.dist[v_to], data.dist[v_from] + weight);
        data.used[v_to] = true;
      }
    }
  }
}

int main() {
  int64_t vertex_count;
  int64_t edges_count;
  std::cin >> vertex_count >> edges_count;

  Data data(vertex_count);

  for (; edges_count > 0; --edges_count) {
    int64_t u_num;
    int64_t v_num;
    int64_t w_num;
    std::cin >> u_num >> v_num >> w_num;
    --u_num;
    --v_num;
    data.edges.push_back({u_num, v_num, w_num});
  }

  FordBellman(data, kStartVertex);

  for (int64_t dist : data.dist) {
    std::cout << dist << " ";
  }
}