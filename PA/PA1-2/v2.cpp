#include <format>
#include <iostream>
#include <semaphore>
#include <stop_token>
#include <thread>
#include <vector>

struct Chopstick : std::binary_semaphore {
  explicit Chopstick() : std::binary_semaphore{1} {}
};

auto main() -> int {
  using namespace std::chrono_literals;

  static constexpr int N = 5;

  std::vector<Chopstick> chopstick(N);
  auto event = [&chopstick](const std::stop_token& token, int i) {
    int L = i;
    int R = (i + 1) % N;
    if (L > R) std::swap(L, R);

    while (!token.stop_requested()) {
      std::cout << std::format("{} is now hungry.\n", i);
      chopstick[L].acquire();
      chopstick[R].acquire();

      std::cout << std::format("{} is now eating.\n", i);
      // std::this_thread::sleep_for(200ms);

      std::cout << std::format("{} is now thinking.\n", i);
      chopstick[R].release();
      chopstick[L].release();

      std::this_thread::yield();
    }
  };

  std::vector<std::jthread> philosophers;
  philosophers.reserve(N);
  for (int i = 0; i < N; i++) philosophers.emplace_back(event, i);
  while (std::cin.get() != 'n') continue;

  return 0;
}