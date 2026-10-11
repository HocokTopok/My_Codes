#include <cmath>
#include <iostream>
#include <string>
#include <unordered_map>

int main() {
  int part1;
  int part2;
  std::cin >> part1 >> part2;
  bool znak = (part1 < 0);
  part1 = abs(part1);
  part2 = abs(part2);
  int integer = part1 / part2;
  int ostatok = part1 % part2;
  if (ostatok == 0) {
    if (znak) {
      std::cout << "-";
    }
    std::cout << integer;
    return 0;
  }
  const int kTen = 10;
  int period_start = -1;
  bool period = false;
  std::string drobnaya_part;
  std::unordered_map<int, int> vse_ostatki;
  int position = 0;
  while (ostatok != 0) {
    if (vse_ostatki.find(ostatok) != vse_ostatki.end()) {
      period = true;
      period_start = vse_ostatki[ostatok];
      break;
    }
    vse_ostatki[ostatok] = position;
    ostatok *= kTen;
    int digit = ostatok / part2;
    drobnaya_part += std::to_string(digit);
    ostatok = ostatok % part2;
    position += 1;
  }
  if (znak) {
    std::cout << "-";
  }
  std::cout << integer << ".";
  if (period) {
    for (int i = 0; i < period_start; i++) {
      std::cout << drobnaya_part[i];
    }
    std::cout << "(";
    int length = drobnaya_part.length();
    for (int i = period_start; i < length; i++) {
      std::cout << drobnaya_part[i];
    }
    std::cout << ")";
  } else {
    std::cout << drobnaya_part;
  }
}