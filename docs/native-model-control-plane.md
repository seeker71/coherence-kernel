# Native model control plane

Date: 2026-07-15

This is the operating overview for Form-native models, locally adapted models,
borrowed local models, and local/remote oracles. Claims in this document are
bounded by executable evidence; a model being installed or trained does not
give it authority.

## Where meaning lives

The control plane is Form-owned:

- `form/form-stdlib/native-model-control-plane.fk` owns the registry,
  classification, schedule, canary, and promotion policy.
- `form/form-stdlib/native-model-evidence.fk` owns normalization, tokenization,
  SHA-256 identities, exact/F1/order-sensitive scores, deterministic rotation,
  and train/audit overlap checks.
- `form/form-stdlib/native-model-eval-form.fk` owns paired evaluation,
  identity-stability checks, and promotion eligibility.
- `form/form-stdlib/native-model-seal-form.fk` owns held-out isolation and
  consent/license/provenance admission.
- `form/form-stdlib/native-model-event-form.fk` owns the minimized event schema,
  validation, hashing, JSON encoding, and atomic append.
- `form/form-stdlib/native-model-ledger-form.fk` recomputes each event digest,
  requires exact canonical JSON, rejects invalid rows, and owns final shares.
- `form/form-stdlib/native-model-daily-form.fk` owns daily admission, training
  closure, an equality-only byte-copy deployment check, and progress
  accounting. Canonical Hati transformation-lineage observation now lives in
  the dedicated lineage cell; owner review and authorization remain pending.
- `form/form-stdlib/native-model-live-loop.fk` owns occurrence classification
  and owned/on-device/remote workload shares.
- `form/form-stdlib/native-model-live-training.fk` contains the currently
  executable bounded native training experiments.
- `form/form-stdlib/native-model-checkpoint.fk` owns the exact f64 checkpoint
  image, content/training-contract admission, atomic publication, reload
  equivalence, and champion keep/revert.
- `form/form-stdlib/native-model-session-world.fk` owns the fixed-shape
  action-conditioned count state and future-session scoring.
- `form/form-stdlib/native-model-session-grounding.fk` owns Form-native lexical
  embedding, ranking, and replay scores for real completed session queries.
- `form/form-stdlib/native-model-lineage-form.fk` owns canonical model-package
  nodes, transformation edges, byte-copy equality, DAG identity, and drift.

The replacement host surface is intentionally thin:

- `form/scripts/native_model_train.sh` invokes the Form training report, runs
  exact checkpoint admission/continuation, and preserves the minimized result.
- `form/scripts/native_model_rag.sh` gates the shipped Form-native CLI against
  a deterministic direct `ask`, then observes the live local world index.
- `form/scripts/native_model_route.sh` currently carries bytes to the registered
  borrowed `llama3.2:3b` route and records the Form-classified occurrence.
- `form/scripts/native_model_eval.sh` performs paired local candidate/incumbent
  I/O while Form owns normalization, scores, comparison, and authority refusal.
- `form/scripts/native_model_tally.sh` asks Form to classify the event ledger
  and compute final-work shares.
- `form/scripts/native_model_real_flows.sh` sequences the real session-world,
  session-grounding, and trained-artifact-lineage witnesses and writes their
  compact daily overview.
- `form/scripts/native_model_daily.sh` re-grounds the checkout and sequences
  Form training, exact keep/revert, native RAG, the optional paired local
  diagnostic, the three real flows, and the Form tally.

The evaluator keeps prompt, expected text, and model responses in a private
temporary directory and passes them to Form over standard input. The durable
summary and event ledger contain identities, aggregates, timing, and bounded
metadata; the raw text is deleted with the temporary directory. This describes
the control-plane carrier's persistence, not an assurance about unrelated
external process logging.

Shell may observe epoch time, file metadata, process return codes, and served
model identity; make bounded loopback HTTP calls; and invoke `fkwu`. It may not
define a score, relabel a model class, admit training data, grant authority, or
perform weight updates. Raw prompt and answer text must not enter a durable
occurrence row.

This shell membrane is a present host boundary, not a second intelligence
layer. The shipped Form shell does not yet execute arbitrary host commands—it
marks them as passthrough—and direct-source `fkwu` does not expose a clock.
Those two gaps are named instead of being hidden inside another host program.
No capability was added to the temporary C seed to close them.

## Classes do not blur

