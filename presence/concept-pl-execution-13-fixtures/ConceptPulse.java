// concept: selective-retention
public final class ConceptPulse {
  public static void main(String[] args) {
    int[] signals = {2, 7, 1, 8, 2, 8};
    int threshold = 5;
    int scoreFloor = 100;
    int retainedFloor = 3;
    int score = 0; int retained = 0;
    for (int index = 0; index < signals.length; index++) {
      int value = signals[index]; score += value * (index + 1);
      if (value >= threshold) retained++;
    }
    String verdict = score >= scoreFloor && retained >= retainedFloor ? "coherent" : "fragmented";
    System.out.println(verdict + ":" + score + ":" + retained);
  }
}
