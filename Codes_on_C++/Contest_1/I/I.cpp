#include <algorithm>
#include <iostream>
#include <vector>

int main() {
  int num_c;
  int target;
  std::cin >> num_c >> target;
  std::vector<int> vec(num_c);
  for (int i = 0; i < num_c; i++) {
    std::cin >> vec[i];
  }
  // Сортировка
  std::sort(vec.begin(), vec.end());
  // Крайние случаи
  int l_summa = vec[0] + vec[1] + vec[2];
  int r_summa = vec[num_c - 1] + vec[num_c - 2] + vec[num_c - 3];
  if (target <= l_summa) {
    std::cout << l_summa;
    return 0;
  }
  if (target >= r_summa) {
    std::cout << r_summa;
    return 0;
  }
  const int kDev = -90000;
  // Ввод важных переменных
  int top = kDev;
  // Основной алгоритма
  int left = 0;
  int right = num_c - 1;
  while (right - left >= 1) {
    int first = vec[left];
    int second = vec[right];
    int temp_top = kDev;
    for (int i = left + 1; i < right; i++) {  // сюда можно бин поиск
      int summa = first + second + vec[i];
      if (abs(target - temp_top) > abs(target - summa)) {
        temp_top = summa;
      }
    }
    if (abs(target - top) > abs(target - temp_top)) {
      top = temp_top;
    }
    if (top == target) {
      std::cout << top;
      return 0;
    }
    if (temp_top > target) {
      right--;
    } else {
      left++;
    }
  }
  std::cout << top;
}