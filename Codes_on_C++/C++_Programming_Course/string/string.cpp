#include "string.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

size_t Strlen(const char* str) {
  size_t length = 0;
  while (str[length] != '\0') {
    ++length;
  }
  return length;
}

void String::Changecap(size_t new_capacity) {
  size_ = std::min(size_, new_capacity);
  char* new_data = new char[new_capacity + 1];
  for (size_t i = 0; i < size_; ++i) {
    new_data[i] = data_[i];
  }
  delete[] data_;
  data_ = new_data;
  capacity_ = new_capacity;
  data_[size_] = '\0';
}

bool Isseparator(char character) {
  return character == ' ' || character == '\n' || character == '\t' ||
         character == '\r' || character == '\v' || character == '\f';
}

String::String() : size_(0), capacity_(0), data_(new char[1]) {
  data_[0] = '\0';
}

String::String(size_t size, char character)
    : size_(size), capacity_(size), data_(new char[capacity_ + 1]) {
  for (size_t i = 0; i < size_; ++i) {
    data_[i] = character;
  }
  data_[size_] = '\0';
}

String::String(const char* str)
    : size_(Strlen(str)), capacity_(size_), data_(new char[capacity_ + 1]) {
  for (size_t i = 0; i < size_; ++i) {
    data_[i] = str[i];
  }
  data_[size_] = '\0';
}

String::~String() { delete[] data_; }

String::String(const String& other)
    : size_(other.size_),
      capacity_(other.capacity_),
      data_(new char[capacity_ + 1]) {
  for (size_t i = 0; i < size_; ++i) {
    data_[i] = other.data_[i];
  }
  data_[size_] = '\0';
}

String& String::operator=(const String& other) {
  if (this != &other) {
    String tmp(other);
    Swap(tmp);
  }
  return *this;
}

void String::Clear() {
  size_ = 0;
  data_[0] = '\0';
}

void String::PushBack(char character) {
  if (size_ >= capacity_) {
    size_t new_capacity = (capacity_ == 0) ? 2 : capacity_ * 2;
    Changecap(new_capacity);
  }
  data_[size_] = character;
  ++size_;
  data_[size_] = '\0';
}

void String::PopBack() {
  --size_;
  data_[size_] = '\0';
}

void String::Resize(size_t new_size, char character) {
  if (new_size > capacity_) {
    Changecap(new_size);
  }
  if (new_size > size_) {
    for (size_t i = size_; i < new_size; ++i) {
      data_[i] = character;
    }
  }
  size_ = new_size;
  data_[size_] = '\0';
}

void String::Reserve(size_t new_cap) {
  if (new_cap > capacity_) {
    Changecap(new_cap);
  }
}

void String::ShrinkToFit() {
  if (capacity_ > size_) {
    Changecap(size_);
  }
}

void String::Swap(String& other) {
  std::swap(size_, other.size_);
  std::swap(capacity_, other.capacity_);
  std::swap(data_, other.data_);
}

bool operator<(const String& lhs, const String& rhs) {
  for (size_t i = 0; i < lhs.Size() && i < rhs.Size(); ++i) {
    if (lhs[i] != rhs[i]) {
      return lhs[i] < rhs[i];
    }
  }
  return lhs.Size() < rhs.Size();
}

bool operator>(const String& lhs, const String& rhs) { return rhs < lhs; }

bool operator<=(const String& lhs, const String& rhs) { return !(rhs < lhs); }

bool operator>=(const String& lhs, const String& rhs) { return !(lhs < rhs); }

bool operator==(const String& lhs, const String& rhs) {
  if (lhs.Size() != rhs.Size()) {
    return false;
  }
  for (size_t i = 0; i < lhs.Size(); ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

bool operator!=(const String& lhs, const String& rhs) { return !(lhs == rhs); }

String& String::operator*=(size_t num) {
  if (num == 0) {
    Clear();
    return *this;
  }

  String buffer = *this;
  Reserve(size_ * num);

  for (size_t i = 1; i < num; ++i) {
    std::memcpy(data_ + size_, buffer.data_, buffer.size_);
    size_ += buffer.size_;
  }

  data_[size_] = '\0';

  return *this;
}

String& String::operator+=(const String& other) {
  Reserve(size_ + other.size_);
  for (size_t i = 0; i < other.size_; ++i) {
    data_[size_ + i] = other.data_[i];
  }
  size_ += other.size_;
  data_[size_] = '\0';
  return *this;
}

String String::operator*(size_t num) const {
  String temp = *this;
  temp *= num;
  return temp;
}

String operator+(const String& lhs, const String& rhs) {
  String temp = lhs;
  temp += rhs;
  return temp;
}

void String::Addstring(std::vector<String>& temp, size_t start_pos,
                       size_t last_pos) const {
  String str;
  if (last_pos > start_pos) {
    str.Reserve(last_pos - start_pos);
    for (size_t i = start_pos; i < last_pos; ++i) {
      str.PushBack(data_[i]);
    }
  }
  temp.push_back(str);
}

std::vector<String> String::Split(const String& delim) const {
  std::vector<String> tokens;
  if (Empty()) {
    tokens.push_back(*this);
    return tokens;
  }
  if (delim.Empty()) {
    tokens.push_back(*this);
    return tokens;
  }
  size_t last_pos = 0;
  size_t start_pos = 0;
  while (last_pos <= size_) {
    bool ismatch = last_pos + delim.Size() <= size_;
    if (ismatch) {
      for (size_t i = 0; i < delim.size_; ++i) {
        if (data_[last_pos + i] != delim.data_[i]) {
          ismatch = false;
          break;
        }
      }
    }
    if (ismatch || last_pos == size_) {
      Addstring(tokens, start_pos, last_pos);
      start_pos = last_pos + delim.Size();
      last_pos = start_pos - 1;
    }
    ++last_pos;
  }
  return tokens;
}

String String::Join(const std::vector<String>& strings) const {
  String str;
  if (strings.empty()) {
    return str;
  }
  if (strings.size() == 1) {
    return strings[0];
  }
  size_t length = 0;
  for (const String& token : strings) {
    length += token.Size();
  }
  length += (strings.size() - 1) * Size();
  str.Reserve(length);
  str += strings[0];
  for (size_t i = 1; i < strings.size(); ++i) {
    str += *this;
    str += strings[i];
  }
  return str;
}

std::istream& operator>>(std::istream& is, String& str) {
  str.Clear();
  std::istream::sentry sentry(is);

  if (sentry) {
    char character;
    while (is.get(character)) {
      if (Isseparator(character)) {
        break;
      }
      str.PushBack(character);
    }
  }

  return is;
}

std::ostream& operator<<(std::ostream& os, const String& str) {
  return os << str.Data();
}