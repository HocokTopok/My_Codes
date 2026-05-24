#include <cmath>
#include <iomanip>
#include <iostream>

static const double kEps = 1e-9;
static const size_t kAccuracy = 10;

struct Line {
  Line(int x1, int y1, int x2, int y2)
      : param_a(y1 - y2), param_b(x2 - x1), param_c(x1 * y2 - x2 * y1) {}

  double PointDistance(double xa, double ya) const noexcept {
    return std::abs(param_a * xa + param_b * ya + param_c) /
           std::sqrt(param_a * param_a + param_b * param_b);
  }

  std::pair<double, double> PerpendPoint(double xa, double ya) const noexcept {
    double param_l = -1.0 * (param_a * xa + param_b * ya + param_c) /
                     (param_a * param_a + param_b * param_b);
    return {xa + param_l * param_a, ya + param_l * param_b};
  }

  bool Collinear(const Line& other) const noexcept {
    return 1LL * param_a * other.param_b == 1LL * other.param_a * param_b &&
           1LL * param_a * other.param_c == 1LL * other.param_a * param_c &&
           1LL * param_b * other.param_c == 1LL * other.param_b * param_c;
  }

  long long Determinant(const Line& other) const noexcept {
    return 1LL * param_a * other.param_b - 1LL * param_b * other.param_a;
  }

  std::pair<double, double> Kramer(const Line& other) const noexcept {
    long long det =
        1LL * param_a * other.param_b - 1LL * param_b * other.param_a;
    long long det1 =
        -1LL * param_c * other.param_b + 1LL * param_b * other.param_c;
    long long det2 =
        -1LL * param_a * other.param_c + 1LL * param_c * other.param_a;

    return {1.0 * det1 / det, 1.0 * det2 / det};
  }

  int param_a;
  int param_b;
  int param_c;
};

struct Segment {
  Segment(int num1, int num2, int num3, int num4)
      : x1(num1), y1(num2), x2(num3), y2(num4), line(num1, num2, num3, num4) {}

  bool IsPoint() const noexcept { return x1 == x2 && y1 == y2; }

  double PointDistance(int xa, int ya) const noexcept {
    auto [xb, yb] = line.PerpendPoint(xa, ya);

    if (PointBelongs(xb, yb)) {
      return line.PointDistance(xa, ya);
    }
    return GetMinVertexDistance(xa, ya);
  }

  bool PointBelongs(double xa, double ya) const noexcept {
    return std::min(x1, x2) - kEps <= xa && xa <= std::max(x1, x2) + kEps &&
           std::min(y1, y2) - kEps <= ya && ya <= std::max(y1, y2) + kEps;
  }

  double GetMinVertexDistance(int xa, int ya) const noexcept {
    return std::min(GetPointPointDistance(xa, ya, x1, y1),
                    GetPointPointDistance(xa, ya, x2, y2));
  }

  static double GetPointPointDistance(int x1, int y1, int x2, int y2) noexcept {
    int katet_x = std::abs(x1 - x2);
    int katet_y = std::abs(y1 - y2);

    return std::sqrt(katet_x * katet_x + katet_y * katet_y);
  }

  bool Collinear(const Segment& other) const noexcept {
    return line.Collinear(other.line);
  }

  static bool Overlap(int a1, int a2, int b1, int b2) noexcept {
    int l1 = std::min(a1, a2);
    int r1 = std::max(a1, a2);
    int l2 = std::min(b1, b2);
    int r2 = std::max(b1, b2);

    return std::max(l1, l2) <= std::min(r1, r2);
  }

  bool CrossHelper(const Segment& other) const noexcept {
    return Overlap(x1, x2, other.x1,
                   other.x2) && Overlap(y1, y2, other.y1, other.y2);
  }

  bool Crosses(const Segment& other) const noexcept {
    int det = line.Determinant(other.line);

    if (det != 0) {
      auto [xb, yb] = line.Kramer(other.line);

      return PointBelongs(xb, yb) && other.PointBelongs(xb, yb);
    }

    return line.Collinear(other.line) && CrossHelper(other);
  }

  int x1;
  int y1;
  int x2;
  int y2;
  Line line;
};

using Pair = std::pair<Segment, Segment>;

Pair Input() {
  int x1;
  int y1;
  int x2;
  int y2;

  std::cin >> x1 >> y1 >> x2 >> y2;
  Segment s1(x1, y1, x2, y2);

  std::cin >> x1 >> y1 >> x2 >> y2;
  Segment s2(x1, y1, x2, y2);

  return {s1, s2};
}

int main() {
  auto [s1, s2] = Input();

  std::cout << std::fixed << std::setprecision(kAccuracy);

  if (s1.IsPoint() && s2.IsPoint()) {
    std::cout << Segment::GetPointPointDistance(s1.x1, s1.y1, s2.x1, s2.y1);

  } else if (s1.IsPoint()) {
    std::cout << s2.PointDistance(s1.x1, s1.y1);

  } else if (s2.IsPoint()) {
    std::cout << s1.PointDistance(s2.x1, s2.y1);

  } else if (s1.Crosses(s2)) {
    std::cout << 0;

  } else {
    std::cout << std::min(std::min(s1.PointDistance(s2.x1, s2.y1),
                                   s1.PointDistance(s2.x2, s2.y2)),
                          std::min(s2.PointDistance(s1.x1, s1.y1),
                                   s2.PointDistance(s1.x2, s1.y2)));
  }
}
