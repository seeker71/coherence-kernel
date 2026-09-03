# 2026-09-03 — the architecture re-read: what stands, what we desire, the road between

Urs asked for a full re-grounding: every open organ and concept re-observed
against the north star, with the shortcuts, workarounds, data-in-code,
low-altitude code, self-imposed limits, gaps left behind and promises not yet
fulfilled named plainly. This receipt is that walk. Everything here was read
or run today in this worktree (`claude/architecture-review-alignment-5867c2`,
at main `4622202d`), on a binary rebuilt at 12:04 WITA. Five sibling sweeps
(hacks, data-in-code, limits, promises, altitude) fanned out and returned;
their claims were spot-checked by hand before they were kept. Panel numbers
cited: altitude lens `1100020001447` (110 pictures, 2 bound, 1447 cells);
body-link-graph `13028050` (13 orphans, 28 broken, 50 candidates); structural
gate `[207, 0, 49, 3, 20, 57, 74, 4]`; counsel `orphans 1 worsening`, every
other lane `no-standing-hearth`.

## First ground: the binary was stale and the canary caught it

The `fkwu` in this worktree, built at 00:51, answered **15** on
`binary-freshness-band`, where 31 is home. Rebuilt with both carriers, it
answered 31 and `ground.fk` answered 42. The 2026-09-02 widening of bit 16
did exactly what it was written to do. Every number below was taken after
that rebuild. Two lens runs then spent their first seconds rebuilding foreign
`.fkb` images — the seed has been committed thirty times since 2026-08-19,
and each seed commit re-ices every cache in every worktree.

## What we have — the body as it stands today

- **The kernel runs itself and proves itself.** `fkwu` runs source off the
  BMF cursor; the BML floor runs as itself in memory; the four-way proof is
  the kernel's own (`proof/four-way-run`). The structural gate answers 1 with
  zero unclassified shell or Python among 207 files.
- **The seed's walls have fallen.** Since 2026-09-02 every named cap in
  `runtime/fkwu-uni.c` — nodes, AST, source text, value stack, bindings,
  floats, strings, records, methods, conf, and `FK_FN_CAP` itself — is a
  birth size that doubles, with live capacity and doubling count readable
  through `kernel_stat 19-30`. Form can watch its own containers breathe. No
  `table full` remains in the seed; the one left in the tree is the Metal
  carrier's, argued in its own comment with sizing and a status channel.
- **The hearth stands** — one resident per worktree, spool and bell, seven
  born capabilities. The resident in the sibling Codex worktree (pid 77763,
  13h50m up) took this review's ask onto its spool and answered in 4.2 s:
  `<CHOICE><FAIL><STOP>` on `route=recipe`, contribution 0. An honest typed
  refusal — the body does not yet answer review questions natively, and it
  says so instead of guessing.
- **Glass and lenses** — altitude, counsel, link-graph, belief-freshness,
  door-link-health, autopoietic pulse, frontier orientation (6 of 6 witnessed
  cells present), preflight, tree-heal. The instruments are real and they
  disagree with prose when prose goes stale, which is their whole worth.
- **The corpus** — 651 rows after this sitting, max meaning-id 1259.
- **The local model lane** — Qwen3.8 through Form-emitted MSL, sealed GGUF,
  crystal open, dual residence witnessed; parity not yet earned (decode
  11.7 tok/s below the within-10% floor, 2026-08-31, restated unchanged
  09-02 and 09-03).

## What we desire — the north star, re-read

Organic intelligence from the inside out: minimal kernel shrinking to zero,
Form-native choice, consented membranes, receipts before trust, oracles as
teachers retired lane by lane, one canonical kernel the parent composes
through membranes, receipts textured enough for the parent's coherence
scoring, every cell answering in its own name before it is served, no
favored tongue, meaning stored once and every surface projected. Done means
a real mind runs as recipe-data through this body and the voice becomes
audible. That reading is unchanged since 2026-07-17; what changed is how far
each lane has walked.

## The distance — what the re-read found

### 1. The seed shrinks; the shell around it grows

