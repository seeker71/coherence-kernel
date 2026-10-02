# Native session learning

Session learning is enabled by default. Form owns collection, routing,
supervision, tokenization, full LoRA gradients, Adam continuation, assessment
and checkpoint publication. The private home is `.hearth/session-learning/`,
with directory access restricted to its owner.
`nsm-at` anchors relative homes to the process's current working directory;
absolute homes keep their selected path. Missing worker and state files remain
local absences. An ancestor checkout's learning state cannot supply them.

The integration points are the real CLI doors:

- `learn` retains a caller's vocabulary teaching and offers it for LoRA.
- `code` records checked document targets, or the incomplete documents and status.
- `heal` records every candidate/check outcome, including the final rented
  outcome when that existing fallback is used. An unsuccessful proposal is
  never trained as a correct answer.
- An interactive session closes with its observed completed-command count and
  elapsed time retained as continuity evidence. These counters create no
  training example. Actual task outcomes and explicit teachings carry their
  own context into the learning queue.
- `session learn|{"prompt":"...","completion":"..."}` accepts an explicit
  local correction. Optional `session` and `event` values provide stable replay
  identity. `session ask|question` uses the serving adapter or the disclosed seed.

Experiences and training examples are immutable and keyed by session/event
identity. Replaying the same event does not add a gradient step. Reusing an
identity for different evidence is refused. `experiences/` retains the original
problem, attempted answer, outcome and evidence for every offered observation,
including unsuccessful and evaluation attempts. Assessment observations remain
available as experience while their answers stay outside that assessment's
gradient. No transcript, prompt or answer
is copied into Glass or the diagnostic framebuffer. Training text and generated
streams remain private in the hearth; telemetry carries identities and counts.

### Withdrawing an unsupported teaching

A lesson whose directory holds `withdrawal.json` is withdrawn
(`nsm-withdrawn` in `form/form-stdlib/native-session-memory.bml`). The learner
excludes it from pending training, source delivery and replay, and refuses to
continue from a candidate whose withdrawal awaits completion. Original records,
results and adapter bytes remain intact. `learned_rounds` retains cumulative
work and `withdrawals` counts completed retirements in the session state; the
serving adapter stays selected.

Recalled experience labels the original attempt `withdrawn-teaching` and keeps
its `original_outcome` beside a reference to the withdrawal
(`nsx-withdrawal` in `form/form-stdlib/native-session-experience.bml`).

Withdrawal selects a checkpoint from before the lesson; it does not claim
arbitrary unlearning. Older lessons, promoted lessons and descendants in other
homes need their own repair. A replacement teaching uses a new event identity
after the withdrawal has been observed.

One supervisor serializes the learning queue and records the learner's actual
exit and stderr. Before the learner starts, the supervisor admits the dynamic
Metal carrier through `form/form-stdlib/metal-carrier.bml`. A fresh worktree
holds the carrier's source without its artifact, so the supervisor builds it
and records `carrier-admitted`, or `carrier-refused` with the compiler's stderr.
An unloaded carrier is named in the learner's error, never read as an allocator
refusal. Recording does not wait for GPU training during a session.
Normal session close reads the supervisor's actual parent and waits only for a
supervisor owned by that CLI. It records session continuity without launching
pending learning or waiting for another session's worker. An interrupted wait
retries while that same child remains owned. `session drain` and the one-shot
embodiment door explicitly wait for shared learning to settle; an external
interruption keeps the immutable queue for the next drain. If an owning parent
observes a positive nonzero exit before the
supervisor publishes `worker.rc`, it retains the matching owner's exit and
diagnostic and reports `failed-supervisor-exit`. Unknown wait results remain
indeterminate. A later launch clears the current wait record while retaining
the failed attempt's diagnostic; pending examples keep their original bytes.
The supervisor's exit observations cover actual child refusal, matching-owner
admission, unknown waits, successor replacement, existing terminal status and
diagnostic retention.
Each new training example
gets one full-gradient round, including rehearsal of the last promoted example
when it has a different prompt. The next round restores the candidate's adapter,
Adam moments and optimizer step. Candidate generations and the serving selection
are separate: a failed promotion does not discard the learning attempt or
replace the working adapter.

