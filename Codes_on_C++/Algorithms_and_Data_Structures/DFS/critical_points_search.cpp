#include <algorithm>
#include <iostream>
#include <vector>

using Graph = std::vector<std::vector<size_t>>;

const size_t kMax = 200'002;

struct Data {
  Data(size_t vertex_count)
      : graph(vertex_count, std::vector<size_t>()),
        used(vertex_count, false),
        t_in(vertex_count, 0),
        up(vertex_count, 0),
        dots(vertex_count, false),
        timer(0) {}

  Graph graph;
  std::vector<bool> used;
  std::vector<size_t> t_in;
  std::vector<size_t> up;
  std::vector<bool> dots;
  size_t timer = 0;
};

void ModifiedDFS(Data& data, size_t current, size_t parent) {
  data.used[current] = true;
  data.t_in[current] = data.timer;
  data.up[current] = data.timer;
  ++data.timer;
  if (parent == kMax) {
    size_t children = 0;
    for (size_t neighbour : data.graph[current]) {
      if (!data.used[neighbour]) {
        ++children;
        ModifiedDFS(data, neighbour, current);
      }
    }
    if (children > 1) {
      data.dots[current] = true;
    }
  } else {
    for (size_t neighbour : data.graph[current]) {
      if (neighbour == parent) {
        continue;
      }
      if (data.used[neighbour]) {
        data.up[current] = std::min(data.up[current], data.t_in[neighbour]);
      } else {
        ModifiedDFS(data, neighbour, current);
        data.up[current] = std::min(data.up[current], data.up[neighbour]);
        if (data.t_in[current] <= data.up[neighbour]) {
          data.dots[current] = true;
        }
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

  for (size_t i = 0; i < edge_count; ++i) {
    size_t u_num;
    size_t v_num;
    std::cin >> u_num >> v_num;
    --u_num;
    --v_num;
    data.graph[u_num].push_back(v_num);
    data.graph[v_num].push_back(u_num);
  }

  for (size_t index = 0; index < vertex_count; ++index) {
    if (!data.used[index]) {
      ModifiedDFS(data, index, kMax);
    }
  }

  std::vector<size_t> result;
  for (size_t index = 0; index < vertex_count; ++index) {
    if (data.dots[index]) {
      result.push_back(index + 1);
    }
  }

  std::cout << result.size() << "\n";
  for (size_t dot : result) {
    std::cout << dot << "\n";
  }
}