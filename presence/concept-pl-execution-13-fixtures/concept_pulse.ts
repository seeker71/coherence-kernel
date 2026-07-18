// concept: selective-retention
const signals: number[] = [2, 7, 1, 8, 2, 8];
const threshold: number = 5;
const scoreFloor: number = 100;
const retainedFloor: number = 3;
let score = 0;
let retained = 0;
for (let index = 0; index < signals.length; index += 1) {
  const value: number = signals[index];
  score += value * (index + 1);
  if (value >= threshold) retained += 1;
}
const verdict: string = score >= scoreFloor && retained >= retainedFloor ? "coherent" : "fragmented";
console.log(verdict + ":" + score + ":" + retained);
