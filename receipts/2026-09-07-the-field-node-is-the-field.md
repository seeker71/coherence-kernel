# 2026-09-07 — the field node is the field

Urs, reading a live frame: "runtime node IDs going up pointing to a leak."

## What stood

The permanent arena grew while the glass ran. Yesterday's mint counter said
how fast (about eleven cells a second) but not where: it counts the kernel,
not the recipe. Every door probed alone minted nothing, and every published
row id was stable second to second, so the site stayed unnamed.

## What stands

**Every mint is charged to the recipe that made it.** `fk_fn_mint` sits
beside `fk_fn_heat`, `fk_fn_fbox` and `fk_fn_unbox`, page-backed at
+104 MiB, filled at all eight mint sites (the seven private interns and
`fk_field_fill`). `kernel_page_box pid n` with a NEGATIVE n answers the mint
ledger instead of the boxing worklist, and every hot row carries mints as its
ninth field. A door already open, a mode in its own argument, no new tag.

**The site, named in one read.** Forty seconds of the live glass:

| recipe | mints |
| --- | --- |
| `fgo-field-node` | 453 |
| `fgo-metric` | 453 |

Two cells per event, 453 events, from `form-glass-observer.bml`.

**The cause.** A field node's kids carried the row's id. An id is the
instance; a field is the shape a row belongs to. Per-kernel rows are keyed
`k.<pid>.<what>`, so every kernel the host ever ran minted two permanent
cells per row, and the arena grew with the number of kernels ever seen. The
projection now carries domain, kind, unit, plane and channel, and the row
carries its own id, where it always belonged.

**Witnessed.** The live glass, mints read from its own page:

| at | mints |
| --- | --- |
| 15 s | 22 |
| 45 s | 22 |

Flat. Before: about eleven a second, without end.

Quartet 42 / 31 / 1 / 2047. observer 67108863, live 1073741823, live-ui
1073741823, dashboard 16777215, kernel-view 511, sensor-rows 2047,
events-channels 8191, machine 511, wait 255, telemetry-membrane 2097151,
observation-v2 2097151, hearth-glass 16777215, jit-lens 16383, cell-store
255, field 255, node-gift 4095, governor 1048575, frame-work 32767.

## The most surprising teaching

The counter that found this was built yesterday for a different question and
could not answer it: a per-kernel total says the arena is growing, never who
grew it. One line moved it from the kernel to the recipe, and the answer was
two names on the first read. A measurement is only as specific as the thing
it is charged to.

## Where discomfort turned to gold

I spent four probes yesterday differencing shared counters across windows,
and every one answered zero. The discomfort was doing arithmetic on a number
nobody could attribute, and calling the result an estimate. Charging the mint
where it happens, to the recipe running, is the same move that made the
dispatch counters honest the day before. The body already knew the shape; I
had to stop differencing and go put the count where the act is.

Signed, a sibling in Sema's worktree, 2026-09-07.

; witnessed: 2026-09-07 -> ground 42, freshness 31, gate 1, drift 2047, mints 22 flat at 15 s and 45 s (was ~11/s), fgo-field-node 453 and fgo-metric 453 in 40 s before the fix
