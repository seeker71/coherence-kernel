# Local reasoning homecoming — who holds which lane

2026-08-24. Three lanes work this prompt. This file is the seam between us —
read it before you claim, append when you land, and replace stale status when
a later witness lands.

| lane | agent | current witnessed state |
|---|---|---|
| Form-knowledge census + ≥95% held-out integration metric | Codex + local Qwen | v1 baseline was 1/15; v2 `h01` is a live source-hit answer at 1,000,000 ppm; `h03` retrieves cleanly but remains 333,333 ppm after answer-contract retry; full denominator still owed |
| Form-native query execution token + current-source substrate | Codex + Claude crossing | one streamed query, sealed path hint, attributed hit, typed injection, and answer are directly witnessed on real Qwen weights |
| unseen BML/BMF live-stream embodiment | Claude | tasked after pushed commit `c427ea85`; live movement in progress, no result claimed here |
| Qwen dispatch/prefill cost, and the body's own text speed | Claude (jit-lane) | prefill 640,326 → 118,681 dispatches byte-identical, floor reached at 75,769 but NOT decided (19% host spread); `str_find` byte-wise (44x at 3.7 MB, no longer degrading) and `substring` joined as a balanced tree (284x at 200 KB), at 1,596 and 847 call sites, each with an equivalence band keeping the OLD implementation verbatim as reference |
| Curriculum fidelity — glosses vs their own cells | Claude (jit-lane) | five of seven family glosses disagreed with the cell they name; re-derived 2026-08-25; sealed rows re-asking under the corrected curriculum as of 23:05 WITA, result not yet in |

## A curriculum audit anyone can repeat, and what it found

The zero-families receipt established that "the audit is measuring the
curriculum, not the model's mood." The obvious next question is whether the
curriculum says what its sources say. Reading each family's gloss against the
cell it names, five of seven disagreed, in three classes:

- **Wrong.** `ingest-name-build-observe` taught a two-state law where
  `ingest/name-build-observe.fk` states three and `nbo-enter` returns 2/1/0 —
  a bare name waits at the door, an attempt that fell short enters as a
  *lesson*, only the observed enters as a *claim*. Its "only" made the middle
  state impossible to infer. `axiom-5-offer` taught **axiom-4**'s content
  ("never by force" — a word absent from the axioms file) under axiom-5's name,
  dropping axiom-5's own statement.
- **Undercounting.** `control-choice-lanes` named five invites where its cell
  says "THREE of the eight" plus "the other FIVE". `axioms-smallest-set` gave
  no count and named no axiom.
- **Discipline dropped.** `learn-distillation` taught that answers come home as
  rows, without the corpus's own practice (the *smallest* question needing *one
  fresh word*) or its warning that generalizing is learning and echoing is
  copying.

`teachings-service` and `axiom-1-states` were faithful and untouched — and
`teachings-service` scored highest of the ten, which is the finding pointing at
itself.

**The method is cheap and repeatable**: open the gloss, open the cell it names
in the paths table of `form-cli-qwen-teach-layer.fk`, and read them against
each other. No model, no GPU, no sealed row. The constraint that makes it safe
is the curriculum's own comment — glosses come from each family's own cells,
never from sealed audit rows — and the teach-layer band's two leakage bits hold
it. Corpus row 1122 `shutgloss` names the dominant defect: a short account
whose claim of completeness makes what it omitted unreachable.

## Two cross-lane facts, 2026-08-25, that change what other lanes may assume

**`str_find` is 44x faster and no longer degrades with length.** It is not a
fkwu native; `core.fk`'s definition is the only one the body has. It used to cut
a fresh `substring` at every position: 3.30 s over 1 MB, 21.92 s over 3.7 MB,
and rising per megabyte. Byte-wise now: 0.21 s and 0.50 s. Ten core/string bands
byte-identical, and `core-str-find-equivalence-band` (2047) keeps the old loop
verbatim as its reference.

For the tokenizer lane specifically: `qwen35-tokfast.fk` records that v1's
11 MB joined blob was "priced out on the present fkwu string waist: lookup must
not depend on a whole-blob `str_find`." Re-measured on the new one, a whole-blob
`str_find` over 11.07 MB costs **0.92 s** worst case. That constraint still
holds — 0.92 s per lookup is 205 s for a 223-token prompt, so v2's binary search
over fixed-width rows remains the right design. What did change is the one-shot
cost: the freeze step and any single blob scan went from tens of seconds to
under one.

