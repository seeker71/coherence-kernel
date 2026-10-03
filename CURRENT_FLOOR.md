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
./fkwu gate/canonical-conformance-run.bml                 -> 1   (13 canonical expressions on fkwu, the pinned FORMBIN2
                                                                 artifact, 12 malformed artifacts; band 511)
```

fkwu is the only runtime and every band answers its pin on it. Nothing in this tree builds, runs or
gates on Go, Rust, TypeScript or Swift: the regeneration of `form/form-stdlib/bootstrap/` needs `cc`,
`./fkwu`, `shasum` and `openssl`.

`runtime/fkwu-uni.c` is the seed (`wc -l` and `git log -1` read its size and its last change), and it
shrinks as its lanes lower into Form organs (`release-ledger.bml` R13, stones R51–R56).

## Body-wide witnesses

```text
./fkwu gate/drift-gates-run.bml          -> drift-gates pass=<fold> full=<mask> refused=0 (15 rows; a row whose ground did not
                                            move since origin/main sits out and leaves the fold)
./fkwu gate/structural-gate-run.fk       -> structural-gate-v4 [7, 0, 1, 0, 0, 6] then 1
                                            (total/unclassified/carrier/oracle/fixture/tooling)
gate/tests/structural-gate-band          -> 8191
./fkwu observe/door-link-health-run.bml  -> docs=41 claims=878 broken=0 (each broken claim named on its own line; 2026-10-03)
                                            then prelude-reach missing=0 untracked=0 shadow=0
                                            (every name a cell loads reaches one tracked file)
./fkwu observe/band-truth-run.bml        -> bands=371 readable=191 unreadable=180 absent=0 seen=1 flaws=0, exit 0
                                            (each band's declared full read against the most its claims sum to, and
                                            the queue and manifest copies of a full against the head pin, with no
                                            band run, in about 0.3 s; a decimal or count fold that stands on purpose
                                            says `; FOLD: decimal|count` on a head line; a sweep that lists no band
                                            is a flaw too; 2026-10-03)
form/form-stdlib/tests/band-truth-band   -> 1048575
./fkwu observe/forbidden-tools-guard.bml -> the PreToolUse door that `.claude/settings.json` registers for every Bash and Monitor call (the two
                                            tools whose input carries a shell command in `command`): the hook's JSON on stdin; a command
                                            that runs sed, awk, perl or python gets one line of deny JSON naming the word and what to use,
                                            anything else (another tool, a reading that did not arrive, a command over 200000 bytes) gets no
                                            byte; exit 0 both ways. The registered one-liner ends 0, passes a line on only when it opens as
                                            the deny JSON, and prints nothing when fkwu or the door is absent (2026-10-03, load average 8: a
                                            warm run 20-25 ms, the registered line 28 ms, the first run of a checkout 2.4 s while the .fkb
                                            cache is made; to switch it off delete the entry in .claude/settings.json). On a read-only checkout
                                            the cache cannot be written: the review saw the door exit 1 with a stale-cache warning on stderr
                                            each call, which the line turns into silence, so nothing is refused there
form/form-stdlib/tests/command-words-band -> 2147483647  (form/form-stdlib/bml/command-words.bml reads which commands a shell string runs, with
                                            no shell: quotes, separators, redirections, heredoc bodies, wrappers, sh -c / eval / trap / find
                                            -exec, substitutions, case arms, [[ ]] and (( )), function bodies, ANSI-C escapes, and the
                                            stream a shell is fed (bash <<'EOF', bash <<< "..", echo ".." | bash, source <(..)). Against the
                                            3813 distinct Bash commands of one session's transcript, read again after the streams, case,
                                            condition and function work: 318 read as running sed or awk, each confirmed by reading it, the
                                            same 318, and 3495 read clean, of which 12 name a tool only as data and 1 writes a script that
                                            runs sed later; four commands that read `<unknown>` (arithmetic and [[ ]] words) now read clean.
                                            Eight plants (no quotes, wrappers, heredoc skipping, -c recursion, basenames, streams, case
                                            patterns, conditions) each drop their claim. It cannot see an alias, a variable that holds the
                                            tool (`$T file` reads <unknown>, never refused: `$cmd x` is an ordinary zsh form here), a script
                                            file that runs sed inside, a runner such as uv run python, a pipe from a producer that is no
                                            literal (cat f | bash), the substitutions in an unquoted heredoc body, or a wrapper its list lacks;
                                            a reading nests at most 256 levels and then answers <too-deep>)
form/form-stdlib/tests/forbidden-tools-guard-band -> 4095  (the deny line parses as JSON with its keys; silence on allow, on malformed
                                            or cut-off stdin, on another tool, on a 240 KB command and on 300 nested substitutions; Monitor is
                                            read like Bash; the registered line from another directory denies sed, passes only the deny JSON
                                            and fails open on a missing project, door or fkwu)
./fkwu observe/belief-stamps.bml         -> 70065000  (field stamped*10^6 + owed*10^3 + laws; 2026-10-01)
observe/tests/belief-rewitness-band      -> 63
./fkwu form/form-stdlib/release-ledger.bml -> open=19 moving=0, then 19000000 (2026-10-03)
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
host-walk-band 1099511627775  (the walk's turn waits for memory within the window and a held answer takes no turn;
                                the sync settles what the body knows through the reunion's rounds: a drawn page takes main's side
                                and is remade (in its directory) into one new commit, a river of at_unix_ms rows with both banks
                                standing is joined by union, each row once, and the sync says `healed:` before sync=0; a branch
                                of any length heals, only a round that comes back with the rebase no further on stops; a file
                                the body does not know, a river one side deleted, or a page it could not settle aborts the
                                rebase and names its files;
                                a walk runs the host-walk parts of its own checkout, so a checkout that stands behind main on a
                                conflict the landed sync heals cannot heal itself (2026-10-03 12:30: the day turn held every turn
                                on docs/rent-ladder.html): observe/walk-sync-run.bml runs the landed sync once on a named checkout
                                ({"dir":"/abs/path"}), leaves a checkout whose walk lock stands alone and names the owner; run on the
                                walk checkout at 14:30 it printed healed:, redrawn:, sync=0;
                                host-walk.bml is an aggregator over ten parts `host-walk-<seam>.bml`, each under 12000 bytes so the
                                local lane reads one whole and can read it twice, and every definition's name (not its body) is pinned
                                in tests/fixtures/host-walk-defs.txt)