| class | exact meaning | may count as owned accepted work? |
|---|---|---:|
| `form-native` | model or learned recipe executes through the Form/native-walker body | yes |
| `local-finetuned` | a pretrained base was adapted locally; the class alone proves neither lineage nor authority | yes, only after accepted final and reviewed lineage |
| `local-native` | borrowed pretrained model executes locally in an external runtime | no |
| `local-oracle` | local external teacher, judge, evaluator, or fallback | no |
| `remote-oracle` | network/subscription teacher or executor | no |
| `policy-fixture` | search policy, operator, architecture, or synthetic fixture without a useful trained artifact | no |

Execution location is separate: `native-recipe`, `local-process`, or
`remote-membrane`. An imported Ollama model does not become native because it
runs on the same machine. Evaluation, training, teacher, and integration-probe
calls are also separate from accepted production finals and cannot inflate
sovereignty.

The registry currently contains 32 family-level occurrences: 14 Form-native,
3 local fine-tunes, 4 borrowed local models, 2 local oracles, 3 remote oracles,
and 6 policy/fixture families. It collapses repeated speech windows and
duplicate synthetic GGUF bands into families rather than inflating the count.

## Actual native training observed

The current clean carrier entry is:

```sh
form/scripts/native_model_train.sh
```

The carrier assembles the declared Form dependency closure into a private
combined source and invokes `fkwu`; the observed run contained zero
`[unresolved-call]` diagnostics. Weight updates and every reported metric are
computed in Form.

It performs two bounded weight-training runs inside Form:

| experiment | data / epochs | before | after | additional evidence |
|---|---|---:|---:|---:|
| two-block transformer component | 13 real EN→FR feature rows, 5 held out, 120 epochs | train loss 1,676,291 micro; held-out 553,645 micro | train loss 361,561 micro; held-out 85,021 micro | held-out mean baseline 359,466 micro; weight delta 196,614 micro |
| small next-token neural LM | 6 train transitions, 6 held out, 400 epochs | 0/6 held-out correct | 6/6 held-out correct | validation returned true |

This is actual training, not a description or fixture-only pass. The learned
state is now serialized exactly, but it is still not a useful native LLM.
`native_model_checkpoint.sh` writes all 192 learned values as 1,536 canonical
IEEE-754 little-endian bytes, bound to a content SHA-256 and the exact training
contract. Form validates the envelope, publishes by same-directory rename,
reloads every weight bit-identically, and proves prediction/metric equivalence.

The first checkpoint was admitted with transformer held-out loss `85021` micro
and neural-LM held-out accuracy `6/6`. A subsequent 20-epoch continuation scored
`85333` micro and `6/6`; because the transformer regressed, Form selected the
incumbent and wrote no replacement. The active checkpoint SHA-256 is
`186e6f94940dfff5b1f05c5727fe0dcf76e004ff4fa7e425f79691c2dec2f1a0`, and
its scope explicitly says `not-useful-generative-llm`. The transformer width is
two and the neural LM has one-token context. The useful generative native voice
remains the larger missing floor.

Existing external LoRA training remains historical evidence. `form-llama` was
trained on 1,205 rows with validation loss reported as 5.083→0.621. The Hati
translator used 3,154 train rows and reported 4.123→1.033. Falling validation
loss did not establish task authority, as the live crossings below show.

## Actual live integration observed

The following are direct local calls, not inferred from file presence:

| probe | observed result | latency | verdict |
|---|---|---:|---|
| `llama3.2:3b`, EN→PT-BR | correct Portuguese translation | about 2,338 ms | borrowed local route works on this item |
| `hati-translator-q4`, same task | English ramble | about 1,764 ms | adapted translator fails this item |
| `llama3.2:3b`, answer `42` | correct | about 1,458 ms | correct narrow answer |
| `form-llama`, answer `42` | correct | about 7,768 ms | tie on answer, large latency regression |
| Axiom-4 calibration | base abstained; both fine-tunes fabricated support | one diagnostic | calibration favors base |
| shipped `form/form-cli`, build publication gate | `grounded:fixture/native-rag-hit` plus local/synthesis lane receipts | not retained | native artifact publication is green |
| shipped `form/form-cli`, daily deterministic `ask` | `grounded:fixture/native-world-hit.fk` | not retained | daily native RAG gate is green |
| shipped `form/form-cli`, real local world index | `grounded:form/form-stdlib/active-inference.fk` | direct route | live native retrieval is green |

