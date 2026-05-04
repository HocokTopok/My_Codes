#include <algorithm>
#include <iostream>
#include <vector>

using Edge = std::pair<size_t, size_t>;
using Graph = std::vector<std::vector<Edge>>;

const size_t kNoParent = 200'002;

struct Data {
  Data(size_t vertex_count)
      : graph(vertex_count, std::vector<Edge>()),
        used(vertex_count, false),
        t_in(vertex_count, 0),
        up(vertex_count, 0),
        timer(0) {}

  Graph graph;
  std::vector<bool> used;
  std::vector<size_t> t_in;
  std::vector<size_t> up;
  std::vector<size_t> bridges;
  size_t timer = 0;
};

void ModifiedDFS(Data& data, size_t current, size_t old_edge_id = kNoParent) {
  data.used[current] = true;
  data.t_in[current] = data.timer;
  data.up[current] = data.timer;

  ++data.timer;

  for (const auto& [neighbour, new_edge_id] : data.graph[current]) {
    if (new_edge_id == old_edge_id) {
      continue;
    }

    if (data.used[neighbour]) {
      data.up[current] = std::min(data.up[current], data.t_in[neighbour]);

    } else {
      ModifiedDFS(data, neighbour, new_edge_id);
      data.up[current] = std::min(data.up[current], data.up[neighbour]);

      if (data.t_in[current] < data.up[neighbour]) {
        data.bridges.push_back(new_edge_id);
      }
    }
  }
}

int main() {
  size_t vertex_count;
  size_t edge_count;
  std::cin >> vertex_count;
  std::cin >> edge_count;

  Data data(vertex_count);

  for (size_t edge_id = 1; edge_id <= edge_count; ++edge_id) {
    size_t first_vertex;
    size_t second_vertex;
    std::cin >> first_vertex >> second_vertex;
    --first_vertex;
    --second_vertex;
    data.graph[first_vertex].push_back({second_vertex, edge_id});
    data.graph[second_vertex].push_back({first_vertex, edge_id});
  }

  for (size_t index = 0; index < vertex_count; ++index) {
    if (!data.used[index]) {
      ModifiedDFS(data, index);
    }
  }
  std::sort(data.bridges.begin(), data.bridges.end());

  std::cout << data.bridges.size() << "\n";
  for (size_t bridge : data.bridges) {
    std::cout << bridge << " ";
  }
}