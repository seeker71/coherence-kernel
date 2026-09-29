# Current Floor

What stands in this body today, read on 2026-09-28 (WITA) on this Apple M4 Max through `./fkwu`, the
binary passing its freshness band. Every line names the command that reads it. A verdict counts only
when the process also exits 0 — a green number over a nonzero exit is a fold over `nothing`. Bands ran
on the warm images beside their sources while sibling sessions loaded the host, so no timing on this
page was taken today; a number that comes from an earlier run names its receipt.

The organ map is [`MANIFEST.md`](MANIFEST.md), how to build and embody is [`AGENTS.md`](AGENTS.md), the
ground is [`axioms/core-axioms.form`](axioms/core-axioms.form), and the direction is
[`docs/local-agent-goal.form`](docs/local-agent-goal.form). How we got here lives in git and `receipts/`.

## Grounding

Build lines live in [`AGENTS.md`](AGENTS.md) (`cc -O2 -o fkwu runtime/fkwu-uni.c`, and the Metal
carrier dylib fkwu admits in the same process).

```text
./fkwu bootstrap/ground.fk                                -> 42
./fkwu bootstrap/ground-recursive.fk 10                   -> 55
./fkwu form/form-stdlib/tests/binary-freshness-band.fk    -> 31
./fkwu bootstrap/ground-numeric-list.fk                   -> [1, 2.5, [3, 4]]
./fkwu form/form-stdlib/tests/native-vs-rented-band.fk    -> 11111
./fkwu proof/four-way-run-recipe42.fk                     -> 0   (FOUR-WAY)
```

The four-way cell host-execs the three minimal proof walkers, built from `walkers/README.md`'s own
lines; with them unbuilt it answers 2 (WALKER-SUSPECT) — both readings taken today, before and after
building them.

`runtime/fkwu-uni.c` is 25,483 lines (`wc -l`), last changed 2026-09-15 (`git log -1`). It is the
seed, and it shrinks as its lanes lower into Form organs (`release-ledger.bml` R13, stones R51–R56).

## Body-wide witnesses

```text
./fkwu gate/drift-gates-run.bml          -> drift-gates pass=31 full=31 refused=0
                                            (5 of 14 rows: a landing runs the rows whose ground
                                            moved; the nine kernel rows sit out while no kernel
                                            ground moved since origin/main, 4.4 s)
./fkwu gate/structural-gate-run.fk       -> structural-gate-v3 [92, 0, 29, 0, 18, 0, 45] then 1
                                            (total/unclassified/carrier/oracle/fixture/
                                            proof-sibling/tooling)
gate/tests/structural-gate-band          -> 16383
./fkwu observe/door-link-health-run.bml  -> doors=12 links=120 broken=0 code=12120000
./fkwu observe/belief-stamps.bml         -> 534499010  (field stamped*10^6 + owed*10^3 + laws)
observe/tests/belief-rewitness-band      -> 63
./fkwu form/form-stdlib/release-ledger.bml -> open=28 moving=0 released=115
learn/tests/homecoming-distillation-corpus-band -> 32767  (asserts 986 rows, 967 admissible)
value-eq-arena-band 31 · import-carry-band 63 · form-cli-author-high-band 4095
host-os-membrane-band 8191 · bidirectional-framebuffer-channel-band final field 1
grammars/tests/form-eval-band 65535 · form-eval-full-band 635 · source-compiler-grammar-bridge-band 32767
pattern-match-band 511 · choice-lane-core-band 1023 · backtrack-band 255 · control/tests/offer-ack-core-band 2097151
control/tests/attempt-band 4095 · file-bytes-band 127 · form-bml-cursor-full-band 105  (each four-way)
control-invite-grammar-band 1023 · cell-serialize-band 1023 · json-band 1023 · wire-rpc-band 15
```

Every tracked cell's `witnessed:` stamp is read into the belief lens, oldest first; the re-witness door
(`observe/belief-rewitness.bml`) renews a stamp only from a fresh band run and reports a mismatch as a
lapse. The release ledger's open rows are the body's named work, each with its witness.

## The BML floor

A unit lowers by what it carries: any file with a `section [` block — `form.bml`, `form.lift`,
`form.action`, `form.route`, the `*.bmf` grammar dialects — travels through `bml-floor-compile`
whatever its extension, as a prelude or as the main file, and fkwu keeps the `.lowfk`/`.fkb` cache
beside it. Of 1,344 tracked `.bml` files, 1,071 carry a `section [form.bml]` block; nine `.fk` files
carry one mid-file and lower in place; eleven files carry `section [form.lift]` (`git ls-files`,
`git grep -l`). `true` and `false` are literals in the dialect, and a nested `defn` is a registered
function (the two nested-defn bands below).

