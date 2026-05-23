#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>

using Pair = std::pair<int64_t, int64_t>;

static const int64_t kMax = (1LL << 60);
static const int64_t kStartVertex = 0;
static const int64_t kNoIndex = -1;

struct Edge {
  Edge(int64_t vertex_to, int64_t capacity, int64_t reverse_index,
       int64_t input_index)
      : vertex_to(vertex_to),
        capacity(capacity),
        ost_capacity(capacity),
        reverse_index(reverse_index),
        input_index(input_index) {}

  int64_t vertex_to;
  int64_t capacity;
  int64_t ost_capacity;
  int64_t reverse_index;
  int64_t input_index;
};

struct Data {
  Data(int64_t vertex_count, int64_t edge_count)
      : edges(vertex_count),
        level(vertex_count),
        ptr(vertex_count),
        res_potok(edge_count),
        end_vertex(vertex_count - 1),
        vertex_count(vertex_count) {}

  std::vector<std::vector<Edge>> edges;
  std::vector<int64_t> level;
  std::vector<int64_t> ptr;
  std::vector<int64_t> res_potok;
  int64_t end_vertex;
  int64_t vertex_count;
};

Data Input() {
  int64_t vertex_count;
  size_t edge_count;
  std::cin >> vertex_count >> edge_count;

  Data data(vertex_count, edge_count);

  for (size_t index = 0; index < edge_count; ++index) {
    int64_t u_num;
    int64_t v_num;
    int64_t w_num;
    std::cin >> u_num >> v_num >> w_num;
    --u_num;
    --v_num;

    int64_t rev_u = (int)data.edges[v_num].size();
    int64_t rev_v = (int)data.edges[u_num].size();

    data.edges[u_num].push_back(Edge(v_num, w_num, rev_u, index));
    data.edges[v_num].push_back(Edge(u_num, 0, rev_v, kNoIndex));
  }

  return data;
}

bool BFS(Data& data, int64_t start_vertex = kStartVertex) {
  std::queue<int64_t> queue;
  data.level[start_vertex] = 0;
  queue.push(start_vertex);

  while (!queue.empty()) {
    int64_t vertex = queue.front();
    queue.pop();

    for (Edge edge : data.edges[vertex]) {
      int64_t new_vertex = edge.vertex_to;
      int64_t new_capacity = edge.ost_capacity;

      if (data.level[new_vertex] == -1 && new_capacity != 0) {
        data.level[new_vertex] = data.level[vertex] + 1;
        queue.push(new_vertex);
      }
    }
  }

  return data.level[data.end_vertex] != -1;
}

int64_t DFS(Data& data, int64_t current_vertex = kStartVertex,
            int64_t delta = kMax) {
  if (current_vertex == data.end_vertex) {
    return delta;
  }

  for (int64_t& index = data.ptr[current_vertex];
       index < (int)data.edges[current_vertex].size(); ++index) {
    Edge& edge = data.edges[current_vertex][index];

    if (data.level[edge.vertex_to] != data.level[current_vertex] + 1 ||
        edge.ost_capacity == 0) {
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

int64_t Dinica(Data& data) {
  int64_t result = 0;

  while (true) {
    std::fill(data.level.begin(), data.level.end(), -1);
    bool not_max = BFS(data);

    if (!not_max) {
      break;
    }

    std::fill(data.ptr.begin(), data.ptr.end(), 0);

    while (true) {
      int64_t dop_potok = DFS(data);

      if (dop_potok == 0) {
        break;
      }

      result += dop_potok;
    }
  }

  return result;
}

void Output(Data& data, int64_t result) {
  for (int64_t vertex = 0; vertex < data.vertex_count; ++vertex) {
    for (Edge edge : data.edges[vertex]) {
      if (edge.input_index != kNoIndex) {
        data.res_potok[edge.input_index] = edge.capacity - edge.ost_capacity;
      }
    }
  }

  std::cout << result << "\n";
  for (int64_t potok : data.res_potok) {
    std::cout << potok << "\n";
  }
}

int main() {
  Data data = Input();
  int64_t result = Dinica(data);
  Output(data, result);
}