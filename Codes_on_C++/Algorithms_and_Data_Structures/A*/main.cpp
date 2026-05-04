#include <algorithm>
#include <functional>
#include <iostream>
#include <numeric>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Constants {
const size_t kBoardSize = 16;
const size_t kBoardSide = 4;
const size_t kBitMask = 0b1111;
const int64_t kMaxDist = 200'000'000;
}  // namespace Constants

struct Data {
  Data()
      : manhattan_distance(Constants::kBoardSize,
                           std::vector<int>(Constants::kBoardSize, 0)) {}

  std::vector<std::vector<int>> manhattan_distance;

  char move_chars[4] = {'L', 'R', 'U', 'D'};
  int delta_row[4] = {0, 0, 1, -1};
  int delta_col[4] = {1, -1, 0, 0};
};

size_t PackBoard(const int* board) {
  size_t result = 0;
  for (size_t i = 0; i < Constants::kBoardSize; ++i) {
    result = (result << 4) | board[i];
  }
  return result;
}

void UnpackBoard(size_t packed, int* board) {
  for (int i = Constants::kBoardSize - 1; i >= 0; --i) {
    board[i] = packed & Constants::kBitMask;
    packed >>= 4;
  }
}

class PuzzleEdge {
 public:
  PuzzleEdge(size_t vertex_to, int new_blank_pos, char move_char)
      : vertex_to_(vertex_to),
        new_blank_pos_(new_blank_pos),
        move_char_(move_char) {}

  size_t GetTo() const noexcept { return vertex_to_; }
  static int GetWeight() noexcept { return 1; }
  int GetNewBlankPos() const noexcept { return new_blank_pos_; }
  char GetMoveChar() const noexcept { return move_char_; }

 private:
  size_t vertex_to_;
  int new_blank_pos_;
  char move_char_;
};

template <typename T, typename U>
class Graph {
 public:
  using VertexT = T;
  using EdgeT = U;

  virtual std::vector<U> GetVertexNeighbours(const T& vertex) const = 0;
  virtual size_t GetNumOfVertices() const = 0;
  virtual size_t GetNumOfEdges() const = 0;
  virtual ~Graph() = default;
};

class PuzzleGraph : public Graph<size_t, PuzzleEdge> {
 public:
  PuzzleGraph(const Data& data) : data_(data) {}

  std::vector<PuzzleEdge> GetVertexNeighbours(
      const size_t& packed) const override {
    std::vector<PuzzleEdge> neighbours;

    int board[Constants::kBoardSize];
    UnpackBoard(packed, board);

    int blank_pos = 0;
    for (int i = 0; i < (int)Constants::kBoardSize; ++i) {
      if (board[i] == 0) {
        blank_pos = i;
        break;
      }
    }

    int blank_row = blank_pos / Constants::kBoardSide;
    int blank_col = blank_pos % Constants::kBoardSide;

    for (int dir = 0; dir < (int)Constants::kBoardSide; ++dir) {
      int new_row = blank_row + data_.delta_row[dir];
      int new_col = blank_col + data_.delta_col[dir];

      if (new_row >= 0 && new_row < (int)Constants::kBoardSide &&
          new_col >= 0 && new_col < (int)Constants::kBoardSide) {
        int new_pos = (new_row * Constants::kBoardSide) + new_col;
        std::swap(board[blank_pos], board[new_pos]);
        neighbours.emplace_back(PackBoard(board), new_pos,
                                data_.move_chars[dir]);
        std::swap(board[blank_pos], board[new_pos]);
      }
    }

    return neighbours;
  }

  size_t GetNumOfVertices() const override { return 0; }
  size_t GetNumOfEdges() const override { return 0; }

 private:
  const Data& data_;
};

void BuildManhattanDistance(Data& data) {
  for (size_t tile = 1; tile < Constants::kBoardSize; ++tile) {
    for (size_t pos = 0; pos < Constants::kBoardSize; ++pos) {
      data.manhattan_distance[tile][pos] =
          std::abs((int)(pos / Constants::kBoardSide) -
                   (int)((tile - 1) / Constants::kBoardSide)) +
          std::abs((int)(pos % Constants::kBoardSide) -
                   (int)((tile - 1) % Constants::kBoardSide));
    }
  }
}

class ManhattanHeuristic {
 public:
  explicit ManhattanHeuristic(const Data& data) : data_(data) {}

  int operator()(size_t packed) const {
    int board[Constants::kBoardSize];
    UnpackBoard(packed, board);
    int result = 0;
    for (int pos = 0; pos < (int)Constants::kBoardSize; ++pos) {
      if (board[pos] != 0) {
        result += data_.manhattan_distance[board[pos]][pos];
      }
    }
    return result;
  }

