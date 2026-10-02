# Current Floor

What stands in this body today, read on 2026-09-29 (WITA) on this Apple M4 Max through `./fkwu`, the
binary passing its freshness band; the lines that had moved were re-read on 2026-10-01 and say so. Every
line names the command that reads it. A verdict counts only
when the process also exits 0 — a green number over a nonzero exit is a fold over `nothing`. Bands ran
on the warm images beside their sources while sibling sessions loaded the host, so no timing on this
page was taken today; a number that comes from an earlier run names its receipt.

The organ map is [`MANIFEST.md`](MANIFEST.md), how to build and embody is [`AGENTS.md`](AGENTS.md), the
ground is [`axioms/core-axioms.form`](axioms/core-axioms.form), and the direction is
[`docs/local-agent-goal.form`](docs/local-agent-goal.form). How we got here lives in git and `receipts/`.

## Grounding

Build lines live in [`AGENTS.md`](AGENTS.md) (`cc -O2 -o fkwu runtime/fkwu-uni.c`, and the Metal
carrier dylib fkwu admits in the same process). A fresh clone holds no `./fkwu`, no `.fkb`/`.sym`
caches and no `form/form-cli`: `cc -O2 -o fkwu runtime/fkwu-uni.c` makes the runtime, the caches
appear beside each source on its first run, and `form/build-form-cli.sh` regenerates
`form/form-stdlib/bootstrap/` and links the launcher.

```text
./fkwu bootstrap/ground.fk                                -> 42
./fkwu bootstrap/ground-recursive.fk 10                   -> 55
./fkwu form/form-stdlib/tests/binary-freshness-band.fk    -> 31
./fkwu bootstrap/ground-numeric-list.fk                   -> [1, 2.5, [3, 4]]
./fkwu form/form-stdlib/tests/native-vs-rented-band.fk    -> 11111
./fkwu proof/four-way-run-recipe42.fk                     -> 0 FOUR-WAY with the walkers built, 2 WALKER-SUSPECT while they are not
```

The four-way cell host-execs the three minimal proof walkers, which `walkers/README.md`'s own lines
build; a fresh checkout holds them unbuilt and reads 2 (re-run 2026-10-01 on such a checkout).

`runtime/fkwu-uni.c` is the seed (`wc -l` and `git log -1` read its size and its last change), and it
shrinks as its lanes lower into Form organs (`release-ledger.bml` R13, stones R51–R56).

## Body-wide witnesses

