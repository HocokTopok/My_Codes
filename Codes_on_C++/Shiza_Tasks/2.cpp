#include <iostream>

void f(...) { std::cout << 'A'; }

struct X {
  friend void f(X) { std::cout << 'B'; }
};

int main() {
  X x;
  (f)(x);
}