 private:
  const Data& data_;
};

class RowConflictsHeuristic {
 public:
  int operator()(size_t packed) const {
    int board[Constants::kBoardSize];
    UnpackBoard(packed, board);
    int result = 0;
    for (int row = 0; row < (int)Constants::kBoardSide; ++row) {
      for (int col = 0; col < (int)Constants::kBoardSide; ++col) {
        int tile_1 = board[(row * Constants::kBoardSide) + col];
        if (tile_1 != 0 && (tile_1 - 1) / (int)Constants::kBoardSide == row) {
          for (int col_2 = col + 1; col_2 < (int)Constants::kBoardSide;
               ++col_2) {
            int tile_2 = board[(row * Constants::kBoardSide) + col_2];
            if (tile_2 != 0 &&
                (tile_2 - 1) / (int)Constants::kBoardSide == row &&
                tile_1 > tile_2) {
              result += 2;
            }
          }
        }
      }
    }
    return result;
  }
};

class ColConflictsHeuristic {
 public:
  int operator()(size_t packed) const {
    int board[Constants::kBoardSize];
    UnpackBoard(packed, board);
    int result = 0;
    for (int target_col = 0; target_col < (int)Constants::kBoardSide;
         ++target_col) {
      for (int row_1 = 0; row_1 < (int)Constants::kBoardSide; ++row_1) {
        int tile_1 = board[(row_1 * Constants::kBoardSide) + target_col];
        if (tile_1 != 0 &&
            (tile_1 - 1) % (int)Constants::kBoardSide == target_col) {
          for (int row_2 = row_1 + 1; row_2 < (int)Constants::kBoardSide;
               ++row_2) {
            int tile_2 = board[(row_2 * Constants::kBoardSide) + target_col];
            if (tile_2 != 0 &&
                (tile_2 - 1) % (int)Constants::kBoardSide == target_col &&
                tile_1 > tile_2) {
              result += 2;
            }
          }
        }
      }
    }
    return result;
  }
};

class CompositeHeuristic {
 public:
  using HeuristicFunc = std::function<int(size_t)>;

  CompositeHeuristic(std::vector<HeuristicFunc> heuristics,
                     std::vector<double> weights)
      : heuristics_(std::move(heuristics)), weights_(std::move(weights)) {}

  int operator()(size_t packed) const {
    std::vector<double> values;
    values.reserve(heuristics_.size());
    for (const auto& heuristic : heuristics_) {
      values.push_back(static_cast<double>(heuristic(packed)));
    }
    return static_cast<int>(std::inner_product(values.begin(), values.end(),
                                               weights_.begin(), 0.0));
  }

 private:
  std::vector<HeuristicFunc> heuristics_;
  std::vector<double> weights_;
};

using Pair = std::pair<int64_t, size_t>;
using MinHeap =
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>>;

template <typename T, typename U>
class AbstractAStarVisitor {
 public:
  virtual void InitVisitor(const T& start_vertex) = 0;
  virtual void InitMinHeap(MinHeap& min_heap, const T& start_vertex) = 0;

  int64_t GetDist(const T& vertex) const {
    auto it = dist_.find(vertex);
    return it != dist_.end() ? it->second : Constants::kMaxDist;
  }

  void SetDist(const T& vertex, int64_t new_dist) { dist_[vertex] = new_dist; }
  void DiscoverVertex(const T& vertex) { used_.insert(vertex); }
  virtual void RelaxEdge([[maybe_unused]] const T& from,
                         [[maybe_unused]] const T& to,
                         [[maybe_unused]] const U& edge) {}
  virtual bool ShouldStop([[maybe_unused]] const T& vertex) const {
    return false;
  }
  virtual bool IsVisited(const T& vertex) const {
    return used_.count(vertex) > 0;
  }
  virtual ~AbstractAStarVisitor() = default;

 protected:
  AbstractAStarVisitor(std::unordered_map<T, int64_t>& dist,
                       std::unordered_set<T>& used)
      : dist_(dist), used_(used) {}

  std::unordered_map<T, int64_t>& dist_;
  std::unordered_set<T>& used_;
};

template <typename T, typename U>
class PuzzleAStarVisitor : public AbstractAStarVisitor<T, U> {
 public:
  PuzzleAStarVisitor(std::unordered_map<T, int64_t>& dist,
                     std::unordered_set<T>& used,
                     std::unordered_map<T, T>& parent_packed,
                     std::unordered_map<T, char>& move_made, T goal_packed)
      : AbstractAStarVisitor<T, U>(dist, used),
        parent_packed_(parent_packed),
        move_made_(move_made),
        goal_packed_(goal_packed) {}

  void InitVisitor(const T& start_vertex) override {
    this->dist_[start_vertex] = 0;
  }