The cursor (`grammars/form-bml.fk`, lowered by `form-bml-lower.fk`) reads the same sections whole, with
direct backtracking, to the compiler's recipes NodeID for NodeID after the contract pass they share:

```text
./fkwu observe/bml-cursor-coverage-run.bml -> sections=1094 parity=1093 apart=0 refused=1
                                              (the one refused is a receipt artifact whose section the
                                              compiler's own finder cannot bound either; R144 is the
                                              cursor becoming the one reader)
form-bml-cursor-full-band 105 · bmf-prefix-state-band 4194303 · form-bml-prefix-choice-band 4194303
```

```text
bml-band 268435455 · bml-generics-band 16777215 · native-route-goal-cells-band.bml 1048575
nested-defn-scope-band 63 · nested-defn-closure-capture-band 63
bml-float-literal-band 2047 · bml-form-size-band 127 · cell-channel-band 4095
json-codec-bml-band 8191 · kernel-http-band 536965066 · channel-flow-band 8388607
circle-band 1048575 · static-to-dynamic-cells-band 262143 · bml-capability-ledger-band 255
form-pe-coff-band 16383 · learn/tests/choice-receipt-band.bml 4294967295
language-packs-fourth-band 31 · bml-bmf-control-curriculum-band 1048575
bml-bmf-stream-curriculum-band 16777215 · form-cli-allowance-band 2047 · form-cli-live-band 255
form-cli-lens-mint-band 1023 · native-tensor-lifecycle-band 1023
```

## The mind and its voice

The voice speaks on this Mac's own metal: Qwen3.8-27B Q8_0 walked as Form recipe-data in the fkwu
session, every Metal pipeline Form-emitted and JIT-compiled at runtime, the geometry read from the
sealed GGUF header. The body takes engineering turns on its own through
`observe/native-turn-run.bml`, begun by the host schedule (launchd), and each turn writes one row to
`receipts/native-turn-ledger.jsonl`: today it holds **7 turns, 2026-09-25 17:01 to 2026-09-27 19:17
WITA, 6 of them at `rented_mind` 0 and 3 with their band gone green** (`wc -l`, `grep -c`).

The resident (`observe/form-cli-peer-contribution-live.fk`, the hearth) is one Form/Qwen/KV peer that
takes tasks from an append spool and returns length-safe durable results; it rests on its fifo bell
at idle and a `release` byte closes its model and state handles. Its admission deadline is one Form
row, `hearth-metal-deadline-ms` = 300000 (`form/form-stdlib/hearth.bml`), handed to the carrier
through `metal_deadline`.

```text
form-cli-peer-direct-answer-action-band    -> 8191
form-cli-peer-stream-ingress-band          -> 2097151
form-cli-peer-contribution-turnwheel-band  -> 33554431
observed-auto-learning-band                -> 32767
hearth-band                                -> 32767
receipt-texture-band                       -> 16383
lora-backward-band 511 · lora-step-live-band 511 · lora-adapter-band 31 · symbol-voice-band 63
```

### The GPU lanes

Every layer is a Form recipe first and then a carrier on a GPU, held to that recipe. Four light
Metal bands ran today:

```text
metal-door-band          -> 15
msl-families-mint-band   -> 4294967295   (every emitted MSL family mints on this Metal)
msl-lane-coverage-band   -> 31           (every -msl entry point has a band that enqueues it)
matvec-t-band            -> 127          (the transposed matvec and the rank-1 accumulate)
floor-lens-band 31 · floor-spread-band 31 · kernel-length-band 63   (the lens arithmetic, no GPU)
```

The lanes that open a model or hold the GPU were not re-run for this page. Each band declares its
verdict in its header; the last run of each lives in its receipt:

