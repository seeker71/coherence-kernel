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

`form-run ./fkwu observe/native-session-withdrawal-run.bml` accepts one JSON
stdin line with the original `session`, `event`, and a nonempty `reason`.
It withdraws the latest completed, unpromoted lesson when no learning is pending.
The learner's lock serializes the operation; a live maintenance owner prevents
that lock from being mistaken for an abandoned worker.

The door verifies the retained parent adapter, optimizer, serving adapter and
base seals, journals the withdrawal, and atomically restores the parent state.
Original records, results and adapter bytes remain intact. `optimizer_step`
reports the selected checkpoint; `learned_rounds` retains cumulative work and
`withdrawals` records completed retirements. The serving adapter stays selected.

The journal excludes the lesson from pending training, source delivery and
replay. Recalled experience labels the original attempt `withdrawn-teaching`.
If execution stops between journal and state publication, continuation refuses
the withdrawn candidate; repeating the same request completes the publication.
A completed repeat returns its original receipt without changing newer state.

This selects a checkpoint from before the lesson; it does not claim arbitrary
unlearning. Older lessons, promoted lessons and descendants in other homes need
their own repair. The door refuses those cases. A replacement teaching uses a
new event identity after the withdrawal has been observed.

One supervisor serializes the learning queue and records the learner's actual
exit and stderr. Before the learner starts, the supervisor admits the dynamic
Metal carrier through `form/form-stdlib/metal-carrier.bml`. A fresh worktree
holds the carrier's source without its artifact, so the supervisor builds it
and records `carrier-admitted`, or `carrier-refused` with the compiler's stderr.
An unloaded carrier is named in the learner's error, never read as an allocator
refusal. Recording does not wait for GPU training during a session.
Normal session close and the one-shot embodiment door retain the parent process
until the supervisor returns; an external interruption keeps the immutable queue
for the next drain. If that parent observes a positive nonzero exit before the
supervisor publishes `worker.rc`, it retains the matching owner's exit and
diagnostic and reports `failed-supervisor-exit`. Unknown wait results remain
indeterminate. A later launch clears the current wait record while retaining
the failed attempt's diagnostic; pending examples keep their original bytes.
The [supervisor-exit observations](evidence/fkwu/session-supervisor-exit.json)
exercise actual child refusal, matching-owner admission, unknown waits,
successor replacement, existing terminal status and diagnostic retention.
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
rounds. The first attendance reads the existing history; subsequent attendances
consume newly appended complete records and retain the current observations.
Each read uses the file extent observed at entry, leaving later appends for the
next attendance. Incomplete records and admission diagnostics remain with the
reader. The `evidence-reader` event records source extent, consumed bytes,
reader identity, read duration and total attendance duration, so event reading
can be distinguished from source discovery and care. A new worker reads the
history again; the cursor is currently process-local.

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

`observe/form-cli-heal-experience-run.bml` recovers existing repair runs with
JSON stdin `{}` (optional `root` and `home`). It admits no model and does not
schedule a gradient for every recovered historical observation.
`observe/native-session-experience-run.bml` accepts a JSON `query`, optional
`skill` and `home`; its stdout names the private recall report. These doors do
not yet digest every full host-agent transcript, and lexical matching does not
establish that every lesson in the recovered sessions has been understood.

`./fkwu observe/native-session-evidence-run.bml` attends to current needs when
the worker is idle. Optional stdin names a session home. The worker owns care
while running. `.hearth/session-learning/evidence-current.json` exposes the
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

`observe/qwen-lora-learning-run.bml` accepts one JSON line on stdin. It captures
completion features and trains an explicitly named Qwen output-head adapter
inside `fkwu`, using the existing Metal carrier. It makes no provider call.
The automatic session worker described above still selects Llama 3B.

The request carries `model` (a registry key), a fresh `output_directory`,
`profile` (`full` or `knowledge-query`), positive integer `context` and `epochs`,
positive finite `learning_rate`, nonnegative finite `max_norm`, and `rows`.
Each row carries a unique `id`, `prompt`, `completion`, and `split` of `train`
or `validation`. Both splits are required; exact prompt overlap is refused.
The caller establishes target correctness and meaningful separation. Distinct
prompts alone do not establish unseen-concept generalization.

One model admission serves the capture and training. Each example receives
fresh conversation state; a refused renewal stops capture with its owner
retained for release. Only completion tokens and their end marker receive
supervision. The `full` profile teaches the answer channel. Validation targets
never enter gradient updates. Cached normalized features allow repeated head
updates without rerunning the transformer.

The run retains the exact request, per-row target IDs and feature offsets,
feature bytes and digest, baseline losses, per-epoch aggregate and per-row
validation losses, checkpoints, release results, and candidate binding.
The binding identifies the sealed base/tokenizer and exact adapter A/B bytes.
Minimum aggregate validation loss, including the unchanged baseline, selects
an **unpromoted candidate**. Answer quality remains unmeasured at this boundary.

For an explicit later comparison, import `bml/qwen-lora-learning.bml` and call
`qll-open-selected(root, prompt, context, profile)` with that retained run's
directory. It verifies the binding and returns the normal owned model session;
the caller must release that session even after refusal. The profile must match
capture. Ordinary input and ownership failures return retained evidence. A
kernel-level Metal or batch-helper exception remains a process-failure boundary;
this BML layer has no exception/finally recovery. The runner emits its result
before returning a nonzero exit on ordinary refusal.

[Executable BML practice](native-bml-execution-learning.md) binds small native
training targets to successful execution, freezes distinct transfer tests, and
compares exact generated proposals before and after native session updates.

Witnesses: `native-session-memory-band.fk`, `native-session-journal-band.fk`,
`native-session-routing-band.fk`,
`native-session-worker-band.fk` and `native-session-code-band.fk` under
`form/form-stdlib/tests/`. The public two-worker, real-3B witness is
`observe/native-session-homecoming-run.fk`; its input is a fresh evidence directory.
