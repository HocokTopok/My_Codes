#pragma once

#include <stack>
#include <vector>

#include "AbstractToken.hpp"
#include "BracketToken.hpp"
#include "InvalidExpr.hpp"
#include "OperandToken.hpp"
#include "OperatorToken.hpp"

template <typename T>
class ExprInPolishNotation {
 public:
  ExprInPolishNotation(const std::string& string);

  const std::vector<AbstractToken*>& GetTokens() const { return tokens_; }

  ~ExprInPolishNotation();

 private:
  std::vector<AbstractToken*> tokens_;
  std::vector<AbstractToken*> current_tokens_;
  std::stack<AbstractToken*> operators_stack_;
  bool next_is_operand_ = true;
  bool last_is_operator_ = false;

  static bool IsOperatorOrBracket(char token) {
    return token == '+' || token == '-' || token == '*' || token == '/' ||
           token == '(' || token == ')';
  }
  static bool IsDigitOrDot(char token) {
    return (std::isdigit(token) != 0) || token == '.' ||
           (token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z');
  }
  static bool IsCorrectNumber(const std::string& token);
  static int GetPriority(const std::string& token);

  static std::vector<std::string> Split(const std::string& string);

  AbstractToken* ConstructOperatorToken(const std::string& string,
                                        bool is_binary);

  void ProcessOperand(const std::string& raw_token);
  void ProcessOpeningBracket(const std::string& raw_token);
  void ProcessClosingBracket();
  void PopHigherPriorityOps(const std::string& raw_token);
  void ProcessPlusMinus(const std::string& raw_token);
  void ProcessMultiDivis(const std::string& raw_token);

  void HelperForNormalToRPN(const std::string& raw_token);
  void OperandProcess(AbstractToken* token,
                      std::vector<std::vector<AbstractToken*>>& tokens_vector);
  void UnaryOperandProcess(
      AbstractToken* token,
      std::vector<std::vector<AbstractToken*>>& tokens_vector);
  void BinaryOperandProcess(
      AbstractToken* token,
      std::vector<std::vector<AbstractToken*>>& tokens_vector);
  void HelperForRPNToPN(
      AbstractToken* token,
      std::vector<std::vector<AbstractToken*>>& tokens_vector);

  std::vector<AbstractToken*> NormalToRPN(
      const std::vector<std::string>& raw_tokens);
  std::vector<AbstractToken*> RPNToPN(std::vector<AbstractToken*>& rpn_tokens);

  void TransportRemainingStack();
  void ErrorNormalToRPN();
};

template <typename T>
ExprInPolishNotation<T>::ExprInPolishNotation(const std::string& string) {
  std::vector<AbstractToken*> temp_tokens;
  try {
    std::vector<std::string> raw_tokens = Split(string);
    temp_tokens = NormalToRPN(raw_tokens);
    tokens_ = RPNToPN(temp_tokens);
  } catch (...) {
    for (auto* token : temp_tokens) {
      delete token;
    }
    for (auto* token : tokens_) {
      delete token;
    }
    throw;
  }
}

template <typename T>
ExprInPolishNotation<T>::~ExprInPolishNotation() {
  for (auto* token : tokens_) {
    delete token;
  }
}

template <typename T>
bool ExprInPolishNotation<T>::IsCorrectNumber(const std::string& token) {
  if (token.empty()) {
    return false;
  }

  std::stringstream stream(token);
  T value;
  return (stream >> value) && stream.eof();
}

template <typename T>
int ExprInPolishNotation<T>::GetPriority(const std::string& token) {
  if (token == "+" || token == "-") {
    return 1;
  }
  if (token == "*" || token == "/") {
    return 2;
  }
  return 0;
}

template <typename T>
std::vector<std::string> ExprInPolishNotation<T>::Split(
    const std::string& string) {
  std::vector<std::string> tokens;
  std::string current_string;

  for (char new_char : string) {
    if (std::isspace(new_char) != 0) {
      if (!current_string.empty()) {
        tokens.push_back(current_string);
        current_string.clear();
      }
      continue;
    }
    if (IsOperatorOrBracket(new_char)) {
      if (!current_string.empty()) {
        tokens.push_back(current_string);
        current_string.clear();
      }
      tokens.push_back(std::string(1, new_char));
    } else if (IsDigitOrDot(new_char)) {
      current_string.push_back(new_char);
    } else {
      throw InvalidExpr();
    }
  }

  if (!current_string.empty()) {
    tokens.push_back(current_string);
  }
  return tokens;
}

template <typename T>
AbstractToken* ExprInPolishNotation<T>::ConstructOperatorToken(
    const std::string& string, bool is_binary) {
  if (is_binary) {
    switch (string[0]) {
      case '+':
        return new PlusOperator<T>();
      case '-':
        return new MinusOperator<T>();
      case '*':
        return new MultiOperator<T>();
      case '/':
        return new DivisionOperator<T>();
      default:
        throw InvalidExpr();
    }
  } else {
    switch (string[0]) {
      case '+':
        return new UnaryPlusOperator<T>();
      case '-':
        return new UnaryMinusOperator<T>();
      default:
        throw InvalidExpr();
    }
  }
}

template <typename T>
void ExprInPolishNotation<T>::ProcessOperand(const std::string& raw_token) {
  if (!next_is_operand_) {
    throw InvalidExpr();
  }

  current_tokens_.push_back(new OperandToken<T>(raw_token));

  if (!operators_stack_.empty()) {
    AbstractToken* last_operator = operators_stack_.top();
    auto* unary_op = dynamic_cast<UnaryOperator<T>*>(last_operator);
    if (unary_op != nullptr) {
      current_tokens_.push_back(last_operator);
      operators_stack_.pop();
    }
  }

  last_is_operator_ = false;
  next_is_operand_ = false;
}

template <typename T>
void ExprInPolishNotation<T>::ProcessOpeningBracket(
    const std::string& raw_token) {
  if (!next_is_operand_) {
    throw InvalidExpr();
  }

  operators_stack_.push(new BracketToken(raw_token));
  last_is_operator_ = false;
  next_is_operand_ = true;
}

template <typename T>
void ExprInPolishNotation<T>::ProcessClosingBracket() {
  if (next_is_operand_) {
    throw InvalidExpr();
  }

  bool found_opening_bracket = false;
  while (!operators_stack_.empty()) {
    AbstractToken* last_operator = operators_stack_.top();
    BracketToken* bracket = dynamic_cast<BracketToken*>(last_operator);

    if (bracket != nullptr && bracket->IsOpening()) {
      delete bracket;
      operators_stack_.pop();
      found_opening_bracket = true;
      break;
    }

    current_tokens_.push_back(last_operator);
    operators_stack_.pop();
  }

  if (!found_opening_bracket) {
    throw InvalidExpr();
  }

  last_is_operator_ = false;
  next_is_operand_ = false;
}

template <typename T>
void ExprInPolishNotation<T>::PopHigherPriorityOps(
    const std::string& raw_token) {
  while (!operators_stack_.empty()) {
    AbstractToken* last_operator = operators_stack_.top();

    BracketToken* bracket = dynamic_cast<BracketToken*>(last_operator);
    if (bracket != nullptr) {
      break;
    }

    auto* unary = dynamic_cast<UnaryOperator<T>*>(last_operator);
    if (unary != nullptr) {
      break;
    }

    auto* top_operator = dynamic_cast<BinaryOperator<T>*>(last_operator);

    if (top_operator != nullptr &&
        GetPriority(top_operator->GetStringToken()) >= GetPriority(raw_token)) {
      current_tokens_.push_back(last_operator);
      operators_stack_.pop();
    } else {
      break;
    }
  }
}

template <typename T>
void ExprInPolishNotation<T>::ProcessPlusMinus(const std::string& raw_token) {
  if (next_is_operand_) {
    operators_stack_.push(ConstructOperatorToken(raw_token, false));
  } else {
    if (last_is_operator_) {
      throw InvalidExpr();
    }

    PopHigherPriorityOps(raw_token);

    operators_stack_.push(ConstructOperatorToken(raw_token, true));
  }

  last_is_operator_ = true;
  next_is_operand_ = true;
}

template <typename T>
void ExprInPolishNotation<T>::ProcessMultiDivis(const std::string& raw_token) {
  if (last_is_operator_ || next_is_operand_) {
    throw InvalidExpr();
  }

  PopHigherPriorityOps(raw_token);

  operators_stack_.push(ConstructOperatorToken(raw_token, true));
  last_is_operator_ = true;
  next_is_operand_ = true;
}

template <typename T>
void ExprInPolishNotation<T>::HelperForNormalToRPN(
    const std::string& raw_token) {
  if (IsCorrectNumber(raw_token)) {
    ProcessOperand(raw_token);
  } else if (raw_token == "(") {
    ProcessOpeningBracket(raw_token);
  } else if (raw_token == ")") {
    ProcessClosingBracket();
  } else if (raw_token == "+" || raw_token == "-") {
    ProcessPlusMinus(raw_token);
  } else if (raw_token == "*" || raw_token == "/") {
    ProcessMultiDivis(raw_token);
  } else {
    throw InvalidExpr();
  }
}

template <typename T>
void ExprInPolishNotation<T>::TransportRemainingStack() {
  while (!operators_stack_.empty()) {
    AbstractToken* last_operator = operators_stack_.top();
    BracketToken* bracket = dynamic_cast<BracketToken*>(last_operator);
    if (bracket != nullptr) {
      throw InvalidExpr();
    }
    current_tokens_.push_back(last_operator);
    operators_stack_.pop();
  }
}

template <typename T>
void ExprInPolishNotation<T>::ErrorNormalToRPN() {
  for (auto* token : current_tokens_) {
    delete token;
  }
  while (!operators_stack_.empty()) {
    AbstractToken* last_operator = operators_stack_.top();
    delete last_operator;
    operators_stack_.pop();
  }
}

template <typename T>
std::vector<AbstractToken*> ExprInPolishNotation<T>::NormalToRPN(
    const std::vector<std::string>& raw_tokens) {
  current_tokens_.clear();

  last_is_operator_ = false;
  next_is_operand_ = true;

  try {
    for (const std::string& raw_token : raw_tokens) {
      HelperForNormalToRPN(raw_token);
    }

    if (next_is_operand_) {
      throw InvalidExpr();
    }

    TransportRemainingStack();

    return current_tokens_;

  } catch (...) {
    ErrorNormalToRPN();

    throw;
  }
}

template <typename T>
void ExprInPolishNotation<T>::OperandProcess(
    AbstractToken* token,
    std::vector<std::vector<AbstractToken*>>& tokens_vector) {
  std::vector<AbstractToken*> part;
  part.push_back(token);
  tokens_vector.push_back(part);
}

template <typename T>
void ExprInPolishNotation<T>::UnaryOperandProcess(
    AbstractToken* token,
    std::vector<std::vector<AbstractToken*>>& tokens_vector) {
  std::vector<AbstractToken*> operand = tokens_vector.back();
  tokens_vector.pop_back();
  std::vector<AbstractToken*> part;
  part.push_back(token);
  part.insert(part.end(), operand.begin(), operand.end());
  tokens_vector.push_back(part);
}

template <typename T>
void ExprInPolishNotation<T>::BinaryOperandProcess(
    AbstractToken* token,
    std::vector<std::vector<AbstractToken*>>& tokens_vector) {
  std::vector<AbstractToken*> operand2 = tokens_vector.back();
  tokens_vector.pop_back();
  std::vector<AbstractToken*> operand1 = tokens_vector.back();
  tokens_vector.pop_back();
  std::vector<AbstractToken*> part;
  part.push_back(token);
  part.insert(part.end(), operand1.begin(), operand1.end());
  part.insert(part.end(), operand2.begin(), operand2.end());
  tokens_vector.push_back(part);
}

template <typename T>
void ExprInPolishNotation<T>::HelperForRPNToPN(
    AbstractToken* token,
    std::vector<std::vector<AbstractToken*>>& tokens_vector) {
  if (dynamic_cast<OperandToken<T>*>(token) != nullptr) {
    OperandProcess(token, tokens_vector);

  } else if (dynamic_cast<UnaryOperator<T>*>(token)) {
    if (tokens_vector.empty()) {
      throw InvalidExpr();
    }
    UnaryOperandProcess(token, tokens_vector);
  } else if (dynamic_cast<BinaryOperator<T>*>(token)) {
    if (tokens_vector.size() < 2) {
      throw InvalidExpr();
    }
    BinaryOperandProcess(token, tokens_vector);
  } else {
    throw InvalidExpr();
  }
}

template <typename T>
std::vector<AbstractToken*> ExprInPolishNotation<T>::RPNToPN(
    std::vector<AbstractToken*>& rpn_tokens) {
  std::vector<std::vector<AbstractToken*>> tokens_vector;

  try {
    for (AbstractToken* token : rpn_tokens) {
      HelperForRPNToPN(token, tokens_vector);
    }

    if (tokens_vector.size() != 1) {
      throw InvalidExpr();
    }

    return tokens_vector.back();

  } catch (...) {
    while (!tokens_vector.empty()) {
      tokens_vector.pop_back();
    }

    throw;
  }
}
