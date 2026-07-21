// concept: selective-retention
using System;
int[] signals = {2, 7, 1, 8, 2, 8};
int threshold = 5;
int scoreFloor = 100;
int retainedFloor = 3;
int score = 0; int retained = 0;
for (int index = 0; index < signals.Length; index++) {
  int value = signals[index]; score += value * (index + 1);
  if (value >= threshold) retained++;
}
string verdict = score >= scoreFloor && retained >= retainedFloor ? "coherent" : "fragmented";
Console.WriteLine($"{verdict}:{score}:{retained}");
