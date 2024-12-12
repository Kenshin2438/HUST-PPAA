#include <algorithm>
#include <chrono>
#include <format>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <numeric>
#include <ranges>
#include <syncstream>
#include <vector>

#include "miller_rabin.hpp"

auto main() -> int {
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::milliseconds;

  std::vector<int> prime;
  std::mutex mu;

  auto task = [&mu, &prime](int id, int L, int R) {
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

  constexpr int N = 100'000'000;
  const int P = 8;  // std::thread::hardware_concurrency() = 16
  const int slice = (N / P) + 1;

  const auto start = high_resolution_clock::now();
  std::vector<std::future<void>> events;
  events.reserve(P);
  for (int i = 0; i < P; i++) {
    const int L = i * slice + 1;
    const int R = (i == P - 1) ? N : L + slice;
    events.emplace_back(std::async(std::launch::async, task, i, L, R));
  }
  for (auto &&fut : events) fut.get();
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