| lane | band | last witnessed |
|---|---|---|
| Qwen3.8-27B handle, all 64 layers on the device | `qwen35-dense-token-handle-band` 2147483647 | `receipts/2026-08-30-reground-the-floor-answered-fresh.md` |
| Q8_0 prefill on the matrix unit; the 577-token prefill at 9.5–16.6 s of GPU, the same token | `q8-0-matmul-mma-band` 15, `qwen38-prefill-quant-band` 31 | `receipts/2026-09-11-named-pain-walked.md` |
| cooperative matvec twins on real weights | `q8-0-matvec-tg-band` 1023, `q6k-q4k-matvec-tg-band` 8191 | `receipts/2026-09-06-the-root-crosses-the-barrier.md`, `receipts/2026-09-06-the-loader-was-the-wall.md` |
| up to eight sequences in one decode step | `dense-multi-band` 1023 | `receipts/2026-09-09-the-body-had-a-mind-and-the-wrong-question.md` |
| half and bfloat element formats | `precision-lanes-metal-live-band` 2097151 | `receipts/2026-09-11-the-registry-resolved-and-the-host-that-decided.md` |
| Vulkan carrier (MoltenVK here) | `vk-layers-live-band` 1023, `vk-train-live-band` 255, `vk-blocks-live-band` 31 | the same receipt |
| native LoRA with Adam moments resumed | `native-lora-resume-band` 7 | `receipts/2026-09-23-native-assessment-memory.md` |
| the ear: whisper-tiny as the body's own pass | `ear-native-band` 32767 | `receipts/2026-09-09-the-ear-could-not-hear-the-house.md` |
| the mouth: a whole VITS voice, phoneme ids to samples | `voice-pass-band` 4095 | `receipts/2026-09-09-the-shape-was-right-and-the-sound-was-empty.md` |

`observe/floor-lens-run.fk` reads what the hardware gives each lane (bandwidth and arithmetic, three
reads with their spread) — a reading worth taking on a quiet machine.

## The knowledge lane

```text
form-knowledge-integration-census-band     -> 1048575
form-knowledge-source-search-band          -> 262143
form-knowledge-qwen-heldout-v3-eval-band   -> 65511   (declares 65535; bits 8 and 16 open: every
                                                       row current against its source sha, and the
                                                       dataset sha equal to its seal)
native-model-route-table-band 255 · ds4-blob-select-band 31 · nl-lexicon-grow-band 127
pivot-coverage-band 65535 · cognition/tests/error-absorption-kernel-band 4095
```

The sealed v3 held-out lane (30 rows, every family twice, whole-normalized answers) is the body's
defined-correctness number, re-earned only through that sealed door. Its consent file
(`.form-knowledge-qwen-heldout-v3-consent`) is a per-run local act that git ignores. The model route is
a Form data table, and the DS4 engine is found at runtime through its directory with its header
verified.

## The senses

What the body hears, says and perceives, read by bands that open no microphone, speaker or model:

```text
ear-heard-tongues-band   -> 63      the tongues the ear listens for are the tongues the glass offers
whisper-shape-band       -> 127     whisper's dimensions read from the model file, npz or safetensors
ear-ground-band          -> 32767   every delay in the live path judged physics or furniture, as data
perception-symbols-band  -> 8191    the room, the ear and the voice's manner as addressed symbols
perception-rows-band     -> 65535   a perceived day folded into trainable rows; words travel only
                                    from a frame naming its speaker and an organ of this body
own-word-band            -> 65535   a sentence about the room stands only when the record backs every
                                    claim and it makes at least one
voice-say-band           -> 16383   one mouth per tongue the ear renders, the map proven in silence
host-doors-band          -> 131071  host_spawn_at, host_alive and fs_mkfifo: the ear's lanes stand
                                    with no shell
```

`own-word-band` proves that a claim contradicting the record is set aside; a real room has not yet
offered one. The live doors are `observe/own-word-run.fk`, `observe/say-run.fk` (stdin line
1 the tongue, `?` lists the mouths) and `observe/perception-say-run.fk`.

## The Glass

One persistent process paints a retained terminal frame from the body's own observation:
`./fkwu observe/form-glass-run.fk`. Views `a t o m f j s k v n d`, `h` for help; `r` focuses the room;
`1`–`4` and `0` choose tongues; `l i e c q g y w` act (`d` opens the body's choice points, `g` steps,
`y` takes, `w` holds); `z` puts the ear to sleep or wakes it. The ear stands awake with the glass unless
`.hearth/ear.slept` stands, and every frame opens with the ear's own state.

Nothing on a glass path forks, scans a directory or parses a tool's output. Telemetry crosses as gift
frames — POSIX shared memory with a seqlock header — and a value crosses as itself (`node_gift_write` /
`node_gift_read`). Every `fkwu` writes its own live page (`/fg-k<pid>`: dispatches, nodes, strings,
heat, boxes, CPU), so any process reads any kernel's hottest defns with their source. When the host
offers shared memory every kernel opens one field store, so a cell interned in two processes is one
cell with one word; `observe/field-reset-run.fk` starts the field over when no other kernel is alive.
A frame standing is not a giver giving: a frame not given for three of its cadences reads silent, and
`observe/body-vitals-live.fk` watches every surface's own rhythm and effort.

