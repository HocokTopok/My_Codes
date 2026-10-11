#include <algorithm>
#include <iostream>
#include <vector>

static const int kMax = 1'000'000'000;

struct Dot {
  Dot() {}

  Dot(int xa, int ya) : x_num(xa), y_num(ya) {}

  bool operator<(const Dot& other) const {
    return (x_num == other.x_num) ? (y_num < other.y_num)
                                  : (x_num < other.x_num);
  }

  bool operator==(const Dot& other) const {
    return x_num == other.x_num && y_num == other.y_num;
  }

  bool operator!=(const Dot& other) const { return !(*this == other); }

  int x_num = 0;
  int y_num = 0;
};

struct Vector {
  Vector(const Dot& d1, const Dot& d2)
      : x_num(1LL * d2.x_num - d1.x_num), y_num(1LL * d2.y_num - d1.y_num) {}

  long long x_num;
  long long y_num;
};

struct Data {
  Data(size_t vertex_count) : dots(vertex_count) {}

  void AddVertex(const Dot& vertex) { vertices.push_back(vertex); }

  std::vector<Dot> dots;
  std::vector<Dot> vertices;
  Dot start_dot = Dot(kMax, kMax);
  long long square = 0;
};

void EraseDublicates(Data& data) noexcept {
  std::sort(data.dots.begin(), data.dots.end());
  data.dots.erase(std::unique(data.dots.begin(), data.dots.end()),
                  data.dots.end());
}

long long Cross(const Vector& v1, const Vector& v2) noexcept {
  return v1.x_num * v2.y_num - v2.x_num * v1.y_num;
}

long long Dist(const Dot& d1, const Dot& d2) noexcept {
  long long delta_x = 1LL * d1.x_num - d2.x_num;
  long long delta_y = 1LL * d1.y_num - d2.y_num;
  return delta_x * delta_x + delta_y * delta_y;
}

bool CompareDist(const Dot& ds, const Dot& d1, const Dot& d2) noexcept {
  return Dist(ds, d1) < Dist(ds, d2);
}

Data Input() noexcept {
  size_t vertex_count;
  std::cin >> vertex_count;

  Data data(vertex_count);

  for (size_t i = 0; i < vertex_count; ++i) {
    int x_num;
    int y_num;
    std::cin >> x_num >> y_num;

    Dot dot(x_num, y_num);

    data.dots[i] = dot;

    if (x_num < data.start_dot.x_num) {
      data.start_dot = data.dots[i];
    } else if (x_num == data.start_dot.x_num && y_num < data.start_dot.y_num) {
      data.start_dot = data.dots[i];
    }
  }

  EraseDublicates(data);

  return data;
}

void Graham(Data& data) noexcept {
  Dot start_dot = data.start_dot;

  std::sort(data.dots.begin(), data.dots.end(),
            [start_dot](const Dot& d1, const Dot& d2) {
              if (d1 == start_dot) {
                return true;
              }
              if (d2 == start_dot) {
                return false;
              }

              long long cr =
                  Cross(Vector(start_dot, d1), Vector(start_dot, d2));

              if (cr != 0) {
                return cr < 0;
              }

              return CompareDist(start_dot, d1, d2);
            });

  for (const Dot& dot : data.dots) {
    while (data.vertices.size() >= 2) {
      const Dot& prev = data.vertices[data.vertices.size() - 1];
      const Dot& base = data.vertices[data.vertices.size() - 2];

      long long cr = Cross(Vector(base, prev), Vector(base, dot));

      if (cr < 0) {
        break;
      }

      data.vertices.pop_back();
    }

    data.vertices.push_back(dot);
  }
}

void Square(Data& data) noexcept {
  Dot ds;

  for (size_t index = 0; index < data.vertices.size(); ++index) {
    Dot d1 = data.vertices[index];
    Dot d2 = data.vertices[(index + 1) % data.vertices.size()];

    data.square += Cross(Vector(ds, d1), Vector(ds, d2));
  }

  if (data.square < 0) {
    data.square = -data.square;
  }
}

void Output(const std::vector<Dot>& vertices, long long square) noexcept {
  std::cout << vertices.size() << "\n";

  for (const Dot& vertex : vertices) {
    std::cout << vertex.x_num << " " << vertex.y_num << "\n";
  }

  std::cout << square / 2;

  if (square % 2 == 0) {
    std::cout << ".0";
  } else {
    std::cout << ".5";
  }
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  Data data = Input();
  Graham(data);
  Square(data);
  Output(data.vertices, data.square);
}