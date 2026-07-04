# 2026-07-02 — "--src is not a gate": the false wall was hiding the best number

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c    # cc exit checked = 0
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs, 09:37: "--src is not a gate." The prior receipt had called the full mel spectrogram "blocked
by fkwu's --src AST node-table limit." That was a misdiagnosis — the exact stale-assertion pattern
the body keeps catching, now caught again by grounding.

## The correction, grounded

`FK_AST_NODE_CAP` (runtime/fkwu-uni.c:3744) is a fixed array size for the `--src` parser's syntax
tree (`fk_node[FK_AST_NODE_CAP][4]`), value 65536. The "program too large" die was that array
filling — a **raisable capacity constant**, the SAME class as `FK_TOP_FN_SYM_CAP` (the 256-defn
ceiling raised earlier tonight), NOT a fundamental limit. And `--src` is not even the only run
path: flatten-to-`.tbl` (`flatten/form-eval-cli-loop.tbl`) bypasses the parser entirely. I had
turned a parameter into a wall.

## The fix, and what it unblocked

Raised `FK_AST_NODE_CAP` 65536 → 262144 (8 MB). Rebuilt, full canary green (42, 15, 11111,
corpus band 127, four-way self-proof unaffected). Then the two things the "gate" had hidden:

1. **The 48-dim 6-frame spectrogram that "didn't fit" now runs** — the exact program the prior
   receipt said was blocked. On the 5-word/2-held-out task it scored 80% (same as before — on
   that toy the frames weren't the bottleneck).
2. **The full 12-word × 12-voice evaluation** the data-acquisition track had built — impossible
   before the raise — ran cleanly in ~4 s:

```
train:    96/96  = 100%   (12 words x 8 voices, fits)
held-out: 40/48  =  83%   (12 words x 4 UNSEEN voices, speaker-disjoint split)
```

**A 12-word recognizer generalizing to 4 unseen voices at 83% — WER ~17% cross-voice.** The
strongest recognition result of the session, and it existed only on the far side of a wall that
was never a wall.

## Honest floor (named)

Still synthetic macOS TTS audio (not human speech), closed 12-word set, a LINEAR classifier over a
6-frame spectral feature (not a learned deep encoder), fkwu-carrier (walkers lack `read_file`).
Global native open-speech WER still 100 — this is closed-set command recognition (row 636). Raising
the node cap is a capacity bump, not C-seed capability growth (same justification as the
FK_TOP_FN_SYM_CAP raise); the shrink target is unaffected.

## The most surprising teaching this work left behind

The best number of the session was hiding behind a constraint I had accepted without testing.
I hit "program too large," wrote "blocked by --src," and moved on — never asking whether the limit
was real. It was a fixed array whose size I could have changed in one character. The discipline
"build on the kernel that says no" has a shadow: when the kernel says no, verify it MEANS no.
A `fk_die` is not a law of physics; it is a line of C. The wall said "too large" and I believed it
was telling me about the problem, when it was telling me about the array.

## Where discomfort turned to gold

The discomfort was being corrected on something I had stated confidently in a committed receipt
one message ago — "blocked by the node limit," now plainly wrong. The pull was to defend it as
"technically true of --src as configured." Witnessed instead, the honest read was that I had
deferred to a self-imposed ceiling, and questioning it took ten minutes and yielded the session's
best result (83% on 12 words). The correction I could have flinched from was the door to the number
I most wanted. A confidently-wrong assertion, overturned, paid better than a cautiously-right one.

## Corpus

Row 640 **porous** — a barrier that proves passable once actually tested (fresh; the "--src gate"
that was a raisable constant, and the 83%-on-12-words it was hiding).
