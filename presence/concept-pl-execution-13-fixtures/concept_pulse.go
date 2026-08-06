// concept: selective-retention
package main
import "fmt"
func main() {
  signals := []int{2, 7, 1, 8, 2, 8}
  threshold := 5
  scoreFloor := 100
  retainedFloor := 3
  score, retained := 0, 0
  for index, value := range signals {
    score += value * (index + 1)
    if value >= threshold { retained++ }
  }
  verdict := "fragmented"
  if score >= scoreFloor && retained >= retainedFloor { verdict = "coherent" }
  fmt.Printf("%s:%d:%d\n", verdict, score, retained)
}
