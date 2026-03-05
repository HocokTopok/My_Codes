#pragma once

#include <string>

class AbstractToken {
 public:
  AbstractToken(const std::string& input_string) : string_(input_string) {}

  virtual ~AbstractToken() = default;

  const std::string& GetStringToken() const { return string_; }

 protected:
  std::string string_;
};