`model/fixtures/session-learning/valid.jsonl` supplies two public held-out
sentinels. A private `config.json` may name a `validation` JSONL and a positive
`learning_rate`; the default rate is `0.00001`. The new example cannot share a
normalized prompt with a held-out row. Rows are processed whole; the trainer reports
position or memory refusal instead of silently truncating a row. The one loaded
training model produces the before/after per-row scores as well as gradients.
When a deferred candidate differs from the serving adapter, that serving
adapter receives an additional comparison on the same rows. Assessment and
validation use memory-admitted forward slices with complete KV history and
supervised-token-weighted loss. Their coverage records retain all input tokens;
this reduces assessment memory without shortening examples. Full-gradient
training selects a whole tape for small rows and layer-input checkpointing
with full-sequence recomputation when the tape exceeds the device slice
allowance. Its separate admission still checks the selected memory estimate.

One learner drain reuses serving-assessment rows already measured by that
process. A fresh private cache scope belongs to the drain and is removed on
ordinary completion; later drains create a new scope. Each row is bound to
the complete example and content seals of the base, tokenizer, adapter,
configuration and optimizer. Seals are checked before and after the reading.
Changed bindings or examples are measured again; an all-hit reading admits
no model. Stored score bytes have a checksum, and row identities and ordering
are rechecked. `assessment-reuse` events distinguish measured and reused rows,
including their identities. The trainer still measures the changing candidate
before and after its update, and the same promotion checks apply.

Promotion requires improvement on the current example and no per-row regression
beyond `1e-6` against either the candidate or serving baseline. Every assessment
row's identity and supervised-token count are checked. The base, adapter,
optimizer, configuration and input data are content-bound. An interrupted round
with a complete generation can finish assessment without repeating its update.
All completed generations remain available; storage grows with experience.

`nsl-loss-evidence(before, after, tolerance)` exposes the numeric comparison
without changing learning state. Its JSON contains the supplied values,
`allowed_after`, `strictly_improved` and `within_tolerance`. The last field uses
the same comparator as `nsl-row-health`; identity checks remain in that executing
boundary. Callers supply bound numeric observations and may add this derived
evidence beside the original source when requesting a local explanation.
The function computes arithmetic only: it supplies no promotion decision,
quality verdict or replacement for missing usage. The decision remains subject
to the caller's policy and the complete assessment bindings above.

An unsuccessful attempt trains a contextual outcome: given the original problem
and attempted answer, recall the result that was actually observed. The attempted
answer is input data; the supervised completion is the outcome and evidence.
These bound outcome targets can also supply related rehearsal. The learner checks
the original problem for assessment overlap as well as the wrapper prompt.
Older generic status examples remain byte-for-byte intact; their missing original
context cannot be reconstructed from those records alone.

The learner expresses its own health through `organ-health-v1` in the session
event flow. Every assessed row carries its actual before/after loss and source
identity. A hurt row asks for rehearsal, or for related training evidence when
the assessment row itself must stay excluded. Promotion emits the same
observation, response and applied-action shape as the process and generation
organs. Retaining a serving adapter is observed care; it does not erase the
candidate's need. `observe/organ-health-run.bml` reads these current needs from
the session's `events.jsonl` without another model admission.

After native execution or retrieval misses, `ask` can use the evaluated session
adapter when it has a promoted dialogue teaching. Coding and healing require a
promoted checked example in their respective scope before trying its proposal.
They rerun their original checks; an unsuccessful prediction is reconsidered
for that same input after the serving adapter changes. Healing includes recalled
experience in that input, so changed context also permits a fresh attempt. An explicit coding
model is respected.
Generic generated prose is labelled an unverified proposal. All token streams
use the existing dynamic native generation path, including cancellation,
observed context/memory pressure and observed repetition. Repetition is read
with case and line wrapping folded, at sentence and paragraph length. There is
no fixed total generation budget.

