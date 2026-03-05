#pragma once

#include "AbstractToken.hpp"
#include "InvalidExpr.hpp"
#include "OperandToken.hpp"

template <typename T>
class OperatorToken : public AbstractToken {
 public:
  OperatorToken(const std::string& input_string)
      : AbstractToken(input_string) {}

  virtual bool IsBinary() const = 0;
};

template <typename T>
class BinaryOperator : public OperatorToken<T> {
 public:
  BinaryOperator(const std::string& input_string)
      : OperatorToken<T>(input_string) {}

  bool IsBinary() const final { return true; }

  virtual T Calculate(OperandToken<T>* lhs, OperandToken<T>* rhs) const = 0;
};

template <typename T>
class UnaryOperator : public OperatorToken<T> {
 public:
  UnaryOperator(const std::string& input_string)
      : OperatorToken<T>(input_string) {}

  bool IsBinary() const final { return false; }

  virtual T Calculate(OperandToken<T>* operand) const = 0;
};

template <typename T>
class PlusOperator : public BinaryOperator<T> {
 public:
  PlusOperator() : BinaryOperator<T>("+") {}

  T Calculate(OperandToken<T>* lhs, OperandToken<T>* rhs) const override {
    return lhs->GetValue() + rhs->GetValue();
  }
};

template <typename T>
class MinusOperator : public BinaryOperator<T> {
 public:
  MinusOperator() : BinaryOperator<T>("-") {}

  T Calculate(OperandToken<T>* lhs, OperandToken<T>* rhs) const override {
    return lhs->GetValue() - rhs->GetValue();
  }
};

template <typename T>
class MultiOperator : public BinaryOperator<T> {
 public:
  MultiOperator() : BinaryOperator<T>("*") {}

  T Calculate(OperandToken<T>* lhs, OperandToken<T>* rhs) const override {
    return lhs->GetValue() * rhs->GetValue();
  }
};

template <typename T>
class DivisionOperator : public BinaryOperator<T> {
 public:
  DivisionOperator() : BinaryOperator<T>("/") {}

  T Calculate(OperandToken<T>* lhs, OperandToken<T>* rhs) const override;
};

template <typename T>
T DivisionOperator<T>::Calculate(OperandToken<T>* lhs,
                                 OperandToken<T>* rhs) const {
  if (rhs->GetValue() == 0) {
    throw InvalidExpr();
  }
  return lhs->GetValue() / rhs->GetValue();
}

template <typename T>
class UnaryPlusOperator : public UnaryOperator<T> {
 public:
  UnaryPlusOperator() : UnaryOperator<T>("+") {}

  T Calculate(OperandToken<T>* operand) const override {
    return operand->GetValue();
  }
};

template <typename T>
class UnaryMinusOperator : public UnaryOperator<T> {
 public:
  UnaryMinusOperator() : UnaryOperator<T>("-") {}

  T Calculate(OperandToken<T>* operand) const override {
    return -operand->GetValue();
  }
};
