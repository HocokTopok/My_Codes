#include <algorithm>
#include <iostream>
#include <queue>
#include <span>
#include <unordered_map>
#include <vector>

using Pair = std::pair<int64_t, int64_t>;
using MinHeap =
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>>;

const int64_t kMax = 200'000'000'000'001;

class WeightedEdge {
 public:
  WeightedEdge(int64_t vertex_to, int64_t weight)
      : vertex_to_(vertex_to), weight_(weight) {}

  int64_t GetTo() const noexcept { return vertex_to_; }

  int64_t GetWeight() const noexcept { return weight_; }

 private:
  int64_t vertex_to_;
  int64_t weight_;
};

template <typename T, typename U>
class Graph {
 public:
  using VertexT = T;
  using EdgeT = U;

  Graph() = default;

  Graph(size_t vertex_count) : vertex_count_(vertex_count) {}

  void AddEdge(const VertexT& vertex_from, const EdgeT& new_edge) noexcept {
    adjacency_list_[vertex_from].push_back(new_edge);
    ++edge_count_;
  }

  std::span<const EdgeT> GetVertexNeighbours(
      const VertexT& vertex) const noexcept {
    auto iterator = adjacency_list_.find(vertex);
    if (iterator != adjacency_list_.end()) {
      return iterator->second;
    }
    return {};
  }

  std::vector<VertexT> GetAllVerteces() const noexcept {
    std::vector<VertexT> vertices;
    for (auto [vertex, _] : adjacency_list_) {
      vertices.push_back(vertex);
    }
    return vertices;
  }

  size_t GetNumOfVerteces() const noexcept { return vertex_count_; }

  size_t GetNumOfEdges() const noexcept { return edge_count_; }

 private:
  size_t vertex_count_ = 0;
  size_t edge_count_ = 0;
  std::unordered_map<VertexT, std::vector<EdgeT>> adjacency_list_;
};

template <typename T, typename U>
Graph<T, U> BuildGraph(size_t vertex_count,
                       const std::vector<std::pair<T, U>>& edges) {
  Graph<T, U> graph(vertex_count);

  for (auto [vertex_from, edge] : edges) {
    graph.AddEdge(vertex_from, edge);
  }

  return graph;
}

template <typename T, typename U>
class AbstractDijkstraVisitor {
 public:
  virtual void InitVisitor(int64_t start_vertex) = 0;
  virtual void InitMinHeap(MinHeap& min_heap, int64_t start_vertex) = 0;

  int64_t GetDist(const T& vertex) const { return dist_[vertex]; }

  void SetDist(const T& vertex, int64_t new_dist) { dist_[vertex] = new_dist; }
  void DiscoverVertex(const T& vertex) { used_[vertex] = true; }
  virtual void FinishVertex(const T&) {}

  virtual bool IsVisited(const T& vertex) const { return used_[vertex]; }
  virtual ~AbstractDijkstraVisitor() = default;

 protected:
  AbstractDijkstraVisitor(std::vector<int64_t>& dist, std::vector<bool>& used)
      : dist_(dist), used_(used) {}

  std::vector<int64_t>& dist_;
  std::vector<bool>& used_;
};

template <typename T, typename U>
class VirusVisitor : public AbstractDijkstraVisitor<T, U> {
 public:
  VirusVisitor(std::vector<int64_t>& dist, std::vector<bool>& used,
               const std::vector<int64_t>& virus_vertices)
      : AbstractDijkstraVisitor<T, U>(dist, used),
        start_vertices_(virus_vertices) {}

  void InitVisitor(int64_t) override {
    for (int64_t start_vertex : start_vertices_) {
      this->dist_[start_vertex] = 0;
    }
  }

  void InitMinHeap(MinHeap& min_heap, int64_t) override {
    for (int64_t start_vertex : start_vertices_) {
      min_heap.push({0, start_vertex});
    }
  }

 protected:
  const std::vector<int64_t>& start_vertices_;
};

template <typename T, typename U>
class AmogusVisitor : public AbstractDijkstraVisitor<T, U> {
 public:
  AmogusVisitor(std::vector<int64_t>& dist, std::vector<bool>& used,
                const std::vector<int64_t>& virus_dist, int64_t end_vertex,
                int64_t& result)
      : AbstractDijkstraVisitor<T, U>(dist, used),
        virus_dist_(virus_dist),
        end_vertex_(end_vertex),
        result_(result) {}