These probes are too small for authority, but they are enough to reject a
blanket claim that fine-tuning improved integration. The current Hati route is
worse than the base on the witnessed translation, `form-llama` is slower on the
narrow tie, and the base is better calibrated on the Axiom-4 diagnostic.

The executable paired evaluator independently confirmed the translation
direction on one fixed item:

| Form score | Hati candidate | borrowed base |
|---|---:|---:|
| exact, ppm | 0 | 1,000,000 |
| token F1, ppm | 352,941 | 1,000,000 |
| sequence, ppm | 222,222 | 1,000,000 |
| conservative score, ppm | 222,222 | 1,000,000 |

Both observations were identity-stable and clean, `paired=1`, Form selected the
incumbent, and `authority_eligible=0`. This is a fixed diagnostic, not a sealed
final audit, and its sample count of one is not sufficient for promotion.
The latest complete daily witness observed the same scores and verdict at
2,161 ms for Hati and 2,086 ms for the borrowed base; latency varies between
warm and cold local calls, while the quality result remained unchanged.

The RAG repair was entirely Form + shell: the direct query had incorrectly used
list-only `nil?` on a string; the Form walker emitter also lagged the already
proven rooted-let semantics. Resident `fkwu` now executes the current Form
flattener and emitter directly, and the build refuses to publish a platform
bootstrap unless the deterministic `ask` gate passes. The regenerated binary
and platform bootstrap are byte-identical at SHA-256
`c0dd77e26565de2a3f5da41f07daaa6b1078cb062419455fd0855035295114b1`.

The fine-tuned tags have no production call sites in this worktree. The earlier
identity alarm is now separated into semantic and physical facts: the LoRAs all
used the Llama-3.2 3B semantic base, while Ollama's `1.1B` report is packed q4
storage accounting. Hati now has a canonical observed five-node/four-edge DAG:
base → adapter, base+adapter → fused, fused → Q4, and Q4 → served. The Q4 blob,
staged Android GGUF, and served manifest layer are byte-identical. A second
complete observation reported zero artifact drift. This does not confer
authority: no transformation edge is reproduced, owner-reviewed, or authorized,
and the current F16 and Q4 packages are correctly represented as sibling
descendants of the fused package rather than a fabricated F16 → Q4 edge.

## Daily metric contract

Each daily witness records these separately:

1. Integrity: fresh `fkwu`, `42`, `55`, `15`, `11111`, the relevant native
   bands, and a deliberate unresolved-call failure on a never-reused path.
2. Registry: observed occurrences by class, with artifact and served-identity
   hashes where available.
3. Quality: paired exact, token F1, order-sensitive score, sample count,
   latency, errors, evaluator identity, and data identity.
4. Integration: explicit pass/fail results for real callable routes, including
   deterministic and live native RAG separately.
5. Work allocation: accepted production finals by class; non-final traffic is
   excluded.
6. Progress: day-over-day native quality, owned-work share, local borrowed
   share, remote share, and gate state.

Until production routes append valid accepted-final events, owned-work share is
**unmeasured**, never zero. Installed models and evaluation traffic do not
supply the missing denominator. After the latest complete daily witness, the
Form tally held 11 total events: 4 local-finetuned evaluation events and 7
borrowed-local evaluation/probe events. Invalid rows and accepted production finals both
remained 0, so owned, on-device, and remote final shares all rendered as
unmeasured. A forged incomplete row and a digest-mismatched edited row were
both rejected as invalid and could not manufacture a share.

## Training and promotion gates

A larger training job remains closed unless all of these are observed:

- exact training rows and a separate held-out set;
- zero forbidden overlap under the Form seal contract;
- row-level provenance plus scoped consent and license receipts;
- a fixed evaluator and content-addressed destination;
- integrity, power, thermal, disk, and toolchain readiness;
- a new candidate identity that never overwrites the incumbent.

Promotion additionally requires stable served identity, reviewed
served-to-training lineage, at least 32 paired sealed samples, no evaluation
errors, a meaningful improvement above noise, and repeated fresh days. Fixed
training-validation or historical-held-out diagnostics cannot be relabeled as
promotion evidence. A tie cannot earn authority.

The current language datasets do not have complete row-level
provenance/consent/license receipts. Therefore a new large LoRA run is blocked.
No scheduled task may manufacture those receipts or treat their absence as a
mere tooling inconvenience.

## Clocked schedule

Training is evidence-triggered, not a requirement to mutate weights every day.

