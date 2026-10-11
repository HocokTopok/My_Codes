#include <algorithm>
#include <array>
#include <iostream>
#include <set>
#include <vector>

int BinarySearch(std::vector<int>& data, int value) {
  int left = -1;
  int right = data.size();
  while (right - left > 1) {
    int middle = (left + right) / 2;
    if (data[middle] > value) {
      right = middle;
    } else {
      left = middle;
    }
  }
  return right;
}

int BinarySearch(std::vector<std::array<int, 2>>& data, int value, int index) {
  int left = 0;
  int right = data.size();
  while (right - left > 1) {
    int middle = (left + right) / 2;
    if (data[middle][0] > value) {
      right = middle;
    } else if (data[middle][0] == value) {
      if (data[middle][1] > index) {
        right = middle;
      } else {
        left = middle;
      }
    } else {
      left = middle;
    }
  }
  return left;
}

int BinarySearch(std::vector<std::array<int, 2>>& data, int value) {
  int left = 0;
  int right = data.size();
  while (right - left > 1) {
    int middle = (left + right) / 2;
    if (data[middle][0] > value) {
      right = middle;
    } else {
      left = middle;
    }
  }
  return left;
}

class FenwickTree {
 public:
  FenwickTree(size_t size, size_t els_size) : size_(size) {
    elements_.resize(size_ + 1);
    tree_.resize(size + 1);
    for (size_t ix = 0; ix <= size; ++ix) {
      elements_[ix].push_back({0, 0});
    }
    points_.resize(els_size + 1);
  }

  void Push(size_t xcoord, int ycoord, int point_index, int value) {
    points_[point_index] = value;
    for (; xcoord <= size_; xcoord += xcoord & -xcoord) {
      elements_[xcoord].push_back({ycoord, point_index});
    }
  }

  void UpdateFenwick(size_t xcoord, size_t ycoord, int value) {
    for (; ycoord < tree_[xcoord].size(); ycoord += ycoord & -ycoord) {
      tree_[xcoord][ycoord] += value;
    }
  }

  void Update(size_t xcoord, int ycoord, int point_index, int value) {
    int delta = value - points_[point_index];
    for (; xcoord <= size_; xcoord += xcoord & -xcoord) {
      int index = BinarySearch(elements_[xcoord], ycoord, point_index);
      UpdateFenwick(xcoord, index, delta);
    }
    points_[point_index] = value;
  }

  void Update() {
    for (size_t ix = 1; ix <= size_; ++ix) {
      std::sort(elements_[ix].begin(), elements_[ix].end());
      tree_[ix].resize(elements_[ix].size(), 0);
      for (size_t yx = 1; yx < tree_[ix].size(); ++yx) {
        UpdateFenwick(ix, yx, points_[elements_[ix][yx][1]]);
      }
    }
  }

  long long GetSum(int xcoord, int ycoord) {
    long long answer = 0;
    for (; xcoord > 0; xcoord -= xcoord & -xcoord) {
      answer += Sum(xcoord, ycoord);
    }
    return answer;
  }

 private:
  std::vector<std::vector<std::array<int, 2>>> elements_;
  std::vector<int> points_;
  std::vector<std::vector<long long>> tree_;
  size_t size_;

  long long Sum(int xcoord, int ycoord) {
    long long answer = 0;
    ycoord = BinarySearch(elements_[xcoord], ycoord);
    for (; ycoord > 0; ycoord -= ycoord & -ycoord) {
      answer += tree_[xcoord][ycoord];
    }
    return answer;
  }
};

void CoordinateCompression(std::vector<std::pair<int, int>>& els,
                           std::vector<int>& xcoords,
                           std::vector<int>& ycoords) {
  std::set<int> tmp;
  for (size_t i = 0; i < els.size(); ++i) {
    tmp.insert(els[i].first);
  }
  for (const auto& el : tmp) {
    xcoords.push_back(el);
  }
  tmp.clear();
  for (size_t i = 0; i < els.size(); ++i) {
    tmp.insert(els[i].second);
  }
  for (const auto& el : tmp) {
    ycoords.push_back(el);
  }
}

void main() {
  std::ios_base::sync_with_stdio(false);
  std::cin.tie(0);
  int size;
  std::cin >> size;
  std::vector<std::pair<int, int>> els(size);
  std::vector<int> weights(size);
  for (int i = 0; i < size; ++i) {
    std::cin >> els[i].first >> els[i].second >> weights[i];
  }
  std::vector<int> xcoords;
  std::vector<int> ycoords;
  CoordinateCompression(els, xcoords, ycoords);
  FenwickTree tree(xcoords.size(), weights.size());
  for (int ix = 0; ix < size; ++ix) {
    els[ix].first = BinarySearch(xcoords, els[ix].first);
    els[ix].second = BinarySearch(ycoords, els[ix].second);
    tree.Push(els[ix].first, els[ix].second, ix + 1, weights[ix]);
  }
  tree.Update();
  int query;
  std::cin >> query;
  for (int i = 0; i < query; ++i) {
    std::string command;
    std::cin >> command;
    if (command == "get") {
      int rx;
      int ry;
      std::cin >> rx >> ry;
      std::cout << tree.GetSum(BinarySearch(xcoords, rx),
                               BinarySearch(ycoords, ry))
                << '\n';
    }
    if (command == "change") {
      int index;
      int weight;
      std::cin >> index >> weight;
      tree.Update(els[index - 1].first, els[index - 1].second, index, weight);
    }
  }
}