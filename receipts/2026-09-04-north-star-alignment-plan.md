# 2026-09-04 — the alignment plan: every non-Form and low-altitude line named, ordered, and measured

Urs's word this morning: complete the re-ground, and plan the cleanup of all
non-Form and low-level Form code so that all code is aligned and all gaps are
closed to the north star. This receipt is the plan. It stands on a fresh
census taken today in this worktree at `20961e66` (binary rebuilt, ground 42,
freshness 31, structural gate `[206, 0, 48, 3, 20, 57, 74, 4]`). The plan
itself lives as rows R51–R68 in `form/form-stdlib/release-ledger.bml`, beside
the 43 rows already released since yesterday morning; this page is the
reading of those rows, not a second copy of them.

## The census, today

| body | files | lines | what it is |
|---|---|---|---|
| `runtime/fkwu-uni.c` (+ optable, tests) | 3 | 16,436 | the seed — a shrink target |
| `form/native/metal/*.sh` | 32 | 17,422 | shell around the seed, larger than the seed |
| `form/scripts/` (.sh + .py) | 63 | 10,383 | build, proof and drift-gate scripts |
| `form/native/cuda` | 17 | 2,540 | PTX host doors + 2 Python emitters |
| `cognition/`, `model/`, `tools/`, `python_bmf/`, `os/` | 36 | 4,415 | fetches, builds, fixtures, one emitted kernel |
| `presence/carriers/`, `native/mlx`, `native/vulkan` | 19 | 1,782 | host doors (Swift, C) — carriers |
| Go / Rust / TS full kernels + minimal walkers | 122 | 84,179 | witnesses |
| `.fk` cells | 5,231 | — | lowerings |
| `.bml` pictures | 232 | — | 4.2% of the authored surface |

Altitude lens today: 114 pictures, 1,448 cells, 2 bound (`1140020001448`).
`host-exec` reaches out from 183 cells; the top borrowed verbs are `printf`
(17), `command -v` (8), `mkdir` (7), `date` (5), `sleep` (6), `seq` (3) —
glue the body already has natives for — and the real borrowed senses
(`ffmpeg`, `ollama`, `swiftc`, `whisper-cli`, `say`, `python3`) are the
membrane.

The seed grew 665 lines since yesterday's re-read and every line was a wrong
answer leaving (#573 nested-defn scope, #574 `true`/`false` literals, #575
witness kernels reading preludes). Row 1263 names it: healgrowth — the one
growth the shrink belief welcomes, owed before any shrink.

## The families, in the order they unblock each other

**A — the seed lowers into its organs (R13, R51–R56).** `fk_walk_cold` is
~1,770 lines of pure tag dispatch, eleven percent of the file, and
`grammars/form-eval.fk` already re-expresses it in 303 lines as a table of
rows: the reducer IS Form (stone S3b, in flight since June). It goes first
because everything else the seed carries — the source runner (~2,100), the
`.fkb` loader (~640), the `--src` parser and `fk_sparse` (~1,330) — has a
Form organ that already describes it (`source-compiler.fk`,
`fkc-table-serialize.fk`, `form-parse.fk`, the BMF cursor). What stays is
the carrier floor (~3,800: sockets, media, host-io, the Metal/MLX/CUDA
doors), and it stays as offered cells — every host effect a named membrane
row with a receipt that can be refused (stone S4). The six grow idioms fold
into one helper; `FK_OPCODE_ARM_CAP` becomes data the moment the optable is
Form. Measure: `wc -l runtime/fkwu-uni.c`, down each landing; proof: the
form-eval bands and `native-vs-rented` bit-identical through the Form
reducer.