  void InitVisitor(int64_t start_vertex) override {
    this->dist_[start_vertex] = 0;
  }

  void InitMinHeap(MinHeap& min_heap, int64_t start_vertex) override {
    min_heap.push({0, start_vertex});
  }

  bool IsVisited(const T& vertex) const override {
    return this->used_[vertex] || virus_dist_[vertex] <= this->dist_[vertex];
  }

  void FinishVertex(const T& vertex) override {
    if (vertex == end_vertex_) {
      result_ = this->dist_[end_vertex_];
    }
  }

 protected:
  const std::vector<int64_t>& virus_dist_;
  int64_t end_vertex_;
  int64_t& result_;
};

template <typename T, typename U, typename Visitor>
void Dijkstra(const Graph<T, U>& graph, const T& start_vertex,
              Visitor visitor) {
  MinHeap min_heap;

  visitor.InitVisitor(start_vertex);
  visitor.InitMinHeap(min_heap, start_vertex);

  while (!min_heap.empty()) {
    auto [min_dist, min_vertex] = min_heap.top();
    min_heap.pop();

    if (visitor.IsVisited(min_vertex)) {
      continue;
    }

    visitor.DiscoverVertex(min_vertex);

    for (const WeightedEdge& edge : graph.GetVertexNeighbours(min_vertex)) {
      T new_vertex = edge.GetTo();
      int64_t new_dist = min_dist + edge.GetWeight();

      if (new_dist < visitor.GetDist(new_vertex)) {
        visitor.SetDist(new_vertex, new_dist);
        min_heap.push({new_dist, new_vertex});
      }
    }

    visitor.FinishVertex(min_vertex);
  }
}

struct InputData {
  InputData(size_t vertex_count, int64_t start_vertex, int64_t end_vertex,
            const std::vector<int64_t>& virus_vertices,
            const std::vector<std::pair<int64_t, WeightedEdge>>& edges)
      : vertex_count(vertex_count),
        start_vertex(start_vertex),
        end_vertex(end_vertex),
        virus_vertices(virus_vertices),
        edges(edges) {}

  size_t vertex_count;
  int64_t start_vertex;
  int64_t end_vertex;
  std::vector<int64_t> virus_vertices;
  std::vector<std::pair<int64_t, WeightedEdge>> edges;
};

InputData Input() {
  size_t vertex_count;
  size_t edges_count;
  size_t virus_count;
  std::cin >> vertex_count >> edges_count >> virus_count;

  std::vector<int64_t> virus_vertices(virus_count);

  for (size_t i = 0; i < virus_count; ++i) {
    int64_t virus_vertex;
    std::cin >> virus_vertex;
    --virus_vertex;
    virus_vertices[i] = virus_vertex;
  }

  std::vector<std::pair<int64_t, WeightedEdge>> edges;

  for (; edges_count > 0; --edges_count) {
    int64_t u_num;
    int64_t v_num;
    int64_t w_num;
    std::cin >> u_num >> v_num >> w_num;
    --u_num;
    --v_num;
    edges.push_back({u_num, WeightedEdge(v_num, w_num)});
    edges.push_back({v_num, WeightedEdge(u_num, w_num)});
  }

  int64_t start_vertex;
  int64_t end_vertex;
  std::cin >> start_vertex >> end_vertex;
  --start_vertex;
  --end_vertex;

  return InputData(vertex_count, start_vertex, end_vertex, virus_vertices,
                   edges);
}

void Output(int64_t result) { std::cout << result; }

int main() {
  InputData info = Input();
  auto graph = BuildGraph<int64_t, WeightedEdge>(info.vertex_count, info.edges);

  std::vector<bool> used_virus(info.vertex_count, false);
  std::vector<int64_t> dist_virus(info.vertex_count, kMax);
  VirusVisitor<int64_t, WeightedEdge> virus_visitor(dist_virus, used_virus,
                                                    info.virus_vertices);
  Dijkstra(graph, info.start_vertex, virus_visitor);

  std::vector<bool> used_amogus(info.vertex_count, false);
  std::vector<int64_t> dist_amogus(info.vertex_count, kMax);
  int64_t result = -1;
  AmogusVisitor<int64_t, WeightedEdge> amogus_visitor(
      dist_amogus, used_amogus, dist_virus, info.end_vertex, result);
  Dijkstra(graph, info.start_vertex, amogus_visitor);

  Output(result);
}
