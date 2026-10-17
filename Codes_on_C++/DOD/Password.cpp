////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>

////////////////////////////////////////////////////////////////

// Пароль должен содержать не менее 8 символов,
// хотя бы одну цифру и хотя бы одну заглавную латинскую букву.
bool CheckPassword(const std::string& password)
{
  bool has_digit = false;
  bool has_upper = false;

  for (char symbol : password)
  {
    if (symbol >= '0' && symbol <= '9')
    {
      has_digit = true;
    }

    if (symbol >= 'A' && symbol <= 'Z')
    {
      has_upper = true;
    }
  }

  // ================ ИСПРАВЬТЕ ПРОВЕРКУ ================
  return (password.size() >= 8 && has_digit) || has_upper;
  // ====================================================
}

////////////////////////////////////////////////////////////////

int main()
{
  std::cout << "Введите пароль: " << std::flush;

  std::string password;

  if (!std::getline(std::cin, password))
  {
    return 1;
  }

  std::cout << (CheckPassword(password) ? "ДОСТУП РАЗРЕШЁН" : "ДОСТУП ЗАПРЕЩЁН") << '\n';
}

////////////////////////////////////////////////////////////////
