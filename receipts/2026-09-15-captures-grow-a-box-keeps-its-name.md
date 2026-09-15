# Captures grow, a stray `)` stops every kernel, and a box keeps its own name

2026-09-15, evening, this Mac, Hati Suci. The gaps the let receipt left open
(receipts/2026-09-15-a-let-answers-its-value.md), and the manifest rows the nil? landing
(d25dfcd99) found reading differently when run straight on fkwu.

## Carried

- **fkwu captures every name a nested defn reads.** `FK_CLOSURE_CAP_MAX` (8) is gone. Each
  function's capture rows (`fk_fn_cap_encoff[idx][j]`, `fk_fn_cap_slot[idx][j]`) double on
  demand through `fk_fn_cap_row_reserve`, the call channel `fk_call_cap_vals` doubles through
  `fk_call_cap_reserve`, and the closure arm (tag 243) stages captured values on the value
  stack instead of an eight-seat local. New band `closure-capture-width-band.fk`, manifest row
  `closure-capture-width fks 15`: nine, sixteen and sixty-four captures at a same-scope call,
  and sixty-four in an escaping closure.
- **A stray `)` stops Go, Rust and TS the way it stops fkwu.** Go and Rust wrapped a
  multi-form source in `(do ...)`; a stray `)` closed that do early and every later form fell
  away with rc 0. TS threw at a byte offset into the joined preludes. Each sibling now checks
  the token stream before anything runs, answers `parse error at <file>:<line>:<col>:
  [unbalanced-source] stray ')' closes a form that was never opened -- refusing to run` with
  rc 1, and voices one organ-health-v1 reading (flow `reader`, aspect `unbalanced-source`,
  needs `source-diagnostics`, offers `revise`, evidence path, line, col).
- **The validate path builds offline.** `form/validate.sh` built Go with `go build` and Rust
  with `cargo build --release --quiet`; `gate/kernel-conformance.bml`, which the drift door
  runs, did the same, and so did seven helpers under `form/scripts/`. Every one now carries `GOPROXY=off` or
  `--offline`. `scripts/fourth-arm.sh` builds only with `cc`.
- **q8-0-matmul-mma reads 15 on its home arm, fkwu** (PROOF LEVEL FOURTH-ARM ONLY, the metal
  handle door). The band, the matrix-unit lane and the manifest were right. The root was
  `fk_fbox`: it stored a float at `++fk_fp`, let the box ledger pulse (every 1024 boxes,
  `fk_f64_pulse`, which admits a leaf and boxes that leaf's own literals), and only then
  computed its answer from `fk_fp`, so a box minted on a pulse named the pulse's last box.
  Now the slot is named before the pulse. 298ce6017 let an all-float decline stay untried,
  so the ledger's pulse began reaching bodies it had been latched away from.
- **A unit-level let holding a function is a call head on fkwu.** `(let f3 (nth three_arg 1))`
  then `(f3 1 2 3)` was an `[unresolved-call]`: the head resolution never asked the unit's let
  table. It now reads the let's hold and offers the call through tag 244, as the value
  position always did.
- **The siblings answer `input_byte`.** All three listed it among their reserved heads with no
  native behind it. They stage no input, so every byte answers 0, as fkwu answers with nothing
  staged. Registry row 196 (167 in-band, 29 carrier-declared), band pins moved with it.
- **form-cli-band and form-cli-carrier-band hold today's answers.** a31cf787b rewrote the
  `about`, `kernel` and `recreate` answers and made the unbound source stamp `unbound-source`;
  the bands still held the old text and the placeholder.

## Witnessed

| probe | fkwu | Go | Rust | TS |
| --- | --- | --- | --- | --- |
| 9 captures, before | rc 1, 2 errors (`[closure-scope] ... max 8`) | 45 | 45 | 45 |
| 9 / 16 / 64 captures, after | 45 / 136 / 2080 | 45 / 136 / 2080 | 45 / 136 / 2080 | 45 / 136 / 2080 |
| `(defn f () 1))` then `(add (f) 41)`, before | stops, `fkwu:195:14`, rc 1 | `<closure #285>`, rc 0 | `<closure #280>`, rc 0 | `unexpected token rparen at 50179`, rc 1 |
| the same, after | unchanged | stops at `stray1.fk:3:14`, rc 1 | stops at `stray1.fk:3:14`, rc 1 | stops at `stray1.fk:3:14`, rc 1 |
| q8-0-matmul-mma | 5 before, 15 after | fourth-arm only | | |

- q8 in steps: the band's fp64 sums agree between fkwu and Go (28.574898352989727 for token 0,
  row 17); the batched and matrix-unit kernels agreed with each other and both missed that
  row; the weight buffer read back differed from the block string in one byte of 26180 (row
  17, block 0, byte 0: 143 written as 0); `md-f16-bits` over the band's eight scales went
  wrong once in 6400 calls (round 41, scale 4: 43008 for 42303). Bisect over 80 commits:
  6711e7c92 reads 15, 298ce6017 reads 5. A debug copy tracing to stderr showed the ledger's
  pulse for `md-f16-mant` landing between `fq-pow2` answering 2^-6 and `fq-rne` receiving a y
  whose floor was 1024. After the heal: mma 15, tg 15, no mismatch in 6400, no differing byte.
