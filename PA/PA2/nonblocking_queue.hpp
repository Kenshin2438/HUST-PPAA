#pragma once

#include <array>
#include <atomic>
#include <cstddef>

namespace PA2 {

static inline constexpr int CACHE_LINE_SIZE{64};

template <typename T, std::size_t MAXN = 2048U>
  requires(std::is_trivial<T>::value && MAXN >= 2 && (MAXN & (MAXN - 1)) == 0)
class NONBLOCKING_QUEUE {
 public:
  using value_type = T;
  using reference = T&;
  using const_reference = const T&;

 private:
  static inline constexpr std::size_t MOD_MASK{MAXN - 1};

 public:
  inline constexpr void push(const T& value) noexcept {
    std::size_t tail = tail_.load(std::memory_order::relaxed);
    while (true) {
      const std::size_t index = tail & MOD_MASK;
      const size_t count_push = buffer_[index].count_push.load(std::memory_order::acquire);
      const size_t count_pop = buffer_[index].count_pop.load(std::memory_order::relaxed);

      if (count_push == count_pop + 1U) continue;

      if (tail / MAXN == count_push) {
        if (tail_.compare_exchange_weak(tail, tail + 1U, std::memory_order::relaxed)) {
          buffer_[index].value = value;
          buffer_[index].count_push.store(count_push + 1U, std::memory_order::release);
          break;
        }
      } else {
        tail = tail_.load(std::memory_order::relaxed);
      }
    }
  }

  inline constexpr void pop(T& value) noexcept {
    std::size_t head = head_.load(std::memory_order::relaxed);
    while (true) {
      const std::size_t index = head & MOD_MASK;
      const size_t count_pop = buffer_[index].count_pop.load(std::memory_order::acquire);
      const size_t count_push = buffer_[index].count_push.load(std::memory_order::relaxed);

      if (count_push == count_pop) continue;

      if (head / MAXN == count_pop) {
        if (head_.compare_exchange_strong(head, head + 1U, std::memory_order::relaxed)) {
          value = buffer_[index].value;
          buffer_[index].count_pop.store(count_pop + 1U, std::memory_order::release);
          break;
        }
      } else {
        head = head_.load(std::memory_order::relaxed);
      }
    }
  }

 private:
  struct data {
    T value;

    alignas(CACHE_LINE_SIZE) std::atomic<std::size_t> count_pop;
    alignas(CACHE_LINE_SIZE) std::atomic<std::size_t> count_push;
  };
  std::array<data, MAXN> buffer_{};

  alignas(CACHE_LINE_SIZE) std::atomic<std::size_t> head_;
  alignas(CACHE_LINE_SIZE) std::atomic<std::size_t> tail_;
};

};  // namespace PA2