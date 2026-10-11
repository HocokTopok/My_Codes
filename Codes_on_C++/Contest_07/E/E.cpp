#include <algorithm>
#include <functional>
#include <iostream>
#include <set>
#include <vector>

const size_t kLogN = 19;
const size_t kStartVertex = 0;

using Pair = std::pair<size_t, size_t>;
using Graph = std::vector<std::vector<size_t>>;

struct Data {
  Data(size_t vertex_count)
      : graph(vertex_count),
        used(vertex_count, false),
        t_in(vertex_count, 0),
        up(vertex_count, 0),
        timer(0),
        comp(vertex_count, vertex_count),
        vertex_count(vertex_count) {}

  Graph graph;
  std::vector<bool> used;
  std::vector<size_t> t_in;
  std::vector<size_t> up;
  std::vector<Pair> bridges;
  size_t timer = 0;
  std::vector<size_t> comp;
  size_t comp_count = 0;
  std::vector<Pair> queries;
  std::vector<size_t> results;
  size_t vertex_count;
};

struct CondensedData {
  CondensedData(size_t vertex_count, size_t target)
      : graph(vertex_count),
        used(vertex_count, false),
        depth(vertex_count, 0),
        up(vertex_count, std::vector<size_t>(kLogN, vertex_count)),
        vertex_count(vertex_count),
        target(target) {}

  Graph graph;
  std::vector<bool> used;
  std::vector<size_t> depth;
  std::vector<std::vector<size_t>> up;
  size_t vertex_count;
  size_t target;
};

std::pair<Data, CondensedData> Input() {
  size_t vertex_count;
  size_t edge_count;
  size_t target;
  std::cin >> vertex_count >> edge_count >> target;
  --target;

  Data data(vertex_count);
  CondensedData c_data(vertex_count, target);

  for (; edge_count > 0; --edge_count) {
    size_t u_num;
    size_t v_num;
    std::cin >> u_num >> v_num;
    --u_num;
    --v_num;
    data.graph[u_num].push_back(v_num);
    data.graph[v_num].push_back(u_num);
  }

  size_t queries_count;
  std::cin >> queries_count;

  for (; queries_count > 0; --queries_count) {
    size_t u_num;
    size_t v_num;
    std::cin >> u_num >> v_num;
    --u_num;
    --v_num;
    data.queries.push_back({u_num, v_num});
  }

  return {data, c_data};
}

void Output(const std::vector<size_t> kResults) {
  for (size_t result : kResults) {
    std::cout << result << "\n";
  }
}

void ModifiedDFS(Data& data, size_t current, size_t parent) {
  data.used[current] = true;
  data.t_in[current] = data.timer;
  data.up[current] = data.timer;
  ++data.timer;
  for (size_t neighbour : data.graph[current]) {
    if (neighbour == parent) {
      continue;
    }
    if (data.used[neighbour]) {
      data.up[current] = std::min(data.up[current], data.t_in[neighbour]);
    } else {
      ModifiedDFS(data, neighbour, current);
      data.up[current] = std::min(data.up[current], data.up[neighbour]);
      if (data.t_in[current] < data.up[neighbour]) {
        data.bridges.push_back({current, neighbour});
      }
    }
  }
}

void OrderDFS(Data& data, std::set<Pair>& bridge_set, size_t current_vertex,
              size_t id) {
  data.comp[current_vertex] = id;
  for (size_t new_vertex : data.graph[current_vertex]) {
    if (data.comp[new_vertex] == data.vertex_count &&
        !bridge_set.contains({std::min(new_vertex, current_vertex),
                              std::max(new_vertex, current_vertex)})) {
      OrderDFS(data, bridge_set, new_vertex, id);
    }
  }
}

Graph BuildCondensedGraph(Data& data) {
  std::set<Pair> bridge_set;
  for (auto [u_num, v_num] : data.bridges) {
    bridge_set.insert({std::min(u_num, v_num), std::max(u_num, v_num)});
  }

  for (size_t vertex = 0; vertex < data.vertex_count; ++vertex) {
    if (data.comp[vertex] == data.vertex_count) {
      OrderDFS(data, bridge_set, vertex, data.comp_count++);
    }
  }

  Graph condensed_graph(data.comp_count);
  for (auto [u_num, v_num] : data.bridges) {
    condensed_graph[data.comp[u_num]].push_back(data.comp[v_num]);
    condensed_graph[data.comp[v_num]].push_back(data.comp[u_num]);
  }
  return condensed_graph;
}

void DFS(CondensedData& data, size_t current_vertex, size_t parent_vertex,
         size_t depth = 0) {
  data.used[current_vertex] = true;
  data.depth[current_vertex] = depth;
  data.up[current_vertex][0] = parent_vertex;

  for (size_t k_num = 1; k_num < kLogN; ++k_num) {
    if (data.up[current_vertex][k_num - 1] != data.vertex_count) {
      data.up[current_vertex][k_num] =
          data.up[data.up[current_vertex][k_num - 1]][k_num - 1];
    } else {
      data.up[current_vertex][k_num] = data.vertex_count;
    }
  }

  for (size_t new_vertex : data.graph[current_vertex]) {
    if (!data.used[new_vertex]) {
      DFS(data, new_vertex, current_vertex, depth + 1);
    }
  }
}

size_t LCA(CondensedData& data, size_t first, size_t second) {
  if (data.depth[second] > data.depth[first]) {
    std::swap(first, second);
  }
  size_t delta_depth = (data.depth[first] - data.depth[second]);

  for (int64_t k_num = static_cast<int64_t>(kLogN - 1); k_num >= 0; --k_num) {
    if (delta_depth >= (1U << k_num)) {
      first = data.up[first][k_num];
      delta_depth -= (1U << k_num);
    }
  }

  if (first == second) {
    return first;
  }

  for (int64_t k_num = static_cast<int64_t>(kLogN - 1); k_num >= 0; --k_num) {
    size_t first_up = data.up[first][k_num];
    size_t second_up = data.up[second][k_num];

    if (first_up != second_up) {
      first = first_up;
      second = second_up;
    }
  }

  return data.up[first][0];
}

size_t Query(CondensedData& data, size_t first, size_t second) {
  size_t result = data.depth[LCA(data, first, second)];
  return result;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  auto [data, c_data] = Input();

  ModifiedDFS(data, kStartVertex, data.vertex_count);
  c_data.graph = BuildCondensedGraph(data);

  size_t target_comp = data.comp[c_data.target];
  DFS(c_data, target_comp, c_data.vertex_count);

  for (auto [u_num, v_num] : data.queries) {
    data.results.push_back(Query(c_data, data.comp[u_num], data.comp[v_num]));
  }

  Output(data.results);
}