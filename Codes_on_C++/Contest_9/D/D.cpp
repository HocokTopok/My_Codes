#include <iostream>
#include <unordered_set>
#include <vector>

static const size_t kFour = 4;
static const long long kBitMove = 32;
static const long long kBitMask = 0xFFFFFFFFLL;

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

struct Data {
  Data(size_t vertex_count)
      : dots_vec(vertex_count), vertex_count(vertex_count) {}

  static long long GetKey(int x_num, int y_num) noexcept {
    return ((1LL * x_num) << kBitMove) | ((1LL * y_num) & kBitMask);
  }

  bool FindDots(const Dot& d1, const Dot& d2) const noexcept {
    return dots_set.count(GetKey(d1.x_num, d1.y_num)) == 1 &&
           dots_set.count(GetKey(d2.x_num, d2.y_num)) == 1;
  }

  std::vector<Dot> dots_vec;
  std::unordered_set<long long> dots_set;
  size_t vertex_count;
  size_t count = 0;
};

Data Input() noexcept {
  size_t vertex_count;
  std::cin >> vertex_count;

  Data data(vertex_count);

  for (size_t i = 0; i < vertex_count; ++i) {
    int x_num;
    int y_num;
    std::cin >> x_num >> y_num;

    data.dots_vec[i] = {x_num, y_num};
    data.dots_set.insert(Data::GetKey(x_num, y_num));
  }

  return data;
}

void KvadratSearch(Data& data) noexcept {
  for (size_t first = 0; first < data.vertex_count; ++first) {
    for (size_t second = first + 1; second < data.vertex_count; ++second) {
      const Dot& d1 = data.dots_vec[first];
      const Dot& d2 = data.dots_vec[second];

      int delta_x = d2.x_num - d1.x_num;
      int delta_y = d2.y_num - d1.y_num;

      Dot dot_rf(d1.x_num + delta_y, d1.y_num - delta_x);
      Dot dot_rs(d2.x_num + delta_y, d2.y_num - delta_x);
      Dot dot_lf(d1.x_num - delta_y, d1.y_num + delta_x);
      Dot dot_ls(d2.x_num - delta_y, d2.y_num + delta_x);

      if (data.FindDots(dot_rf, dot_rs)) {
        ++data.count;
      }
      if (data.FindDots(dot_lf, dot_ls)) {
        ++data.count;
      }
    }
  }

  data.count /= kFour;
}

void Output(size_t result) noexcept { std::cout << result; }

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  Data data = Input();
  KvadratSearch(data);
  Output(data.count);
}