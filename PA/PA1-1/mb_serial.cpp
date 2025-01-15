#include <algorithm>
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <vector>

#include "miller_rabin.hpp"

auto main() -> int {
  using std::chrono::duration_cast;
  using std::chrono::high_resolution_clock;
  using std::chrono::milliseconds;

  constexpr int N = 100'000'000;

  const auto start = high_resolution_clock::now();
  std::vector<int> prime;
  for (int n = 1; n < N; n++) {
    if (miller_rabin_test(n)) prime.emplace_back(n);
  }
  const auto end = high_resolution_clock::now();
  const auto T_ms = duration_cast<milliseconds>(end - start);

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
