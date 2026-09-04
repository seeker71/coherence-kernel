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

`runtime/fkwu-uni.c` is 16,353 lines — a temporary seed and shrink target, not
the destination (`release-ledger.bml` R13); this week's growth on it is
correctness heals (#573 nested-defn scope, #574 bool literals, #575 kernel
preludes, the host-exec stdin door, `metal_deadline` off its scratch slot).

## Body-wide witnesses

```text
gate/structural-gate-run.fk            -> [206, 0, 48, 3, 20, 57, 74, 4] then 1
gate/tests/structural-gate-band        -> 8191
observe/door-link-health-run.bml       -> doors=12 links=63 broken=0 code=12063000
observe/body-link-graph.fk             -> body-link-graph-check 63; blg-field-code 13029046
                                          (13 orphans, 29 broken, 46 candidates; the organ
                                          has no run door — prelude it and call both)
homecoming-distillation-corpus-band    -> 32767   (asserts 658 rows, 647 admissible)
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

A `.bml` runs directly (`./fkwu file.bml`); a `.fk` names its `.bml` preludes
and fkwu lowers them in memory, owning the `.bml.fkb` cache itself. `true` and
`false` are literals in the `section [form.bml] { def ... }` dialect; a nested
`defn` is a registered function with one-level capture. Every file carrying
`section [form.bml]` wears `.bml` (75 renamed 2026-09-04); `section [form.lift]`,
`[form.action]` and `[form.route]` files keep `.fk`.

```text
bml-band                               -> 268435455
bml-generics-band                      -> 16777215
native-route-goal-cells-band.bml       -> 1048575  (full)
nested-defn-scope-band                 -> 63
nested-defn-closure-capture-band       -> 63
bml-float-literal-band                 -> 2047     (a decimal float reads back as the same float
                                                     from eleven positions; the emitter refuses an
                                                     unknown leaf by name instead of writing "")
json-codec-bml-band 8191 · kernel-http-band 536965066 · channel-flow-band 8388607
circle-band 1048575 · static-to-dynamic-cells-band 262143 · bml-capability-ledger-band 255
form-pe-coff-band 16383 · learn/tests/choice-receipt-band 4294967295
                                                  (each compiled for the first time under its own name)
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
./fkwu observe/belief-stamps.bml           -> field stamped*10^6 + owed*10^3 + laws = 489459005
observe/tests/belief-rewitness-band        -> 63         (the re-witness door, observe/belief-rewitness.bml)
./fkwu form/form-stdlib/release-ledger.bml -> open=30 moving=0 released=53 -> 30000053
./fkwu gate/drift-gates-run.bml            -> pass=2015 full=2047 refused=32 names=kernel-conformance
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
- Thirty-two `form-stdlib/*-xtal.fk` prelude targets are absent from the tree
  and about sixty cells — bands and run doors, `form-cli-live-band` and
  `form-cli-mlx-band` among them — die at load on them (rc 2): the flatten
  lane that emitted the mirrors is gone, and each cell owes a repoint to its
  `section [form.lift]` source (`release-ledger.bml` R82).
- The ten `form-stdlib/grammars/*-bmf.fk` files carry the BMF rule dialect
  (`::=`, `=>`, `$name:name`; `python-bmf.fk` alone holds 176 rules) and fkwu
  reads them raw as Form: every band whose preludes reach `python-bmf.fk` —
  35 under `form-stdlib/tests`, none fourth-arm registered — dies with 1,700
  or more `[unbound-name]` errors, rc 1 (`python-bmf-grammar-band`,
  `python-bmf-from-import-band` 1766, `python-bmf-scanner-real-syntax-band`
  1699). The dialect has no lowering lane in the seed the way `.bml` has
  (`release-ledger.bml` R84). Preflight reports these chains clean: it counts
  unresolved calls, and a rule line is not a call.
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
