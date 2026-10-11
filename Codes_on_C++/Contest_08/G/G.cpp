#include <algorithm>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

using Pair = std::pair<int64_t, int64_t>;

static const int64_t kMax = (1LL << 60);
static const int64_t kStartVertex = 0;
static const size_t kNumberOfFictiveVertices = 2;
static const size_t kIstok = 0;
static const size_t kStok = 1;

struct Edge {
  Edge(int64_t vertex_to, int64_t capacity, int64_t reverse_index, bool flag)
      : vertex_to(vertex_to),
        capacity(capacity),
        ost_capacity(capacity),
        reverse_index(reverse_index),
        is_real_edge(flag) {}

  int64_t vertex_to;
  int64_t capacity;
  int64_t ost_capacity;
  int64_t reverse_index;
  bool is_real_edge;
};

struct Info {
  Info() = default;

  void Add(const char kLetter) noexcept {
    if (kLetter == '-') {
      ++minus_count;
    } else if (kLetter == 'X') {
      ++blocked_count;
    } else if (kLetter == '?') {
      ++undefined_count;
    } else {
      ++plus_count;
    }
  }

  int64_t NeedMinuses() const noexcept {
    return (static_cast<int64_t>(undefined_count) +
            static_cast<int64_t>(plus_count) -
            static_cast<int64_t>(minus_count)) /
           2;
  }

  size_t minus_count = 0;
  size_t plus_count = 0;
  size_t undefined_count = 0;
  size_t blocked_count = 0;
};

struct Data {
  Data(size_t vertex_count, const std::vector<std::vector<char>>& table,
       size_t row_separator, size_t column_separator)
      : table(table),
        graph(vertex_count),
        level(vertex_count),
        ptr(vertex_count),
        end_vertex(kStok),
        vertex_count(vertex_count),
        row_separator(row_separator),
        column_separator(column_separator) {}

  std::vector<std::vector<char>> table;
  std::vector<std::vector<Edge>> graph;
  std::vector<int64_t> level;
  std::vector<int64_t> ptr;
  int64_t end_vertex;
  size_t vertex_count;
  size_t row_separator;
  size_t column_separator;
};

void RowHelper(Data& data, const std::vector<Info>& row_status,
               const std::vector<Pair>& row_edges) noexcept {
  for (auto [u_num, v_num] : row_edges) {
    int64_t rev_u = (int)data.graph[v_num].size();
    int64_t rev_v = (int)data.graph[u_num].size();

    data.graph[u_num].push_back(
        Edge(v_num, row_status[v_num - kNumberOfFictiveVertices].NeedMinuses(),
             rev_u, true));
    data.graph[v_num].push_back(Edge(u_num, 0, rev_v, false));
  }
}

void ColumnHelper(Data& data, const std::vector<Info>& column_status,
                  const std::vector<Pair>& column_edges,
                  size_t row_vertex_count) noexcept {
  for (auto [u_num, v_num] : column_edges) {
    int64_t rev_u = (int)data.graph[v_num].size();
    int64_t rev_v = (int)data.graph[u_num].size();

    data.graph[u_num].push_back(
        Edge(v_num,
             column_status[u_num - kNumberOfFictiveVertices - row_vertex_count]
                 .NeedMinuses(),
             rev_u, true));
    data.graph[v_num].push_back(Edge(u_num, 0, rev_v, false));
  }
}

void Helper(Data& data, const std::vector<Pair>& between_edges) noexcept {
  for (auto [u_num, v_num] : between_edges) {
    int64_t rev_u = (int)data.graph[v_num].size();
    int64_t rev_v = (int)data.graph[u_num].size();

    data.graph[u_num].push_back(Edge(v_num, 1, rev_u, true));
    data.graph[v_num].push_back(Edge(u_num, 0, rev_v, false));
  }
}

Data Input() {
  size_t row_count;
  size_t column_count;
  std::cin >> row_count >> column_count;

  std::vector<std::vector<char>> table(row_count,
                                       std::vector<char>(column_count));

  std::vector<Info> row_status(row_count, Info());
  std::vector<Info> column_status(column_count, Info());

  for (size_t row = 0; row < row_count; ++row) {
    std::string str;
    std::cin >> str;
    for (size_t column = 0; column < column_count; ++column) {
      char cell = str[column];
      table[row][column] = cell;
      row_status[row].Add(cell);
      column_status[column].Add(cell);
    }
  }

  std::vector<Pair> row_edges;
  std::vector<Pair> column_edges;
  std::vector<Pair> between_edges;

  size_t row_vertex_count = 0;
  for (size_t row = 0; row < row_count; ++row) {
    ++row_vertex_count;
    row_edges.push_back({kIstok, row + kNumberOfFictiveVertices});
  }

  size_t column_vertex_column = 0;
  for (size_t column = 0; column < column_count; ++column) {
    ++column_vertex_column;
    column_edges.push_back(
        {column + kNumberOfFictiveVertices + row_vertex_count, kStok});
  }

  for (size_t row = 0; row < row_count; ++row) {
    for (size_t column = 0; column < column_count; ++column) {
      if (table[row][column] == '?') {
        between_edges.push_back(
            {row + kNumberOfFictiveVertices,
             column + kNumberOfFictiveVertices + row_vertex_count});
      }
    }
  }

  size_t vertex_count =
      row_vertex_count + column_vertex_column + kNumberOfFictiveVertices;
  Data data(vertex_count, table, kNumberOfFictiveVertices,
            kNumberOfFictiveVertices + row_vertex_count);

  RowHelper(data, row_status, row_edges);
  ColumnHelper(data, column_status, column_edges, row_vertex_count);
  Helper(data, between_edges);

  return data;
}

bool BFS(Data& data, int64_t start_vertex = kStartVertex) {
  std::queue<int64_t> queue;
  data.level[start_vertex] = 0;
  queue.push(start_vertex);

  while (!queue.empty()) {
    int64_t vertex = queue.front();
    queue.pop();

    for (Edge edge : data.graph[vertex]) {
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
       index < (int)data.graph[current_vertex].size(); ++index) {
    Edge& edge = data.graph[current_vertex][index];

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
    data.graph[edge.vertex_to][edge.reverse_index].ost_capacity += res_potok;

    return res_potok;
  }

  return 0;
}

void Dinica(Data& data) {
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
    }
  }
}

void Output(Data& data) {
  for (size_t u_num = data.row_separator; u_num < data.column_separator;
       ++u_num) {
    for (Edge edge : data.graph[u_num]) {
      if (!edge.is_real_edge) {
        continue;
      }

      size_t v_num = edge.vertex_to;
      size_t potok = edge.capacity - edge.ost_capacity;
      size_t first_vertex = u_num - data.row_separator;
      size_t second_vertex = v_num - data.column_separator;
      data.table[first_vertex][second_vertex] = (potok == 0) ? '+' : '-';
    }
  }

  for (const std::vector<char>& str : data.table) {
    for (char letter : str) {
      std::cout << letter;
    }
    std::cout << "\n";
  }
}

int main() {
  Data data = Input();
  Dinica(data);
  Output(data);
}