**B — the shell around the seed transmutes (R57–R61).** This is the larger
body: 32 Metal stones carrying a 43-layer model's per-layer regime decisions
as bash control flow and its kernels as heredocs, while
`dsv4-decode-form.fk`, `moe-router.fk`, `dsv4-router-msl.fk` sit beside them
and the Qwen lane already emits its MSL from Form at runtime. The nine
Python drift gates become Form lenses the `drift-gates` door runs natively
(each byte-agrees with the script it replaces on the current tree
before the script leaves). `fourth-arm.sh`'s chain becomes BML rows, its
cache policy hearth rows; `validate.sh` yields to `proof/four-way-run` and
the drift gates; `build-form-cli.sh` to the BML cache run and the emit door.
Fetches become `http-client` plus a fixtures registry row; builds become
ingest cells; Python codegen becomes Form emitters. The shell-glue verbs in
183 `host-exec` sites become the natives that already exist; the borrowed
senses stay only as `oracle-catalog` rows. Measure: the structural gate's
tooling/fixture/carrier counts and a new carrier-mass lens (R67) — lines of
`.sh`/`.py`/`.c`/`.swift` per directory, run on the land cadence.

**C — altitude rises (R62–R65).** 5,231 cells for 232 pictures. The compiler
chain compiles itself from data first (`bml.fk` 6,440 lines, the source
compiler, the engine, the Python grammar): grammar rules become rows the
engine reads. Then organ cells with logic become `.bml` classes with the
`.fk` as lowering only until the `.bml` runs directly — then the `.fk`
goes; a kept twin is the wrong shape. Bands stay `.fk` (they prove) but
prelude the `.bml`. The `bml/` pictures (114) and the executables (99)
sharing names collapse to one file per name: the executable is the
authority. Measure: the altitude lens — pictures rise, cells fall.

**D — witnesses stay thin (R66).** Go, Rust and TypeScript are proof
siblings; #575 realigned them to read preludes and lower BML so they stop
dying on chains fkwu resolves. The direction is the minimal walker surface
(4.5k lines) plus what a witness needs, and the full kernels (84k) shed
what a witness does not need — JIT mirrors, HTTP servers, DB carriers. A
witness is realigned only when it falls out of step with fkwu.

**E — the gaps to the star that are not code shape.** These stand in the
ledger already: the `.bml` rename sweep (R42, one program with R29), the
lexicon append door now that read-back stands (R36 released, the door
owed), the emitted walker's fate (R68 — retire or heal; a lane nobody runs
is a keloid), the certainty source for choice receipts (the held-out
evaluator, R25 named), the Qwen parity stones, the `SUBMIT_EVERY`
measurement (R47, a run lost with its process), the cross-arm stack
bisection (R44), and the Q80 ordering decision that is Urs's to make
(R46).

## The measure that keeps it honest

Three lenses on the land cadence: the authoring-altitude lens (pictures,
cells, bound), the carrier-mass lens (R67, to be built first in family B),
and the seed's line count. Every movement in this program moves one of
those three numbers and leave a band that proves the same verdict on the
same arm as before. A movement that moves no number is a keloid in waiting.

## What this sitting landed

Row 1263 (healgrowth) and its pins; the plan rows R51–R68; R41 and R45
released in the ledger (witnessed today: `native-route-goal-cells-band.bml`
1048575 full, DS4 discovery live against the real 86 GB file); R29/R33/R42
folded into one rename program (both `json-codec-bml-band` and
`concept-i18n-band` still fail with the same 166 errors, the R31 shape);
four docs re-grounded from a sibling's recovered partial pass; R47's lost
run named. Three sibling sessions are re-grounding `CURRENT_FLOOR.md`, the
eight living doors, and the remaining docs and roadmaps as this is written.

**The most surprising teaching:** the census that sized this plan was
itself one of its findings — the seed everyone watches is 16k lines with a
receipt for every growth, and the shell nobody watches is 17k lines with
none. The body measured what it feared and never measured what it leaned
on.

**Where discomfort turned to gold:** a 665-line overnight growth in the
shrink target read, at first sight, as the star bending twice in one week.
Reading the three commits line by line, every one removed a wrong answer,
and the discomfort became the plan's first rule: correctness before shrink,
and a word for it so the next reader does not decline a heal.

Signed, a sibling in Sema's worktree, 2026-09-04.

; witnessed: 2026-09-04 -> ground 42, freshness 31, gate 1, ledger 24000043
