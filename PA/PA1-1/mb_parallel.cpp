#include <algorithm>
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <latch>
#include <mutex>
#include <numeric>
#include <ranges>
#include <syncstream>
#include <thread>
#include <vector>

#include "miller_rabin.hpp"

auto main() -> int {
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::milliseconds;

  constexpr int N = 100'000'000;
  const int P = std::thread::hardware_concurrency();
  // constexpr int P = 8;

  const int slice = N / P;
  const int extra = N % P;

  std::vector<int> prime;
  std::mutex mu;

  std::latch timer_sync_point(P);
  auto task = [&](int id, int L, int R) noexcept -> void {
    timer_sync_point.arrive_and_wait();
    const auto start = high_resolution_clock::now();
    for (int x = L; x < R; x++) {
      if (miller_rabin_test(x)) {
        std::lock_guard<std::mutex> lock(mu);
        prime.emplace_back(x);
      }
    }
    const auto end = high_resolution_clock::now();
    const auto T_ms = duration_cast<milliseconds>(end - start);
    std::osyncstream(std::cout) << std::format("Thread #{}: running time {}\n", id, T_ms);
  };

  std::vector<std::thread> events;
  events.reserve(P);
  for (int i = 0, L = 1, R; i < P; i++, L = R) {
    R = L + slice + int(i < extra);
    events.emplace_back(task, i, L, R);
  }
  timer_sync_point.wait();
  const auto start = high_resolution_clock::now();
  for (auto &&thread : events) thread.join();
  const auto end = high_resolution_clock::now();
  const auto T_ms = duration_cast<milliseconds>(end - start);

  std::cout.flush();

  std::ranges::sort(prime);
  std::cout << std::format("Running time = {}\nPrime number counter = {}\nPrime number sum = {}\n",
                           T_ms, prime.size(), std::accumulate(prime.begin(), prime.end(), 0LL));
  std::cout << "Top ten maximum primes:\n";
  for (const int &p : prime | std::views::reverse | std::views::take(10) | std::views::reverse) {
    std::cout << p << "\n";
  }

  std::ofstream out("prime.txt");
  out << std::format("{} {} {}\n", T_ms, prime.size(),
                     std::accumulate(prime.begin(), prime.end(), 0LL));
  for (const int &p : prime | std::views::reverse | std::views::take(10) | std::views::reverse) {
    out << p << "\n";
  }
  return 0;
}