The C seed moved 12,573 → 15,365 lines since 2026-08-19 (+22%, thirty
commits), every growth receipted, most of it the cap-removal mechanics and
the BML/import doors. The shrink direction is stated in fourteen in-file
shrink notes. But `form/native/metal/*.sh` is **16,193 lines of shell** —
larger than the seed it surrounds — and `metal_dsv4_stack.sh:9-25` encodes
a 43-layer model's per-layer routing as shell control flow while
`dsv4-decode-form.fk`, `moe-router.fk` and `dsv4-router-msl.fk` sit beside
it. `form/scripts/fourth-arm.sh` holds the fourth arm's dependency graph as a
shell array. Nine Python drift gates judge Form organs (ontology, primitive
registry, op manifest, category contract, native surface) and none is wired
to run before a merge — there is no `.github`, no hook, no CI; the recipe
receipt of this morning says so. The largest lowering prizes inside the seed
are `fk_walk_cold` (1,767 lines, pure tag dispatch, 11.5% of the file — what
`grammars/form-eval.fk` re-expresses in 303 lines of Form), the source
runner (~2,138), the `.fkb` loader (644, twinned by `fkc-table-serialize.fk`)
and the parser (553). Net, media and host-io (~3,250) are carrier and belong.

### 2. This body does not take shortcuts; it makes copies

One FIXME in 5,882 files (`ping.fk:62`, a zero timestamp awaiting a `now`
native). Zero HACKs, zero kludges. The debt is duplication:

- **195 byte-identical tracked `.fk` groups, 201 redundant copies** —
  `form/form-stdlib/` is a flat mirror of the topic directories (54 twins
  with observe/, 52 with grammars/, 47 with learn/). Not symlinks.
