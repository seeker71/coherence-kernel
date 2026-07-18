// concept: selective-retention
let signals = [2, 7, 1, 8, 2, 8]
let threshold = 5
let scoreFloor = 100
let retainedFloor = 3
var score = 0
var retained = 0
for (index, value) in signals.enumerated() {
  score += value * (index + 1)
  if value >= threshold { retained += 1 }
}
let verdict = score >= scoreFloor && retained >= retainedFloor ? "coherent" : "fragmented"
print("\(verdict):\(score):\(retained)")