**An out-of-bounds byte read is answered by three arms and is a deliberate
fatal on two — and WHICH BINARY is the whole fact.** The cell is the single
expression `(str_byte_at "" 0)`, no prelude, one machine, one minute:

| arm | answer | rc |
|---|---|---|
| `./fkwu` | `-1` | 0 |
| `walkers/go` via `go run .` | `-1` | 0 |
| `walkers/rust` via `cargo run` | `-1` | 0 |
| `form/form-kernel-go/bin-go` | crash trace | 1 |
| `form/form-kernel-rust/target/release/form-kernel-rust` | crash trace | 1 |

The two full kernels write `str_byte_at: bounds out of range index=0 len=0`, and
the trace carries its own avoidance line — "check length/bounds before indexing
or use a boundary-aware recipe that returns an explicit error value" — so the
fatality is those kernels' intent, not their accident. Position does not matter:
the same call inside a `defn` behaves as at top level.

So a cell that indexes without checking runs on three arms and dies on two.
Guard before you index.

This entry first said "go and rust" without naming which, a sibling could not
reproduce it on the minimal walkers, and they were right — there it answers -1.
The claim was true at the wrong resolution. Name the binary.

## What is already home (current worktree, witnessed 2026-08-24)

| piece | cell | state |
|---|---|---|
| byte-BPE tokenizer, GGUF vocab/merges, byte-exact decode, chat template | `form/form-stdlib/qwen35-tokenizer.fk` | exists |
| decode loop transcribed from ds4-engine C | `form/form-stdlib/dsv4-decode-loop.fk` | exists |
| per-token hook: an arbitrary Form recipe answers between argmax and the next embedding | `dsv4-decode-hook-door.fk`, `dsv4-decode-token-hook.fk` | band 1023 |
| single forward step: one id + position → next id | `form/native/metal/qwen35-dense-token-handle.fk:240` `q38-forward` | exists |
| **incremental prefill at a position, same state** | same file `:274` `q38-prefill` | exists — this is the KV-preserving seam |
| span injection into a live stream (pre-computed "thoughts") | `form-cli-model-generate.fk:204` `fcmg-offer-stream` | exists |
| RAG: embed, index codec, ask, adaptive-k, freshness, nearest-shape | `form/form-stdlib/rag-*.fk`, `nearest-shape.fk` | exists |
| bounded raw-byte heed cursor: decoded output → query → typed prefill | `form-cli-heed-cursor.fk` | band 65535 |
| attributed lookup against the current Form source body | `form-cli-heed-current-source.fk` | band 8388607; host-filesystem/fkwu lane |
| end-to-end split-envelope cursor → lookup → incremental prefill | `tests/form-cli-heed-current-source-cursor-band.fk` | band 65535; scripted model functions, real source lookup |
| generation-path wiring, two ledgers, sealed-path context hint, bounded counters | `form-cli-model-generate.fk`, `tests/form-cli-model-generate-heed-report-band.fk` | report band 2097151; one real `h01` hit separately observed |
| teach overlay, local-ready marks, one-turn budget | `form-cli-local-ready.bml`, `form-cli-one-turn.bml` | band 1023 each |
| Form knowledge mint | `form-cli-knowledge-mint.bml` | band 1023; n=24 unique=24 leakage=0, heldout 6/6 |
| Universe skill mint (BML, BMF, lift, lookup, craft, framebuffer, micro-thought, evaluate) | `form-cli-universe-mint.bml` | band 1023; n=32 unique=32 leakage=0, heldout 8/8; scale-12-4=128 |
| Unique-cell mint (channel, protocol, axiom, satsang, nothing, timeout, trust, choice-undo, embodied-memory) | `form-cli-unique-mint.bml` | band 1023; n=36 unique=36 leakage=0, heldout 9/9; scale-12-4=144 |
| Domain-cell mint (assemblage, HD, Gene Keys, astrology, physics, chemistry, biology, quantum, math, logic, shape-match, time-direction) | `form-cli-domain-mint.bml` | band 1023; n=48 unique=48 leakage=0, heldout 11/12; scale-12-4=192 |
| Organ mill (phase, frequency, vitality, consent, four-way, collapse, JIT, I Ching, codon, IFS, organic, rag-heed) | `form-cli-organ-mint.bml` | band 1023; n=48 unique=48 leakage=0, heldout 11/12; scale-12-4=192 |
| BML/BMF live-byte curriculum and control curriculum | `bml-bmf-stream-curriculum.*`, `bml-bmf-control-curriculum.*` | bands 16777215, 1048575; grammar-agreement band 1023 |
| Qwen teach overlay with the new semantics | `form-cli-qwen-teach-layer.fk` | band 16777215; 29 concepts/pairs, semantic heldout 10/10, exact/family leakage 0; answer extraction now preserves source words and requested shape |
| LoRA identity `(W+B·A)·x == W·x + B·(A·x)` | `lora-adapter.fk` | band 31 |
| LoRA tensor writer | — | **0**. `LoraWriter = 0`, `fqt-lora?` 0 |

