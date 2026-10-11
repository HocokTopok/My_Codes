#include <benchmark/benchmark.h>

#include "maze_tests.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <queue>
#include <string>
#include <vector>

namespace bench_common {

using Clock = std::chrono::steady_clock;

struct Dot {
  Dot() = default;
  Dot(int x, int y) : row(x), col(y) {}

  int row = 0;
  int col = 0;
};

bool operator==(const Dot& lhs, const Dot& rhs) noexcept {
  return lhs.row == rhs.row && lhs.col == rhs.col;
}

}  // namespace bench_common

// ============================================================================
// 1. Ordinary BFS
// ============================================================================
namespace bfs_impl {

using bench_common::Dot;

struct Data {
  explicit Data(const MazeCase& maze)
      : n(maze.n),
        m(maze.m),
        start(maze.start.row, maze.start.col),
        finish(maze.finish.row, maze.finish.col),
        lab(maze.lab),
        result(false),
        parent(n, std::vector<char>(m)),
        used(n, std::vector<std::uint8_t>(m, false)) {}

  int n;
  int m;
  Dot start;
  Dot finish;
  const std::vector<std::string>& lab;
  bool result;
  std::vector<std::vector<char>> parent;
  std::vector<std::vector<std::uint8_t>> used;
};

void Search(Data& data) {
  std::queue<Dot> queue;
  queue.push(data.start);
  data.used[data.start.row][data.start.col] = true;

  while (!queue.empty()) {
    Dot current = queue.front();
    queue.pop();

    if (current == data.finish) {
      data.result = true;
      return;
    }

    if (data.lab[current.row + 1][current.col] != '#' &&
        !data.used[current.row + 1][current.col]) {
      data.used[current.row + 1][current.col] = true;
      data.parent[current.row + 1][current.col] = 'U';
      queue.push(Dot(current.row + 1, current.col));
    }
    if (data.lab[current.row - 1][current.col] != '#' &&
        !data.used[current.row - 1][current.col]) {
      data.used[current.row - 1][current.col] = true;
      data.parent[current.row - 1][current.col] = 'D';
      queue.push(Dot(current.row - 1, current.col));
    }
    if (data.lab[current.row][current.col + 1] != '#' &&
        !data.used[current.row][current.col + 1]) {
      data.used[current.row][current.col + 1] = true;
      data.parent[current.row][current.col + 1] = 'L';
      queue.push(Dot(current.row, current.col + 1));
    }
    if (data.lab[current.row][current.col - 1] != '#' &&
        !data.used[current.row][current.col - 1]) {
      data.used[current.row][current.col - 1] = true;
      data.parent[current.row][current.col - 1] = 'R';
      queue.push(Dot(current.row, current.col - 1));
    }
  }
}

double RunTimed(const MazeCase& maze, bool& found) {
  // Construction/allocation/reset are intentionally outside the measured part.
  Data data(maze);

  const auto begin = bench_common::Clock::now();
  Search(data);
  const auto end = bench_common::Clock::now();

  benchmark::DoNotOptimize(data.result);
  benchmark::ClobberMemory();
  found = data.result;

  return std::chrono::duration<double>(end - begin).count();
}

}  // namespace bfs_impl

// ============================================================================
// 2. Bidirectional BFS
// ============================================================================
namespace double_bfs_impl {

using bench_common::Dot;

struct Data {
  explicit Data(const MazeCase& maze)
      : n(maze.n),
        m(maze.m),
        start(maze.start.row, maze.start.col),
        finish(maze.finish.row, maze.finish.col),
        lab(maze.lab),
        result(false),
        parent_from_start(n, std::vector<char>(m)),
        parent_from_finish(n, std::vector<char>(m)),
        used_from_start(n, std::vector<std::uint8_t>(m, false)),
        used_from_finish(n, std::vector<std::uint8_t>(m, false)) {}

