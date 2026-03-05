#pragma once

#include "AbstractToken.hpp"
#include "BracketToken.hpp"
#include "ExprInPolishNotation.hpp"
#include "InvalidExpr.hpp"
#include "OperandToken.hpp"
#include "OperatorToken.hpp"

template <typename T>
class Calculator {
 public:
  static T CalculateExpr(const std::string& expr);

 private:
  struct StepResult {
    T value;
    size_t next_index;
  };

  static StepResult RecursiveCalculation(
      const std::vector<AbstractToken*>& tokens, size_t index);
};

template <typename T>
T Calculator<T>::CalculateExpr(const std::string& expr) {
  ExprInPolishNotation<T> polish_tokens(expr);
  const std::vector<AbstractToken*>& tokens = polish_tokens.GetTokens();
  return RecursiveCalculation(tokens, 0).value;
}

template <typename T>
typename Calculator<T>::StepResult Calculator<T>::RecursiveCalculation(
    const std::vector<AbstractToken*>& tokens, size_t index) {
  if (index >= tokens.size()) {
    throw InvalidExpr();
  }

  AbstractToken* token = tokens[index];

  auto* operand = dynamic_cast<OperandToken<T>*>(token);
  if (operand != nullptr) {
    return {operand->GetValue(), index + 1};
  }
  auto* unary_op = dynamic_cast<UnaryOperator<T>*>(token);
  if (unary_op != nullptr) {
    StepResult step = RecursiveCalculation(tokens, index + 1);
    OperandToken<T> temp_operand(step.value);
    T result = unary_op->Calculate(&temp_operand);
    return {result, step.next_index};
  }
  auto* binary_op = dynamic_cast<BinaryOperator<T>*>(token);
  if (binary_op != nullptr) {
    StepResult step1 = RecursiveCalculation(tokens, index + 1);
    StepResult step2 = RecursiveCalculation(tokens, step1.next_index);
    OperandToken<T> temp_lhs(step1.value);
    OperandToken<T> temp_rhs(step2.value);
    T result = binary_op->Calculate(&temp_lhs, &temp_rhs);
    return {result, step2.next_index};
  }
  throw InvalidExpr();
}
