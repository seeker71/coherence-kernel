# 2026-09-03 — form-cli flow reveals observer contention

A physical `model-flow` request now crosses the resident Form CLI door and
leaves one typed path in Glass:

```
Q request > T token position > L layer boundary > X tensor extent
          > E expert route > M owner/Metal window > F framebuffer > G Glass
```

No prompt, answer, or tensor content enters this membrane.  Each stage carries
its own NodeID, plane, value, purpose, age, lifecycle, evidence kind, publisher,
and source.  `*` means a current physical observation; `+` means retained or
derived evidence.  That distinction keeps a completed flow visible without
claiming it is still consuming attention.

## The request found a resource fault

The first observed request advanced the standing Qwen owner from position 33
to 34 and took 9,623 ms.  Its Glass summary was:

```
Q+s1 > T+p34 > L+l47 > X+118MiB > E+top10 > M+9623ms > F+snap > G+s8
```

Host observation then found three simultaneous `form-glass-live-run.fk`
renderers at nice 10, consuming approximately 68%, 69%, and 18% CPU.  The
resident model owner was also nice 10.  Two worktree debugging supervisors
were stopped while the user-facing stable Glass process remained alive.  The
same request then advanced position 34 to 35 in 1,388 ms: a 6.9x reduction,
without unloading or reopening either model.

A later user-visible run found two Glass renderers again, on `ttys003` and
`ttys014`, at roughly 22% and 31% CPU while both remained at nice 10. The
otherwise identical resident-owner request advanced position 35 to 36 in
7,592 ms. This second observation keeps the finding live: renderer
multiplicity, not model reload, tracks the large owner-window change. It does
not yet establish a causal share for either renderer, so the per-renderer CPU
samples and full owner window remain separate evidence rows.

The bounded Flow frame at `TICK 03:45:59.952Z #0` then showed `ev=93`,
`nodes=218`, `cons=14K`, and:

```
Q+s1 > T+p36 > L+l47 > X+118MiB > E+top10 > M+7592ms > F+snap > G+s8
```

The bounded panel after that movement reported `TICK 03:27:05.614Z #0`,
`ev=90`, `nodes=212`, and `cons=13K`.  It showed the exact model name
`Qwen3.8-Flash-Next UD-Q2_K_XL (Unsloth)`, the resident publisher
`models.dual-resident.i1788393488594`, owner nice 10, and Qwen + 3B parallel
availability `1 ready`.  Monitor-local zeros were scoped as GPU `0us(idle)`,
CPU-JIT `0us(idle)`, dispatch `0dsp(idle)`, and queue `0req(drained)`; none was
presented as a global device measurement.

## The cadence root was closed

The live cadence predicate previously treated every physical-live `active`
lifecycle as attention.  Owner heartbeat, loaded/prefilled model state,
pipeline caches, mappings, and monotonic primitive counters therefore held an
idle Glass at four frames per second forever.  Glass now raises cadence only
for a physical request row or the owner's transient `model-state/active` row.
Readiness remains visible at two frames per second; true attention or moving
history reaches four.  Bands prove that a ready model, heartbeat, and mapping
counter do not claim active attention, while a transient active-state row does.

## Honest boundary

The `M` duration is currently the physical
`owner-command.position-window` position-change window. It includes command
pickup and the one-token evaluation; it is not yet device-only Metal time.
Glass names that window directly, labels its evidence `derived-window`, and
retains the exact source `owner-command.physical-position-window`. Per-pipeline GPU
timestamps remain a named carrier gap, so this movement does not claim the
requested within-10%-of-llama.cpp benchmark.

Witnesses:

```
./fkwu form/form-stdlib/tests/form-glass-live-band.fk     # 1073741823
./fkwu form/form-stdlib/tests/form-glass-live-ui-band.fk  # 536870911
./fkwu observe/form-glass-flow-current-run.fk             # exit 0
```

The concurrency-safe `observe/preflight-stdin-run.fk` now marks itself
effectful.  A self-target therefore refuses immediately instead of recursively
waiting on a second stdin line; its dedicated band remains `31`.

Signed, Codex — the observer became part of the measured system, and the flow
made its cost visible.

; witnessed: 2026-09-03 -> position 35→36 in 7592 ms with two live Glass renderers; PID 18249 retained
