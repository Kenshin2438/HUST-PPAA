#pragma once

#include <bit>
#include <cassert>
#include <cstdint>
#include <initializer_list>

using u64 = uint64_t;
using i64 = int64_t;

/** Miller–Rabin primality test: A probabilistic primality test
REMARKS: https://miller-rabin.appspot.com/
  Let a be primality witness. Let n be the number we test for primality.
  1. Depending on your Miller-Rabin implementation, you may need to take a := a mod n;
  2. When the witness a equals 0, the test should return that n is prime;
  3. It is crucial to test all the bases and not just the bases less than n.
SPRP: Deterministic Variant of Miller-Rabin primality test
  Gerhard Jaeschke 1993 bases: [2, 7, 61] -> best solution: n <= 4'759'123'141
*/

inline constexpr auto miller_rabin_test(const i64 n) noexcept -> bool {
  assert(1 <= n && n <= 100'000'000LL);
  if (n == 1 || n % 6 % 4 != 1) return (n | 1) == 3;

  constexpr auto modpow = [](i64 a, i64 n, i64 M) -> i64 {
    i64 res = 1;
    while (n) {
      if (n & 1) res = res * a % M;
      n >>= 1, a = a * a % M;
    }
    return res;
  };

  const int t = std::countr_zero(static_cast<u64>(n - 1));
  const i64 k = (n - 1) >> t;
  for (const i64 a : {2, 7, 61}) {
    i64 tmp = modpow(a, k, n);
    if (tmp <= 1 || tmp == n - 1) continue;
    for (int i = 0; i < t; i++) {
      tmp = tmp * tmp % n;
      if (tmp == n - 1) break;
      if (i == t - 1) return false;
    }
  }
  return true;
}
