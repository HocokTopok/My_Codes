#include <iostream>

class Vault { int secret = 42; };

struct Tag {
  using type = int Vault::*;
  friend type key(Tag);
};

template<Tag::type P>
struct Unlock {
  friend Tag::type key(Tag) { return P; }
};

template struct Unlock<&Vault::secret>;

int main() {
  Vault v;
  std::cout << v.*key(Tag{});
}