#include <iostream>
#include <vector>

static const size_t kAlone = static_cast<size_t>(-1);

struct Data {
  Data(size_t first_vertex_count, size_t second_vertex_count)
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
  size_t first_vertex_count;
  size_t second_vertex_count;
  std::cin >> first_vertex_count >> second_vertex_count;

  Data data(first_vertex_count, second_vertex_count);

  for (size_t first_vertex = 0; first_vertex < first_vertex_count;
       ++first_vertex) {
    size_t second_vertex;
    std::cin >> second_vertex;
    while (second_vertex != 0) {
      --second_vertex;
      data.left_part[first_vertex].push_back(second_vertex);
      std::cin >> second_vertex;
    }
  }

  return data;
}

bool DFS(Data& data, size_t current_vertex) {
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

void Kuno(Data& data) {
  for (size_t start_vertex = 0; start_vertex < data.vertex_count;
       ++start_vertex) {
    std::fill(data.used.begin(), data.used.end(), false);
    DFS(data, start_vertex);
  }
}

void Output(const Data& data) {
  std::vector<std::pair<size_t, size_t>> results;

  for (size_t i = 0; i < data.dop_vertex_count; ++i) {
    if (data.right_part[i] != kAlone) {
      results.push_back({data.right_part[i], i});
    }
  }

  std::cout << results.size() << "\n";
  for (auto [first, second] : results) {
    std::cout << ++first << " " << ++second << "\n";
  }
}

int main() {
  Data data = Input();
  Kuno(data);
  Output(data);
}