#include <algorithm>
#include <iostream>
#include <vector>

using Graph = std::vector<std::vector<std::pair<size_t, size_t>>>;
using CondensedGraph = std::vector<std::vector<size_t>>;

const size_t kMax = 200'002;

struct Data {
  Data(size_t vertex_count, size_t edge_count)
      : graph(vertex_count, std::vector<std::pair<size_t, size_t>>()),
        used1(vertex_count, false),
        used2(vertex_count, false),
        t_in(vertex_count, 0),
        up(vertex_count, 0),
        bridges(edge_count, false),
        comp(vertex_count, 0),
        timer(0) {}

  Graph graph;
  std::vector<bool> used1;
  std::vector<bool> used2;
  std::vector<size_t> t_in;
  std::vector<size_t> up;
  std::vector<bool> bridges;
  std::vector<size_t> comp;
  size_t timer = 0;
};

void FindBridges(Data& data, size_t current, size_t old_edge_id) {
  data.used1[current] = true;
  data.t_in[current] = data.timer;
  data.up[current] = data.timer;
  ++data.timer;
  for (std::pair<size_t, size_t> pair : data.graph[current]) {
    size_t neighbour = pair.first;
    size_t new_edge_id = pair.second;
    if (new_edge_id == old_edge_id) {
      continue;
    }
    if (data.used1[neighbour]) {
      data.up[current] = std::min(data.up[current], data.t_in[neighbour]);
    } else {
      FindBridges(data, neighbour, new_edge_id);
      data.up[current] = std::min(data.up[current], data.up[neighbour]);
      if (data.t_in[current] < data.up[neighbour]) {
        data.bridges[pair.second] = true;
      }
    }
  }
}

void MakeKC(Data& data, size_t vertex, size_t count) {
  data.used2[vertex] = true;
  data.comp[vertex] = count;
  for (std::pair<size_t, size_t> pair : data.graph[vertex]) {
    size_t neighbour = pair.first;
    size_t edge_id = pair.second;
    if (!data.used2[neighbour] && !data.bridges[edge_id]) {
      MakeKC(data, neighbour, count);
    }
  }
}

size_t CountLeafs(CondensedGraph& condensed_graph, size_t vertex,
                  std::vector<bool>& used, size_t count = 0) {
  used[vertex] = true;
  if (condensed_graph[vertex].size() == 2) {
    ++count;
  }
  for (size_t neighbour : condensed_graph[vertex]) {
    if (!used[neighbour]) {
      count = CountLeafs(condensed_graph, neighbour, used, count);
    }
  }
  return count;
}

int main() {
  size_t vertex_count;
  size_t edge_count;
  std::cin >> vertex_count;
  std::cin >> edge_count;

  Data data(vertex_count, edge_count);

  for (size_t i = 0; i < edge_count; ++i) {
    size_t u_num;
    size_t v_num;
    std::cin >> u_num >> v_num;
    --u_num;
    --v_num;
    data.graph[u_num].push_back({v_num, i});
    data.graph[v_num].push_back({u_num, i});
  }

  for (size_t index = 0; index < vertex_count; ++index) {
    if (!data.used1[index]) {
      FindBridges(data, index, kMax);
    }
  }

  size_t count = 0;
  for (size_t index = 0; index < vertex_count; ++index) {
    if (!data.used2[index]) {
      MakeKC(data, index, count);
      ++count;
    }
  }

  CondensedGraph condensed_graph(count, std::vector<size_t>());
  for (size_t u_num = 0; u_num < vertex_count; ++u_num) {
    for (std::pair<size_t, size_t> pair : data.graph[u_num]) {
      size_t v_num = pair.first;
      size_t edge_id = pair.second;
      if (data.bridges[edge_id]) {
        size_t u_con = data.comp[u_num];
        size_t v_con = data.comp[v_num];
        condensed_graph[u_con].push_back(v_con);
        condensed_graph[v_con].push_back(u_con);
      }
    }
  }

  std::vector<bool> used(count, false);
  size_t count_leafs = CountLeafs(condensed_graph, 0, used);
  std::cout << (count_leafs + 1) / 2;
}