  int n;
  int m;
  Dot start;
  Dot finish;
  Dot meeting;
  const std::vector<std::string>& lab;
  bool result;
  std::vector<std::vector<char>> parent_from_start;
  std::vector<std::vector<char>> parent_from_finish;
  std::vector<std::vector<std::uint8_t>> used_from_start;
  std::vector<std::vector<std::uint8_t>> used_from_finish;
};

void Search(Data& data) {
  std::queue<Dot> vertices_from_start;
  std::queue<Dot> vertices_from_finish;
  vertices_from_start.push(data.start);
  vertices_from_finish.push(data.finish);
  data.used_from_start[data.start.row][data.start.col] = true;
  data.used_from_finish[data.finish.row][data.finish.col] = true;

  while (!vertices_from_start.empty() && !vertices_from_finish.empty()) {
    if (vertices_from_start.size() <= vertices_from_finish.size()) {
      const std::size_t layer_size = vertices_from_start.size();

      for (std::size_t i = 0; i < layer_size; ++i) {
        Dot current = vertices_from_start.front();
        vertices_from_start.pop();

        if (data.lab[current.row + 1][current.col] != '#' &&
            !data.used_from_start[current.row + 1][current.col]) {
          data.used_from_start[current.row + 1][current.col] = true;
          data.parent_from_start[current.row + 1][current.col] = 'U';

          if (data.used_from_finish[current.row + 1][current.col]) {
            data.meeting = Dot(current.row + 1, current.col);
            data.result = true;
            return;
          }
          vertices_from_start.push(Dot(current.row + 1, current.col));
        }

        if (data.lab[current.row - 1][current.col] != '#' &&
            !data.used_from_start[current.row - 1][current.col]) {
          data.used_from_start[current.row - 1][current.col] = true;
          data.parent_from_start[current.row - 1][current.col] = 'D';

          if (data.used_from_finish[current.row - 1][current.col]) {
            data.meeting = Dot(current.row - 1, current.col);
            data.result = true;
            return;
          }
          vertices_from_start.push(Dot(current.row - 1, current.col));
        }

        if (data.lab[current.row][current.col + 1] != '#' &&
            !data.used_from_start[current.row][current.col + 1]) {
          data.used_from_start[current.row][current.col + 1] = true;
          data.parent_from_start[current.row][current.col + 1] = 'L';

          if (data.used_from_finish[current.row][current.col + 1]) {
            data.meeting = Dot(current.row, current.col + 1);
            data.result = true;
            return;
          }
          vertices_from_start.push(Dot(current.row, current.col + 1));
        }

        if (data.lab[current.row][current.col - 1] != '#' &&
            !data.used_from_start[current.row][current.col - 1]) {
          data.used_from_start[current.row][current.col - 1] = true;
          data.parent_from_start[current.row][current.col - 1] = 'R';

          if (data.used_from_finish[current.row][current.col - 1]) {
            data.meeting = Dot(current.row, current.col - 1);
            data.result = true;
            return;
          }
          vertices_from_start.push(Dot(current.row, current.col - 1));
        }
      }
    } else {
      const std::size_t layer_size = vertices_from_finish.size();

      for (std::size_t i = 0; i < layer_size; ++i) {
        Dot current = vertices_from_finish.front();
        vertices_from_finish.pop();

        if (data.lab[current.row + 1][current.col] != '#' &&
            !data.used_from_finish[current.row + 1][current.col]) {
          data.used_from_finish[current.row + 1][current.col] = true;
          data.parent_from_finish[current.row + 1][current.col] = 'U';

          if (data.used_from_start[current.row + 1][current.col]) {
            data.meeting = Dot(current.row + 1, current.col);
            data.result = true;
            return;
          }
          vertices_from_finish.push(Dot(current.row + 1, current.col));
        }

        if (data.lab[current.row - 1][current.col] != '#' &&
            !data.used_from_finish[current.row - 1][current.col]) {
          data.used_from_finish[current.row - 1][current.col] = true;
          data.parent_from_finish[current.row - 1][current.col] = 'D';

          if (data.used_from_start[current.row - 1][current.col]) {
            data.meeting = Dot(current.row - 1, current.col);
            data.result = true;
            return;
          }
          vertices_from_finish.push(Dot(current.row - 1, current.col));
        }

        if (data.lab[current.row][current.col + 1] != '#' &&
            !data.used_from_finish[current.row][current.col + 1]) {
          data.used_from_finish[current.row][current.col + 1] = true;
          data.parent_from_finish[current.row][current.col + 1] = 'L';

          if (data.used_from_start[current.row][current.col + 1]) {
            data.meeting = Dot(current.row, current.col + 1);
            data.result = true;
            return;
          }
          vertices_from_finish.push(Dot(current.row, current.col + 1));
        }

        if (data.lab[current.row][current.col - 1] != '#' &&
            !data.used_from_finish[current.row][current.col - 1]) {
          data.used_from_finish[current.row][current.col - 1] = true;
          data.parent_from_finish[current.row][current.col - 1] = 'R';

          if (data.used_from_start[current.row][current.col - 1]) {
            data.meeting = Dot(current.row, current.col - 1);
            data.result = true;
            return;
          }
          vertices_from_finish.push(Dot(current.row, current.col - 1));
        }
      }
    }
  }
}

double RunTimed(const MazeCase& maze, bool& found) {
  Data data(maze);

  const auto begin = bench_common::Clock::now();
  Search(data);
  const auto end = bench_common::Clock::now();

  benchmark::DoNotOptimize(data.result);
  benchmark::ClobberMemory();
  found = data.result;

  return std::chrono::duration<double>(end - begin).count();
}

}  // namespace double_bfs_impl

