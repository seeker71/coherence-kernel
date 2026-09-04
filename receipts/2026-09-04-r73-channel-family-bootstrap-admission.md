# Receipt — R73: the CHANNEL family was registered all along; it was never admitted (2026-09-04)

## What the ledger said, and what the body said

The R73 row names the cause as "CHANNEL-V0 is an unreviewed bootstrap name with no registry
coordinate". Half of that is the body's own word and half is not. Witnessed before any edit:

- `form/form-stdlib/blueprint-registry.json` HAS `CHANNEL-V0` at `1/2/99/1700` and `CHANNEL-MSG` at
  `1/2/99/1701` (both with `"meaning": ""`), beside `CHANNEL-BREATH-GIFT` `1/2/99/6`,
  `CHANNEL-RESONANCE-RECEIPT` `1/2/99/7`, `CHANNEL-OSI-LAYER` `1/2/99/1702`, `CHANNEL-FLOW` `1/2/99/1703`.
  Six rows, one family, present since the phase-1 import (`1c6f456c`).
- All three projections agree: `form-kernel-go/bp_table.go:112`, `form-kernel-rust/src/bp_table.rs:110`,
  `form-kernel-ts/src/bp_table.ts:110` each carry `CHANNEL-V0 → {1,2,99,1700}`.
- `form/form-stdlib/form-ontology-bp.fk` — the reviewed bootstrap set that
  `form/user-blueprint-registry.md` names as the *runtime authority* ("the registry JSON is the
  authoring/generator source, not runtime authority") — carried ZERO channel rows. Not one of the six.

"Unreviewed" was exact; "no registry coordinate" was not. The registry row existed; the runtime
admission did not. The R27 receipt's discipline (coordinates come from the registry, never minted in a
mirror) applied with nothing to mint: every coordinate was already there to copy.

## How one source read four ways

`channel.fk:37-40` binds its four blueprints with `(bp "CHANNEL-...")`. `json.fk` preludes
`form-ontology-bp.fk`, and channel-breath-band preludes `json.fk cache.fk channel.fk` — so the Form
`(defn bp ...)` at `form-ontology-bp.fk:1461`, which raises `form_error "bp: unreviewed bootstrap
name: ..."` on a miss, is in the chain on every arm. What each arm did with a Form defn that shares a
native's name:

| arm | `(bp "CHANNEL-V0")` resolved by | channel-breath-band before |
|---|---|---|
| Go | the Form defn (shadows the native) | `form_error < bp@form-ontology-bp.fk:1462` — refused, crash trace |
| Rust | the Form defn | `fatal[kernel_panic]: bp: unreviewed bootstrap name: CHANNEL-V0` — refused |
| TypeScript | the native `bp` (bp_table) | **500** |
| fkwu | the native `bp` (tag 45, pass-through string) | **200**, exit 1, four `[unresolved-call]` |

Probe, no preludes — `(do (defn bp (name) 7) (bp "add"))`: Go `7`, Rust `7`, TS `@1.2.12.1`,
fkwu `add`. The pair that let the defn win refused; the pair that let the native win answered. That
resolution-order divergence is a kernel wound of its own, not touched here — spawned as a separate
task ("Reconcile Form-defn-over-native shadowing across kernels").

fkwu's 200 has nothing to do with the coordinate. Decomposed with a scratch copy of the band weighting
its five scores 1/2/4/8/16: answer `5` = gift (1) + receipt (4). The three open bits — transport (2),
receipt-transport (8), dedup (16) — are exactly the three that cross the `.fkb` file, and
`write_form_binary`/`read_form_binary` are natives Go/Rust/TS carry and fkwu does not (preflight:
"observed resolves on: go rust", unresolved on fkwu). `CURRENT_FLOOR.md` already names this lane
seam for `persistence-band` and this band. It stays named, not widened into this pass.

## The fix

