#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

const int kBoardSize = 16;
const int kBoardSide = 4;
const int kMaxMove = 80;
const int kInf = 100;

std::vector<std::vector<int>> manhattan_distance(kBoardSize,
                                                 std::vector<int>(kBoardSize,
                                                                  0));

char move_chars[] = {'L', 'R', 'U', 'D'};

int delta_row[] = {0, 0, 1, -1};
int delta_col[] = {1, -1, 0, 0};
int opposite_move[] = {1, 0, 3, 2};

std::string solution_path;
int current_limit;
int next_limit;

int Manhattan(int* board) {
  int result = 0;
  for (int pos = 0; pos < kBoardSize; pos++) {
    if (board[pos] != 0) {
      result += manhattan_distance[board[pos]][pos];
    }
  }
  return result;
}

int RowConflicts(const int* board) {
  int result = 0;
  for (int row = 0; row < kBoardSide; row++) {
    for (int col = 0; col < kBoardSide; col++) {
      int tile_1 = board[row * kBoardSide + col];
      if (tile_1 != 0 && (tile_1 - 1) / kBoardSide == row) {
        for (int col_2 = col + 1; col_2 < kBoardSide; col_2++) {
          int tile_2 = board[row * kBoardSide + col_2];
          if (tile_2 != 0 && (tile_2 - 1) / kBoardSide == row &&
              tile_1 > tile_2) {
            result += 2;
          }
        }
      }
    }
  }
  return result;
}

int ColConflicts(const int* board) {
  int result = 0;
  for (int row = 0; row < kBoardSide; row++) {
    for (int col = 0; col < kBoardSide; col++) {
      int tile_1 = board[col * kBoardSide + row];
      if (tile_1 != 0 && (tile_1 - 1) % kBoardSide == row) {
        for (int row_2 = col + 1; row_2 < kBoardSide; row_2++) {
          int tile_2 = board[row_2 * kBoardSide + row];
          if (tile_2 != 0 && (tile_2 - 1) % kBoardSide == row &&
              tile_1 > tile_2) {
            result += 2;
          }
        }
      }
    }
  }
  return result;
}

int FindHeuristic(int* board) {
  return Manhattan(board) + RowConflicts(board) + ColConflicts(board);
}

bool DFS(int* board, int blank_pos, int moves_made, int heuristic,
         int prev_dir) {
  if (moves_made + heuristic > current_limit) {
    next_limit = std::min(next_limit, moves_made + heuristic);
    return false;
  }
  if (heuristic == 0) {
    return true;
  }

  int blank_row = blank_pos / kBoardSide;
  int blank_col = blank_pos % kBoardSide;

  for (int dir = 0; dir < kBoardSide; dir++) {
    if (dir == prev_dir) {
      continue;
    }

    int new_row = blank_row + delta_row[dir];
    int new_col = blank_col + delta_col[dir];

    if (new_row >= 0 && new_row < kBoardSide && new_col >= 0 &&
        new_col < kBoardSide) {
      int new_pos = new_row * kBoardSide + new_col;

      std::swap(board[blank_pos], board[new_pos]);
      int new_heuristic = FindHeuristic(board);
      solution_path += move_chars[dir];

      if (DFS(board, new_pos, moves_made + 1, new_heuristic,
              opposite_move[dir])) {
        return true;
      }

      solution_path.pop_back();
      std::swap(board[blank_pos], board[new_pos]);
    }
  }

  return false;
}

int main() {
  for (int tile = 1; tile < kBoardSize; tile++) {
    for (int pos = 0; pos < kBoardSize; pos++) {
      manhattan_distance[tile][pos] =
          std::abs(pos / kBoardSide - (tile - 1) / kBoardSide) +
          std::abs(pos % kBoardSide - (tile - 1) % kBoardSide);
    }
  }

  int board[kBoardSize];
  int blank_pos = 0;

  for (int pos = 0; pos < kBoardSize; pos++) {
    std::cin >> board[pos];
    if (board[pos] == 0) {
      blank_pos = pos;
    }
  }

  int inversions = 0;
  for (int i = 0; i < kBoardSize; i++) {
    if (board[i] != 0) {
      for (int j = i + 1; j < kBoardSize; j++) {
        if (board[j] != 0 && board[i] > board[j]) {
          inversions++;
        }
      }
    }
  }

  if ((inversions + blank_pos / kBoardSide) % 2 == 0) {
    std::cout << -1;
    return 0;
  }

  current_limit = FindHeuristic(board);

  while (current_limit <= kMaxMove) {
    next_limit = kInf;

    if (DFS(board, blank_pos, 0, current_limit, -1)) {
      std::cout << solution_path.size() << "\n" << solution_path;
      return 0;
    }

    current_limit = next_limit;
  }

  std::cout << -1;
}