form-cli-landing-band 262143  (the landing and the walk read every child's exit from `host_wait` through
                                `host-child.bml` (`hch-run`), never from a printed mark; a red witness holds the landing; the
                                reunion after a push behind origin is argv in the checkout it joins and is read end to end in a
                                scratch origin: joined, refused with what git said, the side taken, and its default wiring)
host-os-membrane-band 8191 · bidirectional-framebuffer-channel-band final field 1
grammars/tests/form-eval-band 65535 · form-eval-full-band 635 · source-compiler-grammar-bridge-band 32767
pattern-match-band 511 · choice-lane-core-band 1023 · backtrack-band 255 · offer-ack-core-band 32767
control/tests/attempt-band 4095 · file-bytes-band 127
form-bml-cursor-full-band 105
control-invite-grammar-band 1023 · cell-serialize-band 1023 · json-band 1023 · wire-rpc-band 15
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
language-packs-band 31 (2026-10-01) · bml-bmf-control-curriculum-band 1048575
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
verified. One sealed row names a source that is gone: v1 row h11 (family `proof`) pins
`proof/four-way-run-recipe42.fk`, which no longer exists. Its replacement and the dataset
re-seal belong to the sealed door, after its fact is read; the v3 band reads 65511 before and after.

## The Form-native DeepSeek V4 chain

Read on 2026-10-02 (WITA) through `./fkwu form/form-stdlib/tests/<band> </dev/null`, the memory governor
reading room before each run, every exit 0. No band maps the model: the ones that read the registry's file
(the reap25 build) read its header, an 8 MB window, and the recorded logits are 1.5 MB. What stands is the chain
restored model-free: the tokenizer, the quant and tensor readers, the emitters, the reference arithmetic and the
emission door, and the driver (driver stages 1 to 4, below: the fixture, layout and bind on the CPU; the kernel set, the
open, one layer, the head, prefill and the greedy loop, the whole fixture model end to end on the device, all on the
Form-built fixtures, the second in the tensor types of the real IQ2XXS file; the session and generate doors, the derived
memory lease and the validation window written with the lane off). What does not stand: the driver over a real file's
mapped weights (every type of the IQ2XXS file is wired and proven on a fixture in those types, and its header reads no
gap through the validation door; no band maps the real file); the Form-native lane is not wired (`model-registry.bml` keeps the
`deepseek4` lane row at `ds4-query`, off, and `fcds-lane-wired?` in `form-cli-model-ds4.fk` is its twin, 0); the
control-plane rows `challenger.deepseek-ds4-metal` and `oracle.deepseek-ds4-imatrix` are unchanged; and the validation
window has not run: one window, alone, governor-checked, against the recorded references earned on the IQ2XXS file.

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
references     ds4-recorded-references-band.bml 8191   (the logits pinned by a SHA-256 of the whole file, by the body's own door; their 14 chat-templated prompt ids derived by the body's
                                                        template over the IQ2XXS header and held to the row beside them)
driver stage 1 dsv4-fixture-gguf-band.bml 511 · dsv4-layout-band.bml 1023 · native/metal/tests/dsv4-bind-band.bml 511
driver stage 2 dsv4-kernels-band.bml 16383 · native/metal/tests/dsv4-open-band.bml 1023 · native/metal/tests/dsv4-layer-band.bml 2047
driver stage 3 native/metal/tests/dsv4-token-band.bml 255 · native/metal/tests/dsv4-end-to-end-band.bml 511
               dsv4-lease-band.bml 511 · dsv4-door-band.bml 511 · dsv4-validate-band.bml 16383
driver stage 4 dsv4-fixture-q8q2-band.bml 255 · dsv4-kernels-q8q2-band.bml 1023 · native/metal/tests/dsv4-layer-q8q2-band.bml 4095
               native/metal/tests/dsv4-end-to-end-q8q2-band.bml 511   (stage 2 and 3 bands re-read: dsv4-kernels-band 16383 with 104 kernels, dsv4-open-band 1023 with 75 in the graph)
