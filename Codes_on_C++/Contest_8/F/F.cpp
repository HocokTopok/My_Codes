#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <queue>
#include <vector>

using Pair = std::pair<int64_t, int64_t>;

static const size_t kMax = (1LL << 60);
static const size_t kIstok = 0;
static const size_t kStok = 1;
static const size_t kNumberOfFictiveVertices = 2;
static const double kAccuracy = 1e-7;
static const size_t kAccurace = 10;

struct Edge {
  Edge(size_t vertex_to, size_t capacity, size_t reverse_index,
       double dist = 0.0)
      : vertex_to(vertex_to),
        dist(dist),
        capacity(capacity),
        ost_capacity(capacity),
        reverse_index(reverse_index) {}

  size_t vertex_to;
  double dist;
  size_t capacity;
  size_t ost_capacity;
  size_t reverse_index;
};

using Graph = std::vector<std::vector<Edge>>;

struct Data {
  Data(size_t vertex_count, size_t participant_count, size_t station_count)
      : graph(vertex_count),
        participant_coord(participant_count),
        station_coord(station_count),
        level(vertex_count),
        ptr(vertex_count),
        end_vertex(kStok),
        max_limit(0.0),
        participant_count(participant_count),
        vertex_count(vertex_count) {}

  Graph graph;
  std::vector<Pair> participant_coord;
  std::vector<Pair> station_coord;
  std::vector<int64_t> level;
  std::vector<size_t> ptr;
  size_t end_vertex;
  double max_limit;
  size_t participant_count;
  size_t vertex_count;
};

double FindDist(const Pair& first, const Pair& second) {
  int64_t x1 = first.first;
  int64_t y1 = first.second;
  int64_t x2 = second.first;
  int64_t y2 = second.second;

  int64_t x_delta = x1 - x2;
  int64_t y_delta = y1 - y2;

  int64_t kvadrat = x_delta * x_delta + y_delta * y_delta;
  double result = std::sqrt(static_cast<double>(kvadrat));

  return result;
}

Data Input() {
  size_t participant_count;
  size_t station_count;
  std::cin >> participant_count >> station_count;

  Data data(participant_count + station_count + kNumberOfFictiveVertices,
            participant_count, station_count);

  for (size_t index = 0; index < participant_count; ++index) {
    int64_t x_num;
    int64_t y_num;
    std::cin >> x_num >> y_num;

    data.participant_coord[index] = {x_num, y_num};

    size_t u_num = index + kNumberOfFictiveVertices;
    size_t rev_u = data.graph[kStok].size();
    size_t rev_v = data.graph[u_num].size();

    data.graph[u_num].push_back(Edge(kStok, 1, rev_u));
    data.graph[kStok].push_back(Edge(u_num, 0, rev_v));
  }

  for (size_t index = 0; index < station_count; ++index) {
    int64_t x_num;
    int64_t y_num;
    size_t w_num;
    std::cin >> x_num >> y_num >> w_num;

    data.station_coord[index] = {x_num, y_num};

    size_t v_num = index + kNumberOfFictiveVertices + participant_count;
    size_t rev_u = data.graph[v_num].size();
    size_t rev_v = data.graph[kIstok].size();

    data.graph[kIstok].push_back(Edge(v_num, w_num, rev_u));
    data.graph[v_num].push_back(Edge(kIstok, 0, rev_v));
  }

  for (size_t s_index = 0; s_index < station_count; ++s_index) {
    size_t u_num = s_index + kNumberOfFictiveVertices + participant_count;
    for (size_t p_index = 0; p_index < participant_count; ++p_index) {
      size_t v_num = p_index + kNumberOfFictiveVertices;

      double distance = FindDist(data.participant_coord[p_index],
                                 data.station_coord[s_index]);
      data.max_limit = std::max(distance, data.max_limit);

      size_t rev_u = data.graph[v_num].size();
      size_t rev_v = data.graph[u_num].size();

      data.graph[u_num].push_back(Edge(v_num, 1, rev_u, distance));
      data.graph[v_num].push_back(Edge(u_num, 0, rev_v, distance));
    }
  }

  return data;
}

bool BFS(Data& data, double scale, size_t start_vertex = kIstok) {
  std::queue<size_t> queue;
  data.level[start_vertex] = 0;
  queue.push(start_vertex);

  while (!queue.empty()) {
    size_t vertex = queue.front();
    queue.pop();

    for (Edge& edge : data.graph[vertex]) {
      if (edge.dist > scale) {
        continue;
      }

      size_t new_vertex = edge.vertex_to;
      size_t new_capacity = edge.ost_capacity;

      if (data.level[new_vertex] == -1 && new_capacity != 0) {
        data.level[new_vertex] = data.level[vertex] + 1;
        queue.push(new_vertex);
      }
    }
  }

  return data.level[data.end_vertex] != -1;
}

size_t DFS(Data& data, double scale, size_t current_vertex = kIstok,
           size_t delta = kMax) {
  if (current_vertex == data.end_vertex) {
    return delta;
  }

  for (size_t& index = data.ptr[current_vertex];
       index < data.graph[current_vertex].size(); ++index) {
    Edge& edge = data.graph[current_vertex][index];

    if (data.level[edge.vertex_to] != data.level[current_vertex] + 1 ||
        edge.ost_capacity == 0 || edge.dist > scale) {
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

size_t Dinica(Data& data, double scale) {
  size_t result = 0;

  while (true) {
    std::fill(data.level.begin(), data.level.end(), -1);
    bool not_max = BFS(data, scale);

    if (!not_max) {
      break;
    }

    std::fill(data.ptr.begin(), data.ptr.end(), 0);

    while (true) {
      size_t dop_potok = DFS(data, scale);

      if (dop_potok == 0) {
        break;
      }

      result += dop_potok;
    }
  }

  return result;
}

void Update(Data& data) {
  for (size_t vertex = 0; vertex < data.vertex_count; ++vertex) {
    for (Edge& edge : data.graph[vertex]) {
      edge.ost_capacity = edge.capacity;
    }
  }
}

double Process(Data& data) {
  double min_limit = 0.0;
  double max_limit = data.max_limit;

  while (max_limit - min_limit > kAccuracy) {
    double mid = (max_limit + min_limit) / 2;

    Update(data);
    size_t res_potok = Dinica(data, mid);

    if (res_potok == data.participant_count) {
      max_limit = mid;
    } else {
      min_limit = mid;
    }
  }

  return max_limit;
}

void Output(double result) {
  std::cout << std::fixed << std::setprecision(kAccurace) << result << "\n";
}

int main() {
  Data data = Input();
  double result = Process(data);
  Output(result);
}