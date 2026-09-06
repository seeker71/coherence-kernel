# Current Floor

Measured on this Apple M4 Max through the resolver-driven `./fkwu` door (the
binary passes the freshness band, 31), every band compiled fresh from source in
one pass with its `.fkb`/`.sym` beside it removed first. Receipts hold history;
this page holds only what stands. A claim without a number names the band that
declares its own.

## Grounding

```text
cc -O2 -o fkwu runtime/fkwu-uni.c \
  form/native/metal/fk-metal-carrier.m form/native/mlx/fk-mlx-carrier.c \
  -framework Metal -framework Foundation -fobjc-arc \
  -I/opt/homebrew/include -L/opt/homebrew/lib -lmlxc -Wl,-rpath,/opt/homebrew/lib

./fkwu bootstrap/ground.fk                                    -> 42
./fkwu bootstrap/ground-recursive.fk 10                       -> 55
./fkwu form/form-stdlib/tests/binary-freshness-band.fk        -> 31
./fkwu bootstrap/ground-numeric-list.fk                       -> [1, 2.5, [3, 4]]
./fkwu form/form-stdlib/tests/native-vs-rented-band.fk        -> 11111
./fkwu proof/four-way-run-recipe42.fk                         -> 0   (FOUR-WAY)
```

The four-way proof host-execs the three minimal walkers; they build from
`walkers/README.md`'s own lines (`go build -o walker .` in `walkers/go`,
`cargo build --release` in `walkers/rust`, node 26 runs `walkers/ts/main.ts`
directly). Without them the cell answers 2 (WALKER-SUSPECT), which is the
honest reading of an unbuilt walker, not a kernel fault.

`runtime/fkwu-uni.c` is    18,451 lines — a temporary seed and shrink target, not
the destination (`release-ledger.bml` R13); this week's growth on it is
correctness heals (#573 nested-defn scope, #574 bool literals, #575 kernel
preludes, the host-exec stdin door, `metal_deadline` off its scratch slot, the gift
frame in shared memory, the content-keyed lowering lane, the frame pacer and
terminal doors, `kernel_hot`).

## Body-wide witnesses

```text
gate/structural-gate-run.fk            -> [206, 0, 48, 3, 20, 57, 74, 4] then 1
gate/tests/structural-gate-band        -> 8191
observe/door-link-health-run.bml       -> doors=12 links=63 broken=0 code=12063000
observe/body-link-graph.fk             -> body-link-graph-check 63; blg-field-code 13029046
                                          (13 orphans, 29 broken, 46 candidates; the organ
                                          has no run door — prelude it and call both)
homecoming-distillation-corpus-band    -> 32767   (asserts 684 rows, 672 admissible)
no-fixed-tables-band                   -> 63      (every seed table grows; none is a wall)
form-cli-author-high-band              -> 4095
host-os-membrane-band                  -> 8191
bidirectional-framebuffer-channel-band -> final field 1
import-carry-band                      -> 63      (cold and warm; it prints its verdict, the trailing
                                                   0 is print's own value; with a fixture image from
                                                   a different fkwu build beside it, 15 — door 0, told)
grammars/tests/form-eval-band 65535 · form-eval-full-band 635 · source-compiler-grammar-bridge-band 32767
pattern-match-band 511 · choice-lane-core-band 1023 · control/tests/offer-ack-core-band 2097151
control-invite-grammar-band 1023 · node-introspection-band 4095
cell-serialize-band 1023 · json-band 1023 · wire-rpc-band 15 · core-str-find-equivalence-band 2047
```

## The BML floor

A unit lowers by what it carries: any file with a `section [` block on a line
of its own — `form.bml`, `form.lift`, `form.action`, `form.route`, the `*.bmf`
grammar dialects — travels through `bml-floor-compile` whatever its extension,
as a prelude or as the main file, and fkwu owns the `.lowfk`/`.fkb` cache
beside it. 75 `[form.bml]` files and the twelve `[form.lift]` sources wear
`.bml`; `compiler.fk` and the ten `grammars/*-bmf.fk` carry their blocks
mid-file and lower in place. `true` and `false` are literals in the dialect; a
nested `defn` is a registered function with one-level capture; a `let` inside a
`(do …)` never reaches a `defn` frame (top-level lets do).

