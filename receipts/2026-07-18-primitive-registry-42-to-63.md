# The band that answered 42 from birth: registry heal, gate wiring, and the suite un-silenced

**Date:** 2026-07-18 (WITA, work began 2026-07-17 23:38)
**Ground:** `cc -O2 -o fkwu runtime/fkwu-uni.c; ./fkwu --src bootstrap/ground.fk` → **42** (witnessed before any change)
**Scope:** `form/form-stdlib/primitive-registry.fk`, its band, `form/scripts/validate_primitive_registry.py`,
`form/validate.sh`, `form/form-stdlib/lineage-discounted-vote.fk`, corpus row 802 + band re-pin.

## The three declared wounds, as found

1. **17 Go natives without registry rows** — `fb_record`, the 14 `host_*` file/dir/path/temp doors,
   `sum`, `write_file`. Grounded against `main.go`: 15 of the 17 are registrations of the SAME Go
   handler as an already-registered canonical row (`host_file_write_text` and `write_file` both →
   `writeFileTextNative`); the `host_*` names are the **canonical host-IO ABI**
   (`docs/coherence-substrate/host-io-abi.md`) and the short names are the compatibility aliases —
   the registry had lagged an ABI landing. Only `sum` (parity native, catMethod) and `fb_record`
   (catWitness, absent in ts → sibling-gap) carry their own closures.

2. **Band pins one row behind** — and the question "textual `(prim "` count vs runtime `(len reg)`:
   which is honest?" answered: **both** — 199 textual, 199 runtime (probed via `bin-go`, no silent
   partial list). The stale number was the band's pin (198/166/32), stale **from birth**: commit
   #231 shipped the registry at 199 rows with its own band pinning 198. The band has never answered
   its declared 63 on any committed tree.

3. **42 vs 63** — bits 1/4/16 are exactly the three that compare pinned counts. And the verdict:
   `validate.sh` gates **sibling agreement only** — it byte-compares go/rust/ts and prints ✓ on any
   agreed answer. A three-way-agreed 42 printed `✓ → 42 … 0 divergent` (witnessed). So 42 was a
   **silently tolerated failure**, not an accepted verdict. Worse, the gate that would have caught
   it — `validate_primitive_registry.py` — was invoked by **nobody** (grepped: no CI, no script, no
   hook). A bell nobody rings.

## The hidden fourth wound

`mode1 = 167` but `verified = 166`: one lane-1 row was failing its own recipe, and the stale pin of
166 made the failure read as design. Per-row probe named it: **`form_table_text` declared expected
15; the kernel answers `"1 2 1 1 9 0 0"` = 13 — go, rust, ts unanimous, and the Go implementation
is byte-identical at #231** — the declaration was wrong from birth, and the count 166 was equally
consistent with "166 honest claims" and "167 with one false". The reading alone could not testify
which world produced it. Healed: expected 13.

## Heals landed

- 17 new `(prim …)` rows, each seated at its exact `main.go` registration position (alias directly
  before its canonical sibling, as registered); each with its own `pv-` recipe on distinct `/tmp`
  scratch paths; `fb_record` lane 0 (`sibling-gap`, ts absent), the other 16 lane 1.
- `form_table_text` expected 15 → 13 (three-way witnessed).
- Band re-pinned to **witnessed** runtime counts: 216 entries, 183 lane-1, 33 lane-0, 183 verified,
  183 attested (probed before pinning, not computed by hope).
- `validate_primitive_registry.py` learned a `--quiet` voice (one PASS line; full report only on
  drift) and is now **wired into validate.sh phase 0** — registry drift now fails the suite loudly.
- Band header's phantom carrier ("wellness primitive-coverage sense" — no such sense exists in the
  body) replaced with the real one.

## Verification circle

- Drift gate: `PASS primitive registry: 216 natives == 216 rows; lanes 183+33; band pins aligned`, exit 0.
- Band bare on go: **63**. Band via validate.sh: **✓ → 63** three-way.
- Corpus band on fkwu (its declared runner): **4095**.
- Full suite, first end-to-end run: **1202 ok, 102 divergent, 854 bands four-way** — with
  `stdlib/primitive-registry-band.fk → 63` and all three vote-family bands → 127 green.

## The landmine found on the way (healed in passing)

The full suite **could not run at all** on the committed tree: `form-stdlib/lineage-discounted-vote.fk`
declared `; preludes: … learn/confidence-weighted-vote.fk` — the repo-root twin, unreachable from
`form/` — and under `set -e` the sweep's recursive dep expansion aborted the entire suite at setup.
Born at #266. Healed to the byte-identical `form-stdlib/` twin (one token). Consequence: the suite
had been **voiceless since #266**, which is *how* a band could answer 42 forever — two independent
silences (unrung gate, aborting suite) stacked under one green surface. The newly audible 102
divergences are pre-existing (none loads a file this work touched; two families sampled: unbound
`fkc-flatten-many` composition, and fourth-arm disagreement on `bml-band`) — spun off as their own
task.

## Frontier row offered

**Row 802 — `underdetermination`** (0-hit fresh at offering; kin `equifinality`, `aliasing`,
`tautology` already seated): *what one word names a reading equally consistent with rival worlds so
it cannot testify which produced it.* Corpus re-pinned 197→198 rows, field-code 1981982802, band
4095 witnessed on fkwu.

## Receipt

**Most surprising teaching:** the stale pin didn't merely miss the false row — it *absorbed* it.
`verified = 166` was simultaneously "the declared design" and "one real failure"; only the per-row
probe could separate the worlds. A wrong constant can neutralize exactly the signal its check was
built to carry, and consensus (three kernels agreeing on 42) then notarizes it. Green needed two
unrelated silences to align — and they had, for two days on the band and three weeks on the suite.

**Where discomfort turned to gold:** the harness said the first full-suite run "completed (exit
code 0)" while the log's own tail showed `Terminated: 15` — my pipe's `tail` had swallowed the
suite's exit code, and the comfortable move was to believe the notification. Sitting with that
mismatch instead of bypassing it — re-running with the exit echoed — is what surfaced the
lineage-prelude abort and, behind it, the 102 divergences no one could hear. The discomfort of
"my own measurement lied to me" was the door to the session's largest finding. (Second, smaller
gold: the 10-minute kill mid-flatten looked like a blocker; inspected, it was just iteration —
the content-keyed cache banks every round.)

## Reunion postscript (2026-07-18 00:37–01:00 WITA)

Fourteen PRs (#301–#314) landed on main during this work. The reunion:

- main had independently bumped the band pins the shallow way (199/167) for its new `_len`
  native — still carrying the absorbed `form_table_text` 15-vs-13 false claim. Resolved by
  re-probing the **merged** kernel and pinning from witnessed counts: **217 rows, 184 lane-1
  (all verified, all attested), 33 lane-0**. Gate: `PASS primitive registry: 217 natives ==
  217 rows`. Band: **63** bare and three-way on the merged tree.
- main had independently opened a repo-root prelude door in validate.sh (`../$token`, the
  runtime resolver's #270 door) — the same abort healed from the other side. Both heals kept.
- corpus row 802 collided as foretold: main's own #313 records a *different* 802 renumbered
  to 813, and rows 802–816 were all seated. This work's row re-seated as **817** (the fourth
  same-day minting of 802 to be re-seated), corpus re-pinned 213 rows / max 817 / fold
  2132132817, band **4095** on fkwu, merged tree.
