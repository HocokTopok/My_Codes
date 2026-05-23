#include <iostream>
#include <map>
#include <string>
#include <vector>

static const size_t kAlone = static_cast<size_t>(-1);

enum Status { Broken, Empty, White, Black };

struct Edge {
  Edge(size_t x_first, size_t y_first, size_t x_second, size_t y_second)
      : x_first(x_first),
        y_first(y_first),
        x_second(x_second),
        y_second(y_second) {}

  size_t x_first;
  size_t y_first;
  size_t x_second;
  size_t y_second;
};

struct Data {
  Data(size_t x_border, size_t y_border, int duo_price, int once_price)
      : table(y_border, std::vector<size_t>(x_border)),
        x_border(x_border),
        y_border(y_border),
        duo_price(duo_price),
        once_price(once_price) {}

  std::vector<std::vector<size_t>> table;
  std::vector<std::pair<size_t, size_t>> verteces;
  std::vector<Edge> edges;
  size_t x_border;
  size_t y_border;
  int duo_price;
  int once_price;
};

struct KunoData {
  KunoData(size_t first_vertex_count, size_t second_vertex_count)
      : left_part(first_vertex_count),
        right_part(second_vertex_count, kAlone),
        used(first_vertex_count),
        vertex_count(first_vertex_count),
        dop_vertex_count(second_vertex_count) {}

  std::vector<std::vector<size_t>> left_part;
  std::vector<size_t> right_part;
  std::vector<bool> used;
  size_t vertex_count;
  size_t dop_vertex_count;
};

Data Input() {
  size_t y_border;
  size_t x_border;
  int duo_price;
  int once_price;
  std::cin >> y_border >> x_border >> duo_price >> once_price;

  Data data(x_border, y_border, duo_price, once_price);

  for (size_t row = 0; row < y_border; ++row) {
    std::string str;
    std::cin >> str;
    for (size_t col = 0; col < x_border; ++col) {
      if (str[col] == '*') {
        data.table[row][col] = Empty;
        data.verteces.push_back({row, col});
      } else {
        data.table[row][col] = Broken;
      }
    }
  }

  return data;
}

void GetEdges(Data& data) {
  for (size_t row = 0; row < data.y_border; ++row) {
    for (size_t col = 0; col < data.x_border; ++col) {
      if (row > 0 && data.table[row][col] == White &&
          data.table[row - 1][col] == Black) {
        data.edges.push_back(Edge(row, col, row - 1, col));
      }
      if (col > 0 && data.table[row][col] == White &&
          data.table[row][col - 1] == Black) {
        data.edges.push_back(Edge(row, col, row, col - 1));
      }
      if (row + 1 < data.y_border && data.table[row][col] == White &&
          data.table[row + 1][col] == Black) {
        data.edges.push_back(Edge(row, col, row + 1, col));
      }
      if (col + 1 < data.x_border && data.table[row][col] == White &&
          data.table[row][col + 1] == Black) {
        data.edges.push_back(Edge(row, col, row, col + 1));
      }
    }
  }
}

bool DFS(KunoData& data, size_t current_vertex) {
  if (data.used[current_vertex]) {
    return false;
  }

  data.used[current_vertex] = true;

  for (size_t new_vertex : data.left_part[current_vertex]) {
    if (data.right_part[new_vertex] == kAlone ||
        DFS(data, data.right_part[new_vertex])) {
      data.right_part[new_vertex] = current_vertex;
      return true;
    }
  }

  return false;
}

void Kuno(KunoData& data) {
  for (size_t start_vertex = 0; start_vertex < data.vertex_count;
       ++start_vertex) {
    std::fill(data.used.begin(), data.used.end(), false);
    DFS(data, start_vertex);
  }
}

int CountCells(Data& data) {
  int result = 0;

  for (size_t row = 0; row < data.y_border; ++row) {
    for (size_t col = 0; col < data.x_border; ++col) {
      if (data.table[row][col] == Empty) {
        ++result;
      }
    }
  }

  return result;
}

int CountResult(const Data& data, const KunoData& kuno_data, int cells_count) {
  int duo_count = 0;

  for (size_t i = 0; i < kuno_data.dop_vertex_count; ++i) {
    if (kuno_data.right_part[i] != kAlone) {
      ++duo_count;
    }
  }

  return (duo_count * data.duo_price) +
         ((cells_count - (duo_count * 2)) * data.once_price);
}

int Process(Data& data) {
  int cells_count = CountCells(data);

  if (data.duo_price >= data.once_price * 2) {
    return cells_count * data.once_price;
  }

  for (auto [row, col] : data.verteces) {
    if (data.table[row][col] < 2) {
      data.table[row][col] = ((row + col) % 2 == 0 ? White : Black);
    }
  }

  GetEdges(data);

  std::map<std::pair<size_t, size_t>, size_t> white_map;
  std::map<std::pair<size_t, size_t>, size_t> black_map;
  size_t white_count = 0;
  size_t black_count = 0;

  for (auto [row, col] : data.verteces) {
    if (data.table[row][col] == White) {
      white_map[{row, col}] = white_count++;
    } else {
      black_map[{row, col}] = black_count++;
    }
  }

  KunoData kuno_data(white_count, black_count);

  for (const Edge& edge : data.edges) {
    size_t w_id = white_map[{edge.x_first, edge.y_first}];
    size_t b_id = black_map[{edge.x_second, edge.y_second}];
    kuno_data.left_part[w_id].push_back(b_id);
  }

  Kuno(kuno_data);
  int result = CountResult(data, kuno_data, cells_count);

  return result;
}

void Output(int result) { std::cout << result; }

int main() {
  Data data = Input();
  int result = Process(data);
  Output(result);
}
