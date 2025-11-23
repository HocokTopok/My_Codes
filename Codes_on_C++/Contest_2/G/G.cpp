#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

const int kINF = 1e9;

class SegmentTreeMin {
 public:
  SegmentTreeMin(int size) {
    bin_size_ = 1;
    while (bin_size_ < size) {
      bin_size_ *= 2;
    }
    data_.assign(2 * bin_size_, kINF);
  }

  void Update(int index, int state) {
    int station_index = index + bin_size_;
    if (state == 1) {
      data_[station_index] = index;
    } else {
      data_[station_index] = kINF;
    }

    station_index /= 2;
    while (station_index != 0) {
      data_[station_index] =
          std::min(data_[station_index * 2], data_[station_index * 2 + 1]);
      station_index /= 2;
    }
  }

  int Query(int left, int right) {
    if (left > right) {
      return kINF;
    }

    left += bin_size_;
    right += bin_size_;
    int res = kINF;

    while (left <= right) {
      if (left % 2 == 1) {
        res = std::min(res, data_[left]);
        ++left;
      }

      if (right % 2 == 0) {
        res = std::min(res, data_[right]);
        --right;
      }

      left /= 2;
      right /= 2;
    }
    return res;
  }

 private:
  int bin_size_;
  std::vector<int> data_;
};

class SegmentTreeMax {
 public:
  SegmentTreeMax(int size) {
    bin_size_ = 1;
    while (bin_size_ < size) {
      bin_size_ *= 2;
    }
    data_.assign(2 * bin_size_, -1);
  }

  void Update(int index, int state) {
    int station_index = index + bin_size_;
    if (state == 1) {
      data_[station_index] = index;
    } else {
      data_[station_index] = -1;
    }

    station_index /= 2;
    while (station_index != 0) {
      data_[station_index] =
          std::max(data_[station_index * 2], data_[station_index * 2 + 1]);
      station_index /= 2;
    }
  }

  int Query(int left, int right) {
    if (left > right) {
      return -1;
    }

    left += bin_size_;
    right += bin_size_;
    int res = -1;

    while (left <= right) {
      if (left % 2 == 1) {
        res = std::max(res, data_[left]);
        ++left;
      }

      if (right % 2 == 0) {
        res = std::max(res, data_[right]);
        --right;
      }

      left /= 2;
      right /= 2;
    }

    return res;
  }

 private:
  int bin_size_;
  std::vector<int> data_;
};

class MaxHeap {
 public:
  MaxHeap() = default;

  void Insert(std::tuple<int, int, int> segment) {
    heap_.push_back(segment);
    SiftUp(heap_.size() - 1);
  }

  void ExtractMax() {
    if (heap_.empty()) {
      return;
    }
    std::swap(heap_[0], heap_[heap_.size() - 1]);
    heap_.pop_back();
    if (!heap_.empty()) {
      SiftDown(0);
    }
  }

  std::tuple<int, int, int> GetMax() const {
    if (heap_.empty()) {
      return {-1, -1, -1};
    }
    return heap_[0];
  }

  bool Empty() const { return heap_.empty(); }

  void CleanHeap(SegmentTreeMin& min_tree, SegmentTreeMax& max_tree,
                 int size_x) {
    while (!heap_.empty()) {
      auto [segment, left, right] = heap_[0];

      int next_after_left = min_tree.Query(left + 1, size_x - 1);
      int prev_before_right = max_tree.Query(0, right - 1);

      if (next_after_left == right && prev_before_right == left) {
        break;
      }
      ExtractMax();
    }
  }

 private:
  std::vector<std::tuple<int, int, int>> heap_;

  void SiftUp(size_t index) {
    while (index > 0) {
      size_t parent = (index - 1) / 2;

      if (std::get<0>(heap_[parent]) >= std::get<0>(heap_[index])) {
        break;
      }

      std::swap(heap_[parent], heap_[index]);
      index = parent;
    }
  }

  void SiftDown(size_t index) {
    size_t size = heap_.size();
    while (true) {
      size_t left = index * 2 + 1;
      size_t right = index * 2 + 2;
      size_t maximum = index;

      if (left < size &&
          std::get<0>(heap_[left]) > std::get<0>(heap_[maximum])) {
        maximum = left;
      }
      if (right < size &&
          std::get<0>(heap_[right]) > std::get<0>(heap_[maximum])) {
        maximum = right;
      }
      if (index == maximum) {
        break;
      }

      std::swap(heap_[index], heap_[maximum]);
      index = maximum;
    }
  }
};

