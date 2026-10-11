#include <iostream>
#include <queue>
#include <string>
#include <cstdint>
#include <algorithm>
#include <vector>

struct Dot {
  Dot() = default;
  Dot(size_t x, size_t y) : row(x), col(y) {}

  size_t row = 0;
  size_t col = 0;
};

bool operator==(const Dot& lhs, const Dot& rhs) noexcept {
  return lhs.row == rhs.row && lhs.col == rhs.col;
}

struct Data {
  Data(size_t n, size_t m) : n(n), m(m), lab(n), result(false), parent_from_start(n, std::vector<char>(m)), parent_from_finish(n, std::vector<char>(m)), used_from_start(n, std::vector<uint8_t>(m, false)), used_from_finish(n, std::vector<uint8_t>(m, false)) {}

  size_t n;
  size_t m;
  Dot start;
  Dot finish;
  Dot meeting;
  bool result;
  std::vector<std::string> lab;
  std::vector<std::vector<char>> parent_from_start;
  std::vector<std::vector<char>> parent_from_finish;
  std::vector<std::vector<uint8_t>> used_from_start;
  std::vector<std::vector<uint8_t>> used_from_finish;
};

void FindFS(Data& data) noexcept {
  size_t remaining = 2;
  for (size_t i = 0; i < data.n; ++i) {
    for (size_t j = 0; j < data.m; ++j) {
      if (data.lab[i][j] == 'F') {
        data.finish = Dot(i, j);
        --remaining;
      }
      if (data.lab[i][j] == 'S') {
        data.start = Dot(i, j);
        --remaining;
      }

      if (remaining == 0) {
        return;
      }
    }
  }
}

Data Input() noexcept {
  size_t n;
  size_t m;
  std::cin >> n >> m;

  Data data(n, m);

  for (auto& row : data.lab) {
    std::cin >> row;
  }

  FindFS(data);

  return data;
}

void Output(const Data& data) noexcept {
  std::string path;

  if (!data.result) {
    std::cout << -1;
    return;
  }

  Dot current = data.meeting;

  while (current != data.start) {
    char antistep = data.parent_from_start[current.row][current.col];
    
    switch (antistep) {
      case 'U':
        --current.row;
        path.push_back('D');
        break;
      case 'D':
        ++current.row;
        path.push_back('U');
        break;
      case 'L':
        --current.col;
        path.push_back('R');
        break;
      case 'R':
        ++current.col;
        path.push_back('L');
        break;
    }
  }

  std::reverse(path.begin(), path.end());

  current = data.meeting;
  
  while (current != data.finish) {
    char step = data.parent_from_finish[current.row][current.col];

    switch (step) {
      case 'U':
        --current.row;
        break;
      case 'D':
        ++current.row;
        break;
      case 'L':
        --current.col;
        break;
      case 'R':
        ++current.col;
        break;
    }

    path.push_back(step);
  }

  std::cout << path.size() << '\n';
  std::cout << path;
}

