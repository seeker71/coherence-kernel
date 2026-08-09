# 2026-08-08 — the expression world stopped rebuilding itself in the dark

## The observed break

The expression-composition witness was not spending minutes inside one opaque
native call.  Same-child framebuffer cells showed the same builders recurring:
first `symbol-integrated-round-build`, then—after that boundary was repaired—
`mapping-round-build`.  In one five-second interval the first phase grew from
470 to 7,274 framebuffer roots; the next isolated phase grew from 507 to 5,150.
Those were repeated reconstructions, not quality movement or learning.

The first progress prelude also reopened and cleared its lazily bound window on
every event.  Separately, the outside observer rescanned the complete runner log
every five seconds.  The combination amplified one earlier diagnostic log to
336 MB.  The child was stopped; that transient log and the later 3 MB and 2 MB
stopped-run logs were removed.  Repository sources were not removed.

## The Form-native repair

`observe/live-call-framebuffer.fk` now has an append-only nested entry point.
Only the outer bounded witness opens the window; nested call phases can no longer
erase earlier cells.

`form/form-stdlib/framebuffer-value-cache.fk` adds a window-scoped,
content-addressed `FBVC-ENTRY`.  A cache result is `(present?, value)` so a valid
zero, nothing, alternative, or unknown cell is never confused with a miss.
`framebuffer-clear` releases the entries.  The cache does not persist memory,
admit a claim, choose an answer, or grant authority.

The six expensive blueprint derivations and six composition derivations now do:

```text
lookup offered key → reuse on presence
                   → otherwise enter → build → leave → attributed store
```

The observer reads only the newest 256 projection lines.  Shell remains an
observation carrier only; the calls, cache, world, and verdict stay Form-native.
No Python, remote model, `--src`, C-seed growth, training, or promotion entered
this witness.

## Exact focused witness

Direct `.fk` execution returned:

| Band | Verdict | Meaning |
|---|---:|---|
| `observe/tests/live-call-framebuffer-band.fk` | 1023 | nested calls append; one window reaches four ordered roots |
| `form/form-stdlib/tests/framebuffer-value-cache-band.fk` | 255 | two accesses cause one build and one attributed store |
| `form/form-stdlib/tests/living-world-expression-blueprint-band.fk` | 32767 | the prior fifteen semantic claims remain exact |
| `form/form-stdlib/tests/living-world-expression-composition-band.fk` | 32767 | the prior fifteen composition claims remain exact |

All four focused bands completed together in 2.1 seconds.  Under the live
supervisor, the full composition band completed in 2.7 seconds, exited zero,
and reached `currentness-worlds leave` after 51 total framebuffer roots.  Its
outer elapsed field was 156 ms.  Every expensive builder had one enter and one
leave; gaps in frame numbers were the source-attributed cache entries themselves.

## Honest placement

Execution and observability moved materially.  The content-addressed world and
its exact verdict did not change.  Therefore this is evaluation-reuse evidence,
not a better world model, not NL-to-NL score movement, not native vocabulary
coming home, and not a native generative voice.  It removes a feedback-loop tax
that was consuming the witness and gives subsequent learning work a causal,
inspectable path.  Model quality and authority movement remain unclaimed.
