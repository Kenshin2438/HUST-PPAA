#include <format>
#include <fstream>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "miller_rabin.hpp"

auto main() -> int {
  std::vector<int> prime;
  std::mutex mu;

  auto task = [&mu, &prime](int L, int R) {
    for (int x = L; x < R; x++) {
      if (miller_rabin_test(x)) {
        std::lock_guard<std::mutex> lock(mu);
        prime.emplace_back(x);
      }
    }
  };

  constexpr int N = 100'000'000;
  const int P = std::thread::hardware_concurrency();
  const int slice = (N / P) + 1;

  std::vector<std::future<void>> events;
  events.reserve(P);
  for (int i = 0; i < P; i++) {
    const int L = i * slice + 1;
    const int R = (i == P - 1) ? N : L + slice;
    events.emplace_back(std::async(std::launch::async, task, L, R));
  }
  for (auto &&fut : events) fut.get();
  std::cout << std::format("Interval [{}, {}), prime number = {}\n", 1, N, prime.size());

  std::ofstream out("prime.txt");
  for (const int &x : prime) out << x << "\n";

  return 0;
}
