#pragma once

#include <array>
#include <bit>
#include <cstdint>

using u32 = uint32_t;
using i32 = int32_t;
using u64 = uint64_t;
using i64 = int64_t;

static inline constexpr auto modmul(const u64 a, const u64 b, const u64 M) -> u64 {
  const i64 ret = static_cast<i64>(a * b - M * (u64)(1.L / M * a * b));
  return ret + M * (ret < 0) - M * (ret >= (i64)M);
}

static inline constexpr auto modpow(u64 a, u64 p, const u64 M) -> u64 {
  u64 ret{1};
  while (p) {
    if (p & 1) ret = modmul(ret, a, M);
    p >>= 1;
    a = modmul(a, a, M);
  }
  return ret;
}

static inline auto miller_rabin_test(const u64 n) -> bool {
  static constexpr std::array<u64, 7> SPRP{2, 325, 9375, 28178, 450775, 9780504, 1795265022};
  if (n == 1 || n % 6 % 4 != 1) return (n | 1) == 3;

  const int t = std::countr_zero(n - 1);
  const u64 k = std::rotr(n - 1, t);
  for (const u64 &a : SPRP) {
    if (a >= n) break;  // Skip if a is greater than n
    u64 tmp = modpow(a, k, n);
    if (tmp <= 1 || tmp == n - 1) continue;
    for (int i = 0; i < t; i++) {
      tmp = modmul(tmp, tmp, n);
      if (tmp == n - 1) break;
      if (i == t - 1) return false;
    }
  }
  return true;
}
