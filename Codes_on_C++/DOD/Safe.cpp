////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>

////////////////////////////////////////////////////////////////

bool CheckKey(const std::string& key)
{
  if (key.size() != 6)
  {
    return false;
  }

  int sum = 0;

  for (char digit : key)
  {
    if (digit < '0' || digit > '9')
    {
      return false;
    }

    sum += digit - '0';
  }

  if (sum != 27)
  {
    return false;
  }

  int first = key[0] - '0';
  int last = key[5] - '0';

  if (first + last != 9)
  {
    return false;
  }

  int left = (key[0] - '0') * 100 + (key[1] - '0') * 10 + key[2] - '0';
  int right = (key[3] - '0') * 100 + (key[4] - '0') * 10 + key[5] - '0';

  return right == left + 36;
}

////////////////////////////////////////////////////////////////

int main()
{
  std::cout << "Сейф: ключ из шести цифр. У вас три попытки.\n";

  for (int attempt = 1; attempt <= 3; ++attempt)
  {
    std::cout << "Попытка " << attempt << ": " << std::flush;

    std::string key;

    if (!std::getline(std::cin, key))
    {
      break;
    }

    if (CheckKey(key))
    {
      std::cout << "ПОБЕДА\n";

      return 0;
    }

    std::cout << "Ключ не подошёл.\n";
  }

  std::cout << "ПОРАЖЕНИЕ\n";

  return 1;
}

////////////////////////////////////////////////////////////////
