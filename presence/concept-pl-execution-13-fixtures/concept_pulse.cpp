// concept: selective-retention
#include <iostream>
#include <string>
#include <vector>
int main() {
  std::vector<int> signals = {2, 7, 1, 8, 2, 8};
  int threshold = 5;
  int scoreFloor = 100;
  int retainedFloor = 3;
  int score = 0, retained = 0;
  for (std::size_t index = 0; index < signals.size(); ++index) {
    int value = signals[index]; score += value * static_cast<int>(index + 1);
    if (value >= threshold) ++retained;
  }
  std::string verdict = score >= scoreFloor && retained >= retainedFloor ? "coherent" : "fragmented";
  std::cout << verdict << ':' << score << ':' << retained << '\n';
}
