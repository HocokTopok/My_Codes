#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

static const double kMax = 1e18L;
static const double kOneMinute = 60.0;
static const size_t kPrecision = 15;

struct Dot {
  Dot() {}

  Dot(long long xa, long long ya) : x_num(xa), y_num(ya) {}

  bool operator<(const Dot& other) const {
    return (x_num == other.x_num) ? (y_num < other.y_num)
                                  : (x_num < other.x_num);
  }

  bool operator==(const Dot& other) const {
    return x_num == other.x_num && y_num == other.y_num;
  }

  bool operator!=(const Dot& other) const { return !(*this == other); }

  long long x_num = 0;
  long long y_num = 0;
};

struct Vector {
  Vector(const Dot& d1, const Dot& d2)
      : x_num(1LL * d2.x_num - d1.x_num), y_num(1LL * d2.y_num - d1.y_num) {}

  long long x_num;
  long long y_num;
};

struct Line {
  Line(long long x1, long long y1, long long x2, long long y2)
      : param_a(y1 - y2), param_b(x2 - x1), param_c(x1 * y2 - x2 * y1) {}

  double PointDistance(long long xa, long long ya) const noexcept {
    return std::abs(param_a * xa + param_b * ya + param_c) /
           std::sqrt(param_a * param_a + param_b * param_b);
  }

  long long param_a;
  long long param_b;
  long long param_c;
};

struct Data {
  Data(size_t first_vertex_count, size_t second_vertex_count)
      : first(first_vertex_count), second(second_vertex_count) {}

  std::vector<Dot> first;
  std::vector<Dot> second;
  std::vector<Dot> intersection;
};

size_t GetStart(const std::vector<Dot>& dots) {
  size_t start_index = 0;

  for (size_t i = 1; i < dots.size(); ++i) {
    if (dots[i].y_num < dots[start_index].y_num ||
        (dots[i].y_num == dots[start_index].y_num &&
         dots[i].x_num < dots[start_index].x_num)) {
      start_index = i;
    }
  }

  return start_index;
}

void Normalize(std::vector<Dot>& dots) {
  size_t shift = GetStart(dots);
  std::rotate(dots.begin(), dots.begin() + shift, dots.end());
}

Data Input() noexcept {
  size_t first_vertex_count;
  size_t second_vertex_count;
  std::cin >> first_vertex_count >> second_vertex_count;

  Data data(first_vertex_count, second_vertex_count);

  for (size_t i = 0; i < first_vertex_count; ++i) {
    long long x_num;
    long long y_num;
    std::cin >> x_num >> y_num;

    Dot dot(x_num, y_num);
    data.first[i] = dot;
  }

  for (size_t i = 0; i < second_vertex_count; ++i) {
    long long x_num;
    long long y_num;
    std::cin >> x_num >> y_num;

    Dot dot(-x_num, -y_num);
    data.second[i] = dot;
  }

  Normalize(data.first);
  Normalize(data.second);

  return data;
}

long long Cross(const Vector& v1, const Vector& v2) noexcept {
  return v1.x_num * v2.y_num - v2.x_num * v1.y_num;
}

void SummaMinkovskogo(Data& data) noexcept {
  if (data.first.empty() || data.second.empty()) {
    return;
  }

  size_t first_index = 0;
  size_t second_index = 0;
  size_t first_size = data.first.size();
  size_t second_size = data.second.size();

  data.intersection.push_back({data.first[0].x_num + data.second[0].x_num,
                               data.first[0].y_num + data.second[0].y_num});

  while (first_index != first_size && second_index != second_size) {
    Vector v1(data.first[first_index],
              data.first[(first_index + 1) % first_size]);
    Vector v2(data.second[second_index],
              data.second[(second_index + 1) % second_size]);

    long long cr = Cross(v1, v2);

    Dot last = data.intersection.back();

    if (cr > 0) {
      data.intersection.push_back(
          {last.x_num + v1.x_num, last.y_num + v1.y_num});
      ++first_index;
    } else if (cr < 0) {
      data.intersection.push_back(
          {last.x_num + v2.x_num, last.y_num + v2.y_num});
      ++second_index;
    } else {
      data.intersection.push_back(
          {last.x_num + v1.x_num + v2.x_num, last.y_num + v1.y_num + v2.y_num});
      ++first_index;
      ++second_index;
    }
  }

  while (first_index < first_size) {
    Vector v1(data.first[first_index],
              data.first[(first_index + 1) % first_size]);

    Dot last = data.intersection.back();

    data.intersection.push_back({last.x_num + v1.x_num, last.y_num + v1.y_num});
    ++first_index;
  }

  while (second_index < second_size) {
    Vector v2(data.second[second_index],
              data.second[(second_index + 1) % second_size]);

    Dot last = data.intersection.back();

    data.intersection.push_back({last.x_num + v2.x_num, last.y_num + v2.y_num});
    ++second_index;
  }

  data.intersection.pop_back();
}

void Output(double result) noexcept {
  double true_result = std::max(0.0, result - kOneMinute);
  std::cout << std::fixed << std::setprecision(kPrecision) << true_result;
}

double Process(Data& data) noexcept {
  double result = kMax;
  size_t size = data.intersection.size();

  for (size_t index = 0; index < data.intersection.size(); ++index) {
    const Dot& d1 = data.intersection[index];
    const Dot& d2 = data.intersection[(index + 1) % size];

    Line line(d1.x_num, d1.y_num, d2.x_num, d2.y_num);
    result = std::min(result, line.PointDistance(0, 0));
  }

  return result;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  Data data = Input();
  SummaMinkovskogo(data);
  double result = Process(data);
  Output(result);
}