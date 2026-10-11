#include <algorithm>
#include <iostream>
#include <vector>

static const int64_t kMax = (1LL << 60);
static const int64_t kStartVertex = 0;

struct Edge {
  Edge(int64_t vertex_to, int64_t ost_capacity, int64_t reverse_index)
      : vertex_to(vertex_to),
        ost_capacity(ost_capacity),
        reverse_index(reverse_index) {}

  int64_t vertex_to;
  int64_t ost_capacity;
  int64_t reverse_index;
};

struct Data {
  Data(int64_t vertex_count)
      : edges(vertex_count),
        used(vertex_count, false),
        end_vertex(vertex_count - 1),
        vertex_count(vertex_count) {}

  std::vector<std::vector<Edge>> edges;
  std::vector<bool> used;
  int64_t end_vertex;
  int64_t vertex_count;
};

Data Input() {
  int64_t vertex_count;
  int64_t edge_count;
  std::cin >> vertex_count >> edge_count;

  Data data(vertex_count);

  for (; edge_count > 0; --edge_count) {
    int64_t u_num;
    int64_t v_num;
    int64_t w_num;
    std::cin >> u_num >> v_num >> w_num;
    --u_num;
    --v_num;

    int64_t rev_u = (int)data.edges[v_num].size();
    int64_t rev_v = (int)data.edges[u_num].size();

    data.edges[u_num].push_back(Edge(v_num, w_num, rev_u));
    data.edges[v_num].push_back(Edge(u_num, 0, rev_v));
  }

  return data;
}

int64_t DFS(Data& data, int64_t current_vertex = kStartVertex,
            int64_t delta = kMax) {
  if (current_vertex == data.end_vertex) {
    return delta;
  }

  if (data.used[current_vertex]) {
    return 0;
  }

  data.used[current_vertex] = true;

  for (Edge& edge : data.edges[current_vertex]) {
    if (data.used[edge.vertex_to]) {
      continue;
    }

    if (edge.ost_capacity == 0) {
      continue;
    }

    int64_t res_potok =
        DFS(data, edge.vertex_to, std::min(delta, edge.ost_capacity));
    if (res_potok == 0) {
      continue;
    }

    edge.ost_capacity -= res_potok;
    data.edges[edge.vertex_to][edge.reverse_index].ost_capacity += res_potok;

    return res_potok;
  }

  return 0;
}

int64_t FordFulkerson(Data& data) {
  int64_t result = 0;

  while (true) {
    std::fill(data.used.begin(), data.used.end(), false);
    int64_t dop_potok = DFS(data);

    if (dop_potok == 0) {
      break;
    }

    result += dop_potok;
  }

  return result;
}

void Output(int64_t result) { std::cout << result; }

int main() {
  Data data = Input();
  int64_t result = FordFulkerson(data);
  Output(result);
}