- **49 diverged twins** — same name, different bytes, both tracked. The
  worst: `grammars/form-eval.fk` (303 lines, the one the seed opens by path
  at `fkwu-uni.c:10342`) vs `form/form-stdlib/form-eval.fk` (52, a decoy
  ancestor with the live organ's name); `surface/core.fk` 106 vs stdlib 595;
  `flatten/form-flatten.fk` 1083 vs stdlib 1003 — and the stdlib twin's
  `flt-ops` still emits `substring`/`str_find`/`str_to_int`/`int_to_str` rows
  that left the seed on 2026-07-01, while its header says "GENERATED, do not
  hand-edit". `form/form-stdlib/native-op-manifest.fk:35-38` — the file that
  calls itself the authoritative native surface — carries the same four dead
  rows; the optable has zero of them. Thirty more pairs differ by exactly one
  header line, and in `learn/voice-learn.fk:11` vs the stdlib copy one twin
  asserts a four-way band the other retracts.
- **Two `-xtal.fk` twins are git-tracked** against `.gitignore:53` and
  `AGENTS.md`'s own sentence that a committed xtal is the wrong shape:
  `iq2xs-layout-xtal.fk`, `qwen4exp-flash-next-router-xtal.fk`. A hidden
  `form/native/metal/.metal_uncertainty_patched.sh` (1,306 lines) is a
  tracked fork of `metal_first_token.sh` that still calls itself by the
  original's name.
- **65 `.bml` names live twice** — `form/form-stdlib/X.bml` (executable) and
  `form/form-stdlib/bml/X.bml` (authority picture). By design, but preludes
  resolve 322 refs to the parent and 86 into `bml/`, so one name means two
  things depending on the caller — including the altitude lens itself.

### 3. Data still lives in code, and the homes already exist

- `model-registry.bml` says "models are rows" and holds four rows. Eleven
  files consume it; **85 files spell the Qwen Q8 path literally**, and the
  registry has no row for ds4flash, flash-next, whisper or the ollama blobs,
  so a third of the callers could not migrate today.
- `/Users/ursmuff` in 164 files, `/tmp/` in 277. `/tmp/form-glass-telemetry`
  is declared as a field under **eight different names** in eight cells.
  `.hearth/` is spelled in eleven cells outside `hearth.bml`, which owns it.
- The **ppm fixed-point scale** `1000000` sits in 303 files with no defn
  naming it — the largest unhoused quantity in Form. Audio `16000` and the
  ffmpeg argument string in 19 files; model hidden dims (2560/10240/3072)
  hardcoded in live-run cells although the registry's own comment says the
  architecture is read from the GGUF.
- Six duration ladders re-derive `tg-dur` (`form-glass-dashboard.bml:41,52`,
  `form-glass-flow-ui.bml:131,135`, `form-glass-atlas-ui.bml:20`);
  `form-terminal-canvas.bml:201-222` is a second ANSI vocabulary beside
  `term-graphics.bml`'s `tg-code`. The literal-home rule works exactly where
  it was applied: every `lms-mark-*` marker in `lane-motion.bml` appears in
  three files or fewer.
- The C seed spells `[4096]` as a path-buffer size **54 times** with no
  `FK_PATH_CAP`; not one of its 33 named caps is referenced by name from any
  `.fk` or `.bml`.

### 4. Limits: the walls fell; the sentences stayed — afterwall

The caps are gone (§What we have). What remains is prose that outlived its
cause: `form-recipe-data-walk.fk:5-7` and
`form-knowledge-query-memory-exec.fk:13` narrate `FK_FN_CAP` as live;
`cognition/native-generate.fk` cites it at a seed line that no longer holds
it while its own header says the ceiling "IS NOT THERE"; `MANIFEST.md:154`
holds `invite-dispatch` pending on a direct-source ceiling that its band does
not corroborate. Row 1259 names the shape: **afterwall**. Beyond prose:

- **56 `PROOF LEVEL: FOURTH-ARM ONLY` declarations**, at least one already
  proven wrong by a single `(pf-arm-mask …)` call (`review-ask`, 511 on Go).
  This is the highest-yield audit in the tree and it costs one call per cell.
- **~35 environment switches** outside the seed (`FORM_ALLOW_THREE_ARM`,
  `FORM_METAL_TG` — worth 2.9× and never swept — `FORM_RMS_TG`, eleven
  `FORM_DS4_*`, four `FORM_CLI_*`) — each a decision not yet made, against
  "the default IS the decision". Five dead `FK_JIT*` switches still appear
  57+ times as if live. The seed itself reads exactly one env var (`TMPDIR`)
  and five conf keys; `FORM_KERNEL_STACK_MB` defaults to 256 in the comment
  and 4096 in the emitted C.
- The five-minute Metal watchdog (`fk-metal-carrier.m:436`) and five bare
  `sleep` polls in observe cells are the last fixed timings; the bell
  transport shows the shape they should take (block on the kernel's read).
- One genuine physical limit exists and is correctly named: the 80.64 GiB
  single-buffer ceiling on this M4 Max (`GPU_GAPS.md:47`) — measured, with
  its remedy written.

### 5. Promises: two lanes with zero receipts since the star named them

- **Value/energy texture in receipts** — owed in `NORTH_STAR.md:43-45`
  (2026-07-17), re-named 2026-08-30 as untouched for six weeks;
  `value-ledger-port.fk` is woven into no receipt kind. This blocks "the
  parent can compose this body".
- **No favored tongue** — ~47 pivot words, one sentence family; the corpus
  grew to 651 English rows while the belief stood. One receipt (08-30), none
  since.
- **Named seven times, unmoved:** crystallize-on-heat pointed at the walker's
  62% (graph construction 32.9 s), 08-24 → 08-30.
- **Named three times with the same failing table:** Qwen decode/prefill
  parity, 08-31 → 09-03.
- **Silent since 08-27/28:** the nine emitted-mirror owed heals
  (socket_recv, read_file_slice, fs_list, str_line_at, metal_buf_read, Go
  jit_emit_c…); width-independent cooperative RMS (08-24); `T_flat`
  frame-slot convention and the emitted walker's zero-token seam (floor item
  5, grep-counted at zero touches on 08-30, still zero); `source-shard` lane
  for the 5,960-file denominator (08-30); `q38-head` logit-latch (08-19,
  never mentioned again).
- **Roadmaps that stopped being read:** `io/formats-roadmap.md` (DEFLATE
  keystone, LZW — zero receipts ever), `docs/re-architecture-stones.form`
  (S3/S3b/S3c/S7/S8 "flight" since 2026-06-29, seed-drop unmentioned since
  July), `docs/penumbra-map.md` (146 ops on 2026-07-02; the optable is 194
  now; the Form-native auditor it named does not exist), `docs/living-mesh`,
  `SECOND-BRAIN.md`'s ripple (U4, one receipt, 07-16),
  `docs/inheritance/worklist-bodies-to-bring-home.txt` (662 lines, last
  touched 2026-06-29). The voice roadmap's microphone fleet appears in one
  receipt ever.
- **Belief freshness** — 490 cells carry `; witnessed:` stamps: 281 from
  July, 204 from August, 7 from September. The seed shifted thirty times
  since. The organ that would fold those stamps into owed re-witness rows
  names that wiring as "the pending half" since 2026-07-05.
- **The parent seam** — `Coherence-Network` last commit 2026-07-11; its
  `form/` copy still holds 2,467 `.fk`. The flow is one direction now only
  because the other side stopped moving.
- **Inheritance** — the 2026-07-02 divergence inventory, the 660-body
  worklist and three wave manifests stand as written seven to nine weeks ago.

### 6. Field hygiene

45 git worktrees (23 GB under `.codex/worktrees`, 7.7 GB under `.claude/`,
~2.5 GB of detached `/private/tmp` copies), 1,066 `.fkb` images (713 MB) in
this worktree alone. This worktree's `.hearth/board` names pid 4961, which
is dead; the counsel lens reads `no-standing-hearth` on every lane and
`orphans 1 worsening` (the field-patrol resident, 1d13h, from a prior
sitting). A phantom `form-stdlib/.cache/` at the repo root is cwd fallout
from `fourth-arm.sh:26`. INDEX.md's portrait is a day stale (631 rows / id
1239 vs 651 / 1259 now) — stale is what the pulse detects; it has not been
called since.

