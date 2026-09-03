# Current Floor

Re-measured 2026-09-03 on this Apple M4 Max through the resolver-driven `./fkwu`
door (the binary passes the freshness band, 31). Receipts hold history; this
page holds only what stands now. Every number below was re-run today; a claim
without a number names the band that declares its own.

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

`runtime/fkwu-uni.c` is 15,365 lines today — a temporary seed and shrink target,
not the destination (`release-ledger.bml` R13).

## Body-wide witnesses

```text
gate/structural-gate-run.fk           -> [207, 0, 49, 3, 20, 57, 74, 4] then 1
gate/tests/structural-gate-band        -> 8191
door-link-health  (dlh-field-code)     -> 12065000   (12 doors, 65 links, 0 broken)
body-link-graph   (blg-field-code)     -> 13028050   (13 orphans, 28 broken, 50 candidates)
homecoming-distillation-corpus-band    -> 32767      (652 rows, highest meaning-id 1260)
no-fixed-tables-band                   -> 1          (the seed's tables grow; no fixed cap)
form-cli-author-high-band              -> 4095
host-os-membrane-band                  -> 8191
bidirectional-framebuffer-channel-band -> final field 1
form-eval-band 65535 · form-eval-full-band 635 · source-compiler-grammar-bridge-band 32767
pattern-match-band 511 · choice-lane-core-band 1023 · offer-ack-core-band 2097151
control-invite-grammar-band 1023 · node-introspection-band 4095
cell-serialize-band 1023 · json-band 1023 · wire-rpc-band 15 · core-str-find-equivalence-band 2047
```

## The local-model lane (Qwen3.8-27B Q8_0, Form-native, Metal JIT)

All MSL is Form-emitted and JIT-compiled at runtime for the device that answered
`metal_status`; weights stay mmap-backed no-copy; the file is admitted by a
whole-file Form SHA-256 seal; the frozen open equals the scanned open row for row.

```text
metal-door-band                        -> 15
qwen35-dense-token-handle-band         -> 2147483647
qwen35-crystal-band                    -> 255
llama-token-handle-band                -> 255
kat-token-handle-band                  -> 262143
mlx-derived-band                       -> 16777215
jit-metal-lanes-band                   -> 8191
metal-handle-door-band                 -> 65535
```

The permanent resident (`observe/form-cli-peer-contribution-live.fk`, the hearth)
holds one model admission per artifact lifetime behind a FIFO bell, with
hot-swappable JIT policy, declared birth capabilities, source/recipe/patch/
direct-answer effects, turnwheel append, and content-free stage diagnostics. A
policy can select only capabilities present at birth; a successor is born for a
new effect.

```text
form-cli-peer-direct-answer-action-band    -> 8191
form-cli-peer-policy-route-band            -> 131071
form-cli-peer-stream-ingress-band          -> 2097151
form-cli-peer-contribution-turnwheel-band  -> 16777215
observed-auto-learning-band                -> 32767   (live promotion requires a retained
                                                        equivalence witness, not score alone)
hearth-band                                -> 16319
```

Correctness bounds in force: cooperative RMS only at width <= 4096 (wider rows
take the serial attestant); one 256-thread threadgroup per head for GQA decode;
both block kinds batched; batched span prefill is the default for hinted
current-source contexts while the plain one-shot door still walks token-major
prefill.

Timings are not on this floor: none was re-taken today, and a timing taken while
sibling processes compute on the same host is contention-noised (bands are exact
regardless). The distances that remain are named, not numbered:

1. **Prefill parity** with the outside denominator (llama.cpp on the same file):
   the span lane exists at identical output and is default only on heed lanes;
   promoting it into the plain door is the stone.
2. **Decode parity**: the width-independent cooperative RMS (simd tree, no
   `sq[n]`) is the named kernel; the small GDN kernels follow.
3. **Residency visibility**: cold admission is paid once per residence. Since
   2026-09-03 the deadline is the caller's: the resident hands
   `hearth-metal-deadline-ms` (one Form row, `hearth.bml`) to the carrier
   through `metal_deadline` before admission and prints
   `admission-deadline-ms`; every command-buffer wait blocks on the kernel (no
   poll, no backoff) and ends in a typed frame `metal_status` speaks —
   `wait_frame=completed|error|timeout|released` — with a timed-out buffer
   shelved and released by the next wait, never lost
   (`form/form-stdlib/tests/metal-deadline-band.fk` 127 on the real GPU). What
   remains is the client-side reading of that frame across the pipe.
