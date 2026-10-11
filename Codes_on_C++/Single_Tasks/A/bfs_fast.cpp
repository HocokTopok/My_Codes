#include <iostream>
#include <queue>
#include <string>
#include <algorithm>
#include <vector>
#include <cstdio>

struct FastInput {
  static const size_t BUFFER_SIZE = 1 << 20;

  char buffer[BUFFER_SIZE];
  size_t pos = 0;
  size_t size = 0;

  char getChar() {
    if (pos >= size) {
      size = std::fread(buffer, 1, BUFFER_SIZE, stdin);
      pos = 0;
    }

    return buffer[pos++];
  }

  size_t readSizeT() {
    char c = getChar();

    while (c < '0' || c > '9') {
      c = getChar();
    }

    size_t value = 0;

    while (c >= '0' && c <= '9') {
      value = value * 10 + (c - '0');
      c = getChar();
    }

    return value;
  }

  void readString(std::string& s, size_t length) {
    s.resize(length);

    char c = getChar();

    while (c == '\n' || c == '\r' || c == ' ') {
      c = getChar();
    }

    s[0] = c;

    for (size_t i = 1; i < length; ++i) {
      s[i] = getChar();
    }
  }
};

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
  Data(size_t n, size_t m) : n(n), m(m), lab(n), result(false), parent(n, std::vector<char>(m)), used(n, std::vector<uint8_t>(m, false)) {}

  size_t n;
  size_t m;
  Dot start;
  Dot finish;
  bool result;
  std::vector<std::string> lab;
  std::vector<std::vector<char>> parent;
  std::vector<std::vector<uint8_t>> used;
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
  FastInput input;

  size_t n = input.readSizeT();
  size_t m = input.readSizeT();

  Data data(n, m);

  for (auto& row : data.lab) {
    input.readString(row, m);
  }

  FindFS(data);

  return data;
}

void Output(const Data& data) noexcept {
  std::string path;
  Dot current = data.finish;

  if (!data.result) {
    std::cout << -1;
    return;
  }
  
  while (current != data.start) {
    char antistep = data.parent[current.row][current.col];
    
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

  std::cout << path.size() << '\n';
  std::cout << path;
}

void BFS(Data& data) {
  std::queue<Dot> queue;
  queue.push(data.start);
  data.used[data.start.row][data.start.col] = true;

  while (!queue.empty()) {
    Dot current = queue.front();
    queue.pop();
    
    if (current == data.finish) {
      data.result = true;
      return;
    }

    if (data.lab[current.row + 1][current.col] != '#' && !data.used[current.row + 1][current.col]) {
      data.used[current.row + 1][current.col] = true;
      data.parent[current.row + 1][current.col] = 'U';
      queue.push(Dot(current.row + 1, current.col));
    }
    if (data.lab[current.row - 1][current.col] != '#' && !data.used[current.row - 1][current.col]) {
      data.used[current.row - 1][current.col] = true;
      data.parent[current.row - 1][current.col] = 'D';
      queue.push(Dot(current.row - 1, current.col));
    }
    if (data.lab[current.row][current.col + 1] != '#' && !data.used[current.row][current.col + 1]) {
      data.used[current.row][current.col + 1] = true;
      data.parent[current.row][current.col + 1] = 'L';
      queue.push(Dot(current.row, current.col + 1));
    }
    if (data.lab[current.row][current.col - 1] != '#' && !data.used[current.row][current.col - 1]) {
      data.used[current.row][current.col - 1] = true;
      data.parent[current.row][current.col - 1] = 'R';
      queue.push(Dot(current.row, current.col - 1));
    }
  }
}

int main() {
  Data data = Input();
  BFS(data);
  Output(data);
}