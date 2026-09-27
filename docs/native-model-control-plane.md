# Native model control plane

The operating overview for Form-native models, locally adapted models and
borrowed pretrained weights the body runs. Claims here are bounded by executable
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
  nodes, transformation edges, byte-copy equality, DAG identity, and drift
  (`native-model-lineage-band.fk` 33554431).

Model and learning changes use the bidirectional diagnostic membrane in
`observe/bidirectional-framebuffer-channel.fk`: a model observation flows out with
a correlation id; Form returns a bounded control action; the actuator selects the
next state; that state is observed again. For any claimed improvement, retain
per-row or per-stage transitions — an aggregate score alone does not establish a
cause. Nothing, timeout, and mismatched replies select an explicit alternative
node ([`live-dynamic-diagnostics.md`](live-dynamic-diagnostics.md)).

## The ask route

Every local ask runs in the body. `form/form-stdlib/native-model-route-table.bml`
holds the route names as Form data — `form-metal` and `llama32.form-metal`, both
carried by the native Metal door `observe/metal-ask-files-run.bml` with a 12-token
default — and a name that would reach a model server is not a row: it comes back
typed `unknown` (`native-model-route-table-band.fk` 255).
`form/scripts/native_model_route.sh` reads `LOCAL_MODEL_ROUTE` and the `FORM_*`
request values, writes one private request directory, and hands it to that door;
an unknown name exits 2 with the known names. The door owns admission,
generation, identity and publication in one `fkwu` process: the registered
llama3.2:3b GGUF answers the physical witness `observe/metal-ask-live-run.bml`
with 255, and a request it does not admit names its reason (`empty-prompt`,
`model-not-admitted`) before any weights are opened. The body's
larger voice is Qwen3.8-27B in the `fkwu` model session
(`form/form-stdlib/bml/form-cli-native-voice.bml`).

The presence lanes translate through the same door:
`presence/fkwu-local-audio-loop.fk`, `presence/fkwu-many-voices-live.fk` and
`presence/fkwu-production-audio-end-to-end.fk` write a request directory, run
the door in a child `fkwu`, and read the first line of the published answer.

## The host carriers

The `form/scripts/native_model_*.sh` carriers — train, rag, route, checkpoint,
session_world, session_grounding, real_flows, tally, daily — assemble a Form
input, invoke `fkwu`, and keep the minimized result under the private state
directory. None of them asks a model server. Direction: each carrier holds only
what a host boundary is for — environment bytes, stdin, file metadata, process
return codes, the clock — and every score, class, admission, authority and weight
update lives in Form; what a carrier still decides comes home into a Form door.

`native_model_real_flows.sh` carries two daily-comparable flows, the session world
model and session grounding, fresh or from sealed child reports verified by
SHA-256. `native_model_daily.sh` runs the integrity quartet, RAG, the bounded
training and checkpoint witness, the real flows and the tally when invoked; no
scheduler in this tree invokes it.

## Classes do not blur

| class | exact meaning | may count as owned accepted work? |
|---|---|---:|
| `form-native` | model or learned recipe executes through the Form/native-walker body | yes |
| `local-finetuned` | a pretrained base was adapted locally; the class alone proves neither lineage nor authority | yes, only after accepted final and reviewed lineage |
| `local-native` | borrowed pretrained model executes locally | no |
| `local-oracle` | local external teacher, judge, evaluator, or fallback | no |
| `remote-oracle` | network/subscription teacher or executor | no |
| `policy-fixture` | search policy, operator, architecture, or synthetic fixture without a useful trained artifact | no |

Execution location is separate: `native-recipe`, `local-process`, or
`remote-membrane`. Borrowed weights stay borrowed when the body runs them:
`base.llama32-3b-metal` executes as a `native-recipe` and its class is
`local-native`. Model weights are input data. Evaluation, training, teacher, and
integration-probe calls are separate from accepted production finals and cannot
inflate sovereignty. The registry collapses repeated speech windows and duplicate
synthetic GGUF bands into families rather than inflating the count; the registry
cell answers the current count (`native-model-control-plane-band.fk` 65535).

The looped-transformer transfer is executable in
`form/form-stdlib/nanbeige-looped-lane.fk`: 22 stored decoder layers, 44 layer
applications, per-loop KV isolation, the borrowed Nanbeige challenger admitted only
after pinned identity, forward parity, sealed quality improvement, and resource
gates. `nanbeige-looped-transformer.fk` runs the native RMSNorm/RoPE/GQA/SwiGLU
stack twice over the same layer-weight list; `nanbeige-package-admission.fk` and
`nanbeige-gguf-admission.fk` own the pinned safetensors and Q4 identities
(`form/scripts/nanbeige_gguf_verify.sh` hashes the GGUF for that admission); native
bf16 tensor windows enter math through `safetensors-bf16-slice.fk`.

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
neural LM has one-token context.

