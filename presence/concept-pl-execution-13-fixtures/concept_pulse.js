// concept: selective-retention
const signals = [2, 7, 1, 8, 2, 8];
const threshold = 5;
const scoreFloor = 100;
const retainedFloor = 3;
let score = 0;
let retained = 0;
for (let index = 0; index < signals.length; index += 1) {
  const value = signals[index];
  score += value * (index + 1);
  if (value >= threshold) retained += 1;
}
const verdict = score >= scoreFloor && retained >= retainedFloor ? "coherent" : "fragmented";
console.log(verdict + ":" + score + ":" + retained);
