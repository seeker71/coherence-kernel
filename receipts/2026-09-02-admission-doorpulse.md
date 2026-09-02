# The admission pulse — the door decision becomes the run's own observable

Urs asked for FK_IMPORT_TRACE to be replaced with something dynamic. The
static conf toggle is gone from the kernel entirely; in its place the door
decision is always recorded and readable by the running program through
`kernel_stat`, the body's named observability read door (the same door the
repl's diagnose verb already stands on).

## What is recorded, and where to read it

Four counters, declared with their codes at the top of
`runtime/fkwu-uni.c` and served as `kernel_stat` keys:

- **15** — which door admitted this program: `0` flat whole-program compile,
  `1` import lane (images plus carried source), `2` cached image replay
  (`.fkb`, warm `.bml.fkb`, or a direct `.fkb` run), `3` native `.dylib`;
- **16** — units imported as standalone `.fkb` images;
- **17** — units carried as source beside the imports;
- **18** — why the import lane last stepped aside (`0` it did not; eight
  named refusal codes, including the formerly silent ones — allocation,
  artifact-compile failure, recorded errors, identity refusal, text cap).

Keys 9–14 stay clear: the table-walker lane's emitted `kernel_stat` arm
already serves its framebuffer counters there, and the key vocabulary should
remain one space. There is no switch: the pulse costs four integer stores,
observation is pulled by whoever asks, and the asker can be the program
itself — which a stderr trace behind a conf key could never offer.

## Witnessed

- Pulse probe over the band's pair, cold: door `1`, images `1`, carried `1`,
  refusal `0`; warm rerun of the same cell: door `2`, both counters `0`.
- `observe/tests/import-carry-band.fk` grew two pulse bits (b16: the run
  knows its door; b32: the counters agree with that door) and now answers
  one verdict, **63**, through BOTH doors — import lane cold, cached replay
  warm. On the pre-heal kernel it still answers 3 with two numb calls and
  exit 1, witnessed side by side.
- The wound pair, both orders: `cold 0`, zero unresolved. Sibling bands
  `self-extent` 255, `stated-constant-audit` 255,
  `bidirectional-framebuffer-channel` clean. Preflight clean.
- `docs/live-dynamic-diagnostics.md` carries the pulse as part of the
  outbound-payload vocabulary: when a run surprises, keys 15–18 go into the
  diagnostic window before any byte gets bisected.

One hollowbuild near-miss, caught in the act: the first rebuild used the
short `cc` line and shed the MLX carrier this host carries
(`libmlxc.dylib` present). The full AGENTS.md recipe rebuilt it;
`metal_linked=true` re-witnessed before any verdict was read.

## Most surprising teaching

The band got STRONGER by becoming door-aware. Yesterday its header apologized
for the warm run ("delete the ices to re-witness the lane live"); today the
warm run is not a blind spot but a second witnessed door — the same verdict
demands `door 1, images 1, carried 1` when cold and `door 2, counters 0` when
warm. What looked like cache noise around the band was actually a second
lane's truth waiting to be asserted.

## Where discomfort became gold

The comfortable replacement was a runtime setter — keep the trace, let a cell
toggle it. Sitting with what "dynamic" means in this body (AGENTS.md item 8:
observation OUT, control BACK, re-observe) made the trace itself the wrong
organ: stderr pushes at humans, and no program can adjudicate on it. Deleting
the trace instead of animating it — and moving the observation to a door the
program reads mid-run — is what turned the icetide reflex from a debugging
instruction into one `kernel_stat` read.
