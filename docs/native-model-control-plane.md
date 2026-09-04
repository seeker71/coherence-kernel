# Native model control plane

The operating overview for Form-native models, locally adapted models, borrowed
local models, and local/remote oracles. Claims here are bounded by executable
evidence; a model being installed or trained does not give it authority.

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
  closure, an equality-only byte-copy deployment check, and progress accounting.
- `form/form-stdlib/native-model-live-loop.fk` owns occurrence classification
  and owned/on-device/remote workload shares.
- `form/form-stdlib/native-model-live-training.fk` holds the executable bounded
  native training experiments.
- `form/form-stdlib/native-model-checkpoint.fk` owns the exact f64 checkpoint
  image, content/training-contract admission, atomic publication, reload
  equivalence, and champion keep/revert.
- `form/form-stdlib/native-model-session-world.fk` owns the fixed-shape
  action-conditioned count state and future-session scoring.
- `form/form-stdlib/native-model-session-grounding.fk` owns Form-native lexical
  embedding, ranking, and replay scores for real completed session queries.
- `form/form-stdlib/native-model-lineage-form.fk` owns canonical model-package
  nodes, transformation edges, byte-copy equality, DAG identity, and drift.

Model and learning changes use the bidirectional diagnostic membrane in
`observe/bidirectional-framebuffer-channel.fk`: a model observation flows out with
a correlation id; Form returns a bounded control action; the actuator selects the
next state; that state is observed again. For any claimed improvement, retain
per-row or per-stage transitions — an aggregate score alone does not establish a
cause. Nothing, timeout, and mismatched replies select an explicit alternative
node ([`live-dynamic-diagnostics.md`](live-dynamic-diagnostics.md)).

The host surface is intentionally thin — the `form/scripts/native_model_*.sh`
carriers (train, rag, route, eval, tally, real_flows, daily) assemble a Form
dependency closure, invoke `fkwu`, and preserve the minimized result. Shell may
observe epoch time, file metadata, process return codes, and served model
identity; make bounded loopback HTTP calls; and invoke `fkwu`. It may not define a
score, relabel a model class, admit training data, grant authority, or perform
weight updates. Raw prompt and answer text never enters a durable occurrence row:
the evaluator keeps prompt, expected text, and responses in a private temporary
directory, passes them to Form over stdin, and deletes them with the directory.
The route carrier defaults to direct Metal execution with no socket, HTTP, JSON,
Ollama, or llama.cpp membrane; a borrowed comparator or a named challenger carrier
is selected explicitly, and availability grants neither admission nor authority.
The routing decision itself is Form data: which route names exist, which door each
carries the ask through, and each door's step default are rows of
`form/form-stdlib/native-model-route-table.bml`, read through
`native-model-route-table-cli.fk` (band `native-model-route-table-band.fk` 255,
re-run 2026-09-04); the shell keeps only the host boundary — it reads
`LOCAL_MODEL_ROUTE` and `FORM_METAL_STEPS` from the environment and asks the table.

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
calls are separate from accepted production finals and cannot inflate sovereignty.
The registry collapses repeated speech windows and duplicate synthetic GGUF bands
into families rather than inflating the count; the registry cell answers the
current count.

The looped-transformer transfer is executable in
`form/form-stdlib/nanbeige-looped-lane.fk`: 22 stored decoder layers, 44 layer
applications, per-loop KV isolation, the borrowed Nanbeige challenger admitted only
after pinned identity, forward parity, sealed quality improvement, and resource
gates. `nanbeige-looped-transformer.fk` runs the native RMSNorm/RoPE/GQA/SwiGLU
stack twice over the same layer-weight list; `nanbeige-package-admission.fk` and
`nanbeige-gguf-admission.fk` own the pinned safetensors and Q4 identities; native
bf16 tensor windows enter math through `safetensors-bf16-slice.fk`. A verified Q4
carrier's local response is observed local-process evidence, not Form-native
execution or benchmark authority.

## What the bounded native training is

