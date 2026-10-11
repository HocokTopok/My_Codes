#pragma once

#include <cstdint>
#include <string>
#include <vector>

enum class MazeType {
  SparseWalls,   // 10%
  MediumWalls,   // 18%
  DenseWalls     // 32%
};

enum class MazeSize {
  Small,
  Large,
  VeryLarge
};

struct TestPoint {
  int row = 0;
  int col = 0;
};

struct MazeSpec {
  const char* name;
  MazeType type;
  MazeSize size;
  int n;
  int m;
  std::uint64_t seed;
  unsigned wall_percent;
};

struct MazeCase {
  std::string name;
  MazeType type;
  MazeSize size;
  int n = 0;
  int m = 0;
  TestPoint start;
  TestPoint finish;
  std::vector<std::string> lab;
};

// 3 wall densities x 3 sizes = 9 tests.
const std::vector<MazeSpec>& AllMazeSpecs();

// Generation happens outside the measured benchmark region.
MazeCase BuildMaze(const MazeSpec& spec);

const char* MazeTypeName(MazeType type);
const char* MazeSizeName(MazeSize size);
