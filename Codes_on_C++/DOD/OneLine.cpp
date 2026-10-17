////////////////////////////////////////////////////////////////

#include <cmath>
#include <iostream>
#include <vector>

////////////////////////////////////////////////////////////////

// Оценки: от 0 до 100. В группе от 1 до 100 человек.
double AverageScore(const std::vector<int>& scores)
{
  int sum = 0;
  int count = static_cast<int>(scores.size());

  for (int score : scores)
  {
    sum += score;
  }

  // ================ ИЗМЕНИТЕ ЭТУ СТРОКУ ================
  double average = sum / count;
  // =====================================================

  return average;
}

////////////////////////////////////////////////////////////////
//                         CHECKS                             //
////////////////////////////////////////////////////////////////

int main()
{
  const std::vector<std::vector<int>> groups = {
    {4, 5}, {0, 1, 1}, {5, 5, 5}, {0}, {100}, {1, 2, 2, 2}
  };
  const std::vector<double> expected = {4.5, 2.0 / 3.0, 5.0, 0.0, 100.0, 1.75};

  bool correct = true;

  for (std::size_t i = 0; i < groups.size(); ++i)
  {
    double actual = AverageScore(groups[i]);
    bool passed = std::isfinite(actual) && std::abs(actual - expected[i]) < 1e-9;

    std::cout << "Пример " << i + 1 << ": " << actual
              << " (ожидается " << expected[i] << ")\n";

    correct = correct && passed;
  }

  std::cout << (correct ? "ВЕРНО" : "НЕВЕРНО") << '\n';

  return correct ? 0 : 1;
}

////////////////////////////////////////////////////////////////
