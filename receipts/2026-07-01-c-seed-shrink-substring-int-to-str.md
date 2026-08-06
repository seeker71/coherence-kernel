# Receipt — first C-seed shrink: substring/int_to_str retired as native ops (2026-07-01)

**The ask:** focus on C-seed reduction — make all we can, Form-native, core ops and funcs.
Follows directly from `receipts/2026-07-01-narrow-waist-string-cleanup.md`, which built the
Form-native `substring`/`int_to_str` (over the string narrow waist) but only removed the
*redundant walker copies* (Go), leaving fkwu's own C natives — the actual "C seed" — untouched.
This receipt takes the next step: retiring those two from `runtime/fkwu-uni.c` itself.

## The discovery that shaped this receipt: two different kinds of "native"

Removing a native op from a walker (Go/Rust/TS) is low-risk: the walker re-parses source text
every run, so deleting the native registration makes it fall through to a same-named Form `defn`
cleanly — proven all through the string-narrow-waist work.

fkwu is not that simple, for one reason: **flattened `.tbl` artifacts.** `proof/four-way-run.tbl`
and `flatten/form-eval-cli-loop.tbl` are pre-serialized numeric-tag streams — they bypass source
parsing (and therefore any name→Form-defn fallback) entirely. If fkwu's tag-level execution logic
for an op is deleted, any existing `.tbl` file that happens to contain that tag would silently
misbehave, with no "unbound function" signal to catch it — a materially different risk profile
than the walker cleanup, discovered by grounding before cutting anything.

**Also discovered:** `fkwu-optable.h` (the name→tag table fkwu's *parser* reads) is not
hand-maintained — it's generated from `flt-ops` in `flatten/form-flatten.fk` via
`flatten/gen-source-walker-table.fk`, run through fkwu itself (`fkwu --src
flatten/gen-source-walker.fk` then the resulting combined driver — two Form-native calls, no
bash). `flt-ops`'s own comment claims *it* is generated from `form/form-stdlib/native-op-manifest.fk`
— that file does not exist in this tree; `flt-ops` is, in current practice, the real hand-edited
source of truth despite the stale comment. Edited there, not in the generated header.

## The two-layer fix, verified separately

**Layer 1 — the optable (parser-level, zero `.tbl` risk).** Removed `substring`/`int_to_str` rows
from `flt-ops`, regenerated `runtime/fkwu-optable.h` through the real pipeline (not hand-edited).
`fkwu-uni.c`'s execution code was untouched at this point. Verified: fresh `--src` source calling
`substring`/`int_to_str` now falls through to `form-stdlib/core.fk`'s Form `defn`s (confirmed:
without `core.fk` loaded, `(substring ...)` → `nothing`, exactly like `char_at`/`ord` already
behaved — never a crash, an honest decline). Both `.tbl` artifacts produced **byte-identical**
output before and after (expected — they bypass the parser/optable entirely; this was the safe
half).

**Layer 2 — the actual C dispatch code (real byte reduction, real `.tbl` risk).** Located the two
`if (t == 29) { ... }` / `if (t == 32) { ... }` blocks in `fk_walk`'s tag-dispatch chain and
removed them — the part that actually shrinks `fkwu-uni.c`. Rebuilt, then re-ran the exact same
`.tbl` baseline check used for Layer 1. **Byte-identical again** — empirical proof neither
flattened artifact actually exercises tags 29/32 in real execution, not just an assumption. Full
regression suite (every band landed today plus the bootstrap grounding cells) re-run and
unchanged. Reverting layer 2 alone would have been a one-line `git checkout` had anything diverged
— it didn't.

## Proof

```
Layer 1 (optable only):
  proof/four-way-run.tbl          — byte-identical to baseline
  flatten/form-eval-cli-loop.tbl  — byte-identical to baseline
  (substring "hello" 1 4) without core.fk  -> nothing (honest decline, not a crash)

Layer 2 (fk_walk dispatch removed):
  proof/four-way-run.tbl          — byte-identical to baseline (still)
  flatten/form-eval-cli-loop.tbl  — byte-identical to baseline (still)
  ground.fk -> 42, native-vs-rented -> 11111, core-grounding -> 11111
  core-band 255, core-str-shim-band 15, core-str-narrow-waist-band 255,
  reception-consent-band 255, arrival-band 1023, relationship-store-band 31,
  come-in-band 31 — all unchanged from every prior receipt today.

Full four-way, fresh builds, all four kernels, post-shrink:
  core-band 255/255/255/255, core-str-shim-band 15/15/15/15,
  core-str-narrow-waist-band 255/255/255/255, reception-consent-band 255/255/255/255.
```

`runtime/fkwu-uni.c`: **226480 → 225083 bytes** (−1397). `runtime/fkwu-optable.h`: **163 → 161
lines** (−2 rows). Modest in absolute terms — two ops out of ~150 — but real, verified, and
reversible at every step, not asserted.

## Scope — narrowed deliberately, not everything possible today

This is explicitly the FIRST shrink stone, not the whole reduction. Kept native, on purpose:

- **`str_len`/`str_byte_at`/`byte_to_str`/`str_concat`** — the narrow waist itself. These stay
  native everywhere by design (`receipts/2026-07-01-narrow-waist-string-cleanup.md`) — they're
  the floor everything else composes over, not candidates for removal.
- **`str_find`/`str_to_int`/`str_to_float`** — no Form-native replacement built yet. Deferred
  rather than rushed alongside first-time C-seed surgery in the same sitting; each is a
  straightforward extension of today's pattern (loop over the floor) once scoped its own turn.
- **Every non-string native op** (~145 of ~150 entries in `fkwu-optable.h`) — untouched. Control
  flow, arithmetic, list ops, host-effect ports, JIT machinery, HTTP/socket carriers, etc. "Make
  all we can Form-native" is the standing direction, not a claim that today covered it — this
  receipt is the first proof the *mechanism* (optable edit → regenerate → verify `.tbl` safety →
  remove dispatch → re-verify) works safely, repeatable for the next candidates.
- **The other `if (t == 29 || ...)` occurrence** (a leaf/terminal-tag membership check elsewhere
  in `fkwu-uni.c`, unrelated to `fk_walk`'s dispatch) was deliberately left untouched — its
  interaction with old `.tbl` data was less legible at a glance, and the empirical `.tbl`
  byte-identical result didn't require touching it. Named, not silently ignored.
