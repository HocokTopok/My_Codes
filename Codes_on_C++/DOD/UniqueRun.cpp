////////////////////////////////////////////////////////////////

// clang++ -std=c++23 -O3 -Wall -Wextra -Wpedantic main.cpp -I/opt/homebrew/include -L/opt/homebrew/lib -lgtest -lbenchmark -pthread -o main

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////

// Все значения массива лежат в диапазоне от 0 до 255.
// Верните длину самого длинного непрерывного отрезка без повторов.
// Например, для {1, 2, 1, 3, 4} ответ равен 4: отрезок {2, 1, 3, 4}.
std::size_t FastUniqueRun(const std::vector<int>& values)
{
  // ================= ВАШЕ РЕШЕНИЕ =================
  (void)values;
  throw std::logic_error("FastUniqueRun ещё не реализована");
  // =================================================
}

//  ------------------------------------------------------------

std::size_t SlowUniqueRun(const std::vector<int>& values)
{
  std::size_t best = 0;

  for (std::size_t left = 0; left < values.size(); ++left)
  {
    std::array<bool, 256> seen{};
    bool unique = true;

    for (std::size_t right = left; right < values.size(); ++right)
    {
      std::size_t value = static_cast<std::size_t>(values[right]);

      if (seen[value])
      {
        unique = false;
      }

      seen[value] = true;

      if (unique)
      {
        best = std::max(best, right - left + 1);
      }
    }
  }

  return best;
}

////////////////////////////////////////////////////////////////
//                          TESTS                             //
////////////////////////////////////////////////////////////////

using UniqueRunFunction = std::size_t (*)(const std::vector<int>&);

class UniqueRunTest : public ::testing::TestWithParam<UniqueRunFunction>
{
};

//  ------------------------------------------------------------

TEST_P(UniqueRunTest, EmptyAndSingle)
{
  EXPECT_EQ(GetParam()({}), 0u);
  EXPECT_EQ(GetParam()({42}), 1u);
}

//  ------------------------------------------------------------

TEST_P(UniqueRunTest, EqualAndDistinct)
{
  EXPECT_EQ(GetParam()({7, 7, 7, 7}), 1u);
  EXPECT_EQ(GetParam()({0, 1, 2, 3, 255}), 5u);
}

//  ------------------------------------------------------------

TEST_P(UniqueRunTest, BoundaryRepeats)
{
  EXPECT_EQ(GetParam()({1, 2, 3, 1}), 3u);
  EXPECT_EQ(GetParam()({1, 1, 2, 3}), 3u);
  EXPECT_EQ(GetParam()({1, 2, 3, 3}), 3u);
  EXPECT_EQ(GetParam()({1, 2, 1, 3, 4}), 4u);
}

//  ------------------------------------------------------------

TEST_P(UniqueRunTest, BestInMiddle)
{
  EXPECT_EQ(GetParam()({1, 1, 2, 3, 4, 4, 4}), 4u);
  EXPECT_EQ(GetParam()({0, 255, 0, 255}), 2u);
}

//  ------------------------------------------------------------

TEST_P(UniqueRunTest, FullAlphabet)
{
  std::vector<int> values(256);

  std::iota(values.begin(), values.end(), 0);

  EXPECT_EQ(GetParam()(values), 256u);

  values.push_back(0);

  EXPECT_EQ(GetParam()(values), 256u);
}

//  ------------------------------------------------------------

// Независимый эталон: проверяет пары внутри каждого отрезка.
std::size_t ReferenceUniqueRun(const std::vector<int>& values)
{
  std::size_t best = 0;

  for (std::size_t left = 0; left < values.size(); ++left)
  {
    for (std::size_t right = left; right < values.size(); ++right)
    {
      bool unique = true;

      for (std::size_t i = left; i <= right; ++i)
      {
        for (std::size_t j = i + 1; j <= right; ++j)
        {
          if (values[i] == values[j])
          {
            unique = false;
          }
        }
      }

      if (unique)
      {
        best = std::max(best, right - left + 1);
      }
    }
  }

  return best;
}

//  ------------------------------------------------------------

TEST_P(UniqueRunTest, ExhaustiveSmallArrays)
{
  for (std::size_t size = 0; size <= 6; ++size)
  {
    std::size_t count = 1;

    for (std::size_t i = 0; i < size; ++i)
    {
      count *= 3;
    }

    for (std::size_t code = 0; code < count; ++code)
    {
      std::vector<int> values(size);
      std::size_t remaining = code;

      for (int& value : values)
      {
        value = static_cast<int>(remaining % 3);
        remaining /= 3;
      }

      ASSERT_EQ(GetParam()(values), ReferenceUniqueRun(values))
        << "size=" << size << ", code=" << code;
    }
  }
}