// ============================================================================
// 3. A*
// ============================================================================
namespace astar_impl {

using bench_common::Dot;

constexpr int kMax = 1'000'000'000;

struct Node {
  Node() = default;
  Node(Dot dot, int priority) : dot(dot), priority(priority) {}

  Dot dot;
  int priority = 0;
};

bool operator<(const Node& lhs, const Node& rhs) noexcept {
  // Reversed comparison => std::priority_queue behaves as a min-heap.
  return lhs.priority > rhs.priority;
}

struct Data {
  explicit Data(const MazeCase& maze)
      : n(maze.n),
        m(maze.m),
        start(maze.start.row, maze.start.col),
        finish(maze.finish.row, maze.finish.col),
        lab(maze.lab),
        result(false),
        parent(n, std::vector<char>(m)),
        used(n, std::vector<std::uint8_t>(m, false)),
        dist(n, std::vector<int>(m, kMax)) {}

  int n;
  int m;
  Dot start;
  Dot finish;
  const std::vector<std::string>& lab;
  bool result;
  std::vector<std::vector<char>> parent;
  std::vector<std::vector<std::uint8_t>> used;
  std::vector<std::vector<int>> dist;
};

int ManhattanDistance(const Data& data, const Dot& dot) noexcept {
  return std::abs(data.finish.row - dot.row) +
         std::abs(data.finish.col - dot.col);
}

void Search(Data& data) {
  std::priority_queue<Node, std::vector<Node>, std::less<Node>> queue;
  data.dist[data.start.row][data.start.col] = 0;
  queue.push({data.start, ManhattanDistance(data, data.start)});

  while (!queue.empty()) {
    Node new_node = queue.top();
    Dot current = new_node.dot;
    queue.pop();

    if (data.used[current.row][current.col]) {
      continue;
    }
    data.used[current.row][current.col] = true;

    if (current == data.finish) {
      data.result = true;
      return;
    }

    if (data.lab[current.row + 1][current.col] != '#' &&
        !data.used[current.row + 1][current.col] &&
        data.dist[current.row + 1][current.col] >
            data.dist[current.row][current.col] + 1) {
      data.parent[current.row + 1][current.col] = 'U';
      data.dist[current.row + 1][current.col] =
          data.dist[current.row][current.col] + 1;
      queue.push({Dot(current.row + 1, current.col),
                  data.dist[current.row + 1][current.col] +
                      ManhattanDistance(data, Dot(current.row + 1, current.col))});
    }

    if (data.lab[current.row - 1][current.col] != '#' &&
        !data.used[current.row - 1][current.col] &&
        data.dist[current.row - 1][current.col] >
            data.dist[current.row][current.col] + 1) {
      data.parent[current.row - 1][current.col] = 'D';
      data.dist[current.row - 1][current.col] =
          data.dist[current.row][current.col] + 1;
      queue.push({Dot(current.row - 1, current.col),
                  data.dist[current.row - 1][current.col] +
                      ManhattanDistance(data, Dot(current.row - 1, current.col))});
    }

    if (data.lab[current.row][current.col + 1] != '#' &&
        !data.used[current.row][current.col + 1] &&
        data.dist[current.row][current.col + 1] >
            data.dist[current.row][current.col] + 1) {
      data.parent[current.row][current.col + 1] = 'L';
      data.dist[current.row][current.col + 1] =
          data.dist[current.row][current.col] + 1;
      queue.push({Dot(current.row, current.col + 1),
                  data.dist[current.row][current.col + 1] +
                      ManhattanDistance(data, Dot(current.row, current.col + 1))});
    }

    if (data.lab[current.row][current.col - 1] != '#' &&
        !data.used[current.row][current.col - 1] &&
        data.dist[current.row][current.col - 1] >
            data.dist[current.row][current.col] + 1) {
      data.parent[current.row][current.col - 1] = 'R';
      data.dist[current.row][current.col - 1] =
          data.dist[current.row][current.col] + 1;
      queue.push({Dot(current.row, current.col - 1),
                  data.dist[current.row][current.col - 1] +
                      ManhattanDistance(data, Dot(current.row, current.col - 1))});
    }
  }
}

double RunTimed(const MazeCase& maze, bool& found) {
  Data data(maze);

  const auto begin = bench_common::Clock::now();
  Search(data);
  const auto end = bench_common::Clock::now();

  benchmark::DoNotOptimize(data.result);
  benchmark::ClobberMemory();
  found = data.result;

  return std::chrono::duration<double>(end - begin).count();
}

}  // namespace astar_impl