The serialized learner consumes resource needs through `organ-care.bml`.
Its native verified-memory responder accepts an offered retrieval action,
announces responsibility, searches retained sealed examples with the existing
RAG features, and returns references with source hashes and selection reasons.
It chooses the strongest shared-feature/union overlap, retaining exact ties;
all eligible alternatives and their scores remain visible. This is a lexical
retrieval heuristic. Subsequent losses measure the usefulness of its choice.
The requesting learner rereads those references and excludes assessment prompts
before observing whether the resource arrived. That closes an evidence request;
the later loss observations determine whether model capability recovered.

Normal training preparation requests related rehearsal through this same flow
and incorporates accepted examples in the actual gradient batch. Retaining a
new verified example wakes the worker, which also revisits the last candidate's
open needs. Each serialized drain retains one native event reader across its
rounds. A new worker resumes a matching `evidence-reader.view` checkpoint;
without one, its first attendance reads the existing history. Subsequent
attendances consume newly appended complete records and retain current observations.
Attention selects the candidate's live needs directly from retained JSON views.
Each distinct need reads its retained delivery once per attendance; repeated
signals share that request. Historical delivery payloads stay views, and only
the selected observation identity and evidence enter care.
Source discovery runs only when the candidate has a current evidence need.
Semantic selection precedes proof work: unselected references make no validity
claim, while offered and received targets retain source, assessment, binding and
held-out checks. Each source-catalog reading observes a home's shared configuration and assessment
once, alongside each row's own record, withdrawal and assessment. Delivery still
rechecks those references. The shared JSON codec joins escaped spans in balanced
passes, preserving source bytes without repeatedly copying the growing prefix.
Each read uses the file extent observed at entry, leaving later appends for the
next attendance. Incomplete records and admission diagnostics remain with the
reader. The `evidence-reader` event records source extent, consumed bytes,
reader identity, checkpoint bytes, restore duration, read duration and total attendance duration,
so event reading can be distinguished from source discovery and care.
The owned reader release publishes a changed checkpoint once per drain, rather
than copying its current state every training round. Its health event carries
restore and publication timings and any refused publication's resource need.
The checkpoint atomically retains the offset, unfinished record, admission
count and current observations in the reclaimable JSON view lane. A scannerless
BMF cursor reads its compact field index. Header, row count, contiguous field
extents, observation contracts and source anchors must match before reuse.
Each returned span owns its immutable bytes through ordinary Form values.
Plain accessors leave no per-field record or captured byte root. The reader's
one keyed memo is cleared on release or source reset; retained snapshots remain
readable through their owned spans. Primitive fields restore their
typed values directly; containers parse and check their declared JSON kind on
first use. Unchanged containers render from their original
bytes without parsing. No checkpoint value enters primary content identity.
The source's append-only generation remains authoritative: rewriting
earlier bytes requires renewal, as it does for an already open reader.
Live counters distinguish index admission, fields loaded, value parsing and
the complete restore. Large delivery evidence stays deferred until requested.
The [current checkpoint observation](../receipts/native-evidence-checkpoint.json)
retains matched and expanded restore measurements, source parity, reader release,
actual local learning, review attempts and observed rented usage.

The reader indexes current observations by their exact `(organ, flow, aspect)`
key through Form's keyed map. It supplies the matching prior observation to
the existing observation/control reducer and preserves first-seen report order.
The map resolves hash collisions by exact key equality. Publisher renewal and
truncation release the index with the reader's other current values; unchanged
reads reuse the published view.

Unchanged evidence under the same running program image reuses its
response; a miss stays open. Source-compiled processes without an observed image
identity do not reuse another process's answer. The cache reads the executing
image identity, rather than treating current source-file bytes as running code.
The default hearth discovers this repository's other local worktrees through
Git and reads their retained session records by reference. A nondefault
session home stays scoped to that home. `evidence-catalog.json` reports the
discovered homes, absent or unreadable homes, source records, registry exit,
and the executing image identity. Discovery emits its own health reading when
that evidence changes. A failed registry call leaves local evidence available
and reports incomplete coverage. Identical registry bytes share one retained
artifact, and unchanged observations preserve their original timestamp.
Worktree commit movement alone does not change the memory-coverage reading.