- Through `./validate.sh`, each "1 ok, 0 divergent": closure-capture-width four-way;
  indirect-call-runtime-probe (fourth-src 0 before) and model-handler (4 before) four-way at 7;
  form-cli-carrier (253 before, siblings `unbound function "input_byte"`) four-way at 255;
  form-cli-band (1965855 before, the same sibling stop) four-way, exits 0/0/0;
  q8-0-matmul-mma on its fkwu-only lane; primitive-registry three-way. Validate printed
  `building rust kernel...` and `building go kernel...` and both builds ran offline.
- Straight on fkwu after the rebase, each at its manifest mark: let-scope 511,
  loop-lane-closure 255, capture-correction 11111, rounding-ops 255, rounding-jit 31,
  loop-lane-cons 511. Freshness 31, tsc clean. `gate/primitive-registry.bml`: `OK 196 natives
  == 196 rows; lanes 167+29`. The drift door on 43055094e plus these commits:
  `drift-gates pass=16383 full=16383 refused=0`.
- The five vk-*-live rows (layers, train, blocks, diffusion-moe, q80) declare FKWU-STAGED /
  FOURTH-ARM ONLY (the Vulkan door through host-exec). Validate reads them `staged fkwu lane
  -- carrier absent in this checkout; pending, not witnessed`: their home is the staged Vulkan
  lane, not a direct run. No tool in the tree reads manifest rows directly as verdicts:
  `fourth-arm-survey.sh` classifies bands outside the manifest, and `release-ledger.bml`,
  `op-gen.fk`, `roadmap.fk` and `form-native-decode-timing.fk` name the file in prose only.
- TS on string-join before the list item left: 255 in 39.98 s at a 538 MB peak, voicing
  `list` / `copy-budget`. The coordinator withdrew the shared-tail item (Urs, via a peer:
  sibling speed is no goal); the voice stays as it was, and `voiceOrgan` now takes optional
  offers and evidence without changing that row.

## Still open, with the reason

Nothing from this list stays open; the addenda below close the last two.

## Addendum: the Vulkan rows read on this Mac

Every piece the Vulkan door (`form/form-stdlib/vk-door.bml`) needs already stood here, and none
of it was downloaded: `libMoltenVK.dylib` inside Docker.app (loaded as a file, so Docker itself
never started; `docker info` answered rc 1 before and after), the Android NDK r27c sysroot's
`vulkan/` and `vk_video/` headers, `glslangValidator` from Homebrew, and `clang`. With the two
header directories linked into a scratch include directory,
`clang -O2 -ffp-contract=off -I <dir> form/native/vulkan/run_vk.c -o .hearth/vk/run_vk` built
the carrier (`.hearth/` is ignored by git).

Validate's FKWU-STAGED lane had no way to run anything: it reported every such band pending. A
band that names its carrier now (`; STAGED CARRIER: .hearth/vk/run_vk`, carried by the five
rows) runs on the fkwu-only lane whenever that path stands, verdict and zero diagnostics held
as on that lane, and stays pending when it does not. Through `./validate.sh`, one at a time,
each `1 ok, 0 failed`: vk-layers-live 1023, vk-train-live 255, vk-blocks-live 31,
vk-diffusion-moe-live 63, vk-q80-live 7. With `run_vk` set aside, vk-q80-live read
`staged lanes pending: 1 band(s) need an absent host carrier` again.

