# Public query-offer live driver ready

The public 15-family answer-free query plan now has a physical local-Qwen
driver:

`observe/qwen38-public-query-offer-batch-live-run.fk`

It computes the exact effective bound-source prompt ceiling from the live qtf2
cursor (falling back to scanner IDs only when the crystal is absent), sorts the
15 offers largest-first, and carries the returned `fcmr` model residence across
the complete batch.  Each call still creates and releases its own stream state.

The generation side cannot reach the expected answer.  It seals raw bytes
before `fkpqo-observation` reaches the public scoring row.  The resulting lane
is supervised RAG with canonical-heldout credit fixed at zero.  Transport and
answer quality remain separate observations.

## Observation

Preflight after the first framebuffer-named delimiter repair:

```text
preflight observe/qwen38-public-query-offer-batch-live-run.fk
  parens        balanced
  errors        0
  warnings      0
  unresolved    0
  chain         clean — no errors, no unresolved calls; a verdict from it can be read

0
@form fkwu 0 232 0 232
```

Dormant direct execution, with no action-token file and therefore no model or
crystal access:

```text
local-Qwen public query-offer batch is dormant; run-bound action token absent
required-token=run-local-qwen-public-query-offer-batch-v1:439e49a1ed6c5980618f998f77229a5f25ba0d74f1030e3018ebc13a887486cb:model=qwen3.8-27b-q8_0-form-native-metal-jit:families=15
0
@form fkwu 0 262 0 262
```

## Still pending

No physical family in this batch has run yet.  The physical crossing must
retain all 15 observations, one open, zero reopens, final release, local=15,
remote=0, heldout-credit=0, source/frame/binding evidence per family, and the
separate observed answer95 total.  Until that run exists, this receipt names a
ready driver, not local mastery.

— Codex, 2026-08-25
