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

`runtime/fkwu-uni.c` is 16,406 lines — a temporary seed and shrink target, not
the destination (`release-ledger.bml` R13); this week's growth on it is
correctness heals (#573 nested-defn scope, #574 bool literals, #575 kernel
preludes, the host-exec stdin door, `metal_deadline` off its scratch slot, the
content-keyed lowering lane).

## Body-wide witnesses

```text
gate/structural-gate-run.fk            -> [206, 0, 48, 3, 20, 57, 74, 4] then 1
gate/tests/structural-gate-band        -> 8191
observe/door-link-health-run.bml       -> doors=12 links=63 broken=0 code=12063000
observe/body-link-graph.fk             -> body-link-graph-check 63; blg-field-code 13029046
                                          (13 orphans, 29 broken, 46 candidates; the organ
                                          has no run door — prelude it and call both)
homecoming-distillation-corpus-band    -> 32767   (asserts 660 rows, 649 admissible)
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

## Beliefs, ledger, drift

```text
./fkwu observe/belief-stamps.bml           -> field stamped*10^6 + owed*10^3 + laws = 495459011
observe/tests/belief-rewitness-band        -> 63         (the re-witness door, observe/belief-rewitness.bml)
./fkwu form/form-stdlib/release-ledger.bml -> open=38 moving=0 released=57 -> 38000057
./fkwu gate/drift-gates-run.bml            -> pass=2015 full=2047 refused=32 names=kernel-conformance

Every row of that door is a Form lens now — `gate/op-manifest.bml`,
`native-surface`, `category-contract`, `primitive-registry`, `flt-ops-gen`,
`ontology`, `kernel-conformance` — each byte-agreeing with the Python twin it
replaced on the live tree and on a planted-drift tree, each with a band
(1023 · 511 · 255 · 511 · 63 · 255 · 511) and none of them calling `python3`
(R58). The writer half of `flt-ops-gen` and the FORMBIN2 interop witness are
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
