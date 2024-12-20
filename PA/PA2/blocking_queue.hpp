#pragma once

#include <array>
#include <cstddef>
#include <mutex>
#include <semaphore>

namespace PA2 {

template <typename T, std::size_t MAXN = 2048U>
  requires(std::is_trivial<T>::value && MAXN >= 2 && (MAXN & (MAXN - 1)) == 0)
class BLOCKING_QUEUE {
 private:
  static inline constexpr std::size_t MOD_MASK{MAXN - 1};

 public:
  inline void add(const T& value) noexcept {
    std::lock_guard lock(mu_tail_);
    const std::size_t index = (tail_ += 1U) & MOD_MASK;
    sem_enqueue_.acquire();
    buffer_[index] = value;
    sem_dequeue_.release();
  }
  inline void remove(T& value) noexcept {
    std::lock_guard lock(mu_head_);
    const std::size_t index = (head_ += 1U) & MOD_MASK;
    sem_dequeue_.acquire();
    value = buffer_[index];
    sem_enqueue_.release();
  }

 private:
  std::counting_semaphore<MAXN> sem_enqueue_{MAXN};
  std::counting_semaphore<MAXN> sem_dequeue_{0};

  std::mutex mu_head_;
  std::mutex mu_tail_;

  std::array<T, MAXN> buffer_{};
  std::size_t head_{};
  std::size_t tail_{};
};

}  // namespace PA2