#ifndef STRING_HPP
#define STRING_HPP

#include <algorithm>
#include <iostream>
#include <vector>

class String {
 public:
  String();
  String(size_t size, char character);
  String(const char* c_str);

  String(const String& other);
  String& operator=(const String& other);
  ~String();

  void Clear();
  void PushBack(char character);
  void PopBack();
  void Resize(size_t new_size, char character = '\0');
  void Reserve(size_t new_cap);
  void ShrinkToFit();
  void Swap(String& other);

  char& operator[](size_t index) { return data_[index]; }
  const char& operator[](size_t index) const { return data_[index]; }

  char& Front() { return data_[0]; }
  const char& Front() const { return data_[0]; }
  char& Back() { return data_[size_ - 1]; }
  const char& Back() const { return data_[size_ - 1]; }

  bool Empty() const { return size_ == 0; }
  size_t Size() const { return size_; }
  size_t Capacity() const { return capacity_; }
  char* Data() { return data_; }
  const char* Data() const { return data_; }

  String& operator*=(size_t num);
  String& operator+=(const String& other);
  String operator*(size_t num) const;

  std::vector<String> Split(const String& delim = " ") const;
  String Join(const std::vector<String>& strings) const;

 private:
  void Changecap(size_t new_capacity);
  void Addstring(std::vector<String>& temp, size_t start_pos,
                 size_t last_pos) const;

  size_t size_ = 0;
  size_t capacity_ = 0;
  char* data_ = nullptr;
};

bool operator<(const String& lhs, const String& rhs);
bool operator>(const String& lhs, const String& rhs);
bool operator<=(const String& lhs, const String& rhs);
bool operator>=(const String& lhs, const String& rhs);
bool operator==(const String& lhs, const String& rhs);
bool operator!=(const String& lhs, const String& rhs);

String operator*(size_t num, const String& str) = delete;
String operator+(const String& lhs, const String& rhs);

std::ostream& operator<<(std::ostream& os, const String& str);
std::istream& operator>>(std::istream& is, String& str);

static size_t Strlen(const char* str);

#endif  // STRING_HPP