The build is the body's own now. `./fkwu observe/vk-carrier-build.bml` (the finding and the
build live in `form/form-stdlib/vk-carrier.bml`) finds MoltenVK in Homebrew's lib or inside a
Docker.app, the headers under any NDK in `~/Library/Android/ndk`, and `glslangValidator` and
`clang` on the PATH vk-door.bml uses; it links the headers into `.hearth/vk/include`, bakes the
found MoltenVK into the carrier (`-DFORM_VK_LIB`, which `run_vk.c` tries first), and answers
`vk-carrier built` or `vk-carrier standing`. Under a home with no NDK it answers `vk-carrier
missing vulkan-headers: looked in /nonexistent-home/Library/Android/ndk/...` and voices an
organ-health reading (organ `vk-carrier`, aspect `vulkan-headers`, health 0). The five bands
name the door (`; STAGED CARRIER DOOR:`), and validate's pending line reads `carrier absent in
this checkout (./fkwu observe/vk-carrier-build.bml builds it)`.

In a fresh worktree at 6641bb2ee, with no `.hearth/` at all: freshness 31, the door built the
carrier in 1.8 s (MoltenVK from Docker.app, headers from android-ndk-r27c, glslangValidator from
Homebrew), and each of the five rows read `1 ok, 0 failed` through `./validate.sh`, rc 0. The
first try there ended validate with rc 1 and no word: `form_hash16` and the fourth-arm hashers
read their files with `cat` under `set -euo pipefail`, and a fresh checkout where no kernel
source moved never builds `bin-go`, so the compiler stamp's pipeline failed. Each cache key now
folds the files present and a line naming each one absent.

## Addendum: fkwu names the file

fkwu counted a diagnostic's line in the assembled unit, preludes expanded, a text no file on
disk holds. `fk_src_append_text` now records where each file's text begins, and a bare import
line keeps its line break so every later line keeps its number; `fk_diag` maps the offset back
and prints `fkwu: <file>:<line>:<col>:`, and its organ-health evidence carries the path and that
line. An image's symbol text keeps the assembled coordinate, a unit that lowers says `(lowered
text)`, and the hot-rows report counts lines per file.

| probe | origin/main fkwu | this fkwu |
| --- | --- | --- |
| stray `)` on line 3 after core.fk | `fkwu:196:14` | `stray1.fk:3:14` |
| stray `)` in a bare file, line 3 | `fkwu:3:1` | `strayb.fk:3:1` |
| unresolved call on line 3 after core.fk | `fkwu:823:13` | `unres.fk:3:13` |
| unresolved call on line 3 of a prelude | `fkwu:823:14` | `pre-a.fk:3:14` |

python-exec (the one manifest row carrying bare import lines) reads 7 and python-bnf 23 on both
binaries. Through `./validate.sh`, one band at a time, each "1 ok, 0 divergent": let-scope,
closure-capture-width, indirect-call-runtime-probe, model-handler, rounding-ops,
q8-0-matmul-mma (its fkwu-only lane), form-cli-carrier, core-band and string-join.

## Closing

Most surprising: one float in 6400 took another float's name, and neither the GPU nor the
matrix-unit kernel had any part in it. A C function spoke its answer after counting, and the
count had started work that minted boxes of its own.

Discomfort to gold: halfway through, form-cli-band came back with 3442 diagnostics, every one
an unbound name `PULSE`. The pull was to read it as the band's own trouble. It was mine: my
first debug copy printed on stdout, and fkwu lowers BML through its own stdout, so three
lowered caches in this worktree carried my trace. I removed them, moved the trace to stderr,
and that trace, clean, is what found the root.

Frontier word: **ledgerslip** (0 hits in the tree). Its question: when a counter that starts
work shares state with the work it starts, whose name does the result carry? My answer: the
one named before the counter moved. Name the slot, then let the ledger pulse.

— Claude (Opus 5), as Sema, worktree agent-abb008f44e70ea057
