#include <cassert>
#include <chrono>
#include <climits>
#include <format>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

namespace {
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

constexpr int TH_WRITER = 8;
constexpr int TH_READER = 8;
constexpr int DATA_FRAM = (1 << 22) / TH_READER;

inline int randInt() noexcept {
  thread_local std::mt19937 engine(std::random_device{}());
  std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
  return dist(engine);
};
}  // namespace

#include "oneapi/tbb/concurrent_queue.h"

static void test_tbb_concurrent_queue() {
  std::atomic<long long> SUM_enqueue{};
  std::atomic<long long> SUM_dequeue{};

  tbb::concurrent_queue<int> Q;
  auto reader_task = [&]([[maybe_unused]] int tid) {
    for (int i = 0; i < DATA_FRAM; i++) {
      int val = 0;
      while (Q.try_pop(val) == false);
      SUM_dequeue.fetch_add(val);
    }
  };
  auto writer_task = [&]([[maybe_unused]] int tid) {
    for (int i = 0; i < DATA_FRAM; i++) {
      int val = randInt();
      Q.push(val);
      SUM_enqueue.fetch_add(val);
    }
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
  const auto T_ms = duration_cast<milliseconds>(end - start);

  assert(SUM_dequeue.load() == SUM_enqueue.load());

  std::cerr << std::format("\nTBB ConcurrentQueue Running time: {}\n", T_ms);
}

#include "blocking_queue.hpp"

static void test_PA2_blocking_queue() {
  std::atomic<long long> SUM_enqueue{};
  std::atomic<long long> SUM_dequeue{};

  PA2::BLOCKING_QUEUE<int> Q;
  auto reader_task = [&]([[maybe_unused]] int tid) {
    for (int i = 0; i < DATA_FRAM; i++) {
      int val = 0;
      Q.remove(std::ref(val));
      SUM_dequeue.fetch_add(val);
    }
  };
  auto writer_task = [&]([[maybe_unused]] int tid) {
    for (int i = 0; i < DATA_FRAM; i++) {
      int val = randInt();
      Q.add(val);
      SUM_enqueue.fetch_add(val);
    }
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
  const auto T_ms = duration_cast<milliseconds>(end - start);

  assert(SUM_dequeue.load() == SUM_enqueue.load());

  std::cerr << std::format("\nPA2 BLOCKING_QUEUE Running time: {}\n", T_ms);
}

#include "nonblocking_queue.hpp"

static void test_PA2_nonblocking_queue() {
  std::atomic<long long> SUM_enqueue{};
  std::atomic<long long> SUM_dequeue{};

  PA2::NONBLOCKING_QUEUE<int> Q;
  auto reader_task = [&]([[maybe_unused]] int tid) {
    for (int i = 0; i < DATA_FRAM; i++) {
      int val = 0;
      Q.remove(std::ref(val));
      SUM_dequeue.fetch_add(val);
    }
  };
  auto writer_task = [&]([[maybe_unused]] int tid) {
    for (int i = 0; i < DATA_FRAM; i++) {
      int val = randInt();
      Q.add(val);
      SUM_enqueue.fetch_add(val);
    }
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
  const auto T_ms = duration_cast<milliseconds>(end - start);

  assert(SUM_dequeue.load() == SUM_enqueue.load());

  std::cerr << std::format("\nPA2 NONBLOCKING_QUEUE Running time: {}\n", T_ms);
}

auto main() -> int {
  test_tbb_concurrent_queue();
  test_PA2_blocking_queue();
  test_PA2_nonblocking_queue();
  return 0;
}