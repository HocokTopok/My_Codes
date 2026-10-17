#include <iostream>

void Print(bool)
{
  std::cout << "bool";
}

void Print(const std::string&)
{
  std::cout << "string";
}

int main()
{
  Print("Hello");
}