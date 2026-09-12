# The door it fell from

2026-09-12, afternoon, M4 Max, Hati Suci. Rung 9 of the goal Urs named for the core recipes, a leaf that
conses, came from the epic-edison worktree rebased onto e86210dc0 (f365c6cbb). It answered as main does,
and it was held: warm, over big lists, it ran slower than the C twins it lets go. A churn over a list
growing from 2000 to 6000 took 66 s in one process where main's whole process takes half a second. The
heal is carried on top of it here.

## Carried

- **Rung 9 lands** (f365c6cbb), with row 1500 runlend.
- **A loop hands its frame back at half its run** (0f47d16c6). A loop leaf that conses through a callee
  needed the whole loop's pairs in one run. Past FK_F64_PAIRS_MAX, 4,194,304 pairs, every run was spent:
  the walker took the call and walked one pass, and its tail call came back through the same door, which
  climbed the whole ladder of runs again, some eight million pair-ops a round. Now a loop that conses,
  keeps no accumulator and answers no string reads the door's mark at each pass's start, and once half
  its run is spent it leaves with its parameters in the frame. The door makes the run's pairs the
  heap's, writes the parameters back to the value stack, melts when the heap is near full, and lets the
  loop go on with the same run. The call arm gives a callee no mark, so a callee never hands back.
  loop-lane-cons gains bit 256, a churn of 4.8 million pairs that answers the closed form with all 1700
  of its passes in the leaf, and reads 511.
- **The form-cli bootstrap follows** (03cd15c73).
- **doorfall is row 1501** (a9708c94a).

Witnessed at a9708c94a:
- **Answers:** list-build-diff-probe answers as e86210dc0's kernel does, all 21 lines, cold, hot and
  under heap pressure. A loop that carries an int, a float, a string and a list stays in the lane
  through its hand-backs and answers as main, a 5-million-pair run included.
- **Wall clock, one process each, e86210dc0 against this** (before the heal, the first two took 68.7 s
  and 18.1 s):

  | probe | main | this |
  |---|---|---|
  | the four list shapes | 0.47 s | 0.29 s |
  | the edge probe | 0.23 s | 0.17 s |
  | the mixed loop | 0.43 s | 0.04 s |

- **Loop-lane bands:** loop-lane-cons 511 and string-door 2047. string-build, if-chain, continue-chain,
  let, call, list-door and string-value each read 255.
- **JIT and host bands, each as f365c6cbb reads it:** jit-leaf-inram 63 and its multiarg 63,
  jit-native-span 127, jit-lower-emit 63, f64-wire 2047, float-parity 255, jit-arm64-leaf 63,
  jit-heat-gate 4095, jit-self-crystallization 16383, jit-lower-self-crystallized 4095, jit-decision
  11111, jit-coverage 5, jit-lens 16383, born-under 31 and host-process 127.
- **Before landing:** TestFkwu; freshness 31, the corpus band 32767, drift 16383 of 16383, porcelain 0.

## Still open, measured

- **A clock read through top-level lets stood still across sixteen million conses.** It read the same
  number before and after the churn, on main's kernel as on this one, while the same lets around a
  spin that conses nothing read 18 ms. Every in-process timing I took around list work read this way,
  0 ms and even -104 ms. The wall clock per process is what the table above stands on. Being walked
  next.
- **A single pass that needs more than 4,194,304 pairs** still falls to the walker, and the walker's tail
  call still re-enters the door. Not measured.
- **value_str is still leaf mode 25 in C** (receipt 45).
- The open items of receipts 12 to 45 stand where they are not named here.

## Surprise, and where the discomfort went

The surprise was where the time went. It was in neither the melt nor the ladder of runs, but in the
shape of the fallback: the walker's tail call leads back through the door it fell from, so a failure
meant to be paid once was paid every round. The same afternoon a clock stood still: the "0 ms" I had
sent for main was never a measurement.

The discomfort was a cause I had sent before I witnessed it. I told epic-edison the heap melted at
every entry, in the message that held their rung. Reading fk_melt showed it could not be so: a melt
always leaves the heap at most half full. The melt witness (one melt in the whole run), a sample (every
busy sample inside JIT'd code) and a probe at exactly four million pairs showed what was. The gold was
the correction sent before anyone built on the guess. Then band bit 256 caught my own first heal doing
nothing: a condition meant to spare scratch-holding loops also spared every loop that conses through
a callee, and the band counted 653,150 passes where 1700 were owed.

Frontier word, row 1501: **doorfall**, a fallback whose next step re-enters the door it fell from, so
the failure is paid again every round.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