4. **The emitted walker's zero-token seam**: form-cli's REPL lane arms no
   glossary and can return zero generated tokens on a composed prompt; the
   T_flat frame-slot convention is the named repair behind it.
5. **The tokenizer**: the sorted fixed-row index (`qwen35-tokfast-v2`) carries
   encode; the per-open seal verdict (a whole-file SHA-256) folds into the
   resident.

## The knowledge lane

```text
form-knowledge-integration-census-band     -> 1048575   (the census cell counts the
                                                          denominator each run)
form-knowledge-source-search-band          -> 262143
form-knowledge-qwen-heldout-v3-eval-band   -> 65511     (declares 65535; two bits open)
form-cli-heedmark-band 1023 · form-cli-heed-cursor-band 524287
form-cli-heed-current-source-band 16777215 · form-cli-model-generate-heed-report-band 8388607
form-cli-qwen-teach-layer-band 33554431 · lora-adapter-band 31 · error-absorption-kernel-band 4095
```

The unassisted local-answer baseline is measured at route level, not guessed:
the sealed v3 held-out lane (30 rows, two per family, exact-normalized verifier,
no lexical credit, consent dataset-bound) is the body's defined-correctness
integration number, and it is re-earned only through that sealed gate. The
resident's recipe-only route answers a typed `<FAIL>` on a held-out row; the
direct-answer effect executes a live turn. Curriculum / RAG / LoRA credit waits
on the per-family live observations, never on file presence. `LoraWriter = 0`:
no adapter tensor is written yet.

## The string law

`core.fk` composes `substring` / `str_find` / `str_to_int` over the four-native
waist (`str_len`, `str_byte_at`, `byte_to_str`, `str_concat`). `str_find` is
byte-wise (`core-str-find-equivalence-band` 2047 keeps the old loop verbatim as
its reference). Encoding remains the walker's largest fair-priced cost; the
tokenizer index above is the answer in force.

## Not standing today

What answered red or nothing on 2026-09-03, so no one leans on it:

- `control/tests/invite-dispatch-band.fk` answers 763 of its declared 1023
  (preflight clean): the second-`<CHOICE>` bit and the `<TIMEOUT>` bit are open.
- `form/form-stdlib/tests/blueprint-authority-band.fk` answers 51199 of 65535;
  `persistence-band` 2 of 7; `channel-breath-band` 200 (declares 500).
- `channel-flow-band` answers nothing (compile refused: unbound name in value
  position); `observe/tests/import-carry-band.fk` answers 0 of 63.
- `native-route-goal-cells-band` (R31, healed to `.bml`, its true grammar)
  now compiles and answers 643070 of 1048575 on fkwu: the five open bits
  (goal-valid?, observations-valid?, attentions-valid?, and the two
  manifest-status bits) all trace to one root cause — a bare `true`/`false`
  BML literal lowers to 0 in the `section [form.bml] { def ... }` dialect,
  so every branch that returns the literal `true` reads as false.
- Bands whose preludes name a released `-xtal` twin die at load (rc 2):
  `form-cli-mlx-band`, `form-cli-live-band`, `bml-bmf-stream-curriculum-band`,
  `bml-bmf-control-curriculum-band` — the twins release
  (`release-ledger.bml` R1/R2) owes them their surviving prelude.
- The JIT ladder (`docs/form-native-jit-track.form`): of the bands it names, 45
  are absent from the tree and most of the rest answer nothing; nine answer
  green (listed there).
## Honest seams carried forward

- The C seed is 15,365 lines; the shrink direction stands and this number is on
  the floor so the debt stays visible.
- The consent file for the v3 lane is a per-run local act, never committed.
- The served manifest at `hati.earth/sema` carries a `description_for_model`
  that is not byte-identical to `plugin/ai-plugin.json` (re-observed
  2026-09-03); the publish checklist in `plugin/README.md` is owed a run.
