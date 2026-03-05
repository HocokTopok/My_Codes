#pragma once

#include "AbstractToken.hpp"

class BracketToken : public AbstractToken {
 public:
  BracketToken(const std::string& input_string) : AbstractToken(input_string) {}

  bool IsOpening() const { return GetStringToken() == "("; }
};