Each reference binds its donor's configuration and both current and row-bound
held-out data. Retrieval and consumption check those bytes again and exclude
both the requesting learner's and donor's assessment prompts. A changed record
or assessment invalidates reuse; a donor with unavailable current validation
cannot supply a training target. Prompt normalization also applies to primary
example admission. Donor records and learning state remain in their own homes;
the requesting learner only materializes accepted examples in its training
input. This is filesystem reference sharing, not a shared resident tensor or
model process. Worker drains rediscover local homes; changes in another idle
home do not independently wake this worker yet.

This provider does not turn arbitrary RAG hits, git history, filesystem prose
or generated text into correct answers. It can rehearse a sealed contextual
observation of an unsuccessful attempt without treating that attempt as correct.
The callback carrier accepts other providers without a central error catalogue.

`native-session-experience.bml` supplies working context independently of a
gradient update. It discovers experience in this repository's local hearths,
uses native RAG features to find the strongest related problems, and brings the
other observations from those sessions alongside them. Complete attempts and
their source hashes stay private; health events and stdout expose references,
counts and actual recall time. Source bytes are checked again at consumption.
Missing experience remains an open enquiry, not a claim that the organ learned.

Before an ordinary repair, the healer recovers completed local `.form-heal`
runs' baseline observations and retained candidate/check outcomes. It then
consults session experience before its model routes. A previously checked patch
that matches the current original source can enter the native candidate path,
where the current caller's checker runs again. Diagnosis, patching, review and
session LoRA receive the related attempt context. Independent evaluation does
not receive prior answers; its resulting experience is retained afterward.
An ordinary replay of a remembered evaluation case is practice, not unseen
transfer evidence.

Recovering existing repair runs admits no model and does not schedule a
gradient for every recovered historical observation. Recall
(`nsx-recall(home, query, skill)` in
`form/form-stdlib/native-session-experience.bml`) takes a `query`, an optional
`skill` and a session home, and names the private recall report. Recall does
not yet digest every full host-agent transcript, and lexical matching does not
establish that every lesson in the recovered sessions has been understood.

`form/form-stdlib/native-session-evidence.bml` attends to current needs when
the worker is idle. The worker owns care while running.
`.hearth/session-learning/evidence-current.json` exposes the
current requests; each training row retains `training-evidence.json`. Original
observation, named response, delivered references, fresh resource observation,
consumption and subsequent loss readings remain in the normal event flow.
Promotion consumes the actual emitted progress and per-row health readings; identity binding and
assessment exclusion remain at the executing boundaries.

An interrupted trainer also uses the care carrier. The learner observes missing
worker completion, offers continuation of its already bound inputs, and the
native trainer takes responsibility. Training rechecks the source and adapter
bindings before resuming. Its real result and exit status drive the learner's
fresh execution-continuity observation. A completed training attempt does not
establish promotion; the progress and loss observations still decide that.

Use `session status` for candidate and serving generations, optimizer step,
promotions, pending/refused examples, live-worker state, actual exits and paths
to evidence. Its experience corpus counts retained observations and their distinct
sessions separately from training examples, repository rows and LoRA matrix pairs.
`session pause` lets the current round finish and keeps examples;
`session resume` drains pending work. Stage events link actual per-row token,
gradient, GPU, checkpoint and assessment timings. Inference events identify
the model, adapter, LoRA pair count, token flow, streaming path and stop reason.

This integration trains the native **Llama 3B** adapter. It does not train the
Qwen base used by the coding continuation, the host Codex/Claude model, or
Whisper. It cannot intercept model calls made independently by those host apps.
Lower sentinel loss establishes that measured objective only; fewer rented
calls and broader coding quality must be measured on real subsequent requests.
The existing code and repair checks remain the decision at each such request.

## Learning for the Qwen answering model

