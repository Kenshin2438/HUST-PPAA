#include <oneapi/tbb/concurrent_queue.h>

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <random>
#include <thread>

auto main() -> int {
  std::mt19937 rng(std::random_device{}());

  tbb::concurrent_queue<uint64_t> Q;

  return 0;
}