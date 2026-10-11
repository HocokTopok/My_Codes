#include <algorithm>
#include <iostream>
#include <vector>

int main() {
  int count;
  std::cin >> count;
  int resources;
  std::cin >> resources;

  std::vector<int> tasks(count + 1);
  for (int i = 1; i <= count; ++i) {
    std::cin >> tasks[i];
  }

  std::vector<int> rewards(count + 1);
  for (int i = 1; i <= count; ++i) {
    std::cin >> rewards[i];
  }

  std::vector<std::vector<int>> data(count + 1,
                                     std::vector<int>(resources + 1, 0));
  for (int i = 1; i <= count; ++i) {
    for (int cr = 1; cr <= resources; ++cr) {
      if (cr < tasks[i]) {
        data[i][cr] = data[i - 1][cr];
      } else {
        data[i][cr] =
            std::max(data[i - 1][cr - tasks[i]] + rewards[i], data[i - 1][cr]);
      }
    }
  }

  // for (int i = 0; i <= count; ++i) {
  //   for (int cr = 0; cr <= resources; ++ cr) {
  //     std::cout << data[i][cr] << " ";
  //   }
  //   std::cout << "\n";
  // }

  std::vector<int> results;
  while (count > 0) {
    if (data[count][resources] == data[count - 1][resources]) {
      --count;
    } else {
      results.push_back(count);
      resources -= tasks[count];
      --count;
    }
  }

  std::reverse(results.begin(), results.end());

  for (int result : results) {
    std::cout << result << "\n";
  }
}