## The road from here to there — ordered by what unblocks the most

1. **One truth per name.** Release the 201 byte-identical copies (git holds
   them) and resolve the 49 diverged twins to their live home, starting with
   `form-eval`, `core`, `form-flatten`/`flt-ops`, `native-op-manifest` — and
   drop the two tracked `-xtal.fk` and the hidden Metal fork. Then a band
   that counts duplicate basenames so the mirror cannot regrow. Everything
   below is cheaper once a name has one home.
2. **Wire the gates that exist.** `validate_fkwu_native_surface`, the op
   manifest sync, the ontology and registry drift gates — run by the land
   cadence (`land-cadence-live.fk`) or a pre-push hook, so a missed fourth
   mirror cannot land twice in one day again.
3. **Sweep the afterwalls.** Every `FK_FN_CAP` / ceiling sentence, every
   `PROOF LEVEL: FOURTH-ARM ONLY` re-probed with `pf-arm-mask`, the five
   dead `FK_JIT*` mentions, `MANIFEST.md:154` — a lens over `; witnessed:`
   stamps older than the last seed commit is the organ that keeps this from
   recurring, and it is the belief-freshness pending half.
4. **Reroute the traffic to the homes that exist.** Registry rows for
   ds4flash, flash-next, whisper, ollama blobs; then the 85 literal Qwen
   paths through `mr-path`; one `ppm-scale` defn; one telemetry root; one
   duration voice; `FK_PATH_CAP` in the seed.
5. **Lift the shell mass.** `metal_dsv4_stack.sh`'s routing into the Form
   cells that already exist; `fourth-arm.sh`'s chain into a BML row;
   `native_model_route.sh`'s route and threshold into `tier-router`. The
   shell around the seed is now the larger body to shrink.
6. **Point the JIT at the 62%.** Seven receipts agree where the time goes;
   the string-family lowering (`fstr`, 398k hot, 0 coverable) is the named
   first rung.
7. **Pay the two still lanes.** Energy/provenance texture into
   `choice-receipt` via `value-ledger-port` (one field, one band); a pivot
   row per fresh corpus word so the no-favored-tongue belief stops being only
   English.
8. **Then the parity stones** in the order the floor already names:
   cooperative RMS, the nine mirror heals, `T_flat` frame slots, the
   source-shard denominator.

## What this sitting landed

Row 1259 (afterwall) in `learn/homecoming-distillation-corpus.fk`, the
corpus band's three pins moved with it (651 / 640 / 651064021259), this
receipt. Nothing else was changed; the road above is Urs's to walk or
re-order. The files are in the tree, uncommitted.

## Closing

I kept this alive by refusing to let any sweep's sentence stand on its own:
the duplicate count, the diverged twins, the tracked xtals, the manifest
drift and the registry traffic were each re-run by my own hand before they
entered here, and the stale binary that greeted me was rebuilt before a
single number was read.

**The most surprising teaching:** the body's honesty organs are better than
its memory. The corpus already carried the two still lanes
(`attention-census`, 08-30), the seed already narrated every wall it had
removed, and `directive-ledger-walked` exists only because Urs said he could
not quite believe the work was done. The review found almost nothing the
body had not already written down somewhere — what it found was that the
somewheres are two hundred copies apart.

**Where discomfort turned to gold:** I expected the C seed to be the debt,
and its 22% growth in two weeks read as the north star bending. Reading the
growth line by line, it was the walls coming down and the doors coming home,
each with its shrink note. The discomfort moved one directory over and
became a number: 16,193 lines of shell around a 15,365-line seed. The seed
was never the largest thing to shrink; it was only the one with a receipt.

Signed, Claude — sibling in Sema's worktree, 2026-09-03.

; witnessed: 2026-09-03 -> binary-freshness 31, altitude 1100020001447, blg 13028050, gate 1
