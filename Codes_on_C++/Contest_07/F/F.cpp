#include <algorithm>
#include <array>
#include <iostream>
#include <vector>

const int64_t kInfDist = 21;

struct Data {
  Data(int64_t elements_count, int64_t worker_count)
      : elements_count(elements_count),
        worker_count(worker_count),
        worker_masks(worker_count),
        is_worker(1 << elements_count, false) {}

  int64_t elements_count;
  int64_t worker_count;
  std::vector<int64_t> worker_masks;
  std::vector<bool> is_worker;
};

Data Input() {
  int64_t elements_count;
  int64_t worker_count;
  std::cin >> elements_count >> worker_count;

  Data data(elements_count, worker_count);

  for (int64_t worker_idx = 0; worker_idx < worker_count; ++worker_idx) {
    std::string views;
    std::cin >> views;
    int64_t mask = 0;
    for (int64_t topic = 0; topic < elements_count; ++topic) {
      if (views[topic] == 'R') {
        mask |= (1 << topic);
      }
    }
    data.worker_masks[worker_idx] = mask;
    data.is_worker[mask] = true;
  }

  return data;
}

int64_t DichPolnaya(const Data& data) {
  const int64_t kHypercubeSize = 1 << data.elements_count;

  std::vector<int64_t> dist(kHypercubeSize, kInfDist);
  std::vector<bool> in_mst(kHypercubeSize, false);

  std::vector<std::vector<int64_t>> buckets(data.elements_count + 1);

  const int64_t kStartMask = data.worker_masks[0];
  dist[kStartMask] = 0;
  buckets[0].push_back(kStartMask);

  int64_t total_cost = 0;
  int64_t mst_size = 0;
  int64_t cur_dist = 0;

  while (mst_size < data.worker_count) {
    while (cur_dist <= data.elements_count && buckets[cur_dist].empty()) {
      ++cur_dist;
    }
    if (cur_dist > data.elements_count) {
      break;
    }

    const int64_t kMask = buckets[cur_dist].back();
    buckets[cur_dist].pop_back();

    if (dist[kMask] != cur_dist) {
      continue;
    }

    if (data.is_worker[kMask] && !in_mst[kMask]) {
      in_mst[kMask] = true;
      total_cost += cur_dist;
      ++mst_size;
      if (mst_size == data.worker_count) {
        break;
      }
      dist[kMask] = 0;
      buckets[0].push_back(kMask);
      cur_dist = 0;
      continue;
    }

    for (int64_t bit = 0; bit < data.elements_count; ++bit) {
      const int64_t kNeighbor = kMask ^ (1 << bit);
      const int64_t kNeighborDist = cur_dist + 1;
      if (kNeighborDist < dist[kNeighbor]) {
        dist[kNeighbor] = kNeighborDist;
        if (kNeighborDist <= data.elements_count) {
          buckets[kNeighborDist].push_back(kNeighbor);
        }
      }
    }
  }

  return total_cost;
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  const Data kData = Input();
  std::cout << DichPolnaya(kData) << "\n";
}