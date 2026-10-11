#include <algorithm>
#include <iostream>
#include <vector>

const int64_t kInf = 1e18;

void Output(int64_t minimum, std::vector<int>& path) {
  std::reverse(path.begin(), path.end());
  std::cout << minimum << "\n";
  for (int town : path) {
    std::cout << town + 1 << " ";
  }
}

void Solve(int towns_count, const std::vector<std::vector<int64_t>>& towns_info,
           std::vector<std::vector<int64_t>>& data,
           std::vector<std::vector<int>>& ways) {
  int masks_count = 1 << towns_count;

  for (int i = 0; i < towns_count; ++i) {
    data[1 << i][i] = 0;
  }

  for (int mask = 1; mask < masks_count; ++mask) {
    for (int town = 0; town < towns_count; ++town) {
      if ((mask & (1 << town)) == 0) {
        continue;
      }
      int64_t current_value = data[mask][town];
      for (int to = 0; to < towns_count; ++to) {
        if ((mask & (1 << to)) != 0) {
          continue;
        }
        int to_mask = mask | (1 << to);
        int64_t to_value = towns_info[town][to] + current_value;
        if (to_value < data[to_mask][to]) {
          data[to_mask][to] = to_value;
          ways[to_mask][to] = town;
        }
      }
    }
  }
}

int main() {
  int towns_count;
  std::cin >> towns_count;
  std::vector<std::vector<int64_t>> towns_info(
      towns_count, std::vector<int64_t>(towns_count, 0));
  for (int i = 0; i < towns_count; ++i) {
    for (int j = 0; j < towns_count; ++j) {
      std::cin >> towns_info[i][j];
    }
  }
  int masks_count = 1 << towns_count;
  std::vector<std::vector<int>> ways(masks_count,
                                     std::vector<int>(towns_count, -1));
  std::vector<std::vector<int64_t>> data(
      masks_count, std::vector<int64_t>(towns_count, kInf));

  Solve(towns_count, towns_info, data, ways);

  int full_mask = (1 << towns_count) - 1;
  int64_t minimum = kInf;
  int last = -1;
  for (int town = 0; town < towns_count; ++town) {
    if (data[full_mask][town] < minimum) {
      minimum = data[full_mask][town];
      last = town;
    }
  }
  int mask = full_mask;
  int town = last;
  std::vector<int> path;
  while (town != -1) {
    path.push_back(town);
    int next_town = ways[mask][town];
    mask ^= (1 << town);
    town = next_town;
  }
  Output(minimum, path);
}