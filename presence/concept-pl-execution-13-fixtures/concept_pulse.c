/* concept: selective-retention */
#include <stdio.h>
int main(void) {
  int signals[] = {2, 7, 1, 8, 2, 8};
  int threshold = 5;
  int score_floor = 100;
  int retained_floor = 3;
  int score = 0, retained = 0;
  for (int index = 0; index < 6; index++) {
    int value = signals[index]; score += value * (index + 1);
    if (value >= threshold) retained++;
  }
  const char *verdict = score >= score_floor && retained >= retained_floor ? "coherent" : "fragmented";
  printf("%s:%d:%d\n", verdict, score, retained);
  return 0;
}
