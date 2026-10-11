#include <iostream>
#include <vector>

const int kCount = 6;

class FenwickTree3D {
 public:
  FenwickTree3D(size_t size)
      : dots_(size + 1, std::vector<std::vector<int>>(
                            size + 1, std::vector<int>(size + 1, 0))),
        size_(size + 1) {}

  void Update(size_t x_index, size_t y_index, size_t z_index, int delta) {
    while (x_index < size_) {
      size_t y_current = y_index;
      while (y_current < size_) {
        size_t z_current = z_index;
        while (z_current < size_) {
          dots_[x_index][y_current][z_current] += delta;
          z_current += z_current & -z_current;
        }
        y_current += y_current & -y_current;
      }
      x_index += x_index & -x_index;
    }
  }

  int RangeSum(std::vector<size_t>& coordinates) {
    size_t x1_index = coordinates[0];
    size_t y1_index = coordinates[1];
    size_t z1_index = coordinates[2];
    size_t x2_index = coordinates[3];
    size_t y2_index = coordinates[4];
    size_t z2_index = coordinates[5];
    return Query(x2_index, y2_index, z2_index) -
           Query(x2_index, y2_index, z1_index - 1) -
           Query(x2_index, y1_index - 1, z2_index) -
           Query(x1_index - 1, y2_index, z2_index) +
           Query(x2_index, y1_index - 1, z1_index - 1) +
           Query(x1_index - 1, y2_index, z1_index - 1) +
           Query(x1_index - 1, y1_index - 1, z2_index) -
           Query(x1_index - 1, y1_index - 1, z1_index - 1);
  }

 private:
  std::vector<std::vector<std::vector<int>>> dots_;
  size_t size_;

  int Query(size_t x_index, size_t y_index, size_t z_index) {
    int result = 0;
    while (x_index > 0) {
      size_t y_current = y_index;
      while (y_current > 0) {
        size_t z_current = z_index;
        while (z_current > 0) {
          result += dots_[x_index][y_current][z_current];
          z_current -= z_current & -z_current;
        }
        y_current -= y_current & -y_current;
      }
      x_index -= x_index & -x_index;
    }

    return result;
  }
};

int main() {
  int zone_size;
  std::cin >> zone_size;

  FenwickTree3D fenwick(zone_size);

  size_t query_type;
  std::cin >> query_type;
  while (query_type != 3) {
    if (query_type == 1) {
      size_t x_input;
      size_t y_input;
      size_t z_input;
      int delta;
      std::cin >> x_input >> y_input >> z_input >> delta;
      fenwick.Update(x_input + 1, y_input + 1, z_input + 1, delta);

    } else {
      std::vector<size_t> two_dots;
      size_t count = kCount;
      while (count != 0) {
        size_t number;
        std::cin >> number;
        two_dots.push_back(number + 1);
        --count;
      }
      std::cout << fenwick.RangeSum(two_dots) << "\n";
    }

    std::cin >> query_type;
  }
}