driver stage 5 native/metal/tests/dsv4-ledger-band.bml 1023 · native/metal/tests/dsv4-door-run-band.bml 127   (the op ledger of a token, whole-stack and arena ceilings, the flow through the door's functions)
               native/metal/tests/dsv4-token-run-band.bml 1023 · form-stdlib/tests/dsv4-token-run-plan-band.bml 63   (the token door's rows from the ledger, the tap, the plan from the header, the arena mode)
               native/metal/tests/dsv4-arena-band.bml 2047 · form-stdlib/tests/dsv4-expert-cache-band.bml 4095 · form-stdlib/tests/metal-buf-fill-band.bml 31   (the slot arena against the whole-stack control, the least-recently-used cache, the fill door)
```

Driver stage 1 (read 2026-10-03 WITA, each band alone, memory room before each run, every exit 0; each band's planted
defects read below its 511 and came back): `form/form-stdlib/dsv4-fixture-gguf.fk` writes a tiny DeepSeek-shaped
GGUF from the body's Park-Miller generator (3 layers, ratios 0 4 2, layer 0 hashed, 4 experts 2 used, n_embd 256, 86
tensors, 1,515,232 bytes, SHA-256 pinned) and the oracle's own admission accepts it; `form/form-stdlib/dsv4-layout.fk`
computes the widths, layer table, KV bytes a token and scratch sizes from a header alone, on the fixture and on both
real files (88,064 B a token over 43 layers; 1,202 driver tensors on each); `form/native/metal/dsv4-bind.fk` plans a
view per tensor and maps, reads, clips and releases the fixture's 86, while the real files' plans are computed from
their headers and map nothing. The reap25 header settles its layer count: `deepseek4.block_count` reads 43, blk.0 to
blk.42 are the layers, and 3 draft layers (`dspark.0` to `dspark.2`) follow in the table, so the control plane's 46
is 43 plus 3 (the comment beside the control-plane row is left as it stands). The fixture, layout and bind bands
need a binary fixture, host-local headers and a live Metal device, and are not rowed in
`form/band-verdicts.txt`. The driver's kernels must emit `#pragma clang fp reassociate(off)` and `contract(off)`
(each of the three cells says so in its header). Still ahead after stage 4: the physical window.

Driver stage 2 (read 2026-10-03 WITA on this Mac's GPU, each band alone, memory room and no model door resident before
each run, every exit 0; each band's planted defects dropped its reading and were restored): `form/form-stdlib/dsv4-kernels.fk`
composes the restored emitters and the kernels the graph still needed into ten Form-emitted units, every source opening with
the two fp pragmas (a*b+c on 1+2^-12 reads 973078528 with them and 973079552 without), 100 kernels compiled through
metal_pipeline once and each answering small inputs within the oracle harness's 3e-5 floor against the oracle's fp64
arithmetic on the fixture's own tensors (54 readings, 0 to 4.7e-7, plus the argmax line and the NaN-scan line); a NaN or an
infinity in a reading is a failure with its stage named, never a stop (`dkr-nd`, `dkr-nd-why`), and `form_dsv4_nan_count` with
`dkr-logits-clean?` counts the non-finite words of the logits by their exponent bits, which the driver must read as zero before
it trusts the argmax (the argmax skips a single NaN); `form/native/metal/dsv4-open.fk` opens the fixture as
handles in Qwen's context shape (86 views, 50 scratch buffers, 51 since stage 3 added the head's gate buffer so the logits are bs[28], a state of [kv, comp state, comp score, comp rows] a block) so
`q38-context-ok?`, `q38-state-ok?` and `q38-close` apply, counted through metal_live and released to 0 live in synced rounds,
and an open whose kernel the device refuses answers [] before anything is mapped;
`form/native/metal/dsv4-layer.fk` runs one block (hc_pre, MLA with the compressor, hc_post, the router by hash or biased
top-k, experts by device ids, the shared expert) and reads back at 1.1e-6 or less against the oracle's layer on all three
fixture blocks over eight positions (block 1 emits its second ratio-4 row at position 7, reading the shifted state; block 2
emits at 1, 3, 5 and 7), alone and chained (chained 2.2e-7 to 3.4e-5, gate 1.12e-4). The compressor has no independent oracle:
its legs are pinned to the author twin `dsv4-compressor.fk`; one cached element of block 2 differs from the twin by one f16
step (f32 and fp64 on opposite sides of a rounding boundary), which the band names and holds to the chain gate (stages 2.4e-5
to 3.2e-5 after it). The fixture's SwiGLU clamp is 10.0, 1.0, 2.0 (its pinned SHA-256 is now
b90181a115536345bbbe503098c1418d38403c9616555d98d343241ce1353b5e): it bites on blocks 1 and 2, `dkl-clamp` equals the
metadata, and a layer that read no clamp reads wrong. No carrier change was needed. Found on the way: `metal_pipeline`
compiles with fast math, under which `v == v` and `v != v` are not tests of a NaN (both the argmax and the scan test the bits);
and the restored router's softplus, ln(1 + exp(z)) in f32, loses its digits for negative z (probability relative error 2.7e-5
at z = -7, 1.1e-4 at z = -9, the whole value from z = -17 down, against the oracle). The new kernels `form_dsv4_hash_probs_s` and
`form_dsv4_topk_probs_s` use max(z, 0) + log1p(exp(-|z|)) with an eight-term series below 0.1: over z = -20 to +20 the
probability reads at most 9.2e-8 and the softplus 1.4e-7 (4.5e-8 at -7, 6.6e-8 at -9), the restored form stays in the band as the
control, and the layer routes with the new ones (block 0's router weights 2.8e-5 to 1.7e-7, its output streams 1.0e-5 to 4.9e-7;
block 2's output streams 1.48e-5 to 1.49e-5, flip-dominated). The real IQ2XXS file's Q8_0 dense path and Q2_K
down experts were wired in stage 4 (below); `n_hc` other than 4 is refused.

Driver stage 3 (read 2026-10-03 WITA, memory room and no model door resident before each run, every exit 0; each band's planted
defects dropped its reading and were restored): `form/native/metal/dsv4-token-handle.fk` runs the embedding, every block, the
output head (stream rms, F16 `output_hc_fn`, the head gates, the stream sum, `output_norm`, the MXFP8 `output.weight` into the logits
at bs[28], where Qwen's sampler reads them) and the device argmax, which is trusted only after the NaN scan; prefill is token-major
with the head asked once, at the last token. On the fixture the head reads 3e-8 to 4e-7 against the oracle's own head arithmetic;
prefill and the stepwise path agree bit for bit; the whole model (a prompt of four tokens and eight greedy steps, ids 27 24 29 6 17 15
16 3) agrees with the oracle's full forward at every step (nd 1.2e-6 to 6.9e-5 against the tilt-measured bound max(8 ne, 3e-5) =
3.2e-4 to 9.1e-4, the argmax equal with a margin of at least 100 times the disagreement), the run twice is bit-equal, and every
block alone at all eleven positions reads 1.9e-7 to 1.9e-6: chained, the model amplifies f32 noise (steps above the 3e-5 floor), which is its
conditioning and not a kernel's. Not independently witnessed: the compressor (the oracle has none; its leg is pinned to the author
twin), the fixture's own clamp values, and the head's choice of epsilon (the fixture holds `eps` and `hyper_connection.epsilon` equal).
`form/form-stdlib/dsv4-lease.fk` derives what a DeepSeek session asks the governor for, from the header (dense bytes + 64 experts a layer
+ KV; derived, not measured): 27,593,273,180 B for the IQ2XXS file and 31,060,815,708 B for reap25 at 8,192 positions, against a wired cap of
82,463,372,040 B that the files themselves (86.7 and 91.3 GB) exceed; `fcms-weights-bytes` and `adm-admit` charge it for a deepseek4
file and file_size for every other (the Qwen file reads 29,047,086,048 exactly). `form/form-stdlib/form-cli-model-ds4.fk` seats DeepSeek
at the generate and session doors (the DeepSeek tokenizer's raw ids, the lease, the driver's open, prefill, greedy steps and close); the lane
switch reads 0, today's refusals read word for word, a DeepSeek session refuses observations, and nothing here has run on a device.
`observe/dsv4-validate-run.bml` is the validation window, written and not run: its default (empty stdin, `{}`, `{"dry":1}`) prints its plan
(24.9 GiB lease, the stages and asks, the abort rules, the references) and opens nothing; only `{"go":1}` runs it, and while the header names
tensors the driver cannot run it maps nothing and writes a refusal row. (At stage 3 the plan printed eleven gaps for the IQ2XXS file: the
Q8_0 dense tensors, the Q8_0 `output.weight`, the Q2_K down experts and the ds4 order-match units; stage 4 below closes the first three and
turns the fourth into a printed note.) The lease's belief that only touched pages wire is derived, not witnessed:
if whole expert stacks wire, the set is about 85 GB, above the 82.46 GB cap, and the window's wirespan probe is the witness. The doors, as they read after this change:
`./fkwu observe/door-link-health-run.bml </dev/null` -> docs=39 claims=831 broken=0 (prelude-reach missing=0 untracked=0 shadow=0),
`./fkwu observe/band-truth-run.bml </dev/null` -> bands=356 readable=178 unreadable=178 absent=0 seen=1 flaws=0 (the five stage-3 bands are read: 351 -> 356, 173 -> 178).