1. **`form/form-stdlib/form-ontology-bp.fk`** — six rows added to `FOL-BP-BOOTSTRAP-TABLE` and six
   branches to the `fol-bp-coords` decision chain (tail gains six closers: 674 → 680), coordinates
   copied from `blueprint-registry.json` unchanged. The whole registered family, not only the four the
   band needs: a mirror holding half a family is the drift shape that produced this. Additive only —
   table 672 → 678 rows, chain 672 → 678 branches, `git diff` shows one changed line (the
   CONTROL-INVITE row's trailing `))` relocating, content byte-identical), the rest insertions.
2. **`form/form-stdlib/blueprint-registry.json`** — `meaning` filled for `CHANNEL-V0` and
   `CHANNEL-MSG`, text drawn from `channel.fk`'s own header. No new row, no coordinate moved; 445 rows,
   zero `(pkg,level,type,inst)` collisions, zero name collisions after. The `curated` flag is left
   alone: 10 curated rows have no meaning and 108 uncurated rows have one, so it does not mean
   "reviewed" and this pass does not invent a meaning for it.
3. **`form/user-blueprint-registry.md`** — the Channel Breath paragraph said "Go and Rust refuse";
   that is no longer what is. It now carries the three-way 500 and the fkwu lane gap.
4. **`bmf-core.fk`** — not edited. Its `bmf-bp-table` serves `build-emit`/`g-build`; nothing emits a
   channel op through that door (`grep 'bp "CHANNEL'` hits only `channel.fk`, via `intern_node`).
   Mirroring rows no resolver reads would be a third hand-held copy with no reader.
5. **The band** — not edited. It compares `node_eq` between two nodes `channel.fk` built the same
   way; there is no `(bp ...)` oracle in it to switch to `fol-bp`.

## Evidence

Build: the AGENTS.md recipe (metal + mlx carriers), silent success.
`bootstrap/ground.fk` → 42 exit 0; `binary-freshness-band.fk` → 31 exit 0;
`gate/structural-gate-run.fk` → `[206, 0, 48, 3, 20, 57, 74, 4]` then 1, exit 0 — before and after.
`python3 form/scripts/verify_category_contract.py` → PASS (76 injective RBasic slots) before and after.

| band | arm | before | after | exit after |
|---|---|---|---|---|
| `channel-breath-band.fk` (declares 500) | fkwu | 200, exit 1 | 200, exit 1 (transport seam, above) | 1 |
| | Go | refused (form_error, crash trace) | **500** | 0 |
| | Rust | refused (kernel_panic, crash trace) | **500** | 0 |
| | TS | 500 | 500 | 0 |
| | `form/validate.sh` lane | divergent (docs pass, 2026-09-04) | `✓ → 500`, `1 ok, 0 divergent` | 0 |
| `channel-band.fk` (declares 1400) | fkwu | 400, exit 1 | 400, exit 1 (same seam) | 1 |
| | Go | refused, same `bp@form-ontology-bp.fk:1462` | **1400** | 0 |
| | Rust | refused | **1400** | 0 |
| | TS | not baselined | 1400 | 0 |
| `bml-band.fk` | fkwu | 268435455 | 268435455 | 0 |
| `bml-generics-band.fk` | fkwu | 16777215 | 16777215 | 0 |
| `bmf-bp-r40-family-band.fk` | fkwu | 67108863 | 67108863 | 0 |
| `grammars/tests/control-invite-grammar-band.fk` | fkwu | 1023 | 1023 | 0 |
| `channel-interface-band.fk` | fkwu, Go | 127 | 127 | 0 |
| `channel-flow-band.fk` | fkwu | nothing (`section [form.bml]` in a `.fk`) | nothing, same three `[unbound-name]` | 1 |
| `natural-language-band.fk` | fkwu | — | 262143 | 0 |
| `bmf-core-band.fk` | fkwu | — | 700 | 0 |

channel-band shares the wound: it preludes the same three files and was refusing on Go/Rust for the
same missing admission. It reaches its declared 1400 three-way with no edit of its own.

Preflight on the band (`observe/preflight-run.fk`): 4 errors, 2 unresolved, both
`write_form_binary`/`read_form_binary` — "observed resolves on: go rust"; the TS arm was unavailable to
preflight's own probe. The chain is honestly marked CARRIED ERRORS on fkwu.

## Found, not touched

- The defn-over-native shadowing divergence above (Go/Rust vs TS/fkwu). Spawned as its own task.
- `write_form_binary`/`read_form_binary` absent from fkwu — the reason `channel-breath-band`,
  `channel-band`, `persistence-band` stop short on that arm. Already on `CURRENT_FLOOR.md`; still open.
- `bp_table.go`'s header says it is generated by `scripts/gen_bp_table.py`; no file of that name exists
  in the tree (`find` over the repo). Not needed here — no registry row was added — but the projection
  currently has no regenerator to run.

## Files touched

- `form/form-stdlib/form-ontology-bp.fk` — six family rows in table and chain
- `form/form-stdlib/blueprint-registry.json` — two `meaning` fields filled
- `form/user-blueprint-registry.md` — the Channel Breath paragraph says what is
- this receipt

Most surprising teaching: the ledger's root cause was one word too strong — "no registry coordinate"
— and every kernel had been carrying the coordinate for months; what was missing was the admission
row in the one table the registry page calls the runtime authority. The registry, three projections
and the mirror were five copies of one truth, and the four-way answer split not on which copy a
kernel read but on whether a Form defn was allowed to shadow a native.

Where a discomfort turned to gold: the first re-run after the edit failed on ALL four arms at once
with `input-ended-mid-form`, and the reflex was to doubt the fix. Counting showed I had swapped one
closer for six where six new `if`s needed six new closers — net five. Every kernel refused to
auto-close and compute "the right answer to the wrong text"; the loud four-way refusal was the body
doing exactly what its own error message promises.
