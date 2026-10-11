#include <iostream>
#include <string>
#include <vector>

static const long long PRIME = 31;
static const long long MOD = 1'000'000'007;

long long one_hash(char character) {
  return static_cast<long long>(character);
}

struct Data {
  Data(std::string str, std::string pattern) : str(str), pattern(pattern), pref(str.length()) {}

  std::string str;
  std::string pattern;
  std::vector<long long> pref;
  std::vector<size_t> result;
  long long pow_p = 1;
  long long pattern_hash = 0;
};

Data Input() {
  std::string str;
  std::string pattern;
  std::cin >> str >> pattern;

  return Data(str, pattern);
}

void FeelPowP(Data& data) {
  for (size_t i = 0; i < data.pattern.length(); ++i) {
    data.pow_p = (data.pow_p * PRIME) % MOD;
  }
}

void FeelPref(Data& data) {
  data.pref[0] = one_hash(data.str[0]);

  for (size_t i = 1; i < data.str.length(); ++i) {
    data.pref[i] = (data.pref[i - 1] * PRIME + one_hash(data.str[i])) % MOD;
  }
}

void CalculatePatternHash(Data& data) {
  for (char character : data.pattern) {
    data.pattern_hash = (data.pattern_hash * PRIME + one_hash(character)) % MOD;
  }
}

void Process(Data& data) {
  if (data.str.length() < data.pattern.length()) {
    return;
  }

  FeelPowP(data);
  FeelPref(data);
  CalculatePatternHash(data);

  long long hash = data.pref[data.pattern.length() - 1];

  if (hash == data.pattern_hash) {
    data.result.push_back(0);
  }

  for (size_t i = 1; i <= data.str.length() - data.pattern.length(); ++i) {
    hash = ((data.pref[i + data.pattern.length() - 1] - data.pow_p * data.pref[i - 1]) % MOD + MOD) % MOD;

    if (hash == data.pattern_hash) {
      data.result.push_back(i);
    }
  }
}

void Output(const Data& data) {
  for (size_t index : data.result) {
    std::cout << index << "\n";
  }
}

int main() {
  Data data = Input();
  Process(data);  
  Output(data);
}