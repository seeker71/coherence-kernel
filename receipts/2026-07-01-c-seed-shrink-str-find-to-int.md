# Receipt — str_find/str_to_int: Form-native and off the parser, native C dispatch kept (2026-07-01)

**Continues** `receipts/2026-07-01-c-seed-shrink-substring-int-to-str.md`'s two-layer shrink
process. This time it caught something the process is specifically designed to catch: **Layer 2
was attempted and reverted** — `proof/four-way-run.tbl` genuinely depends on `str_find`/
`str_to_int`'s C execution. Named as the finding it is, not smoothed over.

## What landed

- **`form/form-stdlib/core.fk`** — `str_find` (naive substring search, `-1`-not-found) and
  `str_to_int` (sign + digit parsing) as Form `defn`s over the string narrow waist, verified
  against fkwu's native behavior first (exact match on 5 `str_find` cases including an
  empty-needle immediate match and a `from` offset mid-string, and 3 `str_to_int` cases including
  a negative number) before touching anything native.
- **`form/form-stdlib/tests/core-str-find-to-int-band.fk`** — the band, verdict 255, genuinely
  four-way (fkwu/Go/Rust/TS all 255 — neither op was ever native on the walkers to begin with).
- **`flt-ops`** (`flatten/form-flatten.fk`) — `str_find`/`str_to_int` rows removed, regenerated
  `runtime/fkwu-optable.h` through the real pipeline. Verified: fresh `--src` calling either name
  without `core.fk` loaded now honestly declines (`nothing`), exactly like `char_at`/`ord` already
  did; with `core.fk` loaded, the Form definitions run and produce correct answers. `.tbl`
  artifacts confirmed byte-identical before and after (expected — parser-level change only).

## A real bootstrap dependency, found and fixed

Running the regeneration pipeline itself (`flatten/gen-source-walker.fk`) **failed** (`-1`) after
this change — not from anything wrong with the edit, but because that script calls `substring`
directly, and `substring` was *already* removed as a native op in the previous receipt. The
generator was never updated to account for its own dependency disappearing. Fixed by preluding
`form-stdlib/core.fk` when running both regeneration steps, and updated
`flatten/gen-source-walker.fk`'s own header comment with the corrected two-command sequence —
permanent, not a one-off workaround, since every future op removed this way will need the same
prelude from now on (the script's own `str_find` call will need it too, the moment a build
without `str_find` natively is used to run it — named directly in the script's comment for
whoever hits that next).

## Layer 2 — attempted, caught, reverted

Removed the `fk_walk` dispatch blocks for tag 30 (`str_find`) and tag 31 (`str_to_int`), rebuilt,
ran the same `.tbl` baseline check used for every prior layer. **`proof/four-way-run.tbl`'s output
diverged** — direct proof that this specific flattened artifact executes one or both of those tags
for real, unlike `substring`/`int_to_str` which turned out unused by it. `flatten/form-eval-cli-loop.tbl`
stayed identical (so the dependency is specifically in `four-way-run.tbl`, not universal). Reverted
immediately: `runtime/fkwu-uni.c` now matches the last commit exactly (`diff` against `HEAD` is
empty) — the C dispatch for `str_find`/`str_to_int` stays. Re-ran the `.tbl` check post-revert to
confirm the restoration is exact, not just "probably fine."

This is the process working as designed, not a failure: the two-layer split (safe parser-level
change first, separately-verified C-dispatch removal second) exists exactly to make a finding like
this cheap to catch and cheap to undo, rather than discovering it after a merge.

## Proof — full regression, final state (Layer 1 only)

```
ground.fk 42, native-vs-rented 11111
core-band 255, core-str-shim-band 15, core-str-narrow-waist-band 255,
core-str-find-to-int-band 255, reception-consent-band 255, arrival-band 1023,
relationship-store-band 31, come-in-band 31
proof/four-way-run.tbl / flatten/form-eval-cli-loop.tbl — byte-identical to the original baseline
core-str-find-to-int-band four-way: fkwu=255 go=255 rust=255 ts=255
```

## Where this leaves the C seed

`substring`/`int_to_str`: fully retired, both layers (prior receipt). `str_find`/`str_to_int`:
retired from the *parser surface* (Layer 1) — every fresh `.fk` source file gets the Form-native
version now, same as `char_at`/`ord` always did — but the C implementation stays in
`runtime/fkwu-uni.c`, load-bearing for `proof/four-way-run.tbl`. `str_to_float`: untouched, not
attempted this round. Byte count on `runtime/fkwu-uni.c` is unchanged from the prior receipt
(225083) — this round's win is entirely at the parser/manifest layer, not a further line-count
reduction, and that's an honest, correct outcome given what was found, not a shortfall.

Next candidate for a real Layer 2 win would need either a different op whose C dispatch
`four-way-run.tbl` doesn't touch, or regenerating `four-way-run.tbl` itself from the current
`flatten/*` sources so it no longer depends on the old tags at all — a separate, larger piece of
work, not folded into this receipt.