  void InitMinHeap(MinHeap& min_heap, const T& start_vertex) override {
    min_heap.push({0, start_vertex});
  }

  void RelaxEdge(const T& from, const T& to, const U& edge) override {
    parent_packed_[to] = from;
    move_made_[to] = edge.GetMoveChar();
  }

  bool ShouldStop(const T& vertex) const override {
    return vertex == goal_packed_;
  }

 private:
  std::unordered_map<T, T>& parent_packed_;
  std::unordered_map<T, char>& move_made_;
  T goal_packed_;
};

template <typename T, typename U, typename Visitor, typename Heuristic>
void AStar(const Graph<T, U>& graph, const T& start_vertex, Visitor visitor,
           Heuristic heuristic) {
  MinHeap min_heap;

  visitor.InitVisitor(start_vertex);
  visitor.InitMinHeap(min_heap, start_vertex);

  while (!min_heap.empty()) {
    auto [min_f, min_vertex] = min_heap.top();
    min_heap.pop();

    if (visitor.IsVisited(min_vertex)) {
      continue;
    }
    visitor.DiscoverVertex(min_vertex);

    if (visitor.ShouldStop(min_vertex)) {
      break;
    }

    for (const U& edge : graph.GetVertexNeighbours(min_vertex)) {
      T new_vertex = edge.GetTo();
      int64_t new_g = visitor.GetDist(min_vertex) + edge.GetWeight();

      if (new_g < visitor.GetDist(new_vertex)) {
        visitor.SetDist(new_vertex, new_g);
        visitor.RelaxEdge(min_vertex, new_vertex, edge);
        int64_t new_f = new_g + heuristic(new_vertex);
        min_heap.push({new_f, new_vertex});
      }
    }
  }
}

bool IsSolvable(const int* board, int blank_pos) {
  int inversions = 0;
  for (int i = 0; i < (int)Constants::kBoardSize; ++i) {
    if (board[i] != 0) {
      for (int j = i + 1; j < (int)Constants::kBoardSize; ++j) {
        if (board[j] != 0 && board[i] > board[j]) {
          ++inversions;
        }
      }
    }
  }
  return (inversions + (blank_pos / (int)Constants::kBoardSide)) % 2 != 0;
}

void BuildGoalBoard(int* board) {
  for (int i = 0; i < (int)Constants::kBoardSize - 1; ++i) {
    board[i] = i + 1;
  }
  board[Constants::kBoardSize - 1] = 0;
}

std::string ReconstructPath(std::unordered_map<size_t, size_t>& parent_packed,
                            std::unordered_map<size_t, char>& move_made,
                            size_t goal_packed, size_t initial_packed) {
  std::string solution_path;
  size_t curr_packed = goal_packed;
  while (curr_packed != initial_packed) {
    if (!parent_packed.contains(curr_packed)) {
      break;
    }
    solution_path += move_made[curr_packed];
    curr_packed = parent_packed[curr_packed];
  }
  std::reverse(solution_path.begin(), solution_path.end());
  return solution_path;
}

void Input(int* board, int& blank_pos) {
  for (int pos = 0; pos < (int)Constants::kBoardSize; ++pos) {
    std::cin >> board[pos];
    if (board[pos] == 0) {
      blank_pos = pos;
    }
  }
}

void Output(const std::string& solution_path) {
  std::cout << solution_path.size() << "\n" << solution_path;
}

int main() {
  Data data;

  BuildManhattanDistance(data);

  int board[Constants::kBoardSize];
  int blank_pos = 0;
  Input(board, blank_pos);

  if (!IsSolvable(board, blank_pos)) {
    std::cout << -1;
    return 0;
  }

  int goal_board[Constants::kBoardSize];
  BuildGoalBoard(goal_board);
  size_t goal_packed = PackBoard(goal_board);
  size_t initial_packed = PackBoard(board);

  std::unordered_map<size_t, int64_t> dist;
  std::unordered_set<size_t> used;
  std::unordered_map<size_t, size_t> parent_packed;
  std::unordered_map<size_t, char> move_made;

  PuzzleAStarVisitor<size_t, PuzzleEdge> visitor(dist, used, parent_packed,
                                                 move_made, goal_packed);

  PuzzleGraph graph(data);

  ManhattanHeuristic manhattan_heuristic(data);

  CompositeHeuristic heuristic(
      {manhattan_heuristic, RowConflictsHeuristic(), ColConflictsHeuristic()},
      {1.0, 1.0, 1.0});

  AStar(graph, initial_packed, visitor, heuristic);

  if (!used.contains(goal_packed)) {
    std::cout << -1;
    return 0;
  }

  Output(
      ReconstructPath(parent_packed, move_made, goal_packed, initial_packed));
}
