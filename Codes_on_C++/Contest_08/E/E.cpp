#include <algorithm>
#include <iostream>
#include <vector>

static const size_t kTrash = 0;
static const size_t kIstok = 0;
static const size_t kStok = 1;
static const size_t kNumberOfFictiveVertices = 2;
static const size_t kMax = (1ULL << 60);

struct Edge {
  Edge(size_t vertex_to, size_t capacity, size_t reverse_index)
      : vertex_to(vertex_to),
        ost_capacity(capacity),
        capacity(capacity),
        reverse_index(reverse_index) {}

  size_t vertex_to;
  size_t ost_capacity;
  size_t capacity;
  size_t reverse_index;
};

using Graph = std::vector<std::vector<Edge>>;

struct Data {
  Data(size_t vertex_count)
      : graph(vertex_count + kNumberOfFictiveVertices),
        init_weight(vertex_count + kNumberOfFictiveVertices, 0),
        used(vertex_count + kNumberOfFictiveVertices, false),
        vertex_count(vertex_count + kNumberOfFictiveVertices) {}

  Graph graph;
  std::vector<size_t> init_weight;
  std::vector<bool> used;
  size_t res_potok;
  size_t current_limit;
  size_t min_limit;
  size_t max_limit;
  size_t vertex_count;
};

Data Input() {
  size_t vertex_count;
  size_t edge_count;
  std::cin >> vertex_count >> edge_count;

  Data data(vertex_count);
  size_t sum_weight = 0;
  size_t max_weight = 0;

  for (size_t u_num = kNumberOfFictiveVertices; u_num < data.vertex_count;
       ++u_num) {
    std::cin >> data.init_weight[u_num];
    sum_weight += data.init_weight[u_num];
    max_weight = std::max(max_weight, data.init_weight[u_num]);

    size_t rev_u = data.graph[kIstok].size();
    size_t rev_v = data.graph[u_num].size();

    data.graph[kIstok].push_back(Edge(u_num, data.init_weight[u_num], rev_v));
    data.graph[u_num].push_back(Edge(kIstok, 0, rev_u));

    rev_u = data.graph[kStok].size();
    rev_v = data.graph[u_num].size();

    data.graph[u_num].push_back(Edge(kStok, kTrash, rev_u));
    data.graph[kStok].push_back(Edge(u_num, 0, rev_v));
  }

  data.res_potok = sum_weight;
  data.min_limit = (sum_weight + vertex_count - 1) / vertex_count;
  data.max_limit = max_weight;

  for (size_t index = 0; index < edge_count; ++index) {
    size_t u_num;
    size_t v_num;
    std::cin >> u_num;
    std::cin >> v_num;
    ++u_num;
    ++v_num;

    size_t rev_u = data.graph[v_num].size();
    size_t rev_v = data.graph[u_num].size();

    data.graph[u_num].push_back(Edge(v_num, sum_weight, rev_u));
    data.graph[v_num].push_back(Edge(u_num, 0, rev_v));
  }

  return data;
}

void Update(Data& data) {
  for (size_t vertex = 0; vertex < data.vertex_count; ++vertex) {
    for (Edge& edge : data.graph[vertex]) {
      if (edge.vertex_to == kStok) {
        edge.capacity = data.current_limit;
      }
      edge.ost_capacity = edge.capacity;
    }
  }
}

size_t GetStartScale(size_t number) {
  size_t dva = 1;

  while ((dva << 1) <= number) {
    dva <<= 1;
  }

  return dva;
}

size_t DFS(Data& data, size_t scale, size_t current_vertex = kIstok,
           size_t delta = kMax) {
  if (current_vertex == kStok) {
    return delta;
  }

  if (data.used[current_vertex]) {
    return 0;
  }

  data.used[current_vertex] = true;

  for (Edge& edge : data.graph[current_vertex]) {
    if (data.used[edge.vertex_to] || edge.ost_capacity < scale) {
      continue;
    }

    size_t res_potok =
        DFS(data, scale, edge.vertex_to, std::min(delta, edge.ost_capacity));
    if (res_potok == 0) {
      continue;
    }

    edge.ost_capacity -= res_potok;
    data.graph[edge.vertex_to][edge.reverse_index].ost_capacity += res_potok;

    return res_potok;
  }

  return 0;
}

size_t FordFulkerson(Data& data) {
  size_t result = 0;
  size_t scale = GetStartScale(data.res_potok);

  while (scale > 0) {
    while (true) {
      std::fill(data.used.begin(), data.used.end(), false);
      size_t dop_potok = DFS(data, scale);

      if (dop_potok == 0) {
        break;
      }

      result += dop_potok;
    }

    scale >>= 1;
  }

  return result;
}

size_t FlowScaling(Data& data) {
  while (data.min_limit < data.max_limit) {
    data.current_limit = (data.min_limit + data.max_limit) / 2;
    Update(data);
    size_t potok = FordFulkerson(data);

    if (potok == data.res_potok) {
      data.max_limit = data.current_limit;
    } else {
      data.min_limit = data.current_limit + 1;
    }
  }

  return data.min_limit;
}

void Output(size_t result) { std::cout << result; }

int main() {
  Data data = Input();
  size_t result = FlowScaling(data);
  Output(result);
}