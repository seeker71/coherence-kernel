# 2026-07-17 — the sym lens remembers the wound: cached runs stop laundering compile errors

Witnessed live during the v4 lane work (receipts/2026-07-16-fkb-v4-64bit-value-lane.md): a
source with recovered compile errors (axiom-5 unresolved-call recovery) ran degraded with
exit 1 and a loud tally — then its cached rerun replayed the same degraded answer with
**exit 0 and the error voice erased**. The runtime's own comment declares the law ("recovers
INTO a runnable (if degraded) program and runs, carrying a nonzero EXIT via fk_nerr"); the
cache violated it on every rerun.

## What changed (`runtime/fkwu-uni.c`)

- `fk_src_write_sym_text` records `compile-errors N` (fk_nerr at image-write time) as the
  second line of the `.sym` lens — **no `.fkb` byte-format change**, so the day-old v4
  container spec and its Form mirrors stay exactly true; the error count is descriptor-class
  metadata, the same class the spec already keeps outside the hash-covered payload.
- `fk_src_sym_recorded_errors` reads it back; **absent file or absent line reads as -1
  (unknown), not 0** — otherwise deleting the lens would launder a degraded image back to
  exit 0. Unknown means incomplete cache:
  - `--src` cached path: unknown → rebuild (one-time cost per pre-existing artifact);
    recorded > 0 → run the cached image *with* a warning and exit 1; recorded 0 → as before.
  - dep-import lane: recorded ≠ 0 → refuse the import, fall back to the flat compile where
    the full chain resolves (a degraded dep image must never be baked in invisibly).
  - direct `.fkb` execution: recorded > 0 → warning + exit 1; unknown → warn that the record
    is missing (no source to rebuild from). This path also exposed that
    `fk_path_replace_ext` only strips `.fk` — the `.fkb → .sym` swap needed doing by hand.

## Proven

| case | fresh | cached | direct image |
|---|---|---|---|
| degraded cell (unresolved call) | exit 1, tally | exit 1, warning, cached-fast | exit 1, warning |
| lens deleted under a degraded image | — | rebuilds, exit 1, lens restored | warns "no record" |
| clean speech-ledger chain | exit 0, 32767 | exit 0, 32767, byte-untouched reload | exit 0 |
| probe-carrying chain (container band) | exit 1 (2 recorded), full score | exit 1, full score, cached-fast | — |

Full band sweep against the v4 baseline: **zero real regressions** (the corpus band's one
in-sweep empty was the known shared-/tmp transient; deterministically 511 in isolation) and
**39 bands healed** — including `jit-lower-band` (scored 2/15 under the HEAD binary *and* the
v4 binary; 15/15 now) and `jit-lower-bmf-band`, which previously **spun the CPU without
terminating** under every prior binary. The healer was the import-lane refusal: those chains
had been silently importing degraded dep images all along — the poison this receipt set out
to name was not hypothetical but live, scoring bands wrong and spinning runs, and refusing it
at the door healed them wholesale.

## The road not taken, and why

First cut was to **withhold** artifacts from any erroring compile. Grounding killed it: the
stdlib's chains legitimately carry recovered errors by design (capability probes like
`string_byte_fold`, cells whose multi-line prelude headers are documentation, resolved only
in the flat concatenation) — blanket withholding condemned every such chain to permanent
recompiles. A `.fkb` v5 field was next — killed too: it would reopen the C↔Form container
drift closed only hours earlier. The `.sym` lens was already the artifact's metadata sidecar,
written and unlinked as a pair with the image since #265 — the wound record belongs there.

## Most surprising teaching

**Recovered errors are load-bearing.** The tally I treated as a defect ("2 error(s)" on every
fresh chain run) turned out to be the body's capability-probe pattern breathing — axiom-5
recovery used deliberately. The honest fix was therefore not to refuse degraded caches but to
make the cache *carry the same truth the fresh run tells*: same score, same warning class,
same exit. Honesty here meant remembering, not refusing.

## Where discomfort became gold

The blanket-withhold version was already built and proven on the minimal case when the
container band came back with its artifact withheld — the fix was correct on my fixture and
wrong in the body. The discomfort of deleting working code an hour after writing it, sat with
rather than defended: the grounding that followed (who errors, why, what the probes are)
produced a design that needed no format bump and no tax. The deleted code was the tuition.

## Distillation row offered

*what one word names preserving a dead thing so it keeps passing for alive* → **taxidermy**
(0 hits before the row; near-misses both 0 but softer: "embalm" names preservation for
burial, not passing-for-alive; "launder" names the cleaning act, not the artifact that
results). The pre-fix cache did taxidermy on degraded programs; the lens now writes the
cause of death on the plaque.