```text
./fkwu gate/drift-gates-run.bml          -> drift-gates pass=<fold> full=<mask> refused=0 (18 rows; a row whose ground did not
                                            move since origin/main sits out and leaves the fold; 2026-10-02 with the
                                            kernel-laws row: 7 of 18 rows ran, pass=127 full=127 refused=0)
./fkwu gate/structural-gate-run.fk       -> structural-gate-v3 [8, 0, 1, 0, 0, 0, 7] then 1
                                            (total/unclassified/carrier/oracle/fixture/
                                            proof-sibling/tooling)
gate/tests/structural-gate-band          -> 16383
./fkwu observe/door-link-health-run.bml  -> docs=39 claims=817 broken=0 (each broken claim named on its own line; 2026-10-02)
                                            then prelude-reach missing=0 untracked=0 shadow=0
                                            (every name a cell loads reaches one tracked file)
./fkwu observe/band-truth-run.bml        -> bands=345 readable=167 unreadable=178 absent=0 seen=1 flaws=0, exit 0
                                            (each band's declared full read against the most its claims sum to, and
                                            the queue and manifest copies of a full against the head pin, with no
                                            band run, in about 0.3 s; a decimal or count fold that stands on purpose
                                            says `; FOLD: decimal|count` on a head line; a sweep that lists no band
                                            is a flaw too; 2026-10-02)
form/form-stdlib/tests/band-truth-band   -> 1048575
./fkwu observe/belief-stamps.bml         -> 70065000  (field stamped*10^6 + owed*10^3 + laws; 2026-10-01)
observe/tests/belief-rewitness-band      -> 63
./fkwu form/form-stdlib/release-ledger.bml -> open=22 moving=0, then 22000000 (2026-10-01)
learn/tests/homecoming-distillation-corpus-band -> 32767
value-eq-arena-band 31 · import-carry-band 255 · form-cli-author-high-band 4095
closure-lines-band 31 · sort-band 31   (the queue's two gaps, written by the local lane; 2026-10-01)
memory-governor-band 67108863 (form/form-stdlib/bml/memory-governor.bml asks the whole machine, not one process, before a
                                Qwen session opens, a renewal allocates a second KV state, a walk turn begins or the planner
                                opens its voice; the weights themselves are one physical copy in the page cache for every
                                kernel that maps the file, measured 2026-10-02; a grant is a lease other processes see
                                (/private/tmp/form-memory-leases/<pid>.lease, one lock a slow holder keeps, the same weights file
                                counted once, pending bytes counted for 180 s with a renewal's second KV state stamped on its own
                                and given back, rows range-checked, a dead pid's file removed, an unmakeable directory guarded by
                                one reading); each defect planted and read below its head; observe/memory-governor-run.bml prints
                                the reading, the live leases and a lease-aware verdict)
form-cli-code-low-memory-band 1023  (a running coding lane saves its checkpoint and ends with host-memory-low when the machine
                                runs low, continues on a roomy or unreadable reading, and a lane that spent its turns keeps that
                                ending; the walk reads it as a choice point)
native-turn-ladder-band 262143 · local-flow-reading-band 4194303  (a memory ending, host-memory-low or host-memory-held:*, is no
                                tried approach and no option of the review; its checkpoint is the resume's target, with no failure text)
                                (the review's phase fold is standing: every option whose lane output this host holds, in its own
                                checkout or the walk's, is folded once into the ledger row with its nine phases, each one's share of
                                the wall, its renewals and the prompt IDs they re-fed; a lane killed at its deadline writes no counts
                                and its counts are folded from its stamps and said so; the day option reproduces to the ms)
host-walk-band 8589934591     (the walk's turn waits for memory within the window and a held answer takes no turn;
                                host-walk.bml is an aggregator over nine parts `host-walk-<seam>.bml`, each under 12000 bytes so the
                                local lane reads one whole and can read it twice, and every definition's name (not its body) is pinned
                                in tests/fixtures/host-walk-defs.txt)
form-cli-landing-band 16383   (the landing and the walk read every child's exit from `host_wait` through
                                `host-child.bml` (`hch-run`), never from a printed mark; a red witness holds the landing)
host-os-membrane-band 8191 · bidirectional-framebuffer-channel-band final field 1
grammars/tests/form-eval-band 65535 · form-eval-full-band 635 · source-compiler-grammar-bridge-band 32767
pattern-match-band 511 · choice-lane-core-band 1023 · backtrack-band 255 · offer-ack-core-band 32767
control/tests/attempt-band 4095 · file-bytes-band 127   (no PROOF LEVEL line: four-way when the siblings run)
form-bml-cursor-full-band 105   (four-way: it declares no PROOF LEVEL, and Go, Rust, TypeScript and fkwu agree)
control-invite-grammar-band 1023 · cell-serialize-band 1023 · json-band 1023 · wire-rpc-band 15
form/form-stdlib/tests/primitive-registry-band.fk 47   (2026-10-01, rc 0)
form/form-stdlib/tests/form-agent-tools-band.bml 524287 (2026-10-01: the resident agent tools over their JSON wire)
form/form-stdlib/tests/findings-requests-band.bml 1048575 · form/form-stdlib/tests/form-cli-local-plan-band.bml 33554431
                                            (2026-10-02, rc 0: the feeder reads whether the code a request cites moved since its
                                            review, git hunks plus the closing gate and no model, and keeps the definition the
                                            change landed in; the planner proposes a request moot only when the voice's band names
                                            that definition and reads full today and below full under both stubs, and the row reads
                                            moot only while its fixborn stands; {"check":1} runs a moot row's kept band again; the
                                            dry planner's first pick is a standing request:
                                            echo '{"next":1,"dry":1}' | ./fkwu observe/local-plan-run.bml)
```

