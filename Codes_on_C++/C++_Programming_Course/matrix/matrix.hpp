#include <cstddef>
#include <cstdint>
#include <vector>

template <size_t N, size_t M, typename T = int64_t>
class Matrix {
 public:
  Matrix() : matrix_(N, std::vector<T>(M, T())) {}

  Matrix(const std::vector<std::vector<T>>& input) : matrix_(input) {}

  Matrix(const T& elem) : matrix_(N, std::vector<T>(M, elem)) {}

  T& operator()(size_t row_index, size_t column_index) {
    return matrix_[row_index][column_index];
  }

  const T& operator()(size_t row_index, size_t column_index) const {
    return matrix_[row_index][column_index];
  }

  Matrix& operator+=(const Matrix& other) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        (*this)(i, j) += other(i, j);
      }
    }
    return *this;
  }

  Matrix& operator-=(const Matrix& other) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        (*this)(i, j) -= other(i, j);
      }
    }
    return *this;
  }

  Matrix operator*(const T& number) const {
    Matrix result;
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        result(i, j) = (*this)(i, j) * number;
      }
    }
    return result;
  }

  Matrix<M, N, T> Transposed() const {
    Matrix<M, N, T> result;
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        result(j, i) = (*this)(i, j);
      }
    }
    return result;
  }

  T Trace() const {
    static_assert(N == M);
    T result = T();
    for (size_t i = 0; i < N; ++i) {
      result += (*this)(i, i);
    }
    return result;
  }

 private:
  std::vector<std::vector<T>> matrix_;
};

template <size_t N, size_t M, typename T>
Matrix<N, M, T> operator+(const Matrix<N, M, T>& lhs,
                          const Matrix<N, M, T>& rhs) {
  Matrix<N, M, T> result(lhs);
  result += rhs;
  return result;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> operator-(const Matrix<N, M, T>& lhs,
                          const Matrix<N, M, T>& rhs) {
  Matrix<N, M, T> result(lhs);
  result -= rhs;
  return result;
}

template <size_t N, size_t K, size_t M, typename T>
Matrix<N, M, T> operator*(const Matrix<N, K, T>& lhs,
                          const Matrix<K, M, T>& rhs) {
  Matrix<N, M, T> result;
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      for (size_t k = 0; k < K; ++k) {
        result(i, j) +=
            lhs(i, k) *
            rhs(k, j);  // если не поменял на k то ударьте меня лопатой
      }
    }
  }
  return result;
}

template <size_t N, size_t M, typename T>
bool operator==(const Matrix<N, M, T>& lhs, const Matrix<N, M, T>& rhs) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      if (lhs(i, j) != rhs(i, j)) {
        return false;
      }
    }
  }
  return true;
}