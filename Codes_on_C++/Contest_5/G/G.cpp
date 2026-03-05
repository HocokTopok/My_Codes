#include <algorithm>
#include <iostream>
#include <vector>

using Graph = std::vector<std::vector<std::pair<size_t, size_t>>>;

const size_t kMax = 200'002;

struct Data {
  Data(size_t vertex_count)
      : graph(vertex_count, std::vector<std::pair<size_t, size_t>>()),
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

void ModifiedDFS(Data& data, size_t current, size_t old_edge_id) {
  data.used[current] = true;
  data.t_in[current] = data.timer;
  data.up[current] = data.timer;
  ++data.timer;
  for (std::pair<size_t, size_t> pair : data.graph[current]) {
    size_t neighbour = pair.first;
    size_t new_edge_id = pair.second;
    if (new_edge_id == old_edge_id) {
      continue;
    }
    if (data.used[neighbour]) {
      data.up[current] = std::min(data.up[current], data.t_in[neighbour]);
    } else {
      ModifiedDFS(data, neighbour, new_edge_id);
      data.up[current] = std::min(data.up[current], data.up[neighbour]);
      if (data.t_in[current] < data.up[neighbour]) {
        data.bridges.push_back(pair.second);
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

  for (size_t i = 1; i <= edge_count; ++i) {
    size_t u_num;
    size_t v_num;
    std::cin >> u_num >> v_num;
    --u_num;
    --v_num;
    data.graph[u_num].push_back({v_num, i});
    data.graph[v_num].push_back({u_num, i});
  }

  for (size_t index = 0; index < vertex_count; ++index) {
    if (!data.used[index]) {
      ModifiedDFS(data, index, kMax);
    }
  }

  std::cout << data.bridges.size() << "\n";
  std::sort(data.bridges.begin(), data.bridges.end());
  for (auto bridge : data.bridges) {
    std::cout << bridge << " ";
  }
}