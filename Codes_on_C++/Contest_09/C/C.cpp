#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

static const double kEmpty = -1e18;
static const double kPi = std::acos(-1.0);
static const double kGorizontal = -1'000'000;
static const double kEps = 1e-15;
static const double kOneHundredEighty = 180.0;
static const int kPrecision = 10;
static const double kInf = 1e18;

struct Dot {
  Dot() = default;

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

struct Light {
  Light() = default;

  Light(const Dot& dot, int a_num) : dot(dot), a_num(a_num) {}

  Dot dot;
  int a_num;
};

struct Data {
  Data(size_t light_count, Dot left, Dot right)
      : lights(light_count),
        ddpp(1ULL << light_count, kEmpty),
        light_count(light_count),
        left(left),
        right(right) {}

  std::vector<Light> lights;
  std::vector<double> ddpp;
  size_t light_count;
  Dot left;
  Dot right;
};

Data Input() noexcept {
  size_t light_count;
  int l_num;
  int r_num;
  std::cin >> light_count >> l_num >> r_num;

  Data data(light_count, Dot(l_num, 0), Dot(r_num, 0));

  for (size_t i = 0; i < light_count; ++i) {
    int x_num;
    int y_num;
    int a_num;
    std::cin >> x_num >> y_num >> a_num;

    data.lights[i] = Light(Dot(x_num, y_num), a_num);
  }

  return data;
}

double GetIntersectionWithOx(double x0, double y0, double theta) noexcept {
  double k_num = std::tan(theta);

  if (std::abs(k_num) < kEps) {
    return kGorizontal;
  }

  double x_num = x0 - y0 / k_num;

  return x_num;
}

double GetRightPoint(double x0, double y0, double cur, int a_num) noexcept {
  double alpha = a_num * kPi / kOneHundredEighty;

  double theta_left = std::atan2(-y0, cur - x0);
  double theta_right = theta_left + alpha;

  if (theta_right >= 0.0) {
    return kInf;
  }

  return GetIntersectionWithOx(x0, y0, theta_right);
}

void SubsetDp(Data& data) noexcept {
  data.ddpp[0] = data.left.x_num;

  for (int mask = 0; mask < (1 << (int)data.light_count); ++mask) {
    if (data.ddpp[mask] == kEmpty) {
      continue;
    }

    for (int i = 0; i < (int)data.light_count; ++i) {
      if ((mask & (1 << i)) == 0) {
        const Light& light = data.lights[i];
        double result = GetRightPoint(light.dot.x_num, light.dot.y_num,
                                      data.ddpp[mask], light.a_num);
        data.ddpp[mask | (1 << i)] =
            std::max(result, data.ddpp[mask | (1 << i)]);
      }
    }
  }
}

void Output(double result) noexcept {
  std::cout << std::fixed << std::setprecision(kPrecision) << result;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  Data data = Input();
  SubsetDp(data);
  Output(std::min(static_cast<double>(data.right.x_num - data.left.x_num),
                  data.ddpp[(1ULL << data.light_count) - 1] - data.left.x_num));
}