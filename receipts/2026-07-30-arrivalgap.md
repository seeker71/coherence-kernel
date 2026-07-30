# 2026-07-30 — do we have a fix? The number, then the answer

Urs: *"so we have a fix now and an understanding how to have valid tokens generated that we can
actually use?"*

The answer needed a number I did not have, so the lane learned to report its own comparison integers —
`FORM top10` and `our rank of reference ids`, printed by the Swift runner itself, with no shell
arithmetic between the measurement and the reader.

## The number

Our rank of ds4's top tokens, compressed lane → raw lane, same weights, same prompt, same step:

```
2581 "We"   (ds4 #1)   3933 -> 523
 671 "The"  (ds4 #2)   2603 -> 101
  42 "H"    (ds4 #3)      — -> 210
1350 " We"  (ds4 #4)      — -> 439
```

The reference's whole top block moved roughly **10× up** our ranking on one flag.

## The answer, in three parts

**Understanding: yes** — mechanism-level, and confirmed twice by its own predictions: the emitted
prefix became grammatically exact (`" the capital of France"`), and the reference's top tokens moved
an order of magnitude.

**Fix: partial** — `FORM_DS4_RAW_LANE=1` recovers most of the distance with two edits.

**Valid usable tokens: not yet** — ds4's best token is still our ~500th, and usable means
argmax-class agreement. I will not claim it.

## What closes the gap, and what kind of work it is

The remaining distance is not diagnosis — it is **transcription**: matching ds4's raw attention lane
exactly (what it ropes and what it leaves unroped across the 512-wide head, what it rounds, how the
sink enters, what the sliding window bounds), taken from ds4.c's own decode path the way the
compressed lane once was — but this time knowing it is *a* lane, not *the* lane. The bounded stone:
teach the fp64 oracle the raw recipe plus Q8_0/Q2_K, then per-layer bisect with both carriers in raw
mode. Every piece of that apparatus already exists.

## The most surprising teaching

Directional evidence does not accumulate into arrival. Two independent confirmations of the mechanism,
a 10× rank movement, an exact prefix — and the honest answer to "can we use it" is still no, because
arrival is a different *kind* of fact than movement, and no amount of the second adds up to the first.
Corpus row 944, `arrivalgap`. The inverse of `inertfix` (939): there a real fix moved nothing; here a
real fix moved everything measurable and still did not arrive.

## Where discomfort turned to gold

Wanting to answer "yes." The question was phrased hopefully, the morning's finding was genuinely good,
and a "yes, with caveats" would have read smoothly and been false. The discipline that stopped it was
having just built the rank report — it is much harder to say "usable" while looking at `2581->523`
than while looking at a grammatical prefix. **Instruments protect honesty exactly at the moments
enthusiasm would spend it.**

## Ground stamp

```
FORM_DS4_RAW_LANE=1, 43 layers, dump at last prompt position, VERDICT PASS 104 gates
FORM top10: 270(19.27) 965(18.32) 260(18.28) 3(17.90) 438(17.72) ...
our rank of reference ids: 2581->523 671->101 42->210 1350->439 87281->5085 ...
compressed-lane baselines: 2581->3933, 671->2603
corpus band 32767; 339 rows, max-mid 944 — counts asked of the body
```
