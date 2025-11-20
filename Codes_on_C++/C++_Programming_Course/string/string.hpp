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
  void Resize(size_t new_size);
  void Resize(size_t new_size, char character);
  void Reserve(size_t new_cap);
  void ShrinkToFit();
  void Swap(String& other);

  char& operator[](size_t index);
  const char& operator[](size_t index) const;
  char& Front();
  const char& Front() const;
  char& Back();
  const char& Back() const;

  bool Empty() const;
  size_t Size() const;
  size_t Capacity() const;
  char* Data();
  const char* Data() const;

  bool operator<(const String& other) const;
  bool operator>(const String& other) const;
  bool operator<=(const String& other) const;
  bool operator>=(const String& other) const;
  bool operator==(const String& other) const;
  bool operator!=(const String& other) const;

  String& operator*=(size_t num);
  String& operator+=(const String& other);
  String operator*(size_t num) const;

  std::vector<String> Split(const String& delim = " ") const;
  String Join(const std::vector<String>& strings) const;

 private:
  void Changecap(size_t new_capacity);
  static bool Isseparator(char character);
  void Addstring(std::vector<String>& temp, size_t start_pos,
                 size_t last_pos) const;

  char* data_ = nullptr;
  size_t capacity_ = 0;
  size_t size_ = 0;
};

String operator*(size_t num, const String& str) = delete;
String operator+(const String& lhs, const String& rhs);

std::ostream& operator<<(std::ostream& os, const String& str);
std::istream& operator>>(std::istream& is, String& str);

static size_t Strlen(const char* str);

#endif  // STRING_HPP