`native-model-live-training.fk` performs bounded weight training inside Form — a
two-block transformer component on real EN→FR feature rows with a held-out set,
and a small next-token neural LM — with every metric computed in Form. The learned
state serializes exactly: `native-model-checkpoint.fk` writes the learned values as
canonical IEEE-754 little-endian bytes bound to a content SHA-256 and the exact
training contract, publishes by same-directory rename, reloads every weight
bit-identically, and proves prediction/metric equivalence. A continuation that
regresses the held-out loss is rejected and the incumbent stays active. The scope
says it plainly: `not-useful-generative-llm` — the transformer width is two and the
neural LM has one-token context. The useful generative native voice is the larger
missing floor; the real open mind runs through the body on Metal
(`CURRENT_FLOOR.md`), and a LoRA tensor writer is the named stone.

Falling validation loss on an external LoRA run does not establish task authority:
the paired evaluator compares candidate and incumbent on fixed items with exact,
token-F1, and order-sensitive scores, identity-stable and clean, and a tie cannot
earn authority. Fine-tuned tags have no production call sites in this tree.

## Daily metric contract

Each daily witness records these separately:

1. Integrity: fresh `fkwu`, the ground quartet, native-vs-rented `11111`, the
   relevant native bands, and a deliberate unresolved-call failure on a
   never-reused path.
2. Registry: observed occurrences by class, with artifact and served-identity
   hashes where available.
3. Quality: paired exact, token F1, order-sensitive score, sample count, latency,
   errors, evaluator identity, and data identity.
4. Integration: explicit pass/fail results for real callable routes, including
   deterministic and live native RAG separately.
5. Work allocation: accepted production finals by class; non-final traffic is excluded.
6. Progress: day-over-day native quality, owned-work share, local borrowed share,
   remote share, and gate state.

Until production routes append valid accepted-final events, owned-work share is
**unmeasured**, never zero. Installed models and evaluation traffic do not supply
the missing denominator. A forged incomplete row and a digest-mismatched edited row
are rejected as invalid and cannot manufacture a share.

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
training-validation or historical held-out diagnostics cannot be relabeled as
promotion evidence. A tie cannot earn authority.

The language datasets do not have complete row-level provenance/consent/license
receipts, so a new large LoRA run is closed. No scheduled task may manufacture
those receipts or treat their absence as a tooling inconvenience.

## Schedule

Training is evidence-triggered, not a requirement to mutate weights every day.
The bounded focus rotates — deduplicate/provenance/seal preparation; the
Form-knowledge challenger and the translation challenger (closed until eligible
rows and receipts exist); persisted Form-native checkpoint/KV/layer work; the
action-conditioned local world model; consented speaker-disjoint speech work;
a rotating sealed evaluation and rollback check (evaluation only, never training).
The daily carrier runs the bounded Form training/checkpoint gate and the real
evidence flows every day; the rotation is Form-owned policy until the carrier
evaluates `nmfd-plan` from measured host, seal, evaluation, and lineage inputs and
dispatches only an admitted job. A green day may contain no large-model weight
mutation; that is correct when the larger gates are closed. None of these
commands grants production authority by itself.

The procedural transfer from fixed-budget autoresearch (Weco's AIDE², Karpathy's
autoresearch) is exercised, not cited: immutable evaluation, bounded experiments,
keep/revert, a lineage of failures and successes, comparisons above noise, compact
typed memory, explicit reward-hacking checks. `prototype.autoresearch` stays a
`policy-fixture` until it originates a novel proposal stream; harness improvement
and model-weight improvement are separately measured loops.

## Present floor and the order of the next stones

Alive: Form-native bounded training, exact checkpoint/reload/keep-revert,
Form-owned evidence and authority logic, a classified registry, native RAG as
local memory, a real-session issued-tool replay, a bounded real-session grounding
replay, and a canonical artifact-to-served lineage witness. Not alive: no
persisted *useful* native language checkpoint, no trustworthy production authority
for the local fine-tunes, no complete language dataset authorization, and no
measured majority-owned production workload. The grounding replay is not
full-index recall and is too slow for a cheap daily loop; the issued-tool model
predicts issued tool classes, not completed world state.

The order: reproduce, review, and authorize canonical transformation-lineage
edges for the existing local adaptations without mutating current tags; cache or
compile the Form grounding embeddings and add a full-index shadow lane; route one
consented real accepted task through the native body so work share gains a
denominator; then complete one full real-GGUF token path — tensor staging,
all-layer GQA/FFN hidden-state evolution, logits, sampling, and decode — before
spending on another adapter run. The session-derived model next needs result mode,
latency, process lifecycle, terminal state, and held-out calibration. Additional
large-model training waits for authorized, non-overlapping rows.