void Input(int& count, int& y_input, int& x_input,
           std::vector<std::tuple<int, int, int>>& stations) {
  std::cin >> count >> y_input >> x_input;
  stations.resize(count);
  for (int i = 0; i < count; ++i) {
    int station_x;
    int station_y1;
    int station_y2;
    std::cin >> station_x >> station_y1 >> station_y2;
    stations[i] = {station_x, station_y1, station_y2};
  }
}

void InitializationTrees(SegmentTreeMax& max_tree, SegmentTreeMin& min_tree,
                         int size_x) {
  max_tree.Update(0, 1);
  max_tree.Update(size_x - 1, 1);
  min_tree.Update(0, 1);
  min_tree.Update(size_x - 1, 1);
}

void CreateChanges(std::vector<std::tuple<int, int, int>>& stations,
                   std::map<int, int>& x_index, int y_input,
                   std::vector<std::vector<std::pair<int, char>>>& changes) {
  for (auto& station : stations) {
    int x = std::get<0>(station);
    int y1 = std::get<1>(station);
    int y2 = std::get<2>(station);
    int index = x_index[x];

    changes[y1].push_back({index, 'a'});
    if (y2 + 1 <= y_input) {
      changes[y2 + 1].push_back({index, 'r'});
    }
  }
}

struct Arguments {
  SegmentTreeMax& max_tree;
  SegmentTreeMin& min_tree;
  MaxHeap& max_heap;
  const std::vector<int>& all_x;
  int size_x;
  int base_segment;
};

void ProcessLevelChanges(const std::vector<std::pair<int, char>>& changes,
                         Arguments& args) {
  std::vector<std::pair<int, char>> removals;
  std::vector<std::pair<int, char>> additions;

  for (const auto& change : changes) {
    (change.second == 'r' ? removals : additions).push_back(change);
  }

  for (auto& change : removals) {
    int index = change.first;
    int left = args.max_tree.Query(0, index - 1);
    int right = args.min_tree.Query(index + 1, args.size_x - 1);

    args.min_tree.Update(index, 0);
    args.max_tree.Update(index, 0);

    if (left != -1 && right != kINF) {
      args.max_heap.Insert({args.all_x[right] - args.all_x[left], left, right});
    }
  }

  for (auto& change : additions) {
    int index = change.first;
    int left = args.max_tree.Query(0, index - 1);
    int right = args.min_tree.Query(index + 1, args.size_x - 1);

    args.min_tree.Update(index, 1);
    args.max_tree.Update(index, 1);

    if (left != -1) {
      args.max_heap.Insert({args.all_x[index] - args.all_x[left], left, index});
    }
    if (right != kINF) {
      args.max_heap.Insert(
          {args.all_x[right] - args.all_x[index], index, right});
    }
  }
}

int main() {
  int count;
  int y_input;
  int x_input;
  std::vector<std::tuple<int, int, int>> stations;
  Input(count, y_input, x_input, stations);

  std::vector<int> all_x;
  all_x.push_back(0);
  all_x.push_back(x_input);
  for (auto& station : stations) {
    all_x.push_back(std::get<0>(station));
  }

  std::sort(all_x.begin(), all_x.end());
  all_x.erase(std::unique(all_x.begin(), all_x.end()), all_x.end());
  int size_x = all_x.size();
  std::map<int, int> x_index;
  for (size_t i = 0; i < all_x.size(); ++i) {
    x_index[all_x[i]] = i;
  }

  SegmentTreeMax max_tree(size_x);
  SegmentTreeMin min_tree(size_x);
  InitializationTrees(max_tree, min_tree, size_x);

  std::vector<std::vector<std::pair<int, char>>> changes(y_input + 2);
  CreateChanges(stations, x_index, y_input, changes);

  MaxHeap max_heap;
  int base_segment = all_x[size_x - 1] - all_x[0];
  max_heap.Insert({base_segment, 0, size_x - 1});

  Arguments args{max_tree, min_tree, max_heap, all_x, size_x, base_segment};

  for (int i = 0; i <= y_input; ++i) {
    ProcessLevelChanges(changes[i], args);
    args.max_heap.CleanHeap(args.min_tree, args.max_tree, args.size_x);
    std::cout << (args.max_heap.Empty() ? args.base_segment
                                        : std::get<0>(args.max_heap.GetMax()))
              << "\n";
  }
}