Local adaptation of a pretrained base runs in the body:
`form/form-stdlib/native-lora-train.bml` and `native-lora-fuse.bml` write the
adapters under `form/form-stdlib/adapters/`
([`native-lora-training.md`](native-lora-training.md)). Falling validation loss
does not establish task authority: the paired evaluator compares candidate and
incumbent on fixed items with exact, token-F1, and order-sensitive scores,
identity-stable and clean, and a tie cannot earn authority.

## Daily metric contract

Each daily witness records these separately:

1. Integrity: fresh `fkwu`, the ground quartet, native-vs-rented `11111`, the
   relevant native bands, and a deliberate unresolved-call failure on a
   never-reused path.
2. Registry: observed occurrences by class, with artifact hashes where available.
3. Quality: paired exact, token F1, order-sensitive score, sample count, latency,
   errors, evaluator identity, and data identity.
4. Integration: explicit pass/fail results for real callable routes, including
   deterministic and live native RAG separately.
5. Work allocation: accepted production finals by class; non-final traffic is excluded.
6. Progress: day-over-day native quality, owned-work share, local borrowed share,
   remote share, and gate state.

Until production routes append valid accepted-final events, owned-work share is
**unmeasured**, never zero; the native route does not append events yet.
Installed models and evaluation traffic do not supply the missing denominator. A
forged incomplete row and a digest-mismatched edited row are rejected as invalid
and cannot manufacture a share.

## Training and promotion gates

A larger training job remains closed unless all of these are observed:

- exact training rows and a separate held-out set;
- zero forbidden overlap under the Form seal contract;
- row-level provenance plus scoped consent and license receipts;
- a fixed evaluator and content-addressed destination;
- integrity, power, thermal, disk, and toolchain readiness;
- a new candidate identity that never overwrites the incumbent.

Promotion additionally requires a stable model identity, reviewed
base-to-adapter lineage, at least 32 paired sealed samples, no evaluation errors,
a meaningful improvement above noise, and repeated fresh days. Fixed
training-validation or historical held-out diagnostics cannot be relabeled as
promotion evidence. A tie cannot earn authority.

The language datasets do not have complete row-level provenance/consent/license
receipts, so a new large training run is closed. No scheduled task may
manufacture those receipts or treat their absence as a tooling inconvenience.

## Schedule

Training is evidence-triggered, not a requirement to mutate weights every day.
The bounded focus rotates — deduplicate/provenance/seal preparation; the
Form-knowledge challenger and the translation challenger (closed until eligible
rows and receipts exist); persisted Form-native checkpoint/KV/layer work; the
action-conditioned local world model; consented speaker-disjoint speech work;
a rotating sealed evaluation and rollback check (evaluation only, never training).
The rotation is Form-owned policy until the daily carrier evaluates `nmfd-plan`
from measured host, seal, evaluation, and lineage inputs and dispatches only an
admitted job. A green day may contain no large-model weight mutation; that is
correct when the larger gates are closed. None of these commands grants
production authority by itself.

The procedural transfer from fixed-budget autoresearch (Weco's AIDE², Karpathy's
autoresearch) is exercised, not cited: immutable evaluation, bounded experiments,
keep/revert, a lineage of failures and successes, comparisons above noise, compact
typed memory, explicit reward-hacking checks. `prototype.autoresearch` stays a
`policy-fixture` until it originates a novel proposal stream; harness improvement
and model-weight improvement are separately measured loops.

## Present floor and direction

Alive: Form-native bounded training, exact checkpoint/reload/keep-revert,
Form-owned evidence and authority logic, a classified registry, native RAG as
local memory, a real-session issued-tool replay, a bounded real-session grounding
replay, the Form lineage DAG, native LoRA adapters, and a full real-GGUF token path
on Metal behind the only ask route. Not alive: no persisted *useful* native
language checkpoint from the bounded trainer, no production authority for any
local adaptation, no complete language dataset authorization, and no measured
majority-owned production workload. The grounding replay is not full-index recall
and is too slow for a cheap daily loop; the issued-tool model predicts issued tool
classes, not completed world state.

Direction: route one consented real accepted task through the native door with an
appended accepted-final event so work share gains a denominator; bind the native
adapters' base-to-adapter edges through `native-model-lineage-form.fk`; cache or
compile the Form grounding embeddings and add a full-index shadow lane; bring
native decode toward realtime so the presence lanes can translate live. The
session-derived model next needs result mode, latency, process lifecycle, terminal
state, and held-out calibration. Additional large-model training waits for
authorized, non-overlapping rows.
