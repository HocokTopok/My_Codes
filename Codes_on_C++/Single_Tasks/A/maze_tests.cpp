#include "maze_tests.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>

namespace {

constexpr std::uint64_t kMul1 = 0xbf58476d1ce4e5b9ULL;
constexpr std::uint64_t kMul2 = 0x94d049bb133111ebULL;

std::uint64_t Mix(std::uint64_t x) noexcept {
  x += 0x9e3779b97f4a7c15ULL;
  x = (x ^ (x >> 30)) * kMul1;
  x = (x ^ (x >> 27)) * kMul2;
  return x ^ (x >> 31);
}

std::uint64_t CellHash(int r, int c, std::uint64_t seed) noexcept {
  std::uint64_t x = seed;
  x ^= static_cast<std::uint64_t>(static_cast<std::uint32_t>(r)) *
       0x9e3779b185ebca87ULL;
  x ^= static_cast<std::uint64_t>(static_cast<std::uint32_t>(c)) *
       0xc2b2ae3d27d4eb4fULL;
  return Mix(x);
}

void MakeBorderWalls(MazeCase& maze) {
  std::fill(maze.lab.front().begin(), maze.lab.front().end(), '#');
  std::fill(maze.lab.back().begin(), maze.lab.back().end(), '#');

  for (int r = 0; r < maze.n; ++r) {
    maze.lab[r][0] = '#';
    maze.lab[r][maze.m - 1] = '#';
  }
}

void FillWallsByDensity(MazeCase& maze, std::uint64_t seed, unsigned percent) {
  assert(percent <= 100);

  for (int r = 1; r < maze.n - 1; ++r) {
    std::string& row = maze.lab[r];
    for (int c = 1; c < maze.m - 1; ++c) {
      row[c] = (CellHash(r, c, seed) % 100u < percent) ? '#' : '.';
    }
  }
}

// Guarantees connectivity while changing only O(n + m) cells out of O(n*m).
// This is intentionally only one cell wide, so the requested wall density
// is practically unchanged even for SMALL.
void CarveMinimalGuaranteedPath(MazeCase& maze) {
  const int sr = maze.start.row;
  const int sc = maze.start.col;
  const int fr = maze.finish.row;
  const int fc = maze.finish.col;

  const int r_step = (fr >= sr) ? 1 : -1;
  for (int r = sr; r != fr; r += r_step) {
    maze.lab[r][sc] = '.';
  }
  maze.lab[fr][sc] = '.';

  const int c_step = (fc >= sc) ? 1 : -1;
  for (int c = sc; c != fc; c += c_step) {
    maze.lab[fr][c] = '.';
  }
  maze.lab[fr][fc] = '.';
}

}  // namespace

const std::vector<MazeSpec>& AllMazeSpecs() {
  // Approximate cell counts:
  // SMALL      : 1001  * 1001  =   1,002,001
  // LARGE      : 3163  * 3163  =  10,004,569
  // VERY_LARGE : 10001 * 10001 = 100,020,001
  static const std::vector<MazeSpec> specs = {
      {"sparse_small",      MazeType::SparseWalls, MazeSize::Small,     1001,  1001,  0x1101, 10},
      {"sparse_large",      MazeType::SparseWalls, MazeSize::Large,     3163,  3163,  0x1102, 10},
      {"sparse_very_large", MazeType::SparseWalls, MazeSize::VeryLarge, 10001, 10001, 0x1103, 10},

      {"medium_small",      MazeType::MediumWalls, MazeSize::Small,     1001,  1001,  0x2201, 18},
      {"medium_large",      MazeType::MediumWalls, MazeSize::Large,     3163,  3163,  0x2202, 18},
      {"medium_very_large", MazeType::MediumWalls, MazeSize::VeryLarge, 10001, 10001, 0x2203, 18},

      {"dense_small",       MazeType::DenseWalls,  MazeSize::Small,     1001,  1001,  0x3301, 32},
      {"dense_large",       MazeType::DenseWalls,  MazeSize::Large,     3163,  3163,  0x3302, 32},
      {"dense_very_large",  MazeType::DenseWalls,  MazeSize::VeryLarge, 10001, 10001, 0x3303, 32},
  };

  return specs;
}

MazeCase BuildMaze(const MazeSpec& spec) {
  MazeCase maze;
  maze.name = spec.name;
  maze.type = spec.type;
  maze.size = spec.size;
  maze.n = spec.n;
  maze.m = spec.m;

  maze.lab.assign(maze.n, std::string(maze.m, '.'));
  FillWallsByDensity(maze, spec.seed, spec.wall_percent);
  MakeBorderWalls(maze);

  maze.start = {1, 1};
  maze.finish = {maze.n - 2, maze.m - 2};

  CarveMinimalGuaranteedPath(maze);
  MakeBorderWalls(maze);

  maze.lab[maze.start.row][maze.start.col] = 'S';
  maze.lab[maze.finish.row][maze.finish.col] = 'F';

  return maze;
}

const char* MazeTypeName(MazeType type) {
  switch (type) {
    case MazeType::SparseWalls: return "sparse10";
    case MazeType::MediumWalls: return "medium18";
    case MazeType::DenseWalls:  return "dense32";
  }
  return "unknown";
}

const char* MazeSizeName(MazeSize size) {
  switch (size) {
    case MazeSize::Small:     return "SMALL";
    case MazeSize::Large:     return "LARGE";
    case MazeSize::VeryLarge: return "VERY_LARGE";
  }
  return "UNKNOWN";
}
