<?php
// concept: selective-retention
$signals = [2, 7, 1, 8, 2, 8];
$threshold = 5;
$scoreFloor = 100;
$retainedFloor = 3;
$score = 0; $retained = 0;
foreach ($signals as $index => $value) {
  $score += $value * ($index + 1);
  if ($value >= $threshold) $retained++;
}
$verdict = $score >= $scoreFloor && $retained >= $retainedFloor ? "coherent" : "fragmented";
echo "$verdict:$score:$retained\n";