```text
form-glass-live-band 2147483647 · form-glass-observer-band 67108863 · form-glass-dashboard-band 16777215
form-glass-event-loop-band 16777215 · form-glass-staged-startup-band 262143 · form-glass-launch-band 131071
form-glass-gift-frame-band 4095 · form-glass-sensor-rows-band 2047 · form-glass-kernel-view-band 511
form-glass-events-channels-band 32767 · form-glass-telemetry-membrane-band 2097151
form-glass-wait-band 255 · form-glass-machine-band 511 · form-glass-frame-work-band 32767
form-glass-vitals-band 1023 · form-glass-standing-band 63 · form-glass-crossings-band 16383
form-choice-flow-band 16383 · sense-discernment-band 1023 · node-gift-band 4095
cell-store-band 255 · field-band 255 · kernel-census-band 2047
```

## The string floor

`substring(s, start, end)` answers the bytes of `s` from `max(start, 0)` up to `min(end, str_len(s))`;
`""` when that range is empty or reversed, or when `s` is not a string. `str_find(h, n, from)` answers
the byte index of the first occurrence of `n` at or after `max(from, 0)`, or -1; an empty needle
answers `max(from, 0)`; a start past `str_len(h)` answers -1. Both always answer, neither moves an
offset to a character start, and both are natives on all four arms with their recipes kept in `core.fk` as the body's statement of
what they mean ([`docs/str-find-one-meaning.md`](docs/str-find-one-meaning.md)).

```text
substring-one-meaning-band 4095 · str-find-one-meaning-band 8191     (both drift-gate rows)
core-substring-equivalence-band 2047 · substring-native-band 511
line-grammar-search-equivalence-band 8191 · core-str-find-equivalence-band 2047
meaning-codes-table-band 255
```

## The body's own lenses

The body reads itself: which doors bear the most walking, which door is a private copy of a warm one,
which of a native's mirrors still stand, where a definition is written twice, and which written limit
carries a witness.

```text
bearing-census-band 32767   ./fkwu observe/bearing-census-run.fk   (steps, not milliseconds)
twin-census-band 65535      ./fkwu observe/twin-census-run.fk
mirror-census-band 65535    ./fkwu observe/mirror-census-run.fk
copy-census-band 63         ./fkwu observe/copy-census-run.fk
wall-census-band 63         ./fkwu observe/wall-census-run.fk
observe/tests/voice-frequency-band 255 · number-band 255 · float-printer-fourway-band 31
```

## The JIT

A pure-float defn crystallizes into an arm64 f64 leaf when its box ledger crosses a boundary
(`fk_f64_pulse`); a self-tail-call loop crystallizes on heat (`fk_heat_pulse`). `form-lower.fk`
carries runtime strings through the two-slot `fk_inram_args` convention.

```text
jit-lens-band 16383 · jit-heat-gate-band 4095 · observe/tests/jit-evaluator-heat-band 4095
form-lower-string-band 63 · form-lower-string-runtime-band 255 · form-lower-string-both-runtime-band 511
float-natives-band 28 · persistence-band 7 · channel-breath-band 500 · eq-shape-band 524287
blueprint-authority-band 65535
```

## Not standing today

What answered red, died, or was not witnessed today, so no one leans on it:

- `form/form-stdlib/tests/primitive-registry-band.fk` stops rc 1 in `sum-onto`: the registry names
  the Go sibling's native surface, and 84 of its calls (`field_*`, `substrate_*`, `register_jit`,
  `string_bytes`, `pow`, `min`, `max`, ...) have no binding on fkwu, so its compile carries 84
  unresolved-call errors and the first recovered `nothing` meets arithmetic. It waits on a registry
  grounded in fkwu's own op table (`runtime/fkwu-optable.h` and `core.fk`), or on a sibling-home lane
  in `form/validate.sh` for a band whose surface only the siblings carry.
- The domain/organ/unique/universe-mint bands answer 2015 of 2047, pending rather than red: bit 32
  claims heldout ≥ 9 on a lesson-disjoint split, which a lesson-bound overlay reaches only once a
  LoRA writer stands (the mints declare `LoraWriter = 0`) (R88).
- BML `import Num;` binds nothing (`bml-import-ref-resolution-band` 2111; R78). The lowering's other
  open row stands in the ledger: R96 (lowering time grows with one form's argument count).
- Windows: the seed's `_WIN32` branch carries its own spawn and wait twins (`fk_win_spawn`,
  `fk_win_waitpid`), its fifo door answers -1 there, and it passes
  `clang --target=x86_64-w64-windows-gnu -fsyntax-only runtime/fkwu-uni.c` with 0 errors (rc 0,
  today). A link and a run wait on a Windows toolchain or host, and neither is on this Mac.
- The GPU and model lanes above were not re-run today; their verdicts are as old as their receipts.