## Current seam: one resident source-hit answer, full denominator still owed

`form-cli-model-generate.fk` constructs the live Qwen context and sends every
decoded token through the bounded cursor. Query tokens have 48 steps; 32 answer
steps are non-borrowable until a typed hit or miss enters. A completed query is
looked up once, encoded, and returned through the same-state `q38-prefill` seam.

The first v2 `h01` run made a complete query but omitted a usable full path, so
Form returned `nothing` and left the answer reserve untouched. A first inferred
prompt-hint repair passed its scripted band and changed no live result. The
contradiction exposed the actual seam: the held-out row already carried
`axioms/core-axioms.form`, while its prose named only `core-axioms.form`.

The evaluator now carries that sealed row path beside the unchanged evaluated
prompt. The live repetition scored **1,000,000 ppm**, with one query attempt,
one source lookup, one hit, 572 injected observation tokens, one close commit,
29 query tokens, 19 query tokens cut, one answer token, and no timeout or
refusal. The reason was `current-source-context-path-hit`. This is a real local
Qwen source-query-answer crossing. It is one family, not a ≥95% census.

## Landed by claude — the observed heedmark mechanism

A **heedmark** is a Form-native execution token: the model writes it into its
own output as ordinary text, the carrier heeds it, Form looks the query up, and
the answer re-enters as prefill at the current position. The word is 0-hit
fresh in this tree as of today.

The mechanism and its current live-source integration are carried by:

- `form/form-stdlib/bml/form-cli-heedmark.bml` — executable grammar source
- `form/form-stdlib/form-cli-heedmark.bml` — executable lowering
- `form/form-stdlib/form-cli-heedmark-compile.fk`
- `form/form-stdlib/form-cli-heedmark-xtal.fk` — generated
- `form/form-stdlib/form-cli-heedmark-run.fk` — evidence printer
- `form/form-stdlib/tests/form-cli-heedmark-band.fk` — **1023 on fkwu**
- `form/form-stdlib/form-cli-heed-cursor.fk` — bounded streaming cursor
- `form/form-stdlib/form-cli-heed-current-source.fk` — attributed source lookup
- `form/form-stdlib/form-cli-model-generate.fk` — resident generation-path seam
- `form/form-stdlib/tests/form-cli-heed-cursor-band.fk` — **65535 on fkwu**
- `form/form-stdlib/tests/form-cli-heed-current-source-band.fk` — **8388607 on fkwu**
- `form/form-stdlib/tests/form-cli-heed-current-source-cursor-band.fk` — **65535 on fkwu**
- `form/form-stdlib/tests/form-cli-model-generate-heed-report-band.fk` — **2097151 on fkwu**

Evidence, `./fkwu form/form-stdlib/form-cli-heedmark-run.fk`:

```
check=255            (band adds 256 descriptor + 512 grammar agreement = 1023)
logits-executed=0    the standing refusal, a named constant
outcome grounded-row=hit   no-row=miss   window-closed=nothing
        no-index=nothing   budget-gone=spent
span-enters      hit=1 miss=1 nothing=0 spent=0
knowledge-enters hit=1 miss=0
admits-hit no-source=0  with-source=1
bounded 0-marks=0  1-mark=1  5-marks=2      (MaxHeeds=2 — the bound)
prefill-cost=12  naive-cost=1012  forwards-saved=1000  prefix-preserved=1
```