Every tracked cell's `witnessed:` stamp is read into the belief lens, oldest first; the re-witness door
(`observe/belief-rewitness.bml`) renews a stamp only from a fresh band run and reports a mismatch as a
lapse. The release ledger's open rows are the body's named work, each with its witness.

## The BML floor

A unit lowers by what it carries: any file with a `section [` block — `form.bml`, `form.lift`,
`form.action`, `form.route`, the `*.bmf` grammar dialects — travels through `bml-floor-compile`
whatever its extension, as a prelude or as the main file, and fkwu keeps the `.lowfk`/`.fkb` cache
beside it. Of 594 tracked `.bml` files, 528 carry a `section [form.bml]` block and three carry
`section [form.lift]` (`git ls-files '*.bml' | xargs command grep -a -l '^section \[form.bml\]' | wc -l`,
read on the tree that carries this line, 2026-10-02; the counts grow with each new unit). `true` and `false` are literals in the
dialect, and a nested `defn` is a registered function (the two nested-defn bands below).

The cursor (`grammars/form-bml.fk`, lowered by `form-bml-lower.fk`) is the compiler's one reader of a
`form.bml`, `form.route` or `form.action` section — the body, its imports' signature catalog and its
refusals all come through it. It reads a section whole, with direct backtracking, and rebuilds it with
the compiler's own constructors, so a def lowers to the very node its flat Form spelling builds:

```text
./fkwu observe/bml-cursor-coverage-run.bml -> files=1443 sections=1088 read=1088 stops=0 refused=0
                                              (read = sections: every `^section [form.bml|route|action]` line in
                                              the `.bml` and `.fk` files under the root, outside .git, .hearth,
                                              .claude, .cache, node_modules, target and dist; the counts grow
                                              with each new unit; read on the tree that carries this line,
                                              2026-10-02, in 152 s)
                                              a section it refuses is named by line, word and wanted rule
form-bml-cursor-full-band 105
```

The five grammar packs (`form/form-stdlib/grammars/{go,prolog,python,rust,typescript}-bmf.fk`) carry their
`import` lines above their `section [form.bml]`: the cursor reads `import` as a top-level statement,
never inside a section.

```text
bml-band 268435455 · bml-generics-band 16777215 · native-route-goal-cells-band.bml 1048575
nested-defn-scope-band 63 · nested-defn-closure-capture-band 63
bml-float-literal-band 2047 · bml-form-size-band 127 · cell-channel-band 4095
json-codec-bml-band 8191 · kernel-http-band 536965066 · channel-flow-band 8388607
circle-band 1048575 · static-to-dynamic-cells-band 262143 · bml-capability-ledger-band 255
form-pe-coff-band 16383 · learn/tests/choice-receipt-band.bml 4294967295
language-packs-fourth-band 31 (2026-10-01) · bml-bmf-control-curriculum-band 1048575
bml-bmf-stream-curriculum-band 16777215 · form-cli-lens-mint-band 1023
```

