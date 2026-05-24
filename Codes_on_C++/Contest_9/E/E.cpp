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

  std::vector<Dot> dots;
  std::vector<std::vector<Dot>> layer_vertices;
  std::vector<Dot> queries;
  std::vector<size_t> results;
  Dot start_dot = Dot(kMax, kMax);
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

    if (dot < data.start_dot) {
      data.start_dot = data.dots[i];
    }
  }

  std::cin >> vertex_count;

  for (size_t i = 0; i < vertex_count; ++i) {
    int x_num;
    int y_num;
    std::cin >> x_num >> y_num;

    data.queries.push_back({x_num, y_num});
  }

  EraseDublicates(data);

  return data;
}

bool OnSegment(const Dot& d1, const Dot& d2, const Dot& ds) {
  if (Cross(Vector(d1, d2), Vector(d1, ds)) != 0) {
    return false;
  }
  return ds.x_num >= std::min(d1.x_num, d2.x_num) &&
         ds.x_num <= std::max(d1.x_num, d2.x_num) &&
         ds.y_num >= std::min(d1.y_num, d2.y_num) &&
         ds.y_num <= std::max(d1.y_num, d2.y_num);
}

struct AngleComparator {
  Dot start;

  explicit AngleComparator(const Dot& d0) : start(d0) {}

  bool operator()(const Dot& d1, const Dot& d2) const {
    if (d1 == start) {
      return true;
    }
    if (d2 == start) {
      return false;
    }
    long long cr = Cross(Vector(start, d1), Vector(start, d2));
    if (cr != 0) {
      return cr > 0;
    }
    return CompareDist(start, d1, d2);
  }
};

bool Graham(Data& data) noexcept {
  if (data.dots.empty()) {
    return false;
  }
  std::vector<Dot> vertices;
  Dot start_dot = data.start_dot;
  std::sort(data.dots.begin(), data.dots.end(), AngleComparator(start_dot));
  for (const Dot& dot : data.dots) {
    while (vertices.size() >= 2) {
      const Dot& prev = vertices[vertices.size() - 1];
      const Dot& base = vertices[vertices.size() - 2];
      long long cr = Cross(Vector(base, prev), Vector(base, dot));
      if (cr < 0) {
        vertices.pop_back();
      } else {
        break;
      }
    }
    vertices.push_back(dot);
  }
  if (vertices.empty()) {
    return false;
  }
  data.layer_vertices.push_back(vertices);
  auto is_on_boundary = [&](const Dot& d0) {
    if (std::find(vertices.begin(), vertices.end(), d0) != vertices.end()) {
      return true;
    }
    size_t size = vertices.size();
    for (size_t index = 0; index < size; ++index) {
      const Dot& d1 = vertices[index];
      const Dot& d2 = vertices[(index + 1) % size];
      if (OnSegment(d1, d2, d0)) {
        return true;
      }
    }
    return false;
  };
  std::vector<Dot> remaining_dots;
  Dot next_start(kMax, kMax);
  for (const Dot& dot : data.dots) {
    if (!is_on_boundary(dot)) {
      remaining_dots.push_back(dot);
      if (dot < next_start) {
        next_start = dot;
      }
    }
  }
  data.dots = remaining_dots;
  data.start_dot = next_start;
  return !data.dots.empty();
}

void MultiGraham(Data& data) noexcept {
  while (true) {
    bool result = Graham(data);

    if (!result) {
      break;
    }
  }
}

bool IsInsidePolygon(const std::vector<Dot>& zone, const Dot& dot) {
  size_t vertex_count = zone.size();

  if (vertex_count == 1) {
    return dot == zone[0];
  }

  if (vertex_count == 2) {
    Dot d1 = zone[0];
    Dot d2 = zone[1];

    if (Cross(Vector(d1, d2), Vector(d1, dot)) != 0) {
      return false;
    }
    return dot.x_num >= std::min(d1.x_num, d2.x_num) &&
           dot.x_num <= std::max(d1.x_num, d2.x_num) &&
           dot.y_num >= std::min(d1.y_num, d2.y_num) &&
           dot.y_num <= std::max(d1.y_num, d2.y_num);
  }

  for (size_t index = 0; index < vertex_count; ++index) {
    Dot d1 = zone[index];
    Dot d2 = zone[(index + 1) % vertex_count];

    if (Cross(Vector(d1, d2), Vector(d1, dot)) < 0) {
      return false;
    }
  }

  return true;
}

size_t Query(Data& data, const Dot& dot) noexcept {
  if (data.layer_vertices.empty() ||
      !IsInsidePolygon(data.layer_vertices[0], dot)) {
    return 0;
  }

  int left = 0;
  int right = static_cast<int>(data.layer_vertices.size()) - 1;
  size_t result = 0;

  while (left <= right) {
    int mid = left + (right - left) / 2;

    if (IsInsidePolygon(data.layer_vertices[mid], dot)) {
      result = size_t(mid);
      left = mid + 1;
    } else {
      right = mid - 1;
    }
  }

  return result;
}

void Queries(Data& data) noexcept {
  for (const Dot& query_dot : data.queries) {
    data.results.push_back(Query(data, query_dot));
  }
}

void Output(const Data& data) noexcept {
  for (auto result : data.results) {
    std::cout << result << "\n";
  }
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  Data data = Input();
  MultiGraham(data);
  Queries(data);
  Output(data);
}
