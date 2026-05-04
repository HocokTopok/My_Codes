#include <algorithm>
#include <iostream>
#include <vector>

const size_t kStartVertex = 0;
const size_t kLogN = 18;

using Pair = std::pair<size_t, size_t>;
using Graph = std::vector<std::vector<size_t>>;

struct Data {
  Data(size_t vertex_count)
      : graph(vertex_count),
        used(vertex_count, false),
        depth(vertex_count, 0),
        up(vertex_count, std::vector<size_t>(kLogN, vertex_count)),
        vertex_count(vertex_count) {}

  Graph graph;
  std::vector<bool> used;
  std::vector<size_t> depth;
  std::vector<std::vector<size_t>> up;
  std::vector<Pair> queries;
  std::vector<size_t> results;
  size_t vertex_count;
};

Data Input() {
  size_t vertex_count;
  std::cin >> vertex_count;

  Data data(vertex_count);

  for (size_t index = 0; index < vertex_count - 1; ++index) {
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

  return data;
}

void Output(const std::vector<size_t> kResults) {
  for (size_t result : kResults) {
    std::cout << result << "\n";
  }
}

void DFS(Data& data, size_t current_vertex, size_t parent_vertex,
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

size_t LCA(Data& data, size_t first, size_t second) {
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

size_t Query(Data& data, size_t first, size_t second) {
  size_t result = data.depth[first] + data.depth[second] -
                  (2 * data.depth[LCA(data, first, second)]);
  return result;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  Data data = Input();
  DFS(data, kStartVertex, data.vertex_count);

  for (auto [u_num, v_num] : data.queries) {
    data.results.push_back(Query(data, u_num, v_num));
  }
  Output(data.results);
}