//  ------------------------------------------------------------

INSTANTIATE_TEST_SUITE_P(
  BothFunctions, UniqueRunTest,
  ::testing::Values(&FastUniqueRun, &SlowUniqueRun),
  [](const ::testing::TestParamInfo<UniqueRunFunction>& info)
  {
    return info.index == 0 ? "Fast" : "Slow";
  });

//  ------------------------------------------------------------

TEST(UniqueRunComparison, RandomValues)
{
  std::mt19937 generator(67);
  std::uniform_int_distribution<int> distribution(0, 255);

  for (int sample = 0; sample < 150; ++sample)
  {
    std::size_t size = static_cast<std::size_t>(sample * 7 % 513);
    std::vector<int> values(size);

    for (int& value : values)
    {
      value = distribution(generator);
    }

    ASSERT_EQ(FastUniqueRun(values), SlowUniqueRun(values)) << "sample=" << sample;
  }
}

////////////////////////////////////////////////////////////////
//                        BENCHMARKS                          //
////////////////////////////////////////////////////////////////

using InputBatch = std::vector<std::vector<int>>;

InputBatch GenerateInputs(std::size_t size, bool long_runs)
{
  std::mt19937 generator(67u + static_cast<std::uint32_t>(size));
  std::uniform_int_distribution<int> distribution(0, 255);
  InputBatch inputs(16, std::vector<int>(size));

  for (std::vector<int>& values : inputs)
  {
    if (!long_runs)
    {
      for (int& value : values)
      {
        value = distribution(generator);
      }
    }
    else
    {
      std::array<int, 256> alphabet{};

      std::iota(alphabet.begin(), alphabet.end(), 0);

      for (std::size_t offset = 0; offset < size; offset += alphabet.size())
      {
        std::shuffle(alphabet.begin(), alphabet.end(), generator);

        std::size_t count = std::min(alphabet.size(), size - offset);

        for (std::size_t i = 0; i < count; ++i)
        {
          values[offset + i] = alphabet[i];
        }
      }
    }
  }

  return inputs;
}

//  ------------------------------------------------------------

void MeasureUniqueRun(benchmark::State& state, UniqueRunFunction function,
                      const InputBatch& inputs)
{
  std::size_t sample = 0;

  try
  {
    for (auto _ : state)
    {
      const std::vector<int>& values = inputs[sample];

      benchmark::DoNotOptimize(values.data());
      benchmark::ClobberMemory();

      std::size_t result = function(values);

      benchmark::DoNotOptimize(result);

      sample = (sample + 1) % inputs.size();
    }
  }
  catch (const std::exception& error)
  {
    state.SkipWithError(error.what());
  }
  catch (...)
  {
    state.SkipWithError("Неизвестное исключение в функции участника");
  }
}

//  ------------------------------------------------------------

class ComparisonReporter : public benchmark::BenchmarkReporter
{
public:
  bool failed = false;

  bool ReportContext(const Context&) override
  {
    std::cout << "\nСреднее CPU-время одного вызова, 5 повторений.\n";

    return true;
  }

  void ReportRuns(const std::vector<Run>& reports) override
  {
    for (const Run& run : reports)
    {
      if (run.skipped != benchmark::internal::NotSkipped)
      {
        std::cerr << run.benchmark_name() << ": " << run.skip_message << '\n';
        failed = true;

        continue;
      }

      if (run.run_type != Run::RT_Aggregate || run.aggregate_name != "mean")
      {
        continue;
      }

      const std::string& name = run.run_name.function_name;
      std::size_t separator = name.find('/');
      std::string algorithm = name.substr(0, separator);
      std::string scenario = name.substr(separator + 1);
      double time = run.GetAdjustedCPUTime();

      if (!std::isfinite(time) || time <= 0.0)
      {
        failed = true;

        continue;
      }

      if (algorithm == "Slow")
      {
        results[scenario].slow = time;
      }
      else
      {
        results[scenario].fast = time;
      }
    }
  }