```text
bml-band                               -> 268435455
bml-generics-band                      -> 16777215
native-route-goal-cells-band.bml       -> 1048575  (full)
nested-defn-scope-band                 -> 63
nested-defn-closure-capture-band       -> 63
bml-float-literal-band                 -> 2047     (a decimal float reads back as the same float
                                                     from eleven positions; the emitter refuses an
                                                     unknown leaf by name instead of writing "")
bml-form-size-band                     -> 127      (one 40 KB `def` in a single form lowers and
                                                     answers; the normalizer walks spans as a tail
                                                     loop, so one statement's size sets no wall)
cell-channel-band                      -> 4095     (two cells as processes on spool+bell, a shared
- `form/form-stdlib/tests/ear-native-band.fk` = 255 — whisper-tiny as the body's own pass on this metal (native mic stream, weights off the npz, fifteen emitted kernels, base64 words); log-mel equal to the reference to six digits; an 8 s window encodes in 41 ms, a token costs 8 ms; the live window is the last 8 s of real room audio, the cut is the decoder's first timestamp, and a line ends where the world ends it.
- `form/form-stdlib/tests/q8-0-matvec-tg-band.fk` = 27 — three cooperative Q8_0 matvec twins byte for byte the attestant on llama3.2:1b's first projection; the double-buffered one (two simdgroups a row) runs 48x the attestant at 88 GB/s against a measured 330 GB/s floor; the dense lane dispatches it, a token 36 ms (floor 4 ms); the two rmsnorm twins are compiled, not exact, not dispatched.
                                                     field admitting grammar offers by whole sha256,
                                                     evaluation through a child membrane under
                                                     hearth-channel-eval-s; the witness door
                                                     observe/cell-channel-witness-run.fk walks the
                                                     protocol and publishes cell-channel.<name> and
                                                     cell-mesh to the glass under the `mesh` tag)
lora-adapter-band                      -> 31       (the body reads its adapter's safetensors header;
                                                     symbol-voice-band 63 expands symbol lines locally;
                                                     adapters/ carries the Qwen teach overlay and the
                                                     first voice adapter as tree ice)
we-glass-band                          -> 1023     (DO/BE/SEE at five altitudes with a source on
                                                     every row; observe/we-glass-run.fk publishes
                                                     we.glass to the shared bus; we-glass-ask.fk
                                                     answers where any word or node stands)
json-codec-bml-band 8191 · kernel-http-band 536965066 · channel-flow-band 8388607
circle-band 1048575 · static-to-dynamic-cells-band 262143 · bml-capability-ledger-band 255
form-pe-coff-band 16383 · learn/tests/choice-receipt-band 4294967295
                                                  (each compiled for the first time under its own name)
bmf-compiler-runtime 2097279 · bmf-source-scanner-rule-band 4500 · python-bmf-grammar-band 219
python-bmf-from-import-band 54 · python-bmf-class-band 34 · python-bmf-reversible-band 102
language-bmf-program-core 64 · ts-reversible-band 105 · bmf-section-syntax 218
language-packs-fourth-band 31          (every chain through compiler.fk or a grammar died rc 1 on
                                          `::=` until the lane keyed on content, 2026-09-04)
form-cli-allowance-band 2047 · form-cli-live-band 255 · form-cli-mlx-band 63 · form-cli-lens-mint-band 1023
bml-bmf-control-curriculum-band 1048575 · bml-bmf-stream-curriculum-band 16777215
                                          (their `[form.lift]` sources lower in memory)
```

## The local-model lane (Qwen3.8-27B Q8_0, Form-native, Metal JIT)

Form emits every Metal pipeline the dense hybrid walker needs and reads the
geometry from the sealed GGUF header; the frozen open equals the scanned open
row for row (crystal band); a multi-dispatch chain keeps its intermediates on
the device with one wait at the end (handle-door band).

```text
metal-door-band                        -> 15
qwen35-dense-token-handle-band         -> 2147483647
qwen35-crystal-band                    -> 255
llama-token-handle-band                -> 255
kat-token-handle-band                  -> 262143
mlx-derived-band                       -> 16777215
jit-metal-lanes-band                   -> 8191
metal-handle-door-band                 -> 65535
metal-deadline-band                    -> 127     (on the real GPU)
```