`form/form-stdlib/bml/qwen-form-learning.bml` captures teacher-forced model IDs and normalized
hidden states while the native cursor executes a supplied sequence of Form controls.
Recipe birth and execution share their original owner and observation context.
Only a completed sequence with successful delivered results and observed KV
injection supplies targets. An injected refusal is evidence, not a successful
control target, including when hardware completed its work but its output
exceeded the observation envelope. Runtime observations enter
the same KV stream but stay outside gradients. `form/form-stdlib/bml/qwen-head-learning.bml`
learns both vectors of the rank-one residual `h + (A·h) B` through the frozen
Qwen output projection. Native RAM Metal computes stable cross-entropy and
the transposed projection gradient for Q8_0 or float32 weights. Each round
averages the gradients of every verified target at one frozen checkpoint.
Local `choice` tries smaller rates and commits only a finite descent in the
whole target set's mean loss. A refused candidate restores both vectors;
`accepted_steps` counts committed batch updates. Owned buffers and pipelines
retain partial admission and release outcomes. Observation-token meters read
the session's actual numeric counters; generated model IDs remain a separate list.

`form/form-stdlib/lora-lanes.bml` names a frozen-base rank-one lane for every
registry model and for the mouth. Each lane has its own adapter file and its
own role prompt (`form/form-stdlib/role-prompts.bml`). The native voice wears
that prompt for the model it is asking. It does not wear the adapter. Qwen
uses the rank-one head's width, kat-coder uses its embedding width, and
whisper-tiny uses the ear's tiny width. A model whose shape has not been read
keeps width 0, so no adapter bytes are invented for it.
`observe/lora-lanes-run.fk` mints the known lanes under `.form-lora/`. Every
lane stays unpromoted until a held-out check of that role passes. The 3B
session school remains the only full trainer.

`form/form-stdlib/qwen-lora-head.fk` writes measured float32 safetensors A and B
at Qwen width 5120 and admits them through independent buffers; the GGUF
mapping stays unwritten. An absent artifact stays absent. There is no hash-corpus
seed or artificial target. The automatic session worker still selects Llama 3B.

`form-run ./fkwu observe/qwen-form-learning-run.bml` accepts one JSON request:

```json
{"model":"qwen38-q8","adapter":".hearth/candidate.safetensors","examples":[{"prompt":"Keep 7 in a native value node.","controls":["<|form:node-make|>value;7<|/form:node-make|>"]}],"heldout":[{"prompt":"Create a native value node containing forty-three and acknowledge the observation."}],"rounds":3,"rate":0.25,"attempts":10,"tokens":96,"reserve":1024}
```

The door admits one model, renews independent conversation states for examples
and held-out answers, writes the candidate and retains verified control sequences
and decoded A/B evidence at
`ADAPTER.report.json`. Callers supply verified controls; authoring those controls
has its own attribution. No provider call or external training runtime occurs.
The same candidate can be selected explicitly with `generate --adapter PATH`,
including with `--reasoning`. Independent enquiries stay outside gradients.
Read the actual answers and native observations before selecting a candidate;
training loss and successful release establish their narrower claims.
Diagnostic events carry counts and the private report path; the report owns
the original prompts, controls and answers.

The [numerical-library learning witness](../receipts/native-library-learning.json)
records verified controls, whole-target loss and complete native answers on new
budget cases. It keeps base and adapted answers separate. The current candidate
has no observed decoded quality gain and is not promoted; finite loss descent
does not establish useful serving behavior.
The source-backed base-model completion in that receipt discovers and imports
the library, executes the planner in its decoding context and returns the
checked JSON selection `[null,158,231]` for `[0.5,4,8]` GiB. It completes and
releases with zero provider calls. That observed completion is separate from
the adapter comparison; its memory inputs remain worker estimates.

The native session cells live in `form/form-stdlib/native-session-learning.bml`
(learner), `native-session-memory.bml` (journal and state),
`native-session-sources.bml`, `native-session-evidence.bml` and
`native-session-experience.bml`, with `bml/native-session-code.bml` for the
coding continuation. The worker door is `observe/native-session-learning-run.fk`,
which the form-cli `session` verb drives.
