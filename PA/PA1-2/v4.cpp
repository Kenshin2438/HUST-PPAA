#include <cstdlib>
#include <format>
#include <iostream>
#include <mutex>
#include <stop_token>
#include <syncstream>
#include <thread>
#include <vector>

auto main(int argc, char** argv) -> int {
  if (argc < 2) {
    std::cerr << std::format("Usage: {} N\n", argv[0]);
    std::cerr << std::format("    N    Number of philosophers.\n");
    return -1;
  }

  using namespace std::chrono_literals;

  static const int N = std::atoi(argv[1]);

  std::vector<std::mutex> chopstick(N);
  auto event = [&chopstick](const std::stop_token& token, int i) {
    int L = i;
    int R = (i + 1) % N;

    while (!token.stop_requested()) {
      std::osyncstream(std::cout) << std::format("{} is now hungry.\n", i);
      {
        std::unique_lock<std::mutex> lock_L(chopstick[L], std::defer_lock);
        std::unique_lock<std::mutex> lock_R(chopstick[R], std::defer_lock);
        std::lock(lock_L, lock_R);  // std::lock 确保上锁的顺序安全

        std::osyncstream(std::cout) << std::format("{} is now eating.\n", i);
        // std::this_thread::sleep_for(200ms);

        std::osyncstream(std::cout) << std::format("{} is now thinking.\n", i);
      }
      std::this_thread::yield();
    }
  };

  std::vector<std::jthread> philosophers;
  philosophers.reserve(N);
  for (int i = 0; i < N; i++) philosophers.emplace_back(event, i);
  while (std::cin.get() != 'n') continue;

  return 0;
}