Preflight is clean (`parens balanced, errors 0, warnings 0, unresolved 0`).
The former three-call compile seam is now separated by meaning.  Pure Form text
compilation no longer preludes the ontology loader's `walk_recipe_here`, runtime
image persistence moved to the explicit `source-compiler-runtime-image.fk`
carrier, and file-prefix reads use the cursor's bounded `read_file_slice`
window rather than `file_byte_at`.  `form-cli-one-turn-compile.fk` and the mint
compilers preflight clean on fkwu; the optional runtime-image carrier remains
loud on fkwu because `write_form_binary` is still a genuine sibling capability.

### What the current observations say, in four rows

- **hit** — a grounded row above threshold. Knowledge enters **with
  attribution**; `admits-hit` refuses a hit whose source is empty.
- **miss** — the lookup ran and found nothing. A *named status* enters
  (`no grounded row`), not content, so the model is not left to invent a row
  the body does not hold.
- **nothing** — the lookup could not answer inside the window (no index, or
  late). Axiom-1: nothing enters. Silence is whole.
- **spent** — the per-turn budget is gone. The mark is **not heeded** and stays
  plain text. This is the bound: five marks under `MaxHeeds=2` honor exactly 2.

### Two distinctions retained by the observation

1. `heed_model_executed=0` belongs to the lookup observation: the local model did
   not execute repository IO. The surrounding Qwen forward passes did execute;
   the model asked, Form looked, and the typed observation re-entered the model.
2. A direct band witnesses mechanism shape. The `h01` diagnostic separately
   witnesses real weights and a scored answer. Neither one row nor one band is
   allowed to stand in for the fifteen-family denominator.

## Effective Form reasoning in practice

Write the teaching as executable BML, carry the custom grammar as BMF data,
and feed raw decoded bytes into the live cursor. Do not require a tokenizer
pre-step: an incomplete frame is retained within the bounded cursor, a complete
frame lowers and runs, and malformed or over-budget input becomes `nothing`.
The stream curriculum checks every single split point, bytewise chunks,
incomplete and malformed inputs, and bounded state in
`tests/bml-bmf-stream-curriculum-band.fk` (**16777215**). Its BML/BMF source
shape is separately checked at **1023**.

Use the reasoning controls by their observed behavior:

| need | Form move | practical contract |
|---|---|---|
| first usable answer | `oac-choice` | Walk in order, skip `nothing`, return the first non-`nothing` acknowledgement. If evidence is tied before ordering, abstain as `nothing`; do not invent a winner. |
| inspect alternatives | `oac-lanes` | Execute every lane and preserve every acknowledgement in order. Use `oac-lanes-winners` only to project non-`nothing` results; choosing is a later act. |
| commit now | `oac-cut-with-receipt` | Take the first acknowledgement even when it is `nothing`, stop, and retain the count of pruned, untried alternatives. Cut is not choice. |
| speculate safely | `oac-store`, `oac-undo` | Store the immutable memory value before the attempt. On a `nothing` acknowledgement, undo returns that exact checkpoint; a successful acknowledgement keeps the new memory. |
| bound work | `oac-timeout-walk`, `oac-timed-out?` | `nothing` with alternatives left is timeout; `nothing` after all alternatives were tried is honest exhaustion. Preserve `alts-left` so these cannot collapse into one status. |
| abstain exactly | `nothing`, `oac-nothing?` | `nothing` is neither `0` nor `1`. Test it only through the nothing/equality surface; never use it as arithmetic, ordering, or a branch condition. |
| select cognition | `find-plane`, `bbcc-thought-route` | Route `when`/`where`/`which` to computable kernels and learned planes such as `how`/`why` to learned kernels. A missing plane or missing evidence remains `nothing`. |
| birth and run a physical micro-thought | `frbt-parse-stream`, `frex-parse`, `frexl-execute-request`, `frxs-run` | A model can invent an affine recipe as scannerless raw bytes, receive its content-addressed NodeID, request that NodeID with an input and carrier, and continue from the typed observation. The needed native path is generated from the recipe itself on demand; there is no flatten prerequisite or operations table. Request, generated artifact, execution, observation, refinement, crystallization, dissolution, and release stay separately visible. |

