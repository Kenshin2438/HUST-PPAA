#include <format>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

auto main() -> int {
  constexpr int N = 5;

  std::vector<std::jthread> philosophers;
  philosophers.reserve(N);
  std::vector<std::mutex> chopstics(N);

  auto event = [&chopstics](int i) -> void {
    while (true) {
      std::cout << std::format("{} is now hungry.\n", i);
      {
        std::lock_guard<std::mutex> lock_left(chopstics[i]);
        std::lock_guard<std::mutex> lock_right(chopstics[(i + 1) % N]);
        std::cout << std::format("{} is now eating.\n", i);
      }
      std::cout << std::format("{} is now thinking.\n", i);
    }
  };

  for (int i = 0; i < N; i++) philosophers.emplace_back(event, i);
  return 0;
}