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
  inline constexpr NONBLOCKING_QUEUE() noexcept {
    for (std::size_t i = 0; i < MAXN; i++) buffer_[i].version.store(i);
  }

  inline constexpr void push(const T& value) noexcept {
    std::size_t tail = tail_.load(std::memory_order::relaxed);
    while (true) {
      const std::size_t index = tail & MOD_MASK;
      if (buffer_[index].version.load(std::memory_order::acquire) == tail) {
        if (tail_.compare_exchange_weak(tail, tail + 1U, std::memory_order::relaxed)) {
          buffer_[index].value = value;
          buffer_[index].version.store(tail + 1U, std::memory_order::release);
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
      if (buffer_[index].version.load(std::memory_order::acquire) == (head + 1U)) {
        if (head_.compare_exchange_weak(head, head + 1U, std::memory_order::relaxed)) {
          value = std::move(buffer_[index].value);
          buffer_[index].version.store(head + MAXN, std::memory_order::release);
          break;
        }
      } else {
        head = head_.load(std::memory_order::relaxed);
      }
    }
  }

 private:
  struct alignas(CACHE_LINE_SIZE) data {
    T value;
    std::atomic<std::size_t> version;
  };
  std::array<data, MAXN> buffer_{};

  alignas(CACHE_LINE_SIZE) std::atomic<std::size_t> head_;
  alignas(CACHE_LINE_SIZE) std::atomic<std::size_t> tail_;
};

};  // namespace PA2