The deadline is the caller's: `hearth-metal-deadline-ms` (300000, one Form row
in `hearth.bml`) reaches the carrier through `metal_deadline` before admission,
the door answering the deadline that stood before (-1 when none did); every
command-buffer wait blocks on the kernel and ends in a typed frame
`metal_status` speaks — `wait_frame=completed|error|timeout|released` — with a
timed-out buffer shelved and released by the next wait, its answer exact.

The permanent resident (`observe/form-cli-peer-contribution-live.fk`, the
hearth) is one Form/Qwen/KV peer that receives scannerless tasks from an append
spool and returns length-safe durable results; it blocks on the fifo bell at
idle — no polling core, no HTTP/server/model membrane — announces its birth
capabilities and hands its patience before model admission, and a `release`
byte on the bell closes model and state handles. CPU carries file deltas,
scannerless BMF cursors, recovery and diagnostics; native Metal carries Qwen
and any emitted recipe kernels. The turnwheel mints its own choice receipt, and
receipts carry energy and provenance texture (native / local / remote lanes,
sensed planes witnessed and never billed).

```text
form-cli-peer-direct-answer-action-band    -> 8191
form-cli-peer-policy-route-band            -> 131071
form-cli-peer-stream-ingress-band          -> 2097151
form-cli-peer-contribution-turnwheel-band  -> 33554431
observed-auto-learning-band                -> 32767   (live promotion requires a retained
                                                        equivalence witness, not score alone)
hearth-band                                -> 32767
receipt-texture-band                       -> 16383
```

Timings are not on this floor: none was taken in this pass, and a timing taken
while sibling processes compute on the same host is contention-noised (bands
are exact regardless).

## The knowledge lane

```text
form-knowledge-integration-census-band     -> 1048575   (the census cell counts the
                                                          denominator each run)
form-knowledge-source-search-band          -> 262143
form-knowledge-qwen-heldout-v3-eval-band   -> 65511     (declares 65535; bits 8 and 16 open:
                                                          every row current against its source
                                                          sha, and the dataset sha equal to the seal)
form-cli-heedmark-band 1023 · form-cli-heed-cursor-band 524287
form-cli-heed-current-source-band 16777215 · form-cli-model-generate-heed-report-band 8388607
form-cli-qwen-teach-layer-band 33554431 · lora-adapter-band 31 · error-absorption-kernel-band 4095
nl-lexicon-grow-band 127 · pivot-coverage-band 65535
native-model-route-table-band 255 · ds4-blob-select-band 31
```

The unassisted local-answer baseline is measured at route level, not guessed:
the sealed v3 held-out lane (30 rows, two per family, exact-normalized verifier,
no lexical credit, consent dataset-bound) is the body's defined-correctness
integration number, re-earned only through that sealed door. The model route
decision is a Form data table (`native-model-route-table-band`), and the DS4
engine is discovered at runtime through its directory with header verification
(`ds4-blob-select-band`).

## The string floor

`core.fk` composes `substring` / `str_find` / `str_to_int` over the four-native
waist (`str_len`, `str_byte_at`, `byte_to_str`, `str_concat`). `str_find` is
byte-wise (`core-str-find-equivalence-band` 2047 keeps the old loop verbatim as
its reference).

## The JIT string crossing

`form-lower.fk` embeds compile-time strings and carries a runtime haystack and
a runtime needle+`from` through the same two-slot `fk_inram_args` convention
(`release-ledger.bml` R10 / R28 / R34, all released):

```text
form-lower-string-band                 -> 63
form-lower-string-runtime-band         -> 255
form-lower-string-both-runtime-band    -> 511
jit-evaluator-heat-band                -> 4095    (heat on the evaluator's leaves)
jit-heat-gate-band                     -> 4095    (what crystallizes on heat)
```

Of the policy-spine bands `docs/form-native-jit-track.form` names, these answer
their number today: jit-profile-receipt 127, jit-tier-policy 1023,
jit-runtime-fault 511, jit-inline-policy 1023, jit-deopt-cache 511,
jit-policy-front-sweep 31, form-static-analyzer 16383,
jit-dylib-cache-lifecycle 16777215, jit-dylib-live-runtime-proof 4294967295,
jit-source-runtime-orchestrator 1048575.

## The Glass

