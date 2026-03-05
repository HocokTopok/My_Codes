#include <algorithm>
#include <iostream>
#include <vector>

struct Data {
  Data(int size)
      : up_data(size, 1),
        down_data(size, 1),
        up_recov(size, -1),
        down_recov(size, -1),
        size(size) {}

  std::vector<int64_t> towns;
  std::vector<int> up_data;
  std::vector<int> down_data;
  std::vector<int> up_recov;
  std::vector<int> down_recov;
  int size;
};

Data Input() {
  int size;
  std::cin >> size;

  Data data(size);

  data.towns.resize(size);
  for (int i = 0; i < size; ++i) {
    std::cin >> data.towns[i];
  }

  return data;
}

std::vector<int64_t> Process(Data& data) {
  for (int i = 1; i < data.size; ++i) {
    for (int j = 0; j < i; ++j) {
      if (data.towns[j] < data.towns[i]) {
        if (data.up_data[i] < data.down_data[j] + 1) {
          data.up_data[i] = data.down_data[j] + 1;
          data.up_recov[i] = j;
        }
      } else if (data.towns[j] > data.towns[i]) {
        if (data.down_data[i] < data.up_data[j] + 1) {
          data.down_data[i] = data.up_data[j] + 1;
          data.down_recov[i] = j;
        }
      }
    }
  }

  int best_len = 1;
  int best_index = 0;
  bool is_up = true;
  for (int i = 0; i < data.size; ++i) {
    if (data.up_data[i] > best_len) {
      best_len = data.up_data[i];
      best_index = i;
      is_up = true;
    }
    if (data.down_data[i] > best_len) {
      best_len = data.down_data[i];
      best_index = i;
      is_up = false;
    }
  }

  std::vector<int64_t> results;
  int current_index = best_index;
  while (current_index != -1) {
    results.push_back(data.towns[current_index]);
    if (is_up) {
      current_index = data.up_recov[current_index];
      is_up = false;
    } else {
      current_index = data.down_recov[current_index];
      is_up = true;
    }
  }
  std::reverse(results.begin(), results.end());

  return results;
}

void Output(std::vector<int64_t>& results) {
  std::cout << results.size() << "\n";
  for (int64_t result : results) {
    std::cout << result << " ";
  }
}

int main() {
  Data data = Input();
  std::vector<int64_t> results = Process(data);
  Output(results);
}