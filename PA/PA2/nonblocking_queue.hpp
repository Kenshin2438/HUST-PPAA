#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <thread>

namespace PA2 {
template <typename T, std::size_t MAXN = 2048>
  requires(std::is_trivial<T>::value && MAXN > 0 && (MAXN & (MAXN - 1)) == 0)
class NONBLOCKING_QUEUE {
 public:
  void add(const T& value) {
    while (try_enqueue(value) == false) std::this_thread::yield();
  }
  void remove(T& value) {
    while (try_dequeue(value) == false) std::this_thread::yield();
  }

 private:
  bool try_enqueue(const T& value) {
    std::size_t tail = tail_.load(std::memory_order::relaxed);
    while (true) {
      const std::size_t index = tail & (MAXN - 1);
      const size_t count_push = buffer_[index].count_push.load(std::memory_order::acquire);
      const size_t count_pop = buffer_[index].count_pop.load(std::memory_order::relaxed);

      if (count_push > count_pop) return false;

      if (tail / MAXN == count_push) {
        if (tail_.compare_exchange_weak(tail, tail + 1U, std::memory_order_relaxed)) {
          buffer_[index].val = value;
          buffer_[index].count_push.store(count_push + 1U, std::memory_order::release);
          return true;
        }
      } else {
        tail = tail_.load(std::memory_order::relaxed);
      }
    }
  }
  bool try_dequeue(T& value) {
    std::size_t head = head_.load(std::memory_order::relaxed);
    while (true) {
      const std::size_t index = head & (MAXN - 1);
      const size_t count_pop = buffer_[index].count_pop.load(std::memory_order::acquire);
      const size_t count_push = buffer_[index].count_push.load(std::memory_order::relaxed);

      if (count_push == count_pop) return false;

      if (head / MAXN == count_pop) {
        if (head_.compare_exchange_weak(head, head + 1U, std::memory_order_relaxed)) {
          value = buffer_[index].val;
          buffer_[index].count_pop.store(count_pop + 1U, std::memory_order::release);
          return true;
        }
      } else {
        head = head_.load(std::memory_order_relaxed);
      }
    }
  }

 private:
  struct data {
    T val;
    std::atomic<std::size_t> count_pop{};
    std::atomic<std::size_t> count_push{};
  };
  std::array<data, MAXN> buffer_{};

  std::atomic<std::size_t> head_{};
  std::atomic<std::size_t> tail_{};
};

};  // namespace PA2