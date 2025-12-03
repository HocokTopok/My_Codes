// Нерабочий код
#include <algorithm>
#include <iostream>
#include <vector>

size_t DotBinSearch(std::vector<std::vector<size_t>>& vec_vec, int target,
                    int index) {
  int left = 0;
  int right = vec_vec.size();

  while (left < right - 1) {
    int mid = (left + right) / 2;

    if (vec_vec[mid][0] > target) {
      left = mid;
    } else if (target < vec_vec[mid][0]) {
      right = mid;
    } else {
      if (index > vec_vec[mid][1]) {
        left = mid;
      } else {
        right = mid;
      }
    }
    // а зачем это.... у тебя же выше то же самое написано...
    if (vec_vec[mid][0] > target) {
      right = mid;
    } else if (vec_vec[mid][0] == target) {
      if (vec_vec[mid][1] > index) {
        right = mid;
      } else {
        left = mid;
      }
    } else {
      left = mid;
    }
  }
  return left;
}

size_t PrevBinSearch(std::vector<std::vector<size_t>>& vec_vec, int target) {
  int left = 0;
  int right = vec_vec.size();

  while (left < right - 1) {
    int mid = (left + right) / 2;

    if (target > vec_vec[mid][0]) {
      left = mid;
    } else {
      right = mid;
    }
  }
  return left;
}

class FenwickTree2D {
 public:
  FenwickTree2D(size_t size, size_t dots_count)
      : data_(size + 1,
              std::vector<std::vector<size_t>>(1, std::vector<size_t>(2, 0))),
        vector_fenwicks_(size + 1),
        dots_(dots_count + 1),
        x_size_(size) {}

  void Change(size_t x_index, size_t y_index, size_t dot_index, int new_value) {
    int delta = new_value - dots_[dot_index];
    dots_[dot_index] = new_value;

    while (x_index < x_size_ + 1) {
      size_t dot_y_index = DotBinSearch(data_[x_index], y_index, dot_index) + 1;
      Update(x_index, dot_y_index, delta);
      x_index += x_index & -x_index;
    }
  }

  int64_t RangeSum(int x_index, int y_index) {
    int64_t result = 0;

    while (x_index > 0) {
      size_t local_y_index = PrevBinSearch(
          data_[x_index],
          y_index);  // точно ли Prev? Мы ведь хотим y_index тоже учесть, а этих
                     // y_indexов может быть несколько, так как значения могут
                     // повторяться
      ++local_y_index;
      while (local_y_index > 0) {
        result += vector_fenwicks_[x_index][local_y_index];
        local_y_index -= local_y_index & -local_y_index;
      }
      x_index -= x_index & -x_index;
    }

    return result;
  }

  void Add(size_t x_index, size_t y_index, size_t dot_index, int new_value) {
    while (x_index < x_size_ + 1) {
      data_[x_index].push_back({y_index, dot_index});
      x_index += x_index & -x_index;
    }

    dots_[dot_index] = new_value;
  }

  void BuildFenwick2D() {
    for (size_t i = 1; i < x_size_ + 1; ++i) {
      vector_fenwicks_[i].resize(data_[i].size() + 1, 0);

      std::sort(data_[i].begin(), data_[i].end());

      for (size_t j = 1; j < vector_fenwicks_[i].size(); ++j) {
        size_t dot_index = data_[i][j - 1][1];
        Update(i, j, dots_[dot_index]);
      }
    }
  }

 private:
  std::vector<std::vector<std::vector<size_t>>> data_;
  std::vector<std::vector<int64_t>> vector_fenwicks_;
  std::vector<int> dots_;
  size_t x_size_;

  void Update(size_t x_index, size_t y_index, int64_t delta) {
    while (y_index < vector_fenwicks_[x_index].size()) {
      vector_fenwicks_[x_index][y_index] += delta;
      y_index += y_index & -y_index;
    }
  }
};

size_t NextBinSearch(std::vector<size_t> vec, size_t target) {
  int left = -1;
  int right = vec.size();
  while (left < right - 1) {
    int mid = (left + right) / 2;
    if (target < vec[mid]) {
      right = mid;
    } else {
      left = mid;
    }
  }
  return right;
}

// enum class CommandType { Get, Change };

// struct Query {
//   CommandType type;
//   int64_t number1;
//   int64_t number2;

//   Query(CommandType command_name, int64_t num1, int64_t num2)
//       : type(command_name), number1(num1), number2(num2) {}
// };

struct Dot {
  size_t x_index;
  size_t y_index;
};

struct Data {
  std::vector<Dot> dots;
  std::vector<int> values;
  std::vector<size_t> all_x;
  std::vector<size_t> all_y;
  // std::vector<Query> queries;
};

Data Input() {
  Data data;

  size_t elements_count;
  std::cin >> elements_count;

  data.dots.resize(elements_count);
  data.values.resize(elements_count);

  for (size_t i = 0; i < elements_count; ++i) {
    size_t x_index;
    size_t y_index;
    std::cin >> x_index >> y_index >> data.values[i];
    data.dots[i] = {x_index, y_index};
    data.all_x.push_back(x_index);
    data.all_y.push_back(y_index);
  }

  std::sort(data.all_x.begin(), data.all_x.end());
  std::sort(data.all_y.begin(), data.all_y.end());

  data.all_x.erase(std::unique(data.all_x.begin(), data.all_x.end()),
                   data.all_x.end());
  data.all_y.erase(std::unique(data.all_y.begin(), data.all_y.end()),
                   data.all_y.end());

  return data;
}

int main() {
  Data data = Input();

  FenwickTree2D fenwick(data.all_x.size(), data.all_y.size());

  for (size_t i = 0; i < data.dots.size(); ++i) {
    data.dots[i].x_index = NextBinSearch(data.all_x, data.dots[i].x_index);
    data.dots[i].y_index = NextBinSearch(data.all_y, data.dots[i].y_index);
    fenwick.Add(data.dots[i].x_index, data.dots[i].y_index, i + 1,
                data.values[i]);
  }

  fenwick.BuildFenwick2D();

  size_t queries_count;
  std::cin >> queries_count;

  for (size_t i = 0; i < queries_count; ++i) {
    std::string type;
    std::cin >> type;

    if (type[0] == 'c') {
      size_t index;
      size_t new_value;
      std::cin >> index >> new_value;
      fenwick.Change(data.dots[index - 1].x_index, data.dots[index - 1].y_index,
                     index, new_value);
    } else {
      size_t rx;
      size_t ry;
      std::cin >> rx >> ry;
      std::cout << fenwick.RangeSum(NextBinSearch(data.all_x, rx),
                                    NextBinSearch(data.all_y, ry))
                << '\n';
    }
  }
}