One persistent process paints a retained terminal frame from the body's own
observation: ten data views plus help, a front/back frame buffer, row-diff
repaint, and a correlated line-commit control sidecar
(`observe/form-glass-control-run.fk`; `./fkwu observe/form-glass-run.fk` is the foreground
carrier). Keys `h a t o m f j s k v n` choose a view, `1 2 3 4` and `0` select
dialects, `i e c q` inspect, ask evidence, continue, abstain.

```text
form-glass-live-band                   -> 1073741823
form-glass-live-ui-band                -> 1073741823
form-glass-dashboard-band              -> 16777215
form-glass-observer-band               -> 8388607
form-glass-event-loop-band             -> 16777215
form-glass-staged-startup-band         -> 65535
form-glass-launch-band                 -> 32767
form-glass-deadline-cadence-band       -> 4095
form-glass-jit-hold-band               -> 4095
form-glass-meaning-ui-band             -> 8191
form-glass-gift-frame-band             -> 4095
form-glass-sensor-rows-band            -> 255
form-glass-kernel-view-band            -> 511
form-glass-events-channels-band        -> 255
node-gift-band                         -> 4095
cell-store-band                        -> 255
field-band                             -> 255
jit-lens-band                          -> 2047
```

`s` is the meaning view: for zero to four selected dialects (GO, PY, RS, TS —
the four that carry BMF categories in the reviewed table) it samples a bounded
window of that language's real grammar and a bounded window of that language's
real source in this tree, and names the category each construct's own emitter
interns together with its NodeID read from the dialect table:

```text
py ::= import-as ::= "import" $module:name "as" $alias:name => pybmf-emit-import
py -> PY-BMF-IMPORT @1.2.99.501 dialect-categories | verify_category_contract.py: NAME_ALIASES = {
```

Nothing in that view is a fixture, and a sample that is not found says
UNAVAILABLE with its door and reason. The band checks one NodeID against
`form-ontology-bp.fk` so a drifted mirror cannot pass.

Each atlas flow gauge carries the evidence symbol of its own lane, its named
source door, and the standing total beside the per-frame rate — so `G*?.=0u/3M`
(idle now, three million microseconds of GPU work behind it) reads differently
from `C*?.=0u/0` (never ran). Telemetry crosses between processes as files
under a five-second freshness lease; a publisher gone silent is stale for every
lane, the incarnated model owner included, and its last counters stay in the
observation view with their age (`release-ledger.bml` R98, R113).

Telemetry also crosses as a **gift frame**: six seed doors (`shm_offer`,
`shm_receive`, `shm_write`, `shm_read`, `shm_seq`, `shm_release`; tags 184-189) map
a POSIX shared-memory frame with a sixteen-byte seqlock header — seq even is
stable, odd is a give in flight, and a read retries until the sequence it took
equals the one it re-reads, so no reader carries a torn frame. The membrane
gives every published wire into the frame beside the file and reads the frame
first; a child process receives what its parent gave, with no file between them
(`form-glass-gift-frame-band` 4095). Offered, never demanded: release unmaps and
never unlinks, and an absent gift is named absent. Publishers are still indexed
by their files, and a publisher born before this build gives nothing until it
is reborn on it.

A value crosses a gift frame **as itself**: `node_gift_write handle value`
(tag 178) and `node_gift_read handle` (177) carry ints, floats, strings,
`nothing`, lists, trivial nodes, composite cells — category then children,
re-interned on read so the same category over the same children is the same
cell in the reader (axiom 3) — and NodeID coordinates; a function value
refuses the give by name. No wire is written and no parser stands on the
frame path: the reader knows the format because the format is the seed's own
node words (`node-gift-band` 4095, a child process witnessing the same
category). The sensor lane gives `list(schema, sensor, epoch, rows)` this way
and the projection node the glass holds is the same cell the sensor built
(`form-glass-sensor-rows-band` 255). The snapshot publishers in other
processes — owner, hearth, voice, share, governor, jit — still give the text
wire the membrane parses (`release-ledger.bml` R111).

**The frame path is 10 ms.** Measured 2026-09-06 on this M4 Max through
`observe/form-glass-frame-budget-run.fk` — twenty consecutive frames with the
frame processes standing: total mean 10 ms, warm maximum 11 ms, 19 of 20 under
50 ms (the first frame, 53 ms, maps its frames). It was 763–792 ms on the
morning of the 5th and 30 ms that evening. Nothing on the path forks, scans a
directory, opens a file or parses a wire: every row is read from shared memory
or from a door the kernel opens itself.

