#include <cassert>
#include <chrono>
#include <climits>
#include <format>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

namespace internal {
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

inline auto rand_int() noexcept -> int {
  thread_local std::mt19937 engine(std::random_device{}());
  std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
  return dist(engine);
};

constexpr int TH_WRITER = 4;
constexpr int TH_READER = 4;
constexpr int DATA_FRAM = (1 << 20);

template <class Q>
concept PushPopQueue = requires(Q q, Q::const_reference tc, Q::reference tr) {
  { q.push(tc) } -> std::same_as<void>;
  { q.pop(tr) } -> std::same_as<void>;
};

template <PushPopQueue QueueType>
constexpr auto measure() -> void {
  QueueType Q;

  std::atomic<long long> SUM_dequeue{};
  auto reader_task = [&SUM_dequeue, &Q]([[maybe_unused]] int tid) {
    long long SUM = 0;
    for (int i = 0; i < DATA_FRAM; i++) {
      int val = 0;
      Q.pop(val);
      SUM += val;
    }
    SUM_dequeue += SUM;
  };

  std::atomic<long long> SUM_enqueue{};
  auto writer_task = [&SUM_enqueue, &Q]([[maybe_unused]] int tid) {
    long long SUM = 0;
    for (int i = 0; i < DATA_FRAM; i++) {
      int val = rand_int();
      Q.push(val);
      SUM += val;
    }
    SUM_enqueue += SUM;
  };

  const auto start = high_resolution_clock::now();
  {
    std::vector<std::jthread> writers;
    writers.reserve(TH_WRITER);
    for (int i = 0; i < TH_WRITER; i++) {
      writers.emplace_back(writer_task, i);
    }
    std::vector<std::jthread> readers;
    readers.reserve(TH_READER);
    for (int i = 0; i < TH_READER; i++) {
      readers.emplace_back(reader_task, i);
    }
  }
  const auto end = high_resolution_clock::now();
  const auto T = duration_cast<milliseconds>(end - start);
  assert(SUM_dequeue == SUM_enqueue);

  std::cout << std::format(
    "\e[36m{}\e[0m\n"
    "Running time: {}\n",
    __PRETTY_FUNCTION__, T);
}

}  // namespace internal

#include "blocking_queue.hpp"
#include "nonblocking_queue.hpp"
#include "oneapi/tbb/concurrent_queue.h"

auto main() -> int {
  internal::measure<tbb::concurrent_bounded_queue<int>>();
  internal::measure<PA2::BLOCKING_QUEUE<int>>();
  internal::measure<PA2::NONBLOCKING_QUEUE<int>>();
  return 0;
}