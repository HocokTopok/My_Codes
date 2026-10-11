#include <iostream>
#include <queue>
#include <string>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <cmath>

const int kMax = 1'000'000'000;

struct Dot {
  Dot() = default;
  Dot(int x, int y) : row(x), col(y) {}

  int row = 0;
  int col = 0;
};

bool operator==(const Dot& lhs, const Dot& rhs) noexcept {
  return lhs.row == rhs.row && lhs.col == rhs.col;
}

struct Node {
  Node() = default;
  Node(Dot dot, int priority) : dot(dot), priority(priority) {}
  
  Dot dot;
  int priority;
};

bool operator<(const Node& lhs, const Node& rhs) noexcept {
  return lhs.priority > rhs.priority;
}

struct Data {
  Data(size_t n, size_t m) : n(n), m(m), lab(n), result(false), parent(n, std::vector<char>(m)), used(n, std::vector<uint8_t>(m, false)), dist(n, std::vector<int>(m, kMax)) {}

  size_t n;
  size_t m;
  Dot start;
  Dot finish;
  bool result;
  std::vector<std::string> lab;
  std::vector<std::vector<char>> parent;
  std::vector<std::vector<uint8_t>> used;
  std::vector<std::vector<int>> dist;
};

int FindManhattanDistance(const Data& data, const Dot& dot) noexcept {
  return std::abs(data.finish.row - dot.row) + std::abs(data.finish.col - dot.col);
}

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

void AStar(Data& data) {
  std::priority_queue<Node, std::vector<Node>, std::less<Node>> queue;
  data.dist[data.start.row][data.start.col] = 0;
  queue.push({data.start, FindManhattanDistance(data, data.start)});

  while (!queue.empty()) {
    Node new_node = queue.top();
    Dot current = new_node.dot;
    queue.pop();

    if (data.used[current.row][current.col]) {
      continue;
    }
    data.used[current.row][current.col] = true;
    
    if (current == data.finish) {
      data.result = true;
      return;
    }

    if (data.lab[current.row + 1][current.col] != '#' && !data.used[current.row + 1][current.col] && data.dist[current.row + 1][current.col] > data.dist[current.row][current.col] + 1) {
      data.parent[current.row + 1][current.col] = 'U';
      data.dist[current.row + 1][current.col] = data.dist[current.row][current.col] + 1;
      queue.push({Dot(current.row + 1, current.col), data.dist[current.row + 1][current.col] + FindManhattanDistance(data, Dot(current.row + 1, current.col))});
    }
    if (data.lab[current.row - 1][current.col] != '#' && !data.used[current.row - 1][current.col] && data.dist[current.row - 1][current.col] > data.dist[current.row][current.col] + 1) {
      data.parent[current.row - 1][current.col] = 'D';
      data.dist[current.row - 1][current.col] = data.dist[current.row][current.col] + 1;
      queue.push({Dot(current.row - 1, current.col), data.dist[current.row - 1][current.col] + FindManhattanDistance(data, Dot(current.row - 1, current.col))});
    }
    if (data.lab[current.row][current.col + 1] != '#' && !data.used[current.row][current.col + 1] && data.dist[current.row][current.col + 1] > data.dist[current.row][current.col] + 1) {
      data.parent[current.row][current.col + 1] = 'L';
      data.dist[current.row][current.col + 1] = data.dist[current.row][current.col] + 1;
      queue.push({Dot(current.row, current.col + 1), data.dist[current.row][current.col + 1] + FindManhattanDistance(data, Dot(current.row, current.col + 1))});
    }
    if (data.lab[current.row][current.col - 1] != '#' && !data.used[current.row][current.col - 1] && data.dist[current.row][current.col - 1] > data.dist[current.row][current.col] + 1) {
      data.parent[current.row][current.col - 1] = 'R';
      data.dist[current.row][current.col - 1] = data.dist[current.row][current.col] + 1;
      queue.push({Dot(current.row, current.col - 1), data.dist[current.row][current.col - 1] + FindManhattanDistance(data, Dot(current.row, current.col - 1))});
    }
  }
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  Data data = Input();
  AStar(data);
  Output(data);
}