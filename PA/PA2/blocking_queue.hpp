#pragma once

#include <array>
#include <cstddef>
#include <mutex>
#include <semaphore>

namespace PA2 {

template <typename T, std::size_t MAXN = 2048>
  requires(std::is_trivial<T>::value && MAXN > 0 && (MAXN & (MAXN - 1)) == 0)
class BLOCKING_QUEUE {
 public:
  void add(const T& value) {
    std::lock_guard lock(mu_tail_);
    enqueue.acquire();
    buffer_[tail_] = value;
    tail_ = (tail_ + 1) & (MAXN - 1);
    dequeue.release();
  }
  void remove(T& value) {
    std::lock_guard lock(mu_head_);
    dequeue.acquire();
    value = std::move(buffer_[head_]);
    head_ = (head_ + 1) & (MAXN - 1);
    enqueue.release();
  }

 private:
  std::counting_semaphore<MAXN> enqueue{MAXN};
  std::counting_semaphore<MAXN> dequeue{0};

 private:
  std::mutex mu_head_;
  std::mutex mu_tail_;

 private:
  std::array<T, MAXN> buffer_{};
  std::size_t head_{};
  std::size_t tail_{};
};

}  // namespace PA2