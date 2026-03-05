#include <iostream>
#include <vector>

using Matrix = std::vector<std::vector<int64_t>>;

const int64_t kMod = 998244353;

Matrix MultiMatrix(const Matrix& lhs, const Matrix& rhs) {
  Matrix result(2, std::vector<int64_t>(2, 0));

  for (size_t i = 0; i < 2; ++i) {
    for (size_t j = 0; j < 2; ++j) {
      for (size_t k = 0; k < 2; ++k) {
        result[i][j] = (result[i][j] + lhs[i][k] * rhs[k][j]) % kMod;
      }
    }
  }

  return result;
}

Matrix FastPow(const Matrix& copy, int64_t number) {
  Matrix result = {{1, 0}, {0, 1}};
  Matrix matrix = copy;

  while (number > 0) {
    if (number % 2 == 1) {
      result = MultiMatrix(result, matrix);
    }
    matrix = MultiMatrix(matrix, matrix);
    number /= 2;
  }

  return result;
}

int64_t Fibonacci(int64_t number) {
  if (number == 1) {
    return 0;
  }
  if (number == 2) {
    return 1;
  }

  Matrix base = {{1, 1}, {1, 0}};
  Matrix result = FastPow(base, number - 1);
  return result[0][1];
}

int main() {
  int64_t number;
  std::cin >> number;
  std::cout << Fibonacci(number);
}