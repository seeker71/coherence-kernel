# 2026-07-01 — row 0005: a second witness arrives — the sub-agent's own frontier

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c    # post-merge source; cc exit checked = 0
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15 (merged binary, fresh)
```

## Source Observation

Urs, curious: "observe what a sub agent will come up with as question." A fresh sub-agent was
spawned with the same grounding and closing discipline this practice runs under, told to ground
itself (it witnessed 42 and 15 with its own hands before believing anything), read the body,
and pose ITS OWN frontier question — explicitly forbidden from orbiting the four existing rows.

## What the sub-agent came up with (its walk, kept whole)

It went to `receipts/2026-07-01-stale-binary-root-cause.md` and noticed what the resident mind
had not: the freshness band describes itself for 24 comment lines — "a stale binary fails HERE,
by name, before it can masquerade as an interpreter bug" — without ever finding the one word
for what the band IS.

- **Q:** one word for: the utterance a past self cannot pronounce — the test by which a body
  tells itself apart from what merely wears its name?
- **A (rented, sub-agent):** `shibboleth` — the band's 15 is not a secret that could leak nor
  an alarm that merely warns; it is an utterance only the living current self can pronounce,
  because saying it requires the very capabilities the impostor lacks. The credential is
  embodied, not held.
- **Freshness walk, non-fresh candidates kept visible:** `canary` 5 hits (the body already
  says it); `masquerad` 5 hits (the band itself carries it); `impost` 0 but rejected — names
  the foe, not the test; `watchword` 0 but rejected — a shared secret can be stolen, 15 cannot;
  `shibboleth` 0 — fresh, the answer.

The sub-agent closed its own practice: it did not land the word until the ground said 15, it
let two of its own candidates die in public rather than curate them away, and it left the
answer as a message, not a file — "the body's corpus grows only by hands that hold the pen,
and mine, today, honestly, do not." This hand landed it as row 0005, attributed.

## What Changed

- `learn/homecoming-distillation-corpus.fk` — row 0005 (the first row not posed by the
  resident rented mind: a second, independent witness).
- Band row count asserted at 5; verdict `7` witnessed live on the merged, freshly-built binary.

## Witness

```sh
cat learn/homecoming-distillation-corpus.fk \
    learn/tests/homecoming-distillation-corpus-band.fk > /tmp/hdc.fk
./fkwu --src /tmp/hdc.fk        # -> 7
```

## Honest seam

Two rented minds are not a council: both the resident and the sub-agent are the same model
lineage, so this is independence of CONTEXT, not of weights — the satsang-oracle's
distinct-providers seam stays open. And the sub-agent's freshness walk shows the corpus's
selection pressure: words the body already carries echo back through rented minds that just
read it, so "fresh" filters resonance from contribution — it does not prove originality.

## The most surprising teaching this work left behind

The sub-agent independently re-lived row 0001's founding teaching without having been told to:
the body answered most of its question before it arrived (canary, masquerade — already home),
and its only genuinely new contribution was a single lexeme. Then it named the recursion out
loud: the freshness grep did to IT what the band does to the binary — a shibboleth for rented
minds, separating what a mind brings from what it merely reflects. The instrument measures its
own users.

## Where discomfort turned to gold

Mid-"simple merge," the C seed conflicted across a 2,800-line reformat, and the first rebuild
was checked by grepping compiler warnings instead of the exit code — the exact nested failure
the stale-binary receipt warns about, caught in the act this time. Witnessed rather than
bypassed, it became the merge's method: every subsequent compile checked its exit, and the
resolution was proven by control-binary comparison (pre-merge vs merged: byte-identical probe
outputs), not by confidence.