void BFS(Data& data) {
  std::queue<Dot> vertices_from_start;
  std::queue<Dot> vertices_from_finish;
  vertices_from_start.push(data.start);
  vertices_from_finish.push(data.finish);
  data.used_from_start[data.start.row][data.start.col] = true;
  data.used_from_finish[data.finish.row][data.finish.col] = true;

  while (!vertices_from_start.empty() && !vertices_from_finish.empty()) {
    if (vertices_from_start.size() <= vertices_from_finish.size()) {

      size_t layer_size = vertices_from_start.size();

      for (size_t i = 0; i < layer_size; ++i) {
        Dot current = vertices_from_start.front();
        vertices_from_start.pop();

        if (data.lab[current.row + 1][current.col] != '#' && !data.used_from_start[current.row + 1][current.col]) {
          data.used_from_start[current.row + 1][current.col] = true;
          data.parent_from_start[current.row + 1][current.col] = 'U';

          if (data.used_from_finish[current.row + 1][current.col]) {
            data.meeting = Dot(current.row + 1, current.col);
            data.result = true;
            return;
          }

          vertices_from_start.push(Dot(current.row + 1, current.col));
        }

        if (data.lab[current.row - 1][current.col] != '#' && !data.used_from_start[current.row - 1][current.col]) {
          data.used_from_start[current.row - 1][current.col] = true;
          data.parent_from_start[current.row - 1][current.col] = 'D';

          if (data.used_from_finish[current.row - 1][current.col]) {
            data.meeting = Dot(current.row - 1, current.col);
            data.result = true;
            return;
          }

          vertices_from_start.push(Dot(current.row - 1, current.col));
        }

        if (data.lab[current.row][current.col + 1] != '#' && !data.used_from_start[current.row][current.col + 1]) {
          data.used_from_start[current.row][current.col + 1] = true;
          data.parent_from_start[current.row][current.col + 1] = 'L';

          if (data.used_from_finish[current.row][current.col + 1]) {
            data.meeting = Dot(current.row, current.col + 1);
            data.result = true;
            return;
          }

          vertices_from_start.push(Dot(current.row, current.col + 1));
        }

        if (data.lab[current.row][current.col - 1] != '#' && !data.used_from_start[current.row][current.col - 1]) {
          data.used_from_start[current.row][current.col - 1] = true;
          data.parent_from_start[current.row][current.col - 1] = 'R';

          if (data.used_from_finish[current.row][current.col - 1]) {
            data.meeting = Dot(current.row, current.col - 1);
            data.result = true;
            return;
          }

          vertices_from_start.push(Dot(current.row, current.col - 1));
        }
      }
    } else {

      size_t layer_size = vertices_from_finish.size();

      for (size_t i = 0; i < layer_size; ++i) {
        Dot current = vertices_from_finish.front();
        vertices_from_finish.pop();

        if (data.lab[current.row + 1][current.col] != '#' && !data.used_from_finish[current.row + 1][current.col]) {
          data.used_from_finish[current.row + 1][current.col] = true;
          data.parent_from_finish[current.row + 1][current.col] = 'U';

          if (data.used_from_start[current.row + 1][current.col]) {
            data.meeting = Dot(current.row + 1, current.col);
            data.result = true;
            return;
          }

          vertices_from_finish.push(Dot(current.row + 1, current.col));
        }

        if (data.lab[current.row - 1][current.col] != '#' && !data.used_from_finish[current.row - 1][current.col]) {
          data.used_from_finish[current.row - 1][current.col] = true;
          data.parent_from_finish[current.row - 1][current.col] = 'D';

          if (data.used_from_start[current.row - 1][current.col]) {
            data.meeting = Dot(current.row - 1, current.col);
            data.result = true;
            return;
          }

          vertices_from_finish.push(Dot(current.row - 1, current.col));
        }

        if (data.lab[current.row][current.col + 1] != '#' && !data.used_from_finish[current.row][current.col + 1]) {
          data.used_from_finish[current.row][current.col + 1] = true;
          data.parent_from_finish[current.row][current.col + 1] = 'L';

          if (data.used_from_start[current.row][current.col + 1]) {
            data.meeting = Dot(current.row, current.col + 1);
            data.result = true;
            return;
          }

          vertices_from_finish.push(Dot(current.row, current.col + 1));
        }

        if (data.lab[current.row][current.col - 1] != '#' && !data.used_from_finish[current.row][current.col - 1]) {
          data.used_from_finish[current.row][current.col - 1] = true;
          data.parent_from_finish[current.row][current.col - 1] = 'R';

          if (data.used_from_start[current.row][current.col - 1]) {
            data.meeting = Dot(current.row, current.col - 1);
            data.result = true;
            return;
          }

          vertices_from_finish.push(Dot(current.row, current.col - 1));
        }
      }
    }
  }
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  Data data = Input();
  BFS(data);
  Output(data);
}