**`k` is the kernel view**: gift frames mapped and their bytes, functions
defined, and the hottest defns of this process with their source pointers —
`kernel_hot n` answers `heat|name|unit|line|col` from the same per-function
heat the exit report prints, named by the symbol map, in one pass over the
program text (`form-glass-kernel-view-band` 511).

**`v` is the events view and `n` the channels view**: the live samples
selected by the sample's own kind — events, choice points, expert routes,
resolvers, requests, glass flow; channels, channel edges, grammars, mesh, ear
streams, shares, field observers, meaning code — newest first, and when no
sample of a kind is published the view names the organ absent by its door
(`form-glass-events-channels-band` 255). Surprise receipts, choice points,
channel protocols and the grammars have no live publisher yet; each owes one
give into the glass (R112).

**The frame buffer is the read surface.** Every number on the atlas came out of
a shared-memory gift frame, and every row set the glass reads carries the
frame's own witness row — `frame.<sensor>`: the shm name and the sequence it
was taken at — so a lane can say where it read: `G 0 shm:/fg-5f223d19#1024`,
`D 7286422933 shm:/fg-3db9b282#80 + shm:/fg-5f223d19#1024 + shm:/fg-ea12eba1#6878`.
There is no fallback: one id per lane, and `D` `J` `I` sum the counter over
every process that gives a frame. The frames: `machine` (its own fork-free
process, `observe/form-glass-machine-live.fk`, every 50 ms — the host GPU
level and its integral, `host_gpu_utilization` 174 / `host_gpu_busy_us` 175,
host CPU busy over every core from the Mach load info, `host_cpu_busy_us` 173,
and its own kernel counters), `glass` (the glass gives its own kernel, metal,
framebuffer and heat rows into a frame each frame and reads them back like
any other), `host` `process` `storage` `queue` `owner` (the slow sensors,
`observe/form-glass-sensors-live.fk`). The one-shot doors read the same
frames; nothing on any glass path forks or scans. The `k` view lists the
frames read with their sequences. `observe/form-glass-frame-budget-run.fk`
prints the read surface after its twentieth frame. One seam: the
accelerator's `Device Utilization %` is consumed on read and produced about
once a second, so with Activity Monitor open the glass's `G` reads what is
left — 0 (R116).

**No shell on any glass path.** The carrier is a Form cell,
`./fkwu observe/form-glass-run.fk`: it lowers its own priority (`host_nice`),
spawns the two frame processes as argv lists the kernel executes itself
(`host_spawn_quiet` — stdout and stderr to /dev/null so the terminal stays the
glass's), admits and runs the live loop the same way (`host_spawn`,
`host_wait`), asks the supervisor in-process, and ends its children with
`host_kill`. Every host row is a door the kernel opens itself: memory from the
Mach VM statistics (`host_vm_stat`, `sysctl.hw.memsize`), load from
`getloadavg` (`host_load_avg`), disk from every block storage driver's
cumulative statistics (`host_disk_stat`, IOKit by name; rates are the reader's
deltas), processes from libproc (`host_processes name` → pid, resident bytes,
CPU microseconds, elapsed seconds; no `ps`, no `pgrep`), the governor and
launch refresh by argv (`host_capture` where an answer is needed). `tools/`
carries no glass script; the observer carries no text parser for a tool's
output. Tags 151–161.

**The kernel writes its own page.** Every `fkwu` maps `/fg-k<pid>` at its first
dispatch, registers its pid in `/fg-kernels`, and every 1024 primitive
dispatches — and at exit — stores twenty-one words into it in place:
dispatches, heat-lane calls, nodes, strings, cons, fns, gift frames and bytes,
capacities, stack depth, floats, hottest arm, cpu microseconds, alive. No
wire, no serialization, no frame give: a reader maps the same page and reads
the words by offset (`kernel_live_pids` 162, `kernel_live pid` 163). The `D`
and `J` lanes sum those words over every live kernel — `shm:/fg-kernels#4` on
the lens — and the `k` view lists each kernel's page. The hottest defns come
as cells (`kernel_hot_rows` 164), Metal's counters as words (`metal_live` 165:
linked, buffers, pipelines, no-copy buffers, pending, in flight, dispatch,
sync, cpu-jit dispatch and busy, gpu busy, wait, deadline, shelf, batch mode,
slots); the observer and the observation layer read those words, not text.

