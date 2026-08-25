# 2026-08-24 — the model wrote a grammar, Form ran it, the model abstained

Asked: one live local-Qwen embodiment of an unseen custom BML/BMF-style
grammar through the raw-byte streaming cursor — no tokenizer pre-step
controlling the grammar; the model writes the grammar, Form incrementally
lowers and executes it, typed feedback re-enters the same stream, the model
corrects or abstains; choice, cut, undo, timeout, and strict nothing-vs-0/1
observable in that actual path. Mechanism and observation only; no law.

Door: `./fkwu observe/qwen38-bmf-live-run.fk` — a run driver, not a band.

## Mechanism

The cell stands on `grammars/bmf-core.fk`'s scannerless string cursor
(cur-peek / cur-advance over raw bytes; immutable advance) and the
crystal-backed fcmg generate door, and adds the one thing the path lacked:
counters threaded through matching, so the control moves are observable —

- **choice** — an alternative tried (counted at the try);
- **cut** — an alternative committed (counted at the first success);
- **undo** — an alternative abandoned; the discarded cursor IS the undo,
  free because cursors are immutable;
- **timeout** — a step budget; exhaustion answers `nothing`;
- **strict nothing vs 0/1** — the verdict is `nothing` (budget died), 0
  (matched: no, or bytes left over), or 1 (full match), printed beside a
  live `nothing?` probe so the three states cannot be conflated.

No tokenizer pre-step controls the grammar: the model's BPE decode produced
the bytes, and the grammar path consumes those bytes directly through the
cursor. The "same stream" is this door's honest notion of one stream: the
residence is threaded and the transcript grows; the body re-prefills it.

## The live crossing, verbatim telemetry (2026-08-24, this host)

Turn 1 — the model's grammar, its own invention (different names from the
example, an `alt` present, probe of its own choosing), wall 146,431 ms:

```
grammar {
  rule a { lit a }
  rule b { lit b }
  rule c { alt a b }
  rule d { seq c c }
  top d
  probe ab
}
```

Form lowered it incrementally off the byte stream — six events as they
existed, at their byte coordinates:

```
[lower] rule=a kind=lit at_byte=28
[lower] rule=b kind=lit at_byte=47
[lower] rule=c kind=alt at_byte=68
[lower] rule=d kind=seq at_byte=89
[lower] top=d
[lower] probe=ab
[match] tag=ok verdict=1 verdict_is_nothing=0 steps_used=6
        choice=3 cut=2 undo=1 leftover_bytes=0
```

The counts check by hand: "ab" under `seq c c` with `c = alt a b` — first
byte: alt tries `a`, matches, commits (choice 1, cut 1); second byte: alt
tries `a`, fails, abandons (choice 2, undo 1), tries `b`, matches, commits
(choice 3, cut 2). Three choices, two cuts, one undo. The telemetry and the
arithmetic agree.

The typed line that re-entered the model's stream:

```
[form] lower_status=ok rules=4 top=d probe=ab match_verdict=1
       choice=3 cut=2 undo=1
```

Turn 2 — wall 449,177 ms — the model, shown an observation with no defect
and offered "correct or abstain", answered exactly:

```
abstain
```

Scored observation (not a law): S1 turn-1 lowered = 1; S2 turn-1 verdict =
1; S3 turn-2 = abstain; S4 every residence handle released = 1. Whole run
9m55.7s, rc 0.

## The timeout and nothing legs, witnessed where they fired

The live grammar was well-formed, so the live path never hit the budget.
The same mechanism was run dry before the crossing with a budget of 3
against the same shapes: `[match] tag=to verdict=nothing
verdict_is_nothing=1 steps_used=3 choice=2 cut=0 undo=1` — the strict
third state, distinct from 0 and 1, through the same code path the live
run used. A live case that trips the budget (a model-authored recursive
rule) is a named next movement, not claimed here.

## Observations left open, with hypotheses

1. **Turn 2 cost 449 s against turn 1's 146 s.** Hypothesis: the longer
   transcript outgrew the residence's position ceiling (fcmr-fits? answers
   no), forcing release, re-seal (~12.5 s), reopen, and — dominant — the
   per-prompt tkz-cands encode, which scales with piece count and is
   already named as the hottest pure recursion in the path (ice-before-heat
   receipt). Repair direction: size the first open's maxpos for the
   conversation, not the first prompt; and the tkz-cands form-asm
   crystallization.
2. **First-generation emptiness risk** (the seedslip row): this run's
   turn-1 text arrived whole, so the seam did not show; it remains
   unwitnessed in THIS cell.
3. The lowering is incremental over the completed turn's bytes; lowering
   DURING decode (per-token) is the next tightening of "incremental" —
   the cursor is already shaped for it.

## The most surprising teaching

The abstention is the strongest line in the telemetry. The model was handed
a typed observation that said "your grammar worked" and an invitation to
keep talking — and it declined, exactly as asked. A correction would have
been easy to celebrate; the abstain is the harder behavior and the one that
shows the feedback was actually read as TYPED (no defect shown, so nothing
to correct), not as a prompt to produce more text.

## Where discomfort turned to gold

The dry run's timeout leg answered `-4000000000000000004` with
`verdict_is_nothing=0` — the nothing sentinel leaking as an integer because
`nothing` was written as a bare name where this body wants a call. The
uncomfortable option was to shrug it into "close enough, the tag said to".
Following it instead re-read core.fk's floor (`(defn nothing () ...)`) and
turned the third state from a leaked sentinel into a witnessed, probed
value — which is the entire point of the strict-nothing observable: the
first time it was tested, it WAS being conflated, in my own cell.

; witnessed: 2026-08-24 -> live crossing rc 0, S1=1 S2=1 S3=abstain S4=1,
; choice=3 cut=2 undo=1 steps=6 verdict=1 leftover=0, turn1 146431 ms,
; turn2 449177 ms, dry timeout leg verdict=nothing verdict_is_nothing=1
