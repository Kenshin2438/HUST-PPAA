#include <cassert>
#include <chrono>
#include <climits>
#include <format>    // IWYU pragma: keep
#include <iostream>  // IWYU pragma: keep
#include <random>
#include <ranges>  // IWYU pragma: keep
#include <thread>
#include <vector>

#define DRAW

namespace internal {

using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

inline auto rand_int() noexcept -> int {
  thread_local std::mt19937 engine(std::random_device{}());
  thread_local std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
  return dist(engine);
};

template <class Q>
concept PushPopQueue = requires(Q q, Q::const_reference tc, Q::reference tr) {
  { q.push(tc) } -> std::same_as<void>;
  { q.pop(tr) } -> std::same_as<void>;
};

template <PushPopQueue QueueType>
constexpr auto measure(const int TH_READER, const int TH_WRITER) {
  QueueType Q;

  const int BASE = (1 << 25) / (TH_READER * TH_WRITER);

  const int DATA_FRAM_READER = BASE * TH_WRITER;
  std::atomic<long long> SUM_dequeue{};
  auto reader_task = [&SUM_dequeue, &Q, &DATA_FRAM_READER]() {
    long long SUM = 0;
    for (int i = 0; i < DATA_FRAM_READER; i++) {
      int val = 0;
      Q.pop(val);
      SUM += val;
    }
    SUM_dequeue += SUM;
  };

  std::atomic<long long> SUM_enqueue{};
  const int DATA_FRAM_WRITER = BASE * TH_READER;
  auto writer_task = [&SUM_enqueue, &Q, &DATA_FRAM_WRITER]() {
    long long SUM = 0;
    for (int i = 0; i < DATA_FRAM_WRITER; i++) {
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
      writers.emplace_back(writer_task);
    }
    std::vector<std::jthread> readers;
    readers.reserve(TH_READER);
    for (int i = 0; i < TH_READER; i++) {
      readers.emplace_back(reader_task);
    }
  }
  const auto end = high_resolution_clock::now();
  const auto T = duration_cast<milliseconds>(end - start);
  assert(SUM_dequeue == SUM_enqueue);
  assert(DATA_FRAM_READER * TH_READER == DATA_FRAM_WRITER * TH_WRITER);

  double ops = (DATA_FRAM_READER * TH_READER + DATA_FRAM_WRITER * TH_WRITER) /
               static_cast<double>(T.count()) / 1000;  // (M) Operations per second

#if defined(DRAW)
  return ops;
#else
  std::cout << std::format(
    "\e[36m{}\e[0m\n"
    "Running time: {}\n"
    "Operations per second: {:.4} M\n",
    __PRETTY_FUNCTION__, T, ops);
#endif
}

}  // namespace internal

#include "blocking_queue.hpp"
#include "nonblocking_queue.hpp"
#include "oneapi/tbb/concurrent_queue.h"

void data_generator();  // 绘图数据生成

auto main() -> int {
#if defined(DRAW)
  data_generator();
#else
  internal::measure<tbb::concurrent_bounded_queue<int>>(6, 6);
  internal::measure<PA2::BLOCKING_QUEUE<int>>(6, 6);
  internal::measure<PA2::NONBLOCKING_QUEUE<int>>(6, 6);
#endif
  return 0;
}

#if defined(DRAW)
void data_generator() {
  using namespace std::chrono_literals;

  const int Round = 3;
  // {  // 1:1
  //   std::vector Ops(3, std::vector<double>(8));
  //   for (int _ : std::views::iota(0, Round)) {
  //     for (int i = 1; i * 2 <= 16; i++) {
  //       Ops[0][i - 1] += internal::measure<tbb::concurrent_bounded_queue<int>>(i, i);
  //       std::this_thread::sleep_for(2s);
  //       Ops[1][i - 1] += internal::measure<PA2::BLOCKING_QUEUE<int>>(i, i);
  //       std::this_thread::sleep_for(2s);
  //       Ops[2][i - 1] += internal::measure<PA2::NONBLOCKING_QUEUE<int>>(i, i);
  //       std::this_thread::sleep_for(2s);
  //     }
  //   }
  //   for (auto &&res : Ops) {
  //     for (std::cout << "\n"; const double x : res) std::cout << x / Round << " ";
  //   }
  // }
  {  // 1:1 but more threads
    std::vector Ops(3, std::vector<double>(16));
    for (int _ : std::views::iota(0, Round)) {
      for (int i = 1; i * 2 <= 24; i++) {
        Ops[0][i - 1] += internal::measure<tbb::concurrent_bounded_queue<int>>(i, i);
        std::this_thread::sleep_for(2s);
        Ops[1][i - 1] += internal::measure<PA2::BLOCKING_QUEUE<int>>(i, i);
        std::this_thread::sleep_for(2s);
        Ops[2][i - 1] += internal::measure<PA2::NONBLOCKING_QUEUE<int>>(i, i);
        std::this_thread::sleep_for(2s);
      }
    }
    for (auto &&res : Ops) {
      for (std::cout << "\n"; const double x : res) std::cout << x / Round << " ";
    }
  }
  // {  // 1:2
  //   std::vector Ops(3, std::vector<double>(8));
  //   for (int _ : std::views::iota(0, Round)) {
  //     for (int i = 1; i * 3 <= 16; i++) {
  //       Ops[0][i - 1] += internal::measure<tbb::concurrent_bounded_queue<int>>(i, 2 * i);
  //       std::this_thread::sleep_for(2s);
  //       Ops[1][i - 1] += internal::measure<PA2::BLOCKING_QUEUE<int>>(i, 2 * i);
  //       std::this_thread::sleep_for(2s);
  //       Ops[2][i - 1] += internal::measure<PA2::NONBLOCKING_QUEUE<int>>(i, 2 * i);
  //       std::this_thread::sleep_for(2s);
  //     }
  //   }
  //   for (auto &&res : Ops) {
  //     for (std::cout << "\n"; const double x : res) std::cout << x / Round << " ";
  //   }
  // }
  // {  // 2:1
  //   std::vector Ops(3, std::vector<double>(8));
  //   for (int _ : std::views::iota(0, Round)) {
  //     for (int i = 1; i * 3 <= 16; i++) {
  //       Ops[0][i - 1] += internal::measure<tbb::concurrent_bounded_queue<int>>(2 * i, i);
  //       std::this_thread::sleep_for(2s);
  //       Ops[1][i - 1] += internal::measure<PA2::BLOCKING_QUEUE<int>>(2 * i, i);
  //       std::this_thread::sleep_for(2s);
  //       Ops[2][i - 1] += internal::measure<PA2::NONBLOCKING_QUEUE<int>>(2 * i, i);
  //       std::this_thread::sleep_for(2s);
  //     }
  //   }
  //   for (auto &&res : Ops) {
  //     for (std::cout << "\n"; const double x : res) std::cout << x / Round << " ";
  //   }
  // }
}
#endif