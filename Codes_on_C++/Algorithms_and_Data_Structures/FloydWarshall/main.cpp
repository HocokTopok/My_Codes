#include <algorithm>
#include <iostream>
#include <vector>

struct Data {
  Data(int64_t vertex_count)
      : edges(vertex_count, std::vector<bool>(vertex_count, false)),
        vertex_count(vertex_count) {}

  std::vector<std::vector<bool>> edges;
  int64_t vertex_count;
};

void FloydWarshall(Data& data) {
  for (int64_t mid_v = 0; mid_v < data.vertex_count; ++mid_v) {
    for (int64_t start_v = 0; start_v < data.vertex_count; ++start_v) {
      for (int64_t end_v = 0; end_v < data.vertex_count; ++end_v) {
        if (data.edges[start_v][mid_v] && data.edges[mid_v][end_v]) {
          data.edges[start_v][end_v] = true;
        }
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
      bool exist;
      std::cin >> exist;
      if (exist) {
        data.edges[v_from][v_to] = true;
      }
    }
  }

  FloydWarshall(data);

  for (int64_t v_from = 0; v_from < vertex_count; ++v_from) {
    for (int64_t v_to = 0; v_to < vertex_count; ++v_to) {
      std::cout << data.edges[v_from][v_to] << " ";
    }
    std::cout << "\n";
  }
}