Native source lifting is available through `form-source-lift(source, owner)` in
`form/form-stdlib/bml/form-source-lift.bml`. It checks BML candidates against the
original lowered tree before publication, preserving nested scope and literal
bytes. Existing BML is scanned through the BMF cursor and expression grammar;
surface-only migrations can compare exact recipes before imported type resolution.
Its contract witness is `form/form-stdlib/tests/form-source-lift-band.bml`
(35 accepted cases, 3 declined shadowing/binding changes, preserved comparison metadata; 2026-10-01). [Native coding](docs/form-native-coding.md#native-surface-lifting)
describes the callable surface and its scope.

## The mind and its voice

The voice speaks on this Mac's own metal: Qwen3.8-27B Q8_0 walked as Form recipe-data in the fkwu
session, every Metal pipeline Form-emitted and JIT-compiled at runtime, the geometry read from the
sealed GGUF header. The body takes engineering turns on its own through
`observe/native-turn-run.bml`, begun by launchd or by a session as each row's `coordinated_by` records,
and each option writes one row to `receipts/native-turn-ledger.jsonl`. `./fkwu
observe/local-flow-review.bml` reads that ledger live: the options, the gaps gone green and the gaps
still open.

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

The Llama-3.2-3B voice has an adapter for speaking Form in its own stream, `form/form-stdlib/adapters/llama-3.2-3b-form-tokens`:
55 of 64 held-out answers exact (53 of 62 once the two held-out rows that repeated a train question are set
aside; they train now) against plain 25 and taught-by-prompt 5 on the bare model, which is limited by the
continuation seam and does not isolate the training; the Qwen3.8 opening fit's separation 10.0 over 2.2 stands on
n=7 held-out sites and predates the filter that keeps held-out rows out of its sites. What it shows and does not:
`docs/native-model-control-plane.md`, "The Form-token adapter". Bands: `form-token-traces-band` 127,
`form-token-lora-band` 32767, `form-token-eval-band` 8191, `form-token-census-band` 4095.

The controls a voice writes in stream (`form/form-stdlib/bml/form-token-grammar.bml`: nine verbs, `recipe-import`
the newest) run in one trusted living workspace (`form-token-workspace.bml`): what a control makes persists in a
local, request, lineage or global scope, eval reaches every door the runtime carries and every function the body
defines, and each control answers within its deadline. Bands: `form-token-verbs-band` 4095,
`form-token-workspace-band` 4095, `form-token-grammar-band` 1023. The trace corpus is 596 rows in sixteen families.

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

The lanes that open a model or hold the GPU. Each band declares its verdict in its header; the last
run of each lives in its receipt. Re-read on this checkout today: `dense-multi-band` 1023,
`native-lora-resume-band` 7, `ear-native-band` 32767, `voice-pass-band` 4095. The three `vk-*` bands
read 0 here: their staged carrier `.hearth/vk/run_vk` is a compiled program this host does not hold
(`./fkwu observe/vk-carrier-build.bml` names it missing).

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

`form/form-stdlib/floor-lens.bml` is the lens arithmetic for what the hardware gives each lane
(bandwidth and arithmetic, reads with their spread); a hardware reading is worth taking on a quiet
machine.

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

## The Form-native DeepSeek V4 chain

Read on 2026-10-02 (WITA) through `./fkwu form/form-stdlib/tests/<band> </dev/null`, the memory governor
reading room before each run, every exit 0. No band maps the model: the ones that read the registry's file
(the reap25 build) read its header, an 8 MB window, and the recorded logits are 1.5 MB. What stands is the chain
restored model-free: the tokenizer, the quant and tensor readers, the emitters, the reference arithmetic and the
emission door. What does not stand: the physical driver that would run the stack through Metal over the mapped
weights is not in the tree (its design is in the lead's notes, to be built as dsv4-token-handle under
form/native/metal); the Form-native lane is not wired (`model-registry.bml` keeps the
`deepseek4` lane row at `ds4-query`, off); the control-plane rows `challenger.deepseek-ds4-metal` and
`oracle.deepseek-ds4-imatrix` are unchanged; and the validation window is pending: one window, alone,
governor-checked, against the recorded references earned on the IQ2XXS file.

```text
tokenizer      dsv4-tokenizer-band 8191 · dsv4-tokenizer-native-band 15 · dsv4-token-recipe-swap-band 65535
quant, tensor  ds4-quant-layout-band 127 · ds4-tensor-table-band 255 · f16-decode-band 4095 · gguf-manifest-band 255
               iq2xxs-dequant-band 1073741823 · iq2xxs-msl-band 8191 · mx-plane-band 511 · mx-msl-band 511
               q2k-dequant-band 511 · q2k-msl-band 255 · q8-0-msl-band 255 · windowed-residency-band 4095
reference      dsv4-oracle-recipes-band.bml 65535   (the reference arithmetic against the body's carvers, the view
               geometry, the request admission, the header-only plan of 1,406 tensors, the retention and fp64 matrix
               doors, the one minting door of the native organs' owner ids, the pipe owner's flow key)
emitters       dsv4-compressor-band 2047 · dsv4-kv-cache-band 511 · dsv4-hc-band 63 · dsv4-hc-msl-band 63
               dsv4-moe-msl-band 63 · dsv4-forward-band 127 · mla-attn-band 63 · mla-msl-band 127
               moe-msl-band 511 · moe-route-radius-band 63 · moe-route-wide-msl-band 255
stack          dsv4-proof-emission-band.bml 127   (eleven streams, 85 kernels compiled on the device, two run)
               ds4-order-match-band.bml 255       (the quantiser and the Q8_0 row dot against a Form reference of ds4's order)
references     ds4-recorded-references-band.bml 1023   (the logits pinned by a SHA-256 of the whole file, by the body's own door)
```

Bands that read a binary fixture through `read_file_slice` or a host door declare `PROOF LEVEL: FOURTH-ARM ONLY`
(rust and ts hand back a different file there); `./validate.sh --list` prints which. The restored emitters, the
`*-real.fk` cells, `dsv4-token.fk` and the windowed residency emitter say in their own headers whether a band proves
them or they are restored for the spec and not run physically. The reference evidence
`docs/evidence/fkwu/dsv4-oracle.json` is the reading of 2026-09-11 and is left as observed; the sidecar
`docs/evidence/fkwu/dsv4-oracle-reread.json` names which of its pinned identities still match.

The recorded references are `form/form-stdlib/tests/oracles/ds4-logits-capital-of-france.json` (129,280 logits, argmax
2581 "We" at 36.7579117) and the 24-token stream for the raw prompt ids [671 6102 294 8760 344] (a period-7 cycle,
" Paris. The capital of France is"). They were earned on the IQ2XXS file, which ds4 can read; the registry's `ds4flash`
row names the reap25 file, which it cannot, so they judge a native lane on the IQ2XXS file and a reap25 lane is judged by
the Form reference. The radius is one prompt, a cycle, never past position 127. The kernels the order-match band runs
carry `#pragma clang fp reassociate(off)` and `contract(off)`: `metal_pipeline` compiles with the default options, and
without them the eight-thread Q8_0 twin read 8 ulps from the one-thread kernel.

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
offered one. The cells are `form/form-stdlib/own-word.bml` and `form/form-stdlib/perception-rows.bml`;
the doors are `observe/say.fk`, `observe/voice-mouth-lanes-run.fk` and `observe/voice-pass-run.fk`.

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
form-glass-gift-frame-band 4095 · form-glass-sensor-rows-band 2047 · form-glass-machine-band 511
form-glass-telemetry-membrane-band 2097151 · form-glass-vitals-band 1023
form-glass-standing-band 63 · form-glass-crossings-band 16383
form-glass-events-channels-band 32767 · form-glass-wait-band 255 · form-glass-kernel-view-band 511
form-choice-flow-band 16383 · sense-discernment-band 1023 · node-gift-band 4095
cell-store-band 255 · field-band 255 · kernel-census-band 2047
```

The events-channels, wait and kernel-view bands hold live children and timed rests: they read full
one at a time on a quiet host (`/tmp/form-glass-organs-band` is a path every run shares) and low under
load. The reading that counts is one taken on a quiet machine.

## The string floor

`substring(s, start, end)` answers the bytes of `s` from `max(start, 0)` up to `min(end, str_len(s))`;
`""` when that range is empty or reversed, or when `s` is not a string. `str_find(h, n, from)` answers
the byte index of the first occurrence of `n` at or after `max(from, 0)`, or -1; an empty needle
answers `max(from, 0)`; a start past `str_len(h)` answers -1. Both always answer, neither moves an
offset to a character start, and both are natives on all four arms with their recipes kept in `core.fk` as the body's statement of
what they mean ([`docs/str-find-one-meaning.md`](docs/str-find-one-meaning.md)).

```text
substring-one-meaning-band 4095 · str-find-one-meaning-band 8191     (both drift-gate rows)
kernel-laws-band 2147483647     laws 1-9 of docs/kernel-interface.md, four-way (a drift-gate row)
core-substring-equivalence-band 2047 · substring-native-band 511
line-grammar-search-equivalence-band 8191 · core-str-find-equivalence-band 2047
meaning-codes-table-band 255
```

## The body's own lenses

The body reads itself: which doors bear the most walking, which door is a private copy of a warm one,
which of a native's mirrors still stand, where a definition is written twice, and which written limit
carries a witness.

```text
bearing-census-band 32767   form/form-stdlib/bearing-census.bml    (steps, not milliseconds)
twin-census-band 65535      form/form-stdlib/twin-census.bml
mirror-census-band 65535    form/form-stdlib/mirror-census.bml
copy-census-band 63         form/form-stdlib/bml/copy-census.bml
wall-census-band 63         form/form-stdlib/bml/wall-census.bml
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

- `form-source-lift` over a mixed module whose first plain form carries a `; preludes:` header writes the
  lifted `import` inside the first section, and its own verification compile stops: `the cursor does not
  read this form.bml section: line 3: import (wanted: topstmt)` (2026-10-01). The five grammar packs it
  lifted carry their imports above the section by hand; `form-source-lift-band` holds no such case yet.
- `form-cli-allowance-band`, `form-cli-live-band` and `native-tensor-lifecycle-band` reach the Metal
  door and were not re-run in the gates pass (it held no GPU). The integrated review's sweep
  (2026-10-01) read form-cli-allowance 2047 on all four arms, and form-cli-live 255 and
  native-tensor-lifecycle 1023 on fkwu; those two now declare FOURTH-ARM ONLY, since no sibling carries
  a `metal_*` door.
- `form-glass-wait-band` read 246 of 255 on the fkwu-only lane (2026-10-01, several sweeps loading the
  host): it landed 12 of 20 rests inside half a millisecond and its watched frame did not wake it. A
  quiet machine is the reading that counts.
- BML `import Num;` binds nothing (`bml-import-ref-resolution-band` 2111; R78). The lowering's other
  open row stands in the ledger: R96 (lowering time grows with one form's argument count).
- Windows: the seed's `_WIN32` branch carries its own spawn and wait twins (`fk_win_spawn`,
  `fk_win_waitpid`), its fifo door answers -1 there, and it passes
  `clang --target=x86_64-w64-windows-gnu -fsyntax-only runtime/fkwu-uni.c` with 0 errors (rc 0,
  today). A link and a run wait on a Windows toolchain or host, and neither is on this Mac.
- `form-glass-frame-work-band` reads 20387 of 32767: its child
  (`form/form-stdlib/tests/form-glass-frame-work-child.fk`) stops rc 1 after its first frame with
  `len: nothing has no length … in fgsr-publication-stamps` (`form/form-stdlib/form-glass-sensor-rows.bml`),
  so a held frame carried a rows cell that read nothing. The child reads other processes' producer
  frames, and a live glass from another checkout was running; `fgsr-take` over all sixteen sensor names
  answered no nothing rows, so the frame that carries it is not yet found.
- The GPU and model lanes not named above were not re-run today; their verdicts are as old as their
  receipts.
