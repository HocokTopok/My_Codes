#include <algorithm>
#include <iostream>
#include <queue>
#include <tuple>
#include <vector>

const int64_t kMax = 1e10L;
const int64_t kStartVertex = 0;

using Edge = std::tuple<int64_t, int64_t, int64_t>;
using Graph = std::vector<std::vector<Edge>>;

struct Data {
  Data(int64_t vertex_count, int64_t t_max)
      : graph(vertex_count, std::vector<Edge>()),
        dist(vertex_count, std::vector<int64_t>(t_max + 1, kMax)),
        parent(vertex_count,
               std::vector<std::pair<int64_t, int64_t>>(t_max + 1)),
        vertex_count(vertex_count),
        t_max(t_max) {}

  Graph graph;
  std::vector<std::vector<int64_t>> dist;
  std::vector<std::vector<std::pair<int64_t, int64_t>>> parent;
  int64_t vertex_count;
  int64_t t_max;
};

std::pair<int64_t, int64_t> Dijkstra(Data& data, int64_t start_vertex) {
  std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>> min_heap;

  int64_t end_vertex = data.vertex_count - 1;
  int64_t timer = 0;
  int64_t cost = 0;

  data.dist[start_vertex][timer] = cost;
  min_heap.push({0, 0, start_vertex});

  while (!min_heap.empty()) {
    auto [min_cost, min_time, min_vertex] = min_heap.top();
    min_heap.pop();

    if (min_vertex == end_vertex) {
      return {data.dist[end_vertex][min_time], min_time};
    }

    if (min_cost > data.dist[min_vertex][min_time]) {
      continue;
    }

    for (Edge neighbour : data.graph[min_vertex]) {
      auto [new_vertex, cost, time] = neighbour;

      int64_t new_cost = min_cost + cost;
      int64_t new_time = min_time + time;

      if (data.t_max >= new_time &&
          data.dist[new_vertex][new_time] > new_cost) {
        data.dist[new_vertex][new_time] = new_cost;
        data.parent[new_vertex][new_time] = {min_vertex, min_time};
        min_heap.push({data.dist[new_vertex][new_time], new_time, new_vertex});
      }
    }
  }

  return {-1, -1};
}

int main() {
  int64_t vertex_count;
  int64_t edge_count;
  int64_t t_max;
  std::cin >> vertex_count >> edge_count >> t_max;

  Data data(vertex_count, t_max);

  for (; edge_count > 0; --edge_count) {
    int64_t u_num;
    int64_t v_num;
    int64_t w_num;
    int64_t t_num;
    std::cin >> u_num >> v_num >> w_num >> t_num;
    --u_num;
    --v_num;
    data.graph[u_num].push_back({v_num, w_num, t_num});
    data.graph[v_num].push_back({u_num, w_num, t_num});
  }

  auto [result_dist, result_time] = Dijkstra(data, kStartVertex);
  std::cout << result_dist;

  if (result_dist != -1) {
    std::vector<int64_t> path;

    int64_t current_vertex = vertex_count - 1;
    int64_t current_time = result_time;
    while (current_vertex != kStartVertex) {
      path.push_back(current_vertex + 1);
      auto [parent_vertex, parent_time] =
          data.parent[current_vertex][current_time];
      current_vertex = parent_vertex;
      current_time = parent_time;
    }
    path.push_back(kStartVertex + 1);
    std::reverse(path.begin(), path.end());

    std::cout << "\n" << path.size() << "\n";
    for (int64_t vertex : path) {
      std::cout << vertex << " ";
    }
  }
}