Driver stage 4 (read 2026-10-03 WITA 04:36 to 04:44, each band alone after the 03:30 walk had released the GPU, memory room before each run, every
exit 0; each band's planted defects dropped its reading and were restored): the IQ2XXS file's tensor types run in the driver. `form/form-stdlib/dsv4-fixture-gguf.fk`
writes a second fixture, the first one's 86 tensors and seeds with every MXFP8 tensor Q8_0, every gate and up stack IQ2_XXS and every down stack Q2_K at hidden
256 (1,778,144 bytes, SHA-256 eb93eed0aaf888853f74e18329919fe35f8f6db5ab61caf7363272824287b0e1; the first fixture is byte for byte b90181a1...1353b5e still).
`form/form-stdlib/bml/dsv4-oracle-reference.bml` admits, prices and reads types 8 and 10 in fp64 (`dfo-ref-q80`, `dfo-ref-q80-out-a`, `dfo-q2k`), each by the
tensor's own type, and the fixture band holds it to the body's carvers (`q80-flat-at`, `q2k-at`): nd 0 on every row. `dsv4-kernels.fk` puts the Q8_0 unit in
`dkr-graph-units` and adds `form_dsv4_q2k_matvec_id` (101 kernels, 72 in the graph); `dsv4-layer.fk` chooses the dense kernel (`dkl-dense`, `dkl-dense-grouped`)
and the expert kernel (`dkl-expert-kernel`) from the type the tensor's own table row carries, and refuses a block type its first dimension does not tile;
`dsv4-token-handle.fk`'s head accepts a Q8_0 `output.weight`; `dsv4-layout.fk` holds the one table of the types each role carries and `dvl-driver-gaps` reads
the header's gaps from it. On the second fixture: each kernel alone 9.1e-8 to 3.1e-7 against the oracle (the Q8_0 lane, the one-thread form, the grouped
output factor, the Q2_K stacks by ids, IQ2_XXS by ids on layers that were MXFP4); every block alone and chained within the gates (the layer band's stages alone
read at most 7.8e-7, chained 2.4e-7 to 7.2e-6); the whole model, a prompt of four tokens and eight greedy steps, within the tilt-measured bound at every step (nd
3.7e-7 to 8.8e-6 against bounds of 3.1e-4 and up), the run twice bit-equal, every block alone at eleven positions under the 3e-5 floor (1.2e-7 to 1.3e-5). The walk this fixture takes is degenerate (token 3
at every step), so its argmax is weak evidence and its logits are the evidence. Plants: the IQ2_XXS kernel routed to a Q2_K stack, the MXFP8 kernel to a Q8_0
tensor, the Q2_K scale nibble read as the min, the Q8_0 block stride 33 for 34 each read far from the oracle at a kernel and, planted into the context, at a
block (the control and the restored kernel inside the gate). Through the validation door the IQ2XXS header now reads NO gap (dry, `./fkwu observe/dsv4-validate-run.bml
</dev/null`: "the driver cannot run yet: nothing named by the header"); a doctored header reads one line a tensor type. The ordering of ds4's own lanes is
a printed note and not a gap: ds4 quantises a Q8_0 matvec's activation to int8 per 32-block (ds4.c:7051, :6814) and sums Q2_K in a plain ascending f32 order
(:3480), and the driver's lanes read f32 activations, so a near-tie token may differ. What the window reads of that, by leg: the STREAM leg (the 24 greedy ids for the raw prompt, which needs no extra input) matches
ids and now records, per step, OUR top-two logit margin and how far the recorded id's logit sits below our top (`stream_margins`, `stream_recorded_gap` in the
receipt), so a flip is told from a near tie; the reference's own tie budget (its top-two margin against our largest delta over its top 64) belongs to the LOGITS leg,
which runs on the recording's 14 chat-templated prompt ids (pinned in the row beside the recording, below; a request's `logits_prompt_ids` replaces them, and the leg waits only when neither holds). The `ds4q8` and `ds4q2k` units are what to wire if the
readings say so. An expert type no kernel reads is an explicit refusal (`dkl-expert-kernel` reads nothing; the FFN answers [0] before any dispatch). What still blocks the
physical window: the window itself (one run, alone, governor-checked, the door dry by default and `{"go":1}` explicit), for positions below 2048 only: the 126 indexer
tensors (`indexer.attn_q_b`, `indexer.proj`, `indexer_compressor_{kv,gate,ape,norm}`, on the 21 ratio-4 layers) are present in the file and unrouted, and a ratio-4 block
refuses at position 2048 or later (the indexer stops being inert there). The kernels' multi-block rows (4 and 8 blocks a row of Q2_K and IQ2_XXS, shared and per-slot
input) read 1.2e-7 to 2.6e-7 against the oracle on a third file, the second fixture's rows being one block; the Q8_0 and Q2_K plants split into a wrong number (Q2_K scale as the
min, Q8_0 codes unsigned, an IQ2_XXS kernel on a Q2_K stack: finite, far) and not a number (a Q8_0 stride of 33, an MXFP8 kernel on a Q8_0 tensor: non-finite). The doors after this change: door-link-health docs=39 claims=845 broken=0 (prelude-reach missing=0 untracked=0 shadow=0), band-truth bands=360
readable=182 unreadable=178 absent=0 seen=1 flaws=0. Found on the way: a recursive peak that calls itself twice a level takes 2^n steps (64 values never returned), so the band's peak is one fold.