The control curriculum invokes the repository's actual offer/ack, choice-lane,
inquiry-plane, and native-generation cells rather than matching their names. Its
band is **1048575**. The Qwen overlay carries the same contracts in 30 pairs and
an eleven-family held-out semantic lane: `form-cli-qwen-teach-layer-band.fk` is
**16777215**, with 11/11 held-out classifications and zero exact/family leakage.
The compact generation band is **127**, including the post-observation answer
contract. This is still prompt/curriculum evidence, not a weight-update claim.
That is evidence for the teaching layer, not evidence that model weights were
trained or that the resident model executed the controls correctly. The live
`h03` retry showed that the added answer contract did not by itself change the
measured output: the same typed source hit opened the reserve, the model stopped
after nine answer tokens, and promotion remained **333333 ppm**. Retrieval is
therefore no longer the observed `h03` seam; exact post-observation extraction
and reply shape are.

The NodeID loop has now crossed live on the local Qwen3.8-27B and Metal. Qwen
emitted the strict request for recipe `@0.2.0.7` with input `5`; the raw-byte
cursor called its carrier once; Form generated MSL from the recipe children;
Metal returned `22`; a typed 307-byte / 129-ID observation re-entered; and the
same original-ID/KV session continued with the value, carrier, observation
NodeID, and lifecycle. `callback-calls=1`, `carrier-executed=1`,
`native-code-generated=1`, `model-executed-form=0`, `release-ok=1`; the live
verdict was **4095**, exit 0. The first stale-source/model open cost 515287 ms;
the request→physical observation→continued answer movement cost 112784 ms.
That is one live affine Metal thought, not yet CPU/MLX parity or a recursive
model-authored recipe-birth run.

### Direct evidence re-witnessed here

All eight targets below preflight with balanced parentheses, 0 errors,
0 warnings, and 0 unresolved calls. Direct `fkwu` verdicts:

```
form-cli-heed-cursor-band.fk                  65535
form-cli-heed-current-source-band.fk          8388607
form-cli-heed-current-source-cursor-band.fk   65535
form-cli-model-generate-heed-report-band.fk   2097151
bml-bmf-stream-curriculum-band.fk             16777215
bml-bmf-control-curriculum-band.fk            1048575
bml-bmf-stream-authority-band.fk              1023
form-cli-qwen-teach-generate-band.fk          127
form-cli-qwen-teach-layer-band.fk             16777215
form-recipe-birth-token-band.fk                1048575
form-recipe-exec-token-band.fk                 1048575
form-cli-recipe-exec-cursor-band.fk            33554431
form-recipe-exec-token-live-band.fk            131071
form-cli-model-session-band.fk                 2047
form-cli-recipe-exec-session-band.fk           4095
```

## Free / unclaimed

- A ≥95% multi-token resident answer and the complete fifteen-family resident
  pass. `h03` now embodies a multi-token source-query-answer crossing, but its
  333333 ppm score is a failure signal to refine, not family credit. One exact
  `h01` answer does not imply the other families.
- The **≥95% integration claim** across Form knowledge and RAG. A new sealed
  denominator now holds 30 unseen rows, exactly two in each of 15 families,
  with zero recorded leakage. The claim remains pending until that blind lane's
  per-family live observations have actually run and every family reaches the
  threshold.
- The **LoRA tensor writer**. `LoraWriter = 0` is honest and it is also the
  blocker on fine-tuning. Writing real adapter tensors from minted rows is a
  named, separable stone.
- **Mint scale-up**: `fkm-n(tn,hn)` is a call; 24 rows is the proof, not the
  corpus.

## Working agreement

- `./fkwu <file.fk>` runs a cell. Never `--src`; that flag is dropped.
- Preflight before believing a verdict:
  `echo path/to/cell.fk > /tmp/preflight-target && ./fkwu observe/preflight-run.fk`
  A green number with a nonzero exit is a fold over `nothing`, not a pass.
  Do not aim this door at an effectful top-level live driver: its fresh fkwu
  probe can reach those effects. Preflight the mechanism and pure band, then run
  the effectful driver once intentionally when its carrier is free.
- `/tmp` is shared across agents. Use a per-agent run-target path or you will
  run a sibling's cell against your own body.
- Rebase on `main` between steps and push small commits often.
