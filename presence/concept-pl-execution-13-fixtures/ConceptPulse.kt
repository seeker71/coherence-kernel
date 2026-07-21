// concept: selective-retention
fun main() {
  val signals = listOf(2, 7, 1, 8, 2, 8)
  val threshold = 5
  val scoreFloor = 100
  val retainedFloor = 3
  var score = 0
  var retained = 0
  signals.forEachIndexed { index, value ->
    score += value * (index + 1)
    if (value >= threshold) retained++
  }
  val verdict = if (score >= scoreFloor && retained >= retainedFloor) "coherent" else "fragmented"
  println("$verdict:$score:$retained")
}