**The membrane lives in shared memory.** A published snapshot is a cell given
into its publisher's frame; the publisher's name goes into the roster page
`/fg-roster` (`gift_roster_register` 166 / `gift_roster_names` 167, 511 slots
keyed `root|publisher` so a band's space never meets the live one); a reader
lists the roster and takes frames. Control offers and acks are cells in
`<channel>.inbox` and `<channel>.ack` frames; the glass's carried-over cells
(last flow point, last cadence, last pageins) are frames too. The membrane
writes and reads no file — the same thirty-seven organs that publish and read
through it moved with it. What still touches the filesystem, by subject: the
storage sensor (its subject is the catalog) and the queue sensor (the hearth
queue files), both in the sensor process; and the owner-command lease the
glass leaves for a model owner that still reads disk (R119).

**The store is shared memory.** Every value table of a kernel — the node
columns (kind, category word, kids, value, NodeID, source file/line/column,
attribute), both generations of the cons heap, the string bytes and table, the
float pool — lives in one sparse shared-memory reservation per column,
`/fg-c<pid>-<letter>`, sized once and committed page by page (a 4 GiB
reservation touched at three pages costs three pages; the kernel's resident
size is unchanged). A shared table never moves, so another process maps the
same columns and reads any cell by its word — blueprint word, kids, value,
NodeID and source pointer on one surface — with no copy, no wire, no
re-interning: `cell_map pid` (168), `cell_field handle ref k` (169: 0 kind
1 cat 2 kids 3 val 4–7 NodeID 8 source 9 line 10 col 11 attr; for a cons 0 head
1 tail), `cell_value handle ref` (170: a foreign int, string, float or
`nothing` as this process's own value), `cell_ref value` (171: this process's
own word, the reference another process reads by), `cell_unmap` (172). A
foreign word travels as a plain int; the far negatives fold below −2⁶¹ so no
foreign word is ever mistaken for one of the reader's own. The collector melts
between the two heap reservations and the live page says which generation is
current (word 23), whether the store is shared (22), and how many melts (21);
past a reservation the process copies its tables to private memory once and
goes on — never a wall. `cell-store-band` 255: a child interns a composite
over 2 and 3; this process reads kind 2, category `cell-store-band` with NodeID
subtype 2, kids 2 and 3 cons by cons, from the child's columns. Not yet on the
surface: the program AST, the `.fkb` images, `mlx_status` (still text) — R120.

**One field, one word.** The per-kernel store above is the fallback. When the
host offers shared memory every `fkwu` opens the *same* store — `/fg-field-
<letter>`: the node columns, a shared intern index, a shared pair arena for
the kids of shared cells, a shared string pool and a shared float pool for the
values shared cells carry, and a header whose counters every kernel claims
atomically. Interning is one door for every kind: hash by content (strings by
bytes, floats by bits, composites by their children's content), probe the
shared index, compare, and either take the cell another kernel already made or
claim a slot, fill, publish. So a composite interned in one process and again
in another is **one cell with one word** — no second copy, and the field's node
count does not move (`field-band` 255: the child's word equals the parent's,
`kernel_stat 4` before and after equal). The private cons heap, private
strings and floats stay per process for transient values; a value becomes
shared the moment a shared cell carries it, and every reader dispatches by
index range (≥ 2⁴⁰ is the field) behind the same words. The field persists
across processes as a host memory should; `observe/field-reset-run.fk` starts
it over when no other kernel is alive. Live page word 22 reads 2.

**The JIT on the glass.** The kernel charges every float box it mints and
every float box it reads to the defn running (`fk_fn_fbox`, `fk_fn_unbox`),
counts native arm64 leaf calls, and publishes the three totals on its live page
(words 24–26) and as `kernel_stat 45/46/47`. `kernel_hot_rows n` (164) answers
each hot defn as `(heat name unit line col boxes unboxes)`; `kernel_box_rows n`
(191) is the unboxing worklist — the defns minting the most float boxes, the
ones an unboxed float lane would take first. The `j` view shows `jit-boxes`,
`jit-unboxes`, `jit-native-calls`, the worklist rows and the hot defns wearing
both ledgers; `jit-unroll` is a row named absent by its door — the arm64 u32
leaf does not claim the loop (`runtime/fkwu-uni.c:3349`), so no lane in this
seed unrolls one, and the glass says so instead of inventing it.
`jit-lens-band` 2047: a defn adding floats twenty thousand times shows 20001
boxes and 40000 reads on its own row; the int twin shows 0 and 0.

**The kernel counts into the page.** No counter on the live page is copied
there. `fk_arms`, the heat, box, unbox and native-call totals, and the three
per-defn ledgers are pointers the kernel repoints into `/fg-k<pid>` when the
page opens (`fk_live_open`, at `fk_nodes_init`): the increment the dispatch
loop already does IS the write the glass reads. There is no tick, no publish
cadence, no serialization -- the 1024-dispatch sampler that carried the words
before is gone, and a 20-million-iteration loop runs 0.89 s -> 0.67 s (int),
0.98 s -> 0.78 s (float) on this Mac. A defn's name, unit, line and column land
in the page meta the moment it is defined (`fk_live_note_defn`, at every
recording site including `.fkb` ice), so any process reads any kernel's hot
defns with source from the page alone: `kernel_page_hot pid n` (192) and
`kernel_page_box pid n` (193). The words that change at moments -- nodes,
strings, cpu, alive, store, melt generation -- are written where the moment
happens (open, field open, melt, exit, self-read). The `k` view lists every live
kernel's three hottest defns as `k<pid> <defn> <unit>:<line> box n unbox n`.
`jit-lens-band` 2047 and `cell-store-band` 255 stand on the page words.

**The box ledger fires the leaf.** The per-defn box count is not only shown,
it acts: when a cold defn's count crosses a 1024 boundary, `fk_fbox` -- the
increment that was already there -- asks `fk_f64_pulse` once whether the
defn's body is a pure float expression (float and int literals, parameters,
add/sub/mul/div with a float on at least one side). If it is, the body is
emitted as an arm64 f64 leaf: parameters in d0..d7, intermediates in
d16..d31, one FMOV and RET on a MAP_JIT page. The defn's body entry becomes a
tag-194 node carrying the fn index and the original body, so dispatch pays
nothing new: an all-float frame unboxes once and boxes once; any other frame
walks the original body and answers what the walker always answered. Declined
bodies are marked -1 and never asked again; a reload clears every leaf. Native
state is the page's fourth ledger (+96 MiB, layout 3), word 30 and
`kernel_stat 48` count the crystallized defns, hot rows carry native as their
eighth field, the `j` view has `jit-crystallized` and every crystallized hot
defn wears ` native`. A five-op polynomial called two million times: 0.25 s
-> 0.12 s, four boxes per call -> one.

## Beliefs, ledger, drift

```text
./fkwu observe/belief-stamps.bml           -> field stamped*10^6 + owed*10^3 + laws = 495459011
observe/tests/belief-rewitness-band        -> 63         (the re-witness door, observe/belief-rewitness.bml)
./fkwu form/form-stdlib/release-ledger.bml -> open=47 moving=0 released=76 -> 47000076
./fkwu gate/drift-gates-run.bml            -> pass=2015 full=2047 refused=32 names=kernel-conformance

Every row of that door is a Form lens now — `gate/op-manifest.bml`,
`native-surface`, `category-contract`, `primitive-registry`, `flt-ops-gen`,
`ontology`, `kernel-conformance` — each byte-agreeing with the Python twin it
replaced on the live tree and on a planted-drift tree, each with a band
(1023 · 1023 · 255 · 511 · 63 · 255 · 511) and none of them calling `python3`
(R58); `native-surface` also reads `#define FK_TAG_*` sites and refuses a manifest
row that lands on an internal walker tag. The writer half of `flt-ops-gen` and the FORMBIN2 interop witness are
the Python that remains (R59, R60).
```

Every tracked cell's `witnessed:` stamp is read into the belief lens; a stamp
older than the seed is where an afterwall grows, and the lens keeps that list
in front of the body oldest first. The re-witness door renews a stamp only from
a real fresh band run and reports a mismatch as a lapse, never silently.

## Not standing today

What answered red or nothing in this pass, so no one leans on it:

- `control/tests/invite-dispatch-band.fk` answers 763 of its declared 1023
  (preflight clean): bit 4 (a second `<CHOICE>` finding nothing declining) and
  bit 256 (`<TIMEOUT>`) are open.
- `blueprint-authority-band` 51199 of 65535, exit 1: `value_kind` is a native
  the Go/Rust/TS kernels carry and fkwu does not — a lane seam.
  `persistence-band` 2 of 7 and `channel-breath-band` 200 of 500 stop on
  `write_form_binary` the same way; `concept-i18n-band` answers its input-absent
  word with `read_form_binary`/`write_form_binary` unresolved beneath it.
- `mesh-sensings-route-band` 63, `sense-loop-band` 8191,
  `native-mutation-route-side-effects-band` 11111 and `verb-router-band` 3
  each reach their declared verdict yet exit 1: a Go/Rust-only native
  (`write_form_binary`, `recipe_to_bytes`, `pg_exec`) sits unresolved in a
  prelude the run never reaches — lane seams, not defects.
- BML `match` is not lowered on fkwu (`source-language-match-switch-band` 0;
  R77) and `import Num;` binds nothing (`bml-import-ref-resolution-band` 2111
  with `Num` unresolved; R78).
- `form-source-sections` answers 64 errors: `fk-lit` is defined only in
  `hati-os-kernel.fk` and the `bml-source-*-rule-index` names resolve nowhere in
  its chain (`release-ledger.bml` R87). Of the `[form.action]` bands now
  reaching the lane as main files, `form-action-dialect-band` 20 and
  `zero-arg-functions` 19 answer; `higher.fk` and `lists.fk` leave `sum`,
  `any?`, `all?` unresolved after lowering, and `json-meaning-ingestion-band`
  and `runtime-grammar-selector-registry-band` die measuring an absent input
  (R86). Preflight vouches such chains clean — it counts unresolved calls, and
  a rule line is not a call.
- `form-knowledge-exec-grammar-transport-band` dies rc 1 on `str_len` of
  nothing; the domain/organ/unique/universe-mint bands answer 2015 of 2047
  (bit 32, held-out lineage, stamped pending 2026-08-26); `form-cli-gpu-band`
  1009 of 1023 (bits 2/4/8, live `mlx_run` attention numerics) (R88).
- The BML section scanner is line-based: a comment line ending in `{` counts
  as a block opener, and the section then reports "not closed before end of
  source" pointing nowhere near the prose that opened it (R93).
- The BML lowering's depth is bounded but its time is not: one form of 500
  arguments lowers in 0.2s, 1,000 in 0.6s, 2,000 in 2.8s — `append`
  (`line-grammar.fk:88`) is non-tail over its left list and the grammar's arg
  loops call it per item (R96). The same lowering reads a `; preludes:`
  substring inside a string as a directive (R92), and its child's stdin door
  answers 1 when the two lines arrive as two writes instead of one (R95).
- A bare `nothing` in a `.bml` def (`if nothing?(h) then nothing else …`) lowers
  to a raw word that prints as -8000000000000000009 and is not `nothing?`; the
  body writes the call, `nothing()`, and the lowering owes the bare name a
  refusal or the axiom-1 value (R103).
- The snapshot publishers in other processes still give the text wire
  `fgtm-snapshot-wire` the membrane parses; each owes a move to
  `node_gift_write` (R111). Surprise receipts, choice points, channel protocols
  and the grammars have no live publisher; the `v` and `n` views name them
  absent by door (R112). The wait that wakes on a telemetry or control path
  change is still owed; a bounded native rest paces the frame (R107).
  `host_sleep_ms` rests 2–5 ms past the ask on this host (R109).
- `observe/tests/jit-register-lowering-band.fk`,
  `jit-representation-specialization-band.fk` and `jit-stack-frame-band.fk`
  answer nothing: each file ends with one paren open
  (`[input-ended-mid-form]`).
- The `kernel-conformance` row of `gate/drift-gates-run.bml` refuses in this
  checkout: the TypeScript kernel's dependencies are absent, and the row names
  its own remedy (`npm ci` in `form/form-kernel-ts`).

## Honest seams

- The consent file for the v3 lane
  (`.form-knowledge-qwen-heldout-v3-consent`) is a per-run local act, ignored
  by git, never committed.
- `https://hati.earth/sema/.well-known/ai-plugin.json` answered a Cloudflare
  `error code: 522` body (origin unreachable) at this observation, so its
  `description_for_model` could not be compared with `plugin/ai-plugin.json`;
  the publish checklist in `plugin/README.md` stays owed a run.
