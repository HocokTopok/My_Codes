#include <algorithm>
#include <iostream>
#include <vector>

const int64_t kMax = 100'000;

struct Data {
  Data(int64_t vertex_count)
      : dist(vertex_count, 0),
        parent(vertex_count, -1),
        cycle_start(-1),
        vertex_count(vertex_count) {}

  std::vector<std::tuple<int64_t, int64_t, int64_t>> edges;
  std::vector<int64_t> dist;
  std::vector<int64_t> parent;
  int64_t cycle_start;
  int64_t vertex_count;
};

void FordBellman(Data& data) {
  for (int64_t i = 0; i < data.vertex_count; ++i) {
    for (auto [v_from, v_to, weight] : data.edges) {
      if (data.dist[v_to] > data.dist[v_from] + weight) {
        if (i == data.vertex_count - 1) {
          data.cycle_start = v_from;
        }
        data.dist[v_to] = data.dist[v_from] + weight;
        data.parent[v_to] = v_from;
      }
    }
  }
}

int main() {
  int64_t vertex_count;
  std::cin >> vertex_count;

  Data data(vertex_count);

  for (int64_t v_from = 0; v_from < vertex_count; ++v_from) {
    for (int64_t v_to = 0; v_to < vertex_count; ++v_to) {
      int64_t weight;
      std::cin >> weight;
      if (weight != kMax) {
        data.edges.push_back({v_from, v_to, weight});
      }
    }
  }

  FordBellman(data);

  if (data.cycle_start == -1) {
    std::cout << "NO";
  } else {
    std::cout << "YES\n";

    int64_t start_vertex = data.cycle_start;
    for (int64_t i = 0; i < vertex_count; ++i) {
      start_vertex = data.parent[start_vertex];
    }

    std::vector<int64_t> cycle;
    int64_t current_vertex = data.parent[start_vertex];

    cycle.push_back(start_vertex);

    while (current_vertex != start_vertex) {
      cycle.push_back(current_vertex);
      current_vertex = data.parent[current_vertex];
    }

    cycle.push_back(start_vertex);

    std::reverse(cycle.begin(), cycle.end());

    std::cout << cycle.size() << "\n";
    for (int64_t vertex : cycle) {
      std::cout << vertex + 1 << " ";
    }
  }
}