| day | bounded focus | current status |
|---|---|---|
| Monday | deduplicate, provenance, inventory, seal preparation | measurement and data repair |
| Tuesday | Form-knowledge challenger | closed until eligible rows and receipts exist |
| Wednesday | translation challenger | closed; live Hati regression must be diagnosed first |
| Thursday | persisted Form-native checkpoint/KV/layer work | exact small checkpoint + keep/revert alive; useful model still pending |
| Friday | action-conditioned local world model | full 31,827/1,364 Form replay alive; current +733 ppm challenger rejected below materiality |
| Saturday | consented speaker-disjoint speech work | schema-specific trainer/evaluator still pending |
| Sunday | rotating sealed evaluation and rollback check | evaluation only; never training |

`form/scripts/native_model_daily.sh` is the current daily entry. The observed
cycle passed ground `42`, recursion `55`, freshness `15`, native-vs-rented
`11111`, live-loop `4095`, training `255`, checkpoint `4095`, RAG `7`, SHA-256
`2`, and Form replacement `262143`. It then ran bounded native training, exact
checkpoint keep/revert, deterministic and live RAG, and the Form event tally.
The paired Hati/base diagnostic is optional and was disabled for the latest
verification so the ledger remained at 11 events. A green day may still contain no large-model weight
mutation; that is correct when the larger training gates are closed. None of
these commands grants production authority by itself.

The 06:30 automation is an enacted daily witness, not yet a weekday-job
dispatcher. It runs the same bounded Form training/checkpoint gate and all
three real evidence flows every day. The Monday–Sunday table above remains
Form-owned policy until the daily carrier evaluates `nmfd-plan` from measured
host, seal, evaluation, and lineage inputs and dispatches only an admitted job.
That distinction is intentional today: the large translation/form-knowledge
training gates are closed, so no shell fallback may turn a policy row into an
unauthorized training run.

## What the eight-day autoresearch result contributes

Weco AI's [AIDE² report](https://www.weco.ai/blog/first-evidence-of-recursive-self-improvement)
is preliminary evidence that a frozen-model agent can improve its own research
harness under a fixed budget. It is not evidence that the language-model
weights recursively improved: the outer and inner models stayed frozen. The
reported process made 100 harness rewrites over eight unattended days, promoted
seven versions, and rejected roughly 90% of proposals. The useful transfer is
procedural: immutable evaluation, bounded experiments, keep/revert, a lineage
of failures and successes, comparisons above noise, compact typed memory, and
explicit reward-hacking checks.

That transfer is now exercised rather than merely cited: the first continuation
from the persisted incumbent regressed from `85021` to `85333` micro held-out
loss and was automatically rejected, leaving the exact incumbent checkpoint
active. The loop still repeats that deterministic candidate; useful search now
requires novel content-addressed proposals and memory of rejected identities.

[Karpathy's autoresearch](https://github.com/karpathy/autoresearch) provides the
smaller fixed-budget keep/revert pattern; the original
[AIDE implementation](https://github.com/WecoAI/aideml) provides a local
search-harness reference. In this registry `prototype.autoresearch` remains a
`policy-fixture` after one real reject cycle because it does not yet originate a
novel proposal stream. Harness improvement and model-weight improvement remain
separately measured loops.

## Present floor

What is alive now is modest but real: Form-native bounded training, exact
checkpoint/reload/keep-revert, Form-owned evidence and authority logic, a
classified registry, a regenerated native RAG artifact, a full real-session
issued-tool replay, a bounded real-session grounding replay, and a canonical
artifact-to-served lineage witness. What is not alive is equally explicit: no persisted *useful* native
language checkpoint, no trustworthy production authority for the current
fine-tunes, no complete language dataset authorization, and no measured
majority-owned production workload. The grounding replay is not full-index
recall and is currently too slow for a cheap daily loop; the issued-tool model
does not yet observe tool success, latency, terminal state, or world effects.

The north-star order is now: reproduce, review, and authorize canonical
transformation-lineage edges for the existing local adaptations without
mutating current tags; cache or compile the Form grounding embeddings and add a
full-index shadow lane; route one
consented real accepted task through the native body so work share gains a
denominator; then complete one full real-GGUF token path—tensor staging,
all-layer GQA/FFN hidden-state evolution, logits, sampling, and decode—before
spending on another adapter run. Native RAG is useful local memory. The current
session-derived model predicts issued tool classes, not completed world state;
that lane next needs result mode, latency, process lifecycle, terminal state,
and held-out calibration. Additional large-model training waits for authorized,
non-overlapping rows.
