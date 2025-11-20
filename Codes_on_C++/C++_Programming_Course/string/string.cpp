#include "string.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

// Доп функции
size_t Strlen(const char* str) {
  if (str == nullptr) {
    return 0;
  }
  size_t length = 0;
  while (str[length] != '\0') {
    ++length;
  }
  return length;
}

void String::Changecap(size_t new_capacity) {
  if (new_capacity == 0) {
    capacity_ = 0;
    size_ = 0;
    delete[] data_;
    data_ = nullptr;
  } else {
    size_ = std::min(size_, new_capacity);
    char* new_data = new char[new_capacity + 1];
    if (data_ != nullptr) {
      for (size_t i = 0; i < size_; ++i) {
        new_data[i] = data_[i];
      }
      delete[] data_;
    }
    data_ = new_data;
    capacity_ = new_capacity;
    data_[size_] = '\0';
  }
}

bool String::Isseparator(char character) {
    return character == ' ' || character == '\n' || character == '\t' || 
           character == '\r' || character == '\v' || character == '\f';
}

// Конструкторы
String::String() = default;

String::String(size_t size, char character) : capacity_(size), size_(size) {
  if (capacity_ > 0) {
    data_ = new char[capacity_ + 1];
    for (size_t i = 0; i < size_; ++i) {
      data_[i] = character;
    }
    data_[size_] = '\0';
  } else {
    data_ = nullptr;
  }
}

String::String(const char* str)
    : capacity_(Strlen(str)), size_(Strlen(str)) {
  if (capacity_ > 0) {
    data_ = new char[capacity_ + 1];
    for (size_t i = 0; i < size_; ++i) {
      data_[i] = str[i];
    }
    data_[size_] = '\0';
  } else {
    data_ = nullptr;
  }
}

// Правило трех
String::~String() { delete[] data_; }

String::String(const String& other)
    : capacity_(other.capacity_), size_(other.size_) {
  if (capacity_ != 0) {
    data_ = new char[capacity_ + 1];
    for (size_t i = 0; i < size_; ++i) {
      data_[i] = other.data_[i];
    }
    data_[size_] = '\0';
  } else {
    data_ = nullptr;
  }
}

String& String::operator=(const String& other) {
  if (this != &other) {
    delete[] data_;
    size_ = other.size_;
    capacity_ = other.capacity_;
    if (capacity_ == 0) {
      data_ = nullptr;
    } else {
      data_ = new char[capacity_ + 1];
      for (size_t i = 0; i < size_; ++i) {
        data_[i] = other.data_[i];
      }
      data_[size_] = '\0';
    }
  }
  return *this;
}

// Методы
void String::Clear() {
  size_ = 0;
  data_[0] = '\0';
}

void String::PushBack(char character) {
  if (data_ == nullptr or size_ >= capacity_) {
    size_t new_capacity = (capacity_ == 0) ? 2 : capacity_ * 2;
    Changecap(new_capacity);
  }
  data_[size_] = character;
  ++size_;
  data_[size_] = '\0';
}

void String::PopBack() {
  if (size_ != 0) {
    --size_;
    data_[size_] = '\0';
  }
}

void String::Resize(size_t new_size) { Resize(new_size, '\0'); }

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
  size_t temp_capacity = other.capacity_;
  size_t temp_size = other.size_;
  char* temp_data = other.data_;

  other.capacity_ = capacity_;
  other.size_ = size_;
  other.data_ = data_;

  capacity_ = temp_capacity;
  size_ = temp_size;
  data_ = temp_data;
}

// Доступ к элементам
char& String::operator[](size_t index) { return data_[index]; }

const char& String::operator[](size_t index) const { return data_[index]; }

char& String::Front() { return data_[0]; }

const char& String::Front() const { return data_[0]; }

char& String::Back() { return data_[size_ - 1]; }

const char& String::Back() const { return data_[size_ - 1]; }

// Получение информации
bool String::Empty() const { return size_ == 0; }

size_t String::Size() const { return size_; }

size_t String::Capacity() const { return capacity_; }

char* String::Data() { return data_; }

const char* String::Data() const { return data_; }

// Перегрузка операторов
bool String::operator<(const String& other) const {
  for (size_t i = 0; i < size_ and i < other.size_; ++i) {
    if (data_[i] != other.data_[i]) {
      return data_[i] < other.data_[i];
    }
  }
  return size_ < other.size_;
}

bool String::operator>(const String& other) const { return other < *this; }

bool String::operator<=(const String& other) const { return !(other < *this); }

bool String::operator>=(const String& other) const { return !(*this < other); }

bool String::operator==(const String& other) const {
  if (size_ == other.size_) {
    for (size_t i = 0; i < size_; ++i) {
      if (data_[i] != other.data_[i]) {
        return false;
      }
    }
  } else {
    return false;
  }
  return true;
}

bool String::operator!=(const String& other) const { return !(*this == other); }

String& String::operator*=(size_t num) {
  if (num == 0) {
    Clear();
    return *this;
  }

  if (num == 1) {
    return *this;
  }

  String temp = *this;
  Clear();
  for (size_t j = 0; j < num; ++j) {
    for (size_t i = 0; i < temp.size_; ++i) {
      PushBack(temp.data_[i]);
    }
  }

  return *this;
}

String& String::operator+=(const String& other) {
  for (size_t i = 0; i < other.size_; ++i) {
    PushBack(other.data_[i]);
  }
  return *this;
}

String String::operator*(size_t num) const {
  if (num != 0) {
    String temp;
    temp.size_ = size_ * num;
    temp.Reserve(temp.size_);
    for (size_t i = 0; i < temp.size_; i++) {
      temp.data_[i] = data_[i % size_];
    }
    return temp;
  }
  return String();
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
  std::vector<String> temp;
  if (size_ == 0) {
    temp.push_back(String());
    return temp;
  }
  if (delim.Size() == 0) {
    temp.push_back(*this);
    return temp;
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
    if (ismatch or last_pos == size_) {
      Addstring(temp, start_pos, last_pos);
      start_pos = last_pos + delim.Size();
      last_pos = start_pos - 1;
    }
    ++last_pos;
  }
  return temp;
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
  for (const String& temp : strings) {
    length += temp.Size();
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
  char character;
  while (is.get(character) and Isseparator(character)) {
    // skip
  }
  if (is) {
    do {
      str.PushBack(character);
    } while (is.get(character) and !Isseparator(character));
  }
  return is;
}

std::ostream& operator<<(std::ostream& os, const String& str) {
  if (os) {
    for (size_t i = 0; i < str.Size(); ++i) {
      os << str[i];
    }
  }
  return os;
}