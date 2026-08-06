# Receipt — Tier 0: the char_at/ord shim, verified (2026-07-01)

**Tier 0 of the compile-time-gate plan** (`receipts/2026-07-01-come-in-flow-relationship-store.md`,
sharpened by an independent Grok review). The smallest honest movement first: stop today's actual
bleeding before building any checker. Not a substitute for Tier 1/2/3 — a floor under them.

## What landed

- **`form/form-stdlib/core.fk`** — `char_at`/`ord`, defined once over the confirmed-working
  `substring`/`str_byte_at`, exactly matching the prelude `fkwu --feval` already injects
  (`runtime/fkwu-uni.c`'s `helpers` literal in `fk_run_feval`) — so `--src` now carries the same
  shim `--feval` always had. Any file that already preludes `core.fk` (most of `form-stdlib/`)
  gets this for free, no per-file change needed.
- **`form/form-stdlib/tests/core-str-shim-band.fk`** — a regression probe: `(ord (char_at "hello"
  0))` = `104`, `(ord (char_at "hello" 4))` = `111` (not just index 0), `(char_at "hello" 1)` =
  `"e"`, `(ord (char_at "a" 0))` = `97`. Verdict **15** on `fkwu --src`. This is the thing that
  keeps this exact class of bug from silently regressing.

## Proof

- Direct probe: `(ord (char_at "a" 0))` → `97` on `fkwu --src` (was `nothing` before).
- `tests/core-str-shim-band.fk` → **15**.
- `tests/core-band.fk` (pre-existing, unmodified) → **255**, unchanged — no regression from
  adding the two new `defn`s.
- Re-ran every band built today (`reception-consent-band` 255, `arrival-band` 1023,
  `relationship-store-band` 31, `come-in-band` 31) — all unchanged, confirming the shim doesn't
  touch anything those files depend on.
- **The original corruption bug, re-tested directly**: writing through `cell-log-store.fk`'s
  `cls-put` now produces real ASCII bytes on disk (`0 3 11 urshello world`) instead of null bytes.
  The byte-encoding corruption named in `receipts/2026-07-01-come-in-flow-relationship-store.md`
  is fixed.

## A second, separate, pre-existing bug found while re-testing `cell-log-store.fk`

Fixing the byte-encoding did **not** make `cell-log-store.fk` reliable end-to-end. Reopening a
store in a fresh process (`cls-open`, which replays existing segments into a fresh keydir) does
not find records that were correctly written and are confirmed present on disk:

- `fs_exists` on the segment path: `1` (file is there, confirmed).
- `cls-keys` on a freshly-`cls-open`ed store pointed at that same directory: `0` keys.

This is a bug in `cls-replay-all`/`cls-replay-seg`'s parsing, unrelated to `ord`/`char_at` — the
bytes on disk are now correct, but the replay-on-open path still doesn't rebuild the keydir from
them. **Not fixed here** — out of scope for Tier 0, named rather than silently left for someone to
rediscover. `cell-log-store.fk` still should not be treated as reliable; `relationship-store.fk`
(the file-per-handle store built for `come-in`) remains independent of it, which this finding
retroactively validates.

## A methodology note, for whoever runs probes on this binary next

Piping a concatenated source through `/dev/stdin` (`cat a.fk b.fk | fkwu --src /dev/stdin`) gave
one **inconsistent** result during this work — a single `0` where every other run (via a written
temp file, `/tmp/fkwu_body --src /tmp/file.fk`) reliably gave `255`, three-for-three, both before
and after. Re-ran the exact stdin-piped command afterward and it also returned `255`. Not chased
further — logged as a possible buffering/race artifact of `/dev/stdin` piping specifically, not a
bug in the shim (bisected six ways with reconstructed files; every variant agreed). Prefer a
written temp file over `/dev/stdin` piping for probes going forward; it's what every other
verification in this session's receipts already used.

## Still open (Tier 1, 2, 3 — not this receipt)

- Tier 1: a real per-target op-availability manifest (native + walker + lowering + prelude
  columns, per Grok's refinement), generated from `flt-ops`/`fkwu-optable.h`, not hand-maintained.
- Tier 2: wire `form-static-analyzer.fk` to real parsed source.
- Tier 3: value-domain tags for the `intern_trivial_string`-vs-raw-string class — deferred behind
  1/2 per Grok's explicit pushback, with a cheaper middle ground (typed wrapper convention +
  runtime tag guards + a narrow syntactic check) named as the likely right scope when it comes up.
- The newly-found `cell-log-store.fk` replay bug, above — its own stone.