// ============================================================================
// Common Google Benchmark wrapper
// ============================================================================

using TimedAlgorithm = double (*)(const MazeCase&, bool&);

void BenchmarkAlgorithmCase(benchmark::State& state,
                            const MazeSpec* spec,
                            TimedAlgorithm algorithm) {
  for (auto _ : state) {
    (void)_;

    // Maze generation is NOT measured.
    MazeCase maze = BuildMaze(*spec);

    bool found = false;
    const double seconds = algorithm(maze, found);

    if (!found) {
      state.SkipWithError("Algorithm did not find the guaranteed path");
      return;
    }

    state.SetIterationTime(seconds);
  }

  state.counters["cells"] =
      static_cast<double>(spec->n) * static_cast<double>(spec->m);
  state.counters["walls_%"] = static_cast<double>(spec->wall_percent);
}

void RegisterAlgorithm(const char* algorithm_name, TimedAlgorithm algorithm) {
  for (const MazeSpec& spec : AllMazeSpecs()) {
    const std::string name = std::string(algorithm_name) + "/" +
                             MazeTypeName(spec.type) + "/" +
                             MazeSizeName(spec.size);

    benchmark::RegisterBenchmark(
        name.c_str(),
        [&spec, algorithm](benchmark::State& state) {
          BenchmarkAlgorithmCase(state, &spec, algorithm);
        })
        ->UseManualTime()
        ->Unit(benchmark::kMillisecond)
        ->Iterations(1);
  }
}

int main(int argc, char** argv) {
  RegisterAlgorithm("BFS", bfs_impl::RunTimed);
  RegisterAlgorithm("DoubleBFS", double_bfs_impl::RunTimed);
  RegisterAlgorithm("AStar", astar_impl::RunTimed);

  // When IDA* is ready:
  // RegisterAlgorithm("IDAStar", ida_star_impl::RunTimed);

  benchmark::Initialize(&argc, argv);
  if (benchmark::ReportUnrecognizedArguments(argc, argv)) {
    return 1;
  }
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