The first physical window (receipts/dsv4-validation.jsonl, 2026-10-03 06:47:57 WITA, run by the lead alone) stopped itself after 988 ms at the state stage: wired +198,148,096 B against an estimate of 18,270,208 B, over
twice max(estimate, 64 MiB). The price was right and the sensor was not (read 06:50 to 07:19 WITA, each band alone, memory room before each run, every exit 0). From the header's 44 ratios and key length 512 with plain
arithmetic (`dsv4-layout-band.bml` bit 512): 43 raw arenas of 131,072 B (5,636,096), the compressed rows (17 of 2,048 B on each of 21 ratio-4 layers, 1 on each of 20 ratio-128 layers: 772,096) and the compressors'
state and score (32,768 B twice on the 21, 262,144 B twice on the 20: 11,862,016, the same at any capacity) make 18,270,208, which is `dvl-kv-total(m, 64)` to the byte (now the sum of `dvl-raw-total`,
`dvl-comp-rows-total` and `dvl-comp-state-total`); at 8,192 positions 824,068,096. The device's own ledger agrees (`dsv4-open-band.bml` bit 512, metal_live word 18 = currentAllocatedSize, which no other process moves): the
state at 1,024, 8,192 and 65,536 positions grows it within 13,312 B of its price and frees back to the byte, and the first dispatch in a process (the init kernel) moved wired memory by 0. What moved wired memory is the
machine: it is every process's, and on this Mac a sibling not yet named moves it by 306 to 328 MB every second or so, up and down (900 readings 10 ms apart: 7 steps over 16 MiB, 5 over 128 MiB; spreads over 1 s from 0.7 MB
to 963 MB; a stage that does no work read +0.6 GiB, and a 3 s reading was 19.8 MB then 331.9 MB: it is bursty). The lease did not move (`dsv4-lease-band.bml` bit 256): shared 26,787,475,292 B and the 8 GiB margin, 35,377,409,884 B at
64 positions, 805,797,888 B more at 8,192 and only the capacity-bound terms grow. The window names the movement and subtracts none of it: every process on this machine is ours, so `dvr-organs` reads the process list through the
body's door (`host_processes`: pid, resident bytes) and names the organs of ours resident by command (the dry plan prints them, every stage line carries them with the ones whose resident bytes moved by 64 MiB or more, and the receipt
holds `organs` and `moved`), and the rule that guards is the lease: wired above what the window holds, past the baseline, with host-memory-low and a denied ask (`dsv4-validate-band.bml` bit 2048). A stage's wired growth against its price is
read and told (OVER twice its price, with the organs that moved), never an abort of its own, and the wirespan probe's kind (slice, between, stack) is recorded. Every stage line and receipt row carries the device's own bytes
(`own_delta`) beside the wired growth. The compile and map estimates are bounds (512 MiB, and 512 MiB plus the scratch plan), not readings: the window's +309,854,208 B and +43,286,528 B are the first measurements of them.

The second window (receipts/dsv4-validation.jsonl, 2026-10-03 07:29:28 to 07:30:04 WITA) passed compile, map (own device bytes +86,284,566,528: the 80 GiB mapping), state, wirespan ('between') and layers 0 to 15 on the real file and
ended at layer 16 on ask-denied:compressor with the reading that denied thrown away; 13 s later the compressor read 6.3 GiB against a cap of 32. Field 8 of `host_vm_stat` is the pages the compressor OCCUPIES; the pages it STORES are
vm_stat's ('Pages stored in compressor', 27.9 GiB then), read by `dvr-stored-pages`. A denial is now a reading carried whole (the receipt's `denied`), and a denial for reserve or compressor is waited out with the governor's naps (10 s
doubling to 60 s, 5 minutes an ask at most, never past the window's 20) before the run ends (`dsv4-validate-band.bml` bit 4096). Wired memory followed the weights a layer's dispatches touch and returned them (layer 0 +3,207,380,992 B,
layer 1 -1,368,260,608 B, layer 14 -2,233,942,016 B; net +2.55 GB over 16 layers), so a layer is priced at the bytes of the tensors its dispatches can touch, whole stacks included: layer 0 3,018,191,320 B (the largest), layer 1
1,959,129,560 B, the 43 summing 85.7 GB, which is why they are asked one at a time and never summed (bit 8192). The validation window no longer stages the layers (a sync and a machine reading a layer is a probe protocol, not the
token path); it keeps compile, map, the arena, state, wirespan, the head and its argmax, and the recorded references. The token path is measured by `observe/dsv4-token-run.bml` (`form-stdlib/bml/dsv4-token-run.bml`;
dry by default, `{"go":1}` runs; run once on the real file with whole mapped stacks, 2026-10-03 19:04:25 to 19:05:21 WITA, below): one row a token read from the landed ledger (`dll-line`: dispatches, barriers, command buffers, waits, host reads, buffers made, bytes, device busy, ms) with the machine's wired growth, pageins and
the device's own bytes, the plan priced from the bare header by the ledger's own formulas (`dll-token-count-m`, `dll-token-bytes-m`: 1,856 dispatches at the quietest position and 2,082 at the loudest, 22 command buffers), and wired
memory read after EVERY LAYER of the first three tokens without a sync, through the one function the token handle calls at its seams (`dkr-hz-tap-set`; no tap is installed in production), stopping the token before the machine is at
risk (host-memory-low, wired above 60 percent of memory, available under the reserve, 20 minutes) with the curve in the receipt (`native/metal/tests/dsv4-token-run-band.bml`, `form-stdlib/tests/dsv4-token-run-plan-band.bml`).

