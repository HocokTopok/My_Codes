#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <vector>

template <size_t N, size_t M, typename T>
class Matrix;

template <size_t N, size_t M, typename T>
class MatrixBase {};

template <size_t N, typename T>
class MatrixBase<N, N, T> {
 public:
  T Trace() const {
    T result = T();
    for (size_t i = 0; i < N; ++i) {
      result += static_cast<const Matrix<N, N, T>&>(*this)(i, i);
    }
    return result;
  }
};

template <size_t N, size_t M, typename T = int64_t>
class Matrix : public MatrixBase<N, M, T> {
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

  Matrix& operator+=(const Matrix& other);
  Matrix& operator-=(const Matrix& other);
  Matrix operator*(const T& number) const;

  Matrix<M, N, T> Transposed() const;

  const std::vector<std::vector<T>>& GetMatrix() const { return matrix_; }

 private:
  std::vector<std::vector<T>> matrix_;
};

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator+=(const Matrix& other) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      matrix_[i][j] += other.matrix_[i][j];
    }
  }
  return *this;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T>& Matrix<N, M, T>::operator-=(const Matrix& other) {
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      matrix_[i][j] -= other.matrix_[i][j];
    }
  }
  return *this;
}

template <size_t N, size_t M, typename T>
Matrix<N, M, T> Matrix<N, M, T>::operator*(const T& number) const {
  Matrix result;
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      result.matrix_[i][j] = matrix_[i][j] * number;
    }
  }
  return result;
}

template <size_t N, size_t M, typename T>
Matrix<M, N, T> Matrix<N, M, T>::Transposed() const {
  Matrix<M, N, T> result;
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < M; ++j) {
      result(j, i) = matrix_[i][j];
    }
  }
  return result;
}

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
        result(i, j) += lhs(i, k) * rhs(k, j);
      }
    }
  }
  return result;
}

template <size_t N, size_t M, typename T>
bool operator==(const Matrix<N, M, T>& lhs, const Matrix<N, M, T>& rhs) {
  return lhs.GetMatrix() == rhs.GetMatrix();
}