  void Finalize() override
  {
    std::size_t fast_wins = 0;
    std::size_t slow_wins = 0;
    std::size_t equal = 0;

    std::cout << std::fixed << std::setprecision(2);

    for (const auto& [scenario, times] : results)
    {
      if (times.slow <= 0.0 || times.fast <= 0.0)
      {
        std::cout << scenario << ": нет пары замеров для сравнения.\n";

        continue;
      }

      double reduction = 100.0 * (times.slow - times.fast) / times.slow;

      std::cout << scenario << ": Slow = " << times.slow << " нс, Fast = "
                << times.fast << " нс; Slow / Fast = " << times.slow / times.fast
                << "; сокращение времени = " << reduction << "%. ";

      if (times.fast < times.slow)
      {
        ++fast_wins;
        std::cout << "Fast быстрее в " << times.slow / times.fast << " раза.\n";
      }
      else if (times.fast > times.slow)
      {
        ++slow_wins;
        std::cout << "Fast медленнее в " << times.fast / times.slow << " раза.\n";
      }
      else
      {
        ++equal;
        std::cout << "Время одинаковое.\n";
      }
    }

    std::cout << '\n';

    if (fast_wins + slow_wins + equal == 0)
    {
      std::cout << "Недостаточно данных, чтобы сравнить реализации.\n";
    }
    else if (slow_wins == 0 && equal == 0)
    {
      std::cout << "Ваша реализация (Fast) быстрее готовой (Slow) во всех сравниваемых случаях.\n";
    }
    else if (fast_wins == 0 && equal == 0)
    {
      std::cout << "Готовая реализация (Slow) быстрее вашей (Fast) во всех сравниваемых случаях.\n";
    }
    else if (fast_wins == 0 && slow_wins == 0)
    {
      std::cout << "Обе реализации показали одинаковое время во всех сравниваемых случаях.\n";
    }
    else
    {
      std::cout << "Результат зависит от входных данных: ваша реализация быстрее в "
                << fast_wins << " случаях, готовая — в " << slow_wins
                << ", одинаковое время — в " << equal << ".\n";
    }
  }

private:
  struct Times
  {
    double slow = 0.0;
    double fast = 0.0;
  };

  std::map<std::string, Times> results;
};

//  ------------------------------------------------------------

int main(int argc, char** argv)
{
  bool tests_only = false;

  for (int i = 1; i < argc;)
  {
    if (std::string(argv[i]) == "--tests-only")
    {
      tests_only = true;

      for (int j = i; j < argc; ++j)
      {
        argv[j] = argv[j + 1];
      }

      --argc;
    }
    else
    {
      ++i;
    }
  }

  ::testing::InitGoogleTest(&argc, argv);

  int result = RUN_ALL_TESTS();

  if (result != 0)
  {
    std::cout << "\nЗамеры отменены: сначала исправьте решение.\n";

    return result;
  }

  if (tests_only)
  {
    return 0;
  }

  benchmark::Initialize(&argc, argv);

  if (benchmark::ReportUnrecognizedArguments(argc, argv))
  {
    return 1;
  }

  // Хранилище живёт до завершения всех замеров; лямбды ссылаются на его элементы.
  std::array<InputBatch, 8> batches;
  std::size_t batch_index = 0;

  for (bool long_runs : {false, true})
  {
    for (std::size_t size : {64u, 256u, 1024u, 4096u})
    {
      InputBatch& inputs = batches[batch_index++];

      inputs = GenerateInputs(size, long_runs);

      // Проверяем также точные входные данные будущих замеров.
      for (const std::vector<int>& values : inputs)
      {
        try
        {
          if (FastUniqueRun(values) != SlowUniqueRun(values))
          {
            std::cerr << "Ошибка на данных бенчмарка. Замеры отменены.\n";

            return 1;
          }
        }
        catch (const std::exception& error)
        {
          std::cerr << "Ошибка на данных бенчмарка: " << error.what() << '\n';

          return 1;
        }
        catch (...)
        {
          std::cerr << "Исключение на данных бенчмарка. Замеры отменены.\n";

          return 1;
        }
      }

      std::string scenario = std::string(long_runs ? "LongRuns/" : "Random/")
                             + std::to_string(size);

      for (bool fast : {false, true})
      {
        UniqueRunFunction function = fast ? &FastUniqueRun : &SlowUniqueRun;
        std::string name = std::string(fast ? "Fast/" : "Slow/") + scenario;

        benchmark::RegisterBenchmark(name.c_str(),
          [function, &inputs](benchmark::State& state)
          {
            MeasureUniqueRun(state, function, inputs);
          })->Unit(benchmark::kNanosecond)->MinTime(0.05)->Repetitions(5);
      }
    }
  }

  ComparisonReporter reporter;

  std::size_t matched = benchmark::RunSpecifiedBenchmarks(&reporter);

  benchmark::Shutdown();

  if (matched == 0)
  {
    std::cerr << "Нет бенчмарков, соответствующих фильтру.\n";

    return 1;
  }

  return reporter.failed ? 1 : 0;
}

////////////////////////////////////////////////////////////////