Driver stage 5, the flow held to the op ledger of the Swift reference runner it was learned from (git history 93154a720; a lesson, not a dependency; read 2026-10-03 WITA 09:30 to 10:50, each band alone, memory room before each run; `docs/dsv4-flow-history.md` is the map of where the
whole flow stood: the Form cell of 08-10 held the stream at 226 ms a token warm, the Swift runner's 34 to 38 ms was never a Form number, and neither was ever in main). Measured on the fixture before this change (a machine at load average 33 to 43, best of six reps of 16 tokens): the 137
dispatches of a token cost 9 to 10 ms of host, 66 to 76 us a dispatch, where Swift pays 1.1 us: `md-bind` built an eight-word binding in 36 us, `dkl-supported?` ran 700 us a block three times a block a token, `dkl-clamp` scanned the header 230 us twice a block, and the NaN scan made a buffer,
three syncs and a free a token. Now: `dkr-bind` (4 us, the same bytes), the admission read once a context (`dsv4-admitted-once?`), the clamp once a block, the scan in the token's own batch behind one sync, and `dkr-hz` in `dsv4-kernels.fk`: a concurrent batch whose
barriers are planned once a block from the reads and writes each kernel's own MSL signature declares and replayed, a batch handed to the queue every second block, the real file's four-row IQ2_XXS and Q2_K expert kernels and ds4's Metal-order Q8_0 matvec (`dkr-unit-lanes`: 104 kernels over
eleven sources, 75 in the graph). `form/native/metal/dsv4-ledger.fk` reads one row a token and a row a block from the carrier's counters. On both fixtures a decode token reads 137, 142 or 148 dispatches (the layout's formula, held at every step),
111, 116 or 121 barriers (81%), 2 command buffers, 1 wait, 2 host reads, 0 buffers made; the serial walk and the concurrent walk are bit-equal in ids and logits; the best of six host encodes is 3.3 to 3.5 ms for 137 dispatches (24 to 26 us a dispatch, 2.7 to 3 times
under the 66 to 76; 17 to 20 us at a quieter hour). Derived for the real file, not run: 1,856 dispatches a token, about 1,500 barriers, 22 command buffers, a host floor of 45 to 50 ms (33 to 37 at the quieter figure) overlapped with the device. Not witnessed: any real-file number, any wall-clock gain at real dimensions, the Swift runner's rate on
this Mac today. The session's open and step and the generate door's one-shot run on the fixture through the door's own functions (`dsv4-door-run-band.bml`, the lane flag untouched and 0; the fixture has no vocabulary, so `fcds-text` now renders none instead of reading pieces
at offsets a missing array gives).

