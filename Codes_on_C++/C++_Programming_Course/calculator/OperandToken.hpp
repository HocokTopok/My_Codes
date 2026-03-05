#pragma once

#include <sstream>

#include "AbstractToken.hpp"
#include "InvalidExpr.hpp"

template <typename T>
class OperandToken : public AbstractToken {
 public:
  OperandToken(const std::string& input_string);

  OperandToken(const T& value)
      : AbstractToken(ValueToString(value)), value_(value) {}

  const T& GetValue() const { return value_; }

 protected:
  T value_;

  static std::string ValueToString(const T& value);
};

template <typename T>
OperandToken<T>::OperandToken(const std::string& input_string)
    : AbstractToken(input_string) {
  std::stringstream stream(input_string);
  if (!(stream >> value_)) {
    throw InvalidExpr();
  }
}

template <typename T>
std::string OperandToken<T>::ValueToString(const T& value) {
  std::stringstream stream;
  stream << value;
  return stream.str();
}
