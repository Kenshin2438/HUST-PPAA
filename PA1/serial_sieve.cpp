#include <cmath>
#include <format>
#include <fstream>
#include <iostream>
#include <utility>
#include <vector>

#if defined(EULER_SIEVE)
  #include <sys/resource.h>

  #include <bitset>
#endif

auto main(int argc, char **argv) -> int {

#if defined(EULER_SIEVE)  // set stack-size = 256MB
  rlimit rlim;
  if (getrlimit(RLIMIT_STACK, &rlim)) return 1;
  rlim.rlim_cur = 256 * 1024 * 1024;
  if (setrlimit(RLIMIT_STACK, &rlim)) return 2;
#endif

  constexpr int N = 100'000'000;

  std::vector<int> prime;
  prime.reserve(int(N / log(N) * 1.1));

#if defined(EULER_SIEVE)
  std::bitset<N> flag;
  for (int i = 2; i < N; i++) {
    if (flag.test(i) == false) prime.emplace_back(i);
    for (const int &p : prime) {
      if (p * 1LL * i >= N) break;
      flag.set(p * 1LL * i, true);
      if (i % p == 0) break;
    }
  }
#else  // default algorithm: Fast-Eratosthenes-Sieve
  const int S = (int)round(sqrt(N)), R = N / 2;
  std::vector<int> sieve(S + 1);
  std::vector<std::pair<int, int> > cp;

  prime.emplace_back(2);
  for (int i = 3; i <= S; i += 2)
    if (!sieve[i]) {
      cp.push_back({i, i * i / 2});
      for (int j = i * i; j <= S; j += 2 * i) sieve[j] = 1;
    }
  for (int L = 1; L <= R; L += S) {
    std::vector<int> block(S);
    for (auto &[p, idx] : cp)
      for (int i = idx; i < S + L; idx = (i += p)) block[i - L] = 1;
    for (int i = 0; i < std::min(S, R - L); i++)
      if (!block[i]) prime.push_back((L + i) * 2 + 1);
  }
#endif

  std::cout << std::format("Interval [{}, {}), prime number = {}\n", 1, N, prime.size());

  std::ofstream out("prime_serial_sieve.txt");
  for (const int &x : prime) out << x << "\n";

  return 0;
}