THE REAL FILE'S FIRST TOKEN, over whole mapped stacks (the token door, 2026-10-03 19:04:25 to 19:05:21 WITA, memory read each stage; the lead's run, the receipt and stdout in the land-int checkout): prefill position 0 read 1,848 dispatches, 1,461
barriers, 22 command buffers, 1 wait, 0 host reads, 0 buffers made, 8,544,878,408 B of weight to read and the device busy 167,726 us (the compute is hardware-class) in 49,272 ms of wall clock: pageins +5,241,090 pages (16 KiB each: the WHOLE 80
GiB file read in one token), wired only +2.2 GiB, and 1,816,908 pages (27.7 GiB) of other processes' memory pushed to swap; the door held at token 1 on host-memory-low, as it must. Binding a whole 0.55 GB expert stack makes the device read every page
of it a dispatch names (read, not wire), so a cold token costs the file and the machine, the pattern that restarted the OS before. THE ROUTED EXPERTS ARE THEREFORE READ THROUGH A SLOT ARENA (`native/metal/dsv4-arena.fk`, the door's default,
`arena_slots` 64; 0 is the whole-stack control): the routed stacks are not mapped; each layer holds a bounded set of experts in buffers allocated once (the lease's expert term to the byte: 19,478,347,776 B on the IQ2XXS file) and an
expert the router picked and the cache (`dsv4-expert-cache.bml`: least recently used, keyed by layer and expert) does not hold is filled from the file by its own span through the FILL DOOR (`metal_buf_fill(buf, boff, [path, foff, len])`: the host
preads the span straight into the slot, no Form string, no mapping, no dispatch; carrier `fk_metal_buf_fill_external` in `form/native/metal/fk-metal-carrier.m`, mode 33 of fkwu's leaf door in `runtime/fkwu-uni.c` with its rewrite row, every refusal answers -1
(metal_buf_write's convention beside it) and writes nothing, a path holding a NUL byte included, `metal-buf-fill-band.bml` 31). Measured 2026-10-03 on a Qwen file's 2 MB spans (2,162,688 B): the door 113 us a span with the pages cached (19 GB/s) and 566 to 578 us for spans not cached (3.7 GB/s: the disk); the
road it replaced (a no-copy view of the span and a copy kernel, still taken when the carrier lacks the door, bit-equal in the bands) 726 us with the pages cached (3 GB/s: the device faults the file's pages in one by one, 31 us for the view alone), and any
road through a Form string 4.2 to 5.0 ms. The SAME expert kernels read slots by slot number, bit-equal to the whole-stack control stage by stage and end to end on both fixtures and on both roads (`dsv4-arena-band.bml` 2047). A SLOT IS VALID ONLY AFTER ITS FILL SUCCEEDED: every failure after the cache took its plan (the slot numbers not written, a refused or short fill, a fill that stops)
forgets the slots the plan placed, and the band holds three planted failures to a bit-equal rerun on the same cache and the stale plan itself to a differing one; the pin rule is held end to end on a two-slot walk (the pure model over the logged routing
reads the arena's counters and keys, and a victim rule that ignores pins does not). A block is two planned stages with ONE sync between them, a plan boundary of the token's concurrent batch (`dxa-boundary`:
`dkr-hz-sync`, one counted read of the router's ids, the cache's slots, a batch armed again, the fills): a token waits blocks + 1 times, reads blocks + 2, commits blocks + blocks / 2 + 1 command buffers, makes no buffer, and its row carries the span fills,
their copy dispatches (none through the fill door), the road they took (`road=door|view` on the row line, `fill_road` in the receipt row, the fills through each road in the summary), the experts hit and missed and the bytes filled; the ledger pins both modes' ceilings (`dsv4-ledger-band.bml` bit 512: `dll-arena-ceil`). The plan prices them from the header: the device reads 9,107,652,444 B a token (16.6 ms at the stated 546 GB/s) hit or
miss alike, and the file gives the dense tensors faulted in on the first token (the open maps them and reads no byte: 7,281,557,340 B the token reads plus 1,059,053,568 B of token_embd's other rows, a bound view being read whole by the floor's
theory, which the lease does not price: it leaves token_embd out as one row), at most 1,826,095,104 B for a cold decode token, at most 25,565,331,456 B for the 14-id prefill (84 lookups x 43 layers x 7,077,888 B: the
worst case, which the slots do not bound; the arena is a separate 19,478,347,776 B) and only the missed experts warm (7,077,888 B for one at layer 0). A checkout takes the fill door by rebuilding fkwu (`cc -O2 -o fkwu runtime/fkwu-uni.c`) and the carrier (the dylib line in AGENTS.md): a file that names `metal_buf_fill` needs the new fkwu, and a carrier built before the door still loads (the door
is an optional symbol) and runs the view road. Not witnessed: the arena on the real file (fixtures only), the cache's hit rate over real routing, the fill's rate from the real file's disk.

Bands that read a binary fixture through `read_file_slice` or a host door run on fkwu like every band;
`./validate.sh --list` prints each band's staging and pins. The restored emitters, the
`*-real.fk` cells, `dsv4-token.fk` and the windowed residency emitter say in their own headers whether a band proves
them or they are restored for the spec and not run physically. The reference evidence
`docs/evidence/fkwu/dsv4-oracle.json` is the reading of 2026-09-11 and is left as observed; the sidecar
`docs/evidence/fkwu/dsv4-oracle-reread.json` names which of its pinned identities still match.

The recorded references are `form/form-stdlib/tests/oracles/ds4-logits-capital-of-france.json` (129,280 logits, argmax
2581 "We" at 36.7579117) and the 24-token stream for the raw prompt ids [671 6102 294 8760 344] (a period-7 cycle,
" Paris. The capital of France is"). They were earned on the IQ2XXS file, which ds4 can read; the registry's `ds4flash`
row names the reap25 file, which it cannot, so they judge a native lane on the IQ2XXS file and a reap25 lane is judged by
the Form reference. The radius is one prompt, a cycle, never past position 127. The logits' own prompt ids are the row
`form/form-stdlib/tests/oracles/ds4-logits-capital-of-france-prompt.json` (read 2026-10-03 WITA through `./fkwu form/form-stdlib/tests/ds4-recorded-references-band.bml </dev/null`
-> 8191 and `dsv4-validate-band.bml` (then 2047), the governor reading room before each run, every exit 0): the 14 ids [0 3476 477 260 11502 22896 128803 671 6102 294 8760 344 128804 128821],
which are BOS, ds4's default system "You are a helpful assistant" (5 ids), User, the prompt "The capital of France is" (the raw prompt's 5), Assistant and `<think>`. `form/form-stdlib/bml/dsv4-chat-template.bml` derives them
from the IQ2XXS header alone, finding each marker by its bytes in the file's own vocabulary (BOS equals the header's `tokenizer.ggml.bos_token_id`) and detokenizing back to the template's text; the template is ds4's own
(`ds4.c:36195`, the default system at `ds4_cli.c:1767`, thinking on at `:1774`) and the count it gives, 1 + 5 + 1 + 5 + 1 + 1, is the recording's `"prompt_tokens":14`. The same code gives the token counts
the official DeepSeek API reported for three prompts with no system text (21, 18, 27). What the records do not say: the recording names neither the prompt nor the flags, so the prompt is read from the
commit that took it (`09c69ba8e`, "for the same prompt") and the corpus rows 939 and 948, and thinking on is the default path (a `</think>` ending, thinking off, has the same count and differs in the one last id, 128822; the recording's argmax "We" is, in the commit's words, ds4 opening its
reasoning, which fits `<think>`, and no flag is recorded). The dry door prints the ids and the leg's ask in place of the old WAITS line. Plants (each read a different list or no ids): a wrong id, a wrong count, a missing BOS in the row
(the recorded-references band's bits 1024 to 4096 and the validate band's 1024). The kernels the order-match band runs
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
offset to a character start, and both are natives with their recipes kept in `core.fk` as the body's statement of
what they mean ([`docs/str-find-one-meaning.md`](docs/str-find-one-meaning.md)).

```text
substring-one-meaning-band 4095 · str-find-one-meaning-band 8191     (both drift-gate rows)
kernel-laws-band 2147483647     laws 1-9 of docs/kernel-interface.md (a drift-gate row)
core-substring-equivalence-band 2047 · substring-native-band 511
line-grammar-search-equivalence-band 8191 · core-str-find-equivalence-band 2047
meaning-codes-table-band 255
```

## What a melt gives back

fkwu's melt compacts the cons heap and then frees what no root names. It frees two more things than the heap.

A dead string's bytes become a hole in the string arena (`fk_hole_add`), the melt orders and joins the holes, and
`fk_sintern` places a new string in a hole that fits and discards its scratch. Live strings never move and the
arena's top never retreats. Before this, a dead string's slot came back and its bytes did not: the glass organs and
sensors processes held 6.8 and 7.5 GB behind ten thousand live strings, growing 200-300 KB/s, because the supervisor
starts them once and keeps them for its life. `kernel_stat(64)` reads the top, `kernel_stat(65)` the bytes in holes;
`string-holes-band` reads 127, and 115 on a build that takes no hole (`-DFK_HOLE_LOOK=0`). A dead string's slot owns
no bytes, so a stale word naming one reads length 0 where it used to read the old bytes until the slot was reused.

A closure row lives while a root reaches it: the value stack, memory cells, records, nodes, held lets, the method
table, and the captures of a row that is itself reached. A melt also keeps the rows made since the melt before it
(`FK_CLO_YOUNG`: a row survives exactly one melt unrooted), copies only what a kept row captured, slides those values
down and hands the other rows to `fk_clo_free`; a row's index is the fn-value's own word and never changes, so a row
handed back and taken again is another closure under the same word. The mark walks what a row captured from an
explicit worklist, never one C frame per link: a chain of four million closures each capturing the one before is a
band claim. Before this, every evaluation of a capturing lambda kept what it captured for the life of the process: a
hermetic glass loop held 4.25 of its 4.36 million surviving pairs in closures, the heap doubled to 2^29 pairs in the
live glass, and each melt moved gigabytes of the machine's memory; the same loop now holds 125 thousand pairs flat in
a 1 M-pair heap and 87 MB. `kernel_stat(66)` reads the rows standing, `(67)` the rows reclaimed, `(68)` the captured
values held, and `kernel_live` word 21 the melt count. `closure-reclaim-band` reads 1023 and `closure-callee-band`
7; their plants (compile flags) read: `-DFK_PLANT_NOMOVABLE` 895 (eq leaves a closure word on no root),
`-DFK_PLANT_NOCALLEE` 2 on the callee band, `-DFK_CLO_YOUNG=0` 767, `-DFK_CLO_YOUNG=1000000000` 753,
`-DFK_PLANT_RECURSIVE_MARK` dies at the first melt of the long chain (rc 138). With `FK_MELT_WITNESS 1` in
`fkwu.conf` each melt prints which root held the pairs it kept (stack, mem, records, nodes, closures, holds) and the
string and closure tables.

Because a melt can move a pair, reuse a string slot or hand a closure row back, an evaluator arm that holds a word in a
C local while it walks something else must have that word on the value stack. What each arm does today
(`fk_walk` and `fk_walk_body`):
- on the stack across the walk: `cons` head and tail, `nth` list, `value_eq` both sides, the string arms (`str_eq`,
  `str_concat`, `str_byte_at`, `str_find`, `substring`), `record_get`, `record_set` (record and key),
  `method_define` (blueprint and name), `method_has`, `scan`, the indirect call's callee (`tag 244`, both arms),
  `apply` (`tag 44`), `eq` and `lt` (through `fk_movable`: a pair, a local string slot or a closure word), the
  two-argument direct call's first argument (`tag 240`, both arms), `intern_composite`'s category (`tag 47`),
  `fb_record`'s file word (`tag 128`), the metal, cuda, socket and wav doors that take a string or a handle first;
- in a C local with no root, and why that holds: `add`, `sub`, `le`, `mul`, `div`, `mod`, the bit doors and
  `round_ndigits` read numbers (a pair, string or closure word stops by name at the next check and is never read
  through its row), `and` and `or` read the word's truth only, `mem_set` and the page, cell and gift doors hold
  ints, `method_invoke` holds a record (records never move) and takes its method from the method table, which is a
  root; the single-walk arms keep nothing across a walk.
The claims that fail when an arm drops its root: `closure-reclaim-band` 128 (`eq`), `closure-callee-band` 1 (the
indirect call's callee); the arms under `tag 47` and `tag 128` have no claim yet, and `tag 240` is a node the parser
never emits (a lowered image carries it), so no source program reaches it.

A kernel started before these doors keeps the old tables until it is restarted, and a binary is replaced by
building beside it and renaming (`cc -O2 -o fkwu.new runtime/fkwu-uni.c && mv fkwu.new fkwu`; copying over a
running binary kills it). The glass fleet is no launchd job: `observe/form-glass-run.fk` is a terminal session whose
supervisor starts the sensors, machine, organs and ear processes once and the live loop under them; ending the
supervisor (`q` in the glass, or its pid) ends all of them, and `./fkwu observe/form-glass-run.fk` from the same
checkout starts them again on whichever `./fkwu` that checkout holds. The ear process stands the microphone while
`.hearth/ear.wanted` stands, so a restart is Urs's to choose.

## The body's own lenses

The body reads itself: which doors bear the most walking, which door is a private copy of a warm one,
which of a native's mirrors still stand, where a definition is written twice, and which written limit
carries a witness.

```text
bearing-census-band 32767   form/form-stdlib/bearing-census.bml    (steps, not milliseconds)
twin-census-band 65535      form/form-stdlib/twin-census.bml
mirror-census-band 16383    form/form-stdlib/mirror-census.bml
copy-census-band 63         form/form-stdlib/bml/copy-census.bml
wall-census-band 63         form/form-stdlib/bml/wall-census.bml
observe/tests/voice-frequency-band 255 · number-band 255 · float-printer-band 31
```

## The JIT

A pure-float defn crystallizes into an arm64 f64 leaf when its box ledger crosses a boundary
(`fk_f64_pulse`); a self-tail-call loop crystallizes on heat (`fk_heat_pulse`). `form-lower.fk`
carries runtime strings through the two-slot `fk_inram_args` convention.

```text
jit-lens-band 16383 · jit-heat-gate-band 4095 · observe/tests/jit-evaluator-heat-band 4095
form-lower-string-band 63 · form-lower-string-runtime-band 255 · form-lower-string-both-runtime-band 511
float-natives-band 28 · persistence-band 7 · channel-breath-band 500 · eq-shape-band 524287
blueprint-authority-band 8191
```

## Not standing today

What answered red, died, or was not witnessed today, so no one leans on it:

- The guard's refusal inside a live Claude Code session is read from the hook contract and from the door
  run by hand and by `sh -c` with the registered line (2026-10-03); a subagent's Bash call passing through
  the project hook, and a deny under `bypassPermissions`, were not witnessed here.

- `form-source-lift` over a mixed module whose first plain form carries a `; preludes:` header writes the
  lifted `import` inside the first section, and its own verification compile stops: `the cursor does not
  read this form.bml section: line 3: import (wanted: topstmt)` (2026-10-01). The five grammar packs it
  lifted carry their imports above the section by hand; `form-source-lift-band` holds no such case yet.
- `form-cli-allowance-band`, `form-cli-live-band` and `native-tensor-lifecycle-band` reach the Metal
  door and were not re-run in the gates pass (it held no GPU). The integrated review's sweep
  (2026-10-01) read form-cli-allowance 2047, form-cli-live 255 and native-tensor-lifecycle 1023 on fkwu.
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
