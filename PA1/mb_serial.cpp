#include <format>
#include <fstream>
#include <iostream>
#include <vector>

#include "miller_rabin.hpp"

auto main() -> int {
  constexpr int N = 100'000'000;

  std::vector<int> prime;
  for (int n = 1; n < N; n++) {
    if (miller_rabin_test(n)) prime.emplace_back(n);
  }

  std::cout << std::format("Interval [{}, {}), prime number = {}\n", 1, N, prime.size());

  std::ofstream out("prime_serial_mb.txt");
  for (const int &x : prime) out << x << "\n";

  return 0;
}
