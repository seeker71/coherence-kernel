# Qwen coding inside Form

`code` first offers an evaluated session LoRA a document proposal, unless the
caller explicitly selects a model, sets `evaluation: 1`, or requests
`mode: "review"`. The original caller
checks decide whether that proposal suffices. Otherwise it gives local Qwen one
native session for prompt refinement, planning,
ordered task splitting, implementation, review, and verification. Generated token
IDs and KV state stay resident across tool observations. No Ollama, HTTP,
foreign model engine, shell tool, or rented fallback is invoked by this loop.
The optional LoRA proposal and asynchronous learner run as native `fkwu` workers.
The existing `heal ... local` command is a different route: it still permits
Ollama. Do not use that spelling to request this native-only workflow.

A caller can separately offer the optional [response resource](form-response-resource.md)
for a retained, failing read-only report, either through its standalone door or
as a bounded session-manifest allowance. Form owns that provider CLI process,
usage receipt and unchanged assertions. This does not add a fallback to the
native `code` controller itself.

Native tool assertions compare both returned bytes and exit status. Omitted
`exit` means zero; an explicit canonical nonnegative integer selects another
expected status. For example, a known phrase can be required absent:

```json
{"tool":"rg","arguments":["-F","obsolete claim","answer.md"],"stdout":"","exit":1}
```

This accepts the native search's ordinary no-match result. A missing source or
tool diagnostic still fails, even if its status was requested. Source checks
and read-only report checks use the same comparison. A literal absence check
establishes that absence only; retain the original completeness, source and
behavior checks alongside it.

Failed source assertions retain the exact actual output, exit, diagnostics and
checked document identities in `form-native-source-check-v1`. Full source text
stays in the resident documents and remains readable through native tools; it
is not duplicated inside the failure message. A tool's actual output is kept
intact, including matching text needed to repair the assertion.

Native assessment sessions (`observe/form-cli-response-session-run.bml`, one
manifest file path on stdin) accept an optional nonblank string `run_id`. The
pair `session` + `run_id` names one logical execution. Repeating it verifies the
frozen manifest, terminal seal, requested case identities, retained results,
native assertions and provider evidence before returning `terminal-replay`.
`native_executions_new` and `provider_processes_new` are zero on replay;
historical counts, case receipts and usage events remain retained evidence.
Native execution counts name case-controller invocations, not model admissions.

An incomplete claim remains unresolved, with unknown historical work represented
as null. Owner metadata is admission evidence, not proof that a process is live.
Changed bytes under the same identity and invalid terminal evidence start no
new work. Use a new `run_id` for a deliberate new execution. With no `run_id`,
each invocation remains fresh. `frss-file`/`frss-text` preserve input bytes;
`frss-run` uses JSON encoding, so use consistent input encoding for a replay.
This verifies retained work rather than establishing fresh semantic quality.
The [replay observation](../receipts/2026-09-20-response-session-replay.md)
records the executing boundary and the failed provider implementation attempt.

For a read-only assessment that explicitly requests provider synthesis after
native grounding, the separate [synthesis door](form-response-synthesis.md)
collects the tasks into one provider call and rechecks every report in Form.
This path has zero local-model generation and retains its provider attribution.

### Explicit Qwen adapter sessions

Native embeddings can attach a rank-one Qwen head adapter with
`fcms-open-adapted(modelPath,prompt,context,profile,adapterPath)`, or call
`fcms-attach-adapter(session,adapterPath)` immediately after opening a fresh
session. The returned session owns the adapter. Always use the returned value,
including on refusal, and release its final owner through `fcms-release-ok?`.
Attachment after generation or a tool observation, and a second attachment,
are refused. Inspect `fcms-live?` and `fcms-reason` before generation.

An attached session carries its adapter through ordinary generation, coding
continuations, recipe/NodeID/BML-prefix continuations, tool observations and
independent-stream renewal. Observation and renewal prefill the base stream,
then recompute its final adapted head without replaying an input token. A
successful renewal transfers ownership to its returned session; the previous
stream has already been released and must not be released again.

`fcmd-carrier(session)` exposes the owner; `fcmd-path`, `fcmd-digest` and
`fcmd-owned` read its artifact path, exact admitted A/B tensor-byte SHA-256 and
owned buffer count. Admission checks the 5,120-wide model boundary, artifact
layout, byte lengths and finite float32 values. This digest does not bind
artifact metadata or establish the adapter's training-base identity. The caller
must supply an adapter trained for the selected base model.

This is an explicit Form API. The JSON `code` request and resident turnwheel
do not select this Qwen adapter automatically. It does not promote a candidate
or connect the separate Llama session learner to Qwen. The live
[session witness](../receipts/2026-09-17-qwen-session-adapter.md) establishes
transport, native-head correspondence and resource release for one existing
candidate; response quality and held-out improvement remain unproved.

### Full-vocabulary Qwen head learning

`bml/qwen-lora-full-loss.bml` supplies the loss and gradient needed to train
the head adapter against every output token. It reuses the native Llama
cross-entropy operation and adds a chunked transpose multiply over Qwen's
immutable Q8_0 output projection. It does not expand that matrix into float32.

- `qlfg-open(rows,cols)` returns an owner for six scratch buffers and cached
  Metal pipelines. `rows` is the complete vocabulary and `cols` the hidden
  width. Check `qlfg-live` before use.
- `qlfg-loss(owner,logits,target)` returns full-vocabulary cross-entropy.
  `target` is an integer token index. Invalid targets, refused admission and
  nonfinite loss return `nothing`.
- `qlfg-gradient(owner,projection,logits,target)` returns
  `[ok,loss,hiddenGradient]`. It computes `W^T(softmax(logits)-onehot(target))`.
  The existing `lbw-backward` carries this through `h + scale*B*(A*h)` to A/B.
- `qlfg-owned` counts owned buffers; `qlfg-close` synchronizes and releases
  them, including partial admission. Release once even after refusal.

The caller owns and validates the supplied buffer handles, their lengths,
the Q8_0 storage type and matching geometry. This API bounds packed byte
offsets before multiplication and requires whole Q8_0 blocks per row. It
synchronizes pending device work at entry and reads back a finite loss and
hidden gradient. It neither updates parameters nor writes an adapter by itself.

The [actual Qwen witness](../receipts/2026-09-17-qwen-full-vocabulary-gradient.md)
measured a 248,320-by-5,120 gradient in 13 ms. A private persisted step reduced
training loss; two held-out losses increased slightly while both greedy
answers stayed correct. The candidate remains unpromoted. These observations
establish a working learning operation and an adverse generalization reading,
not response-quality improvement or end-to-end training throughput.

### Cached head batches

`bml/qwen-lora-head-batch.bml` reuses normalized hidden features from a frozen
Qwen teacher-forced pass. A sample is `[float32Bytes,targetToken]`; callers
own the completion mask and the separation of training, validation and tests.
Head optimization can revisit those features without replaying the transformer.

`qlhb-gradients(ctx,lossOwner,adapter,samples)` returns
`[tokenCount,meanLoss,meanGradientA,meanGradientB]` over the full vocabulary.
The mean weights every supplied token equally. `qlhb-loss` returns the count
and mean loss without gradients. Both require a matching Q8_0 projection and
finite, complete normalized features. Empty batches are refused.

`qlhb-optimizer(adapter)` owns six gradient/Adam buffers while borrowing A/B
from the adapter. `qlhb-step(state,batch,rate,step,maxNorm)` writes the complete
mean gradients and uses the native finite-gradient check, clipping and Adam
update. Release those buffers through `nlb-state-close`; its return value is
a success flag, not a count. The adapter and full-loss scratch retain their
separate owners. Model weights stay unchanged.

These calls overwrite the context's head scratch. Before resuming generation,
refresh the session head from its retained stream, or attach the persisted
candidate to a fresh session through the explicit adapter API. The projection
boundary synchronizes pending work, including optimizer initialization. The
API chooses no corpus, checkpoint or serving candidate by itself.

The first real batch experiment trained on six verified boolean repair
programs and selected against two separate programs. Validation loss fell
from 0.312946 to 0.097776 over eight rounds. On the original, separate review
utility repair, the selected adapter still produced 14 compiler errors and
repeated comments until the 1,536-token limit. It remains unpromoted. This
establishes reusable native batch learning, with unsuccessful transfer to that
repair task; see `receipts/2026-09-17-qwen-cached-head-batches.md`.

## Native BML boolean repair

The coding loop offers `repair-bml` when its documents contain a
`section [form.bml]` declaration. A model can call
`{"tool":"repair-bml","arguments":["candidate.bml"],"input":""}`
during implementation on a caller-writable document. The native operation
inserts right-nested binary calls: `and(a,b,c)` becomes `and(a,and(b,c))`;
`or` follows the same pattern. Nested calls are handled in the same pass.

The source compiler names invalid boolean arity before lowering the call:
`and(1,1,1)` reports `form.bml boolean call requires 2 arguments; got 3`.
The call itself follows in the diagnostic. Valid binary calls keep their
short-circuit behavior; repairing a rejected call remains an explicit edit.

The tool preserves every original byte and inserts only operator names and
parentheses. It reports native attribution, edit byte locations, original
arity and before/after hashes. Strings, line comments, other dialect sections
and qualified names are preserved. Malformed groups, empty arguments,
`and`/`or` function declarations, block comments and unbraced `do` are refused
without changing the document. Source size and nesting are bounded.

This operation repairs source syntax; it does not change compiler acceptance
or establish correct behavior. The model must still use `verify` and resolve
any remaining failure. Review mode and non-writable documents cannot invoke
the mutation. Unrelated coding contexts receive no additional tool guidance.
Each `verify` result identifies the exact resident documents passed to the
callback by ID, path, byte length and SHA-256. An unchanged error after an edit
therefore carries the changed source identity. The callback's output and exit
status stay intact; these identities establish which input was supplied,
not the adequacy of the caller's checks.
The pure source API is `fbr-repair(source)` in
`bml/form-bml-boolean-repair.bml`, returning
`[ok,candidate,edits,reason]` without evaluation or file access.

## Rehearse a small BML repair

In implementation, `rehearse-bml` takes `arguments=[path]` and empty input.
The native allowance is at most `64` checks, including the original-source
check. An optional second argument sets a smaller positive decimal budget.
For example:
`{"tool":"rehearse-bml","arguments":["candidate.bml"],"input":""}`.

The native search wraps one bare value expression with a unary call already
present in the same source's `form.bml` sections. It preserves all other
bytes. Each candidate starts from the original source and runs the same
caller-owned source checker as `verify`. A passing original stays unchanged;
a passing candidate changes only the writable target; exhausted or refused
searches retain the original. Review and final verification remain in the
coding workflow. This searches one repair family, not arbitrary programs.

The result retains each actual check's source digest, edit span, wrapper,
status and observation. Check counts include malformed callback responses.
Read-only review and non-writable targets cannot invoke the search. The tool
accepts no replacement checker, test contract or host command from a model.

The budget bounds checker invocations. The caller must separately bound each
candidate's execution time: even a small pure edit can make recursion fail
to advance. The existing native process organ supplies explicit deadlines
and owned-process cleanup; timeouts remain failed observations. The checker
also owns admission, test coverage and the meaning of a pass. Source syntax
must already be admissible; boolean-arity repair remains a separate operation.

`fbvs-search(source,checker,contract,budget)` in
`bml/form-bml-value-search.bml` is the pure search API. It returns
`[status,source,attempts,reason]`, performs no IO or model calls itself, and
delegates execution exclusively to the caller's callback.

### Caller-enabled rehearsal after a failed action

A coding request can set `native_rehearsal_checks` to an integer from 0 to 64.
Zero, the default, disables automatic rehearsal. An in-process caller uses
`fcap-with-rehearsal-budget(state, allowance)`. This is a total allowance for
automatic checks in that request, including original-source checks; it does
not change the explicit tool's allowance. The caller still bounds each check's
execution separately. Review requests cannot enable it.

When an implementation edit finds one matching occurrence but supplies
identical old and new text, the original failed action is retained. For a
writable BML source, the policy can then run native rehearsal without another
model response. Each path and source digest is attempted at most once in the
retained state. Only a passing candidate changes the document. Failed,
malformed or exhausted attempts preserve it. Every actual check spends the
caller allowance; model tool and reply counts do not increase for native work.

A failed `verify` during implementation can trigger the same search when
exactly one caller-writable BML document is available. Multiple eligible
documents leave target selection to the model's explicit tool call. Passing
or malformed checks, other stages and read-only review do not trigger this
search. The initial verification remains counted separately; rehearsal's
own original-source check and candidate checks spend the native allowance.

The model receives the native source attribution, final check observation and
exact current source or reconstructed source patch. Full attempts and the
original triggering failure remain in `native_rehearsals` in the returned
result and checkpoint. `native_rehearsal_checks_remaining` reports the balance.
The candidate still goes through task completion, review and final caller
verification. Making this assistance available does not attest model adoption
or broader correctness.

## Complete text review with explicit edits

`bml/form-cli-review-edits.bml` connects indexed participation to actual text
changes. `fcre-packet(reportText, fields)` returns `[ok, packet, diagnostic]`.
The caller selects unique existing top-level string fields. The packet carries
the exact report's SHA-256 base and ordered byte spans across those fields.
Its punctuation heuristic and limits are the existing review-span utility's.

`fcre-apply(reportText, fields, message)` requires the matching base and exactly
one ordered decision for every span:

```json
{"base":"<packet base>","edits":[
  {"id":0,"status":"supported","action":"keep","reason":"brief assessment"},
  {"id":1,"status":"style","action":"replace","reason":"brief assessment","text":"Actual replacement text. "},
  {"id":2,"status":"action-gap","action":"remove","reason":"brief assessment"}
]}
```

Statuses are `supported`, `inference`, `unsupported`, `style` and `action-gap`.
They are the reviewing model's judgments; the carrier does not certify them.
Each action is `keep`, `replace` or `remove`. Replacement text must differ
from the original and be nonempty; removal is explicit. Text is joined exactly,
without added punctuation or spacing. Keep/remove cannot carry replacement
text. Unselected values, including boolean observations, remain unchanged.

The result is `[ok, candidateJson, diagnostic]`. A stale base, missing or repeated
index, malformed decision or unchanged replacement refuses the whole edit;
the original remains with the caller. Retain the packet and model message
beside the candidate, then run the original response checks and read the whole
answer. Complete participation establishes coverage, not correct judgments,
coherent prose or response-quality parity. This API is a callable native
review/composition step; ordinary generation policy is unchanged.

## Call without knowing Form syntax

In form-cli, enter `code` followed by a JSON object. The standalone native door
is `form-run ./fkwu observe/form-cli-code-run.fk`; send that same JSON object as
one stdin line, without the `code` prefix.

For larger requests, use `code @request.json`, or send `@request.json` as the
standalone door's stdin line. This reads the complete native file before JSON
admission. The host line reader returns at most 8,191 bytes per call; passing a
large inline JSON request through that reader loses the remainder before
admission. The file-backed route preserves the existing request-size checks
and needs no additional runtime. For example, this request may be passed inline
or saved to a file:

```json
{"goal":"Enable local coding in config.json. Preserve provider and remote.","documents":[{"id":"config","path":"config.json","text":"{\"enabled\":false,\"provider\":\"native-qwen\",\"remote\":false}\n"}],"writable":["config.json"],"checks":[{"tool":"jq","arguments":[".enabled","config.json"],"stdout":"true\n"},{"tool":"jq","arguments":["-r",".provider","config.json"],"stdout":"native-qwen\n"},{"tool":"jq","arguments":[".remote","config.json"],"stdout":"false\n"}],"model":"qwen38-q8","context":8192,"turns":64}
```

`turns` limits generated replies, including replies that request tools. Native
admission and subsequent observations disclose the limit, completed count and
remaining replies. The final available reply is identified before generation,
so the model can use it for the supported deliverable and retain unresolved
findings honestly. Source observations and caller checks remain intact; budget
awareness grants neither extra replies nor permission to claim unchecked work.
Initial reasoning and same-resident review follow-ups carry the same counters.

Optional `max_reply_tokens` sets a positive integer ceiling for **each**
generated reply. For example, `"max_reply_tokens":384` bounds generation
while retaining the normal native admission: original goal, source documents,
source observations, retained report, tools and checks. The model sees the
ceiling before generation. Omission preserves the existing context-only
default; zero, negative, fractional, string and null limits are rejected.

The decode quantum shrinks to the remaining allowance. If the ceiling arrives
before the reply ends, the controller returns `attention` with
`reply-token-budget-reached;partial-reply-retained;no-action`, preserves the
last candidate, and releases the model. A fragment that happens to parse as
JSON still cannot act without the completed reply boundary. Private partial
text is referenced by `form-code-incomplete` metadata; it does not enter the
framebuffer. A complete reply still runs the unchanged caller checks.
The result reports `max_reply_tokens` (0 means no separate ceiling).
The allowance survives checkpoints; on resume the current request selects
the allowance, with omission restoring the context-only default.

Documents are **resident values**, not filesystem permissions. The result returns
the candidate document values; it does not overwrite repository files. The caller
owns loading files, publication, stale-file checks, and repository landing. This
first door therefore does not claim autonomous arbitrary-repository completion.
Multiple supplied documents and multiple ordered tasks are supported. Only paths
in `writable` can change, during implementation or authorized code-mode repair.

For a document with a length requirement, a caller may supply a range check:

```json
{"kind":"word-range","path":"answer.md","minimum":350,"maximum":450}
```

It counts the current resident document with the same ASCII-whitespace counter
as ordinary generation, including headings. Bounds are nonnegative integers
and include both endpoints. A missing document fails separately from an empty
document. A failed `verify` or completion check returns the observed count,
required range and a computed repair direction, minimum word change and midpoint
target to the existing repair flow. The target guides revision; the original
range remains the acceptance rule. Passing counts receive no repair target.
This establishes length only;
source fidelity, completeness and useful wording still require review. The
check reads document bytes and accepts exactly the four fields shown above.

For a read-only report, use `field` in `report_checks` to select one exact
top-level JSON string field:

```json
{"kind":"word-range","field":"answer","minimum":350,"maximum":450}
```

The same counter runs on the decoded string, excluding JSON syntax and other
fields. A missing, null or non-string field fails even when zero words are
allowed. Source checks and all subsequent report checks remain in force.
The observed count enters the ordinary report repair flow, including a
caller-offered provider resource. Both forms accept exactly their four fields;
`path` selects a resident document and `field` selects report text.

### Edit a retained response

A retained prose answer can be a caller-writable document in a `mode: "code"`,
`code_entry: "direct"` request. Supply the actual draft, the original enquiry
and source documents, and the original checks. Keep sources read-only. An
existing answer requires guarded `edit`; `write` creates an absent path and
refuses to overwrite an existing document. The guard supplies the current
document identity after refusal, so the native controller can repair its call.

Read the returned document, then re-run its checks before publishing it. A
completed coding result is distinct from its review verdict and from the
answer's usefulness. Use `evaluation: 1` when comparing evaluated responses;
their text stays out of learning and continuity reuse. Caller-selected edits
executed through the native tools retain their caller attribution separately
from model-selected edits.

The [retained answer-edit movement](../receipts/2026-09-25-native-answer-edit.md)
changed a repeated 509-word response to 397 words and repaired several source
distinctions. Its native review still accepted self-praise, which the arriving
reviewer removed through explicit native guarded edits. This establishes those
actual changes, not a general quality or resonance verdict.

The [subsequent native review](../receipts/2026-09-25-native-answer-review.md)
retains its acceptance findings and exact reviewed identities. It revised the
translation explanation, returning 411 words, while still accepting explicit
self-assessment and describing it as absent. The retained reason makes that
contradiction inspectable; a nonempty explanation is not proof of its judgment.

### Direct implementation entry

For a fully specified task, set `"code_entry":"direct"`. A fresh coding
session begins in implementation with one pending task referring to the original
goal. The full goal is admitted in the model context and on resume; later tool
observations carry that reference, including a reminder to complete explicitly
requested tool calls. It does not generate separate refinement, planning and
task-splitting replies first. Original documents, writable paths and caller checks remain
authoritative. The brief and plan stay empty rather than claiming work that
did not occur.

The candidate still goes through model review and the caller's native checks.
An unchanged candidate cannot complete. Failed checks retain the existing
repair and replanning path. Omitting the field, or using `"staged"`, keeps the
ordinary entry. The option applies to coding; review has its own
`review_entry`. A resumed checkpoint retains its saved phase. Optional initial
reasoning is independent of this entry choice.
The controlled timing and source-quality observations are recorded in
[`direct-code-entry`](../receipts/2026-09-16-direct-code-entry.md); fewer stages
alone did not make the initial-reasoning variant faster.

### Resident instruction context

Coding and review bootstraps receive the shared current Form meanings from
`form-cli-qwen-meaning.bml`: symbol identity, model tokenization, language
surfaces, source evidence and reference-conditioned persistence. This is the
same meaning source used by the ordinary Qwen teaching overlay. It enters
alongside the code/review protocol, including after context renewal; caller
goals, document trust boundaries, writable paths and verification retain their
authority. Teaching delivery is observable in the bootstrap. The returned
answer establishes whether the model used it accurately.

Within one resident model context, repeated role instructions may be replaced
by a shorter reference to the instructions already supplied. The full text
enters at bootstrap and when a role's exact instruction is first encountered.
The controller records a later instruction only after its observation has
completed successfully. A failed or partial observation does not mark it as
delivered. A resumed admission resets that record and supplies the current
role's full instruction again. Tool results, failure evidence, pending work and
caller constraints retain their existing paths.
The measured coding and review pairs are recorded in
[`resident-instruction-context`](../receipts/2026-09-16-resident-instruction-context.md).

### Read-only review

Set `"mode":"review"`, keep `writable` empty, and supply both source `checks`
and `report_checks`. The result carries a separate nonempty `report` string;
every source document must remain byte-identical. A review never needs a dummy
edit or writable report file. For example:

```json
{"mode":"review","evaluation":1,"goal":"Inspect config.json without changing it. Use verify once. Return a report string containing a JSON object with enabled, provider and remote copied accurately from the source.","documents":[{"id":"config","path":"config.json","text":"{\"enabled\":false,\"provider\":\"native-qwen\",\"remote\":true}\n"}],"writable":[],"checks":[{"tool":"jq","arguments":[".enabled","config.json"],"stdout":"false\n"},{"tool":"jq","arguments":[".remote","config.json"],"stdout":"true\n"}],"report_checks":[{"tool":"jq","arguments":[".enabled"],"stdout":"false\n"},{"tool":"jq","arguments":["-r",".provider"],"stdout":"native-qwen\n"},{"tool":"jq","arguments":[".remote"],"stdout":"true\n"}],"model":"qwen38-q8","context":8192,"turns":32}
```

`report_checks` use the existing read-only native tool assertions. Each receives
the returned report as stdin with **no document access**; an `input` field is
refused rather than silently replacing the report. Exit zero, empty diagnostics
and exact stdout are required. These checks establish only their assertions;
extracting configuration fields is not a benchmark of code-review quality.

Read-only review accepts the requested JSON report object directly. The older
`{"verdict":"accept","report":"evidence-backed findings"}` wrapper remains
available, and its `report` may also be an object or array. Every form reaches
the same caller-owned report checks. Here `accept` submits the report for
checking; it does not declare that the audited source passed. Negative findings
belong in the report. `reject` with a `reason` means continue inspection.
Read-only restrictions apply in every role, including repair.

A rejected read-only review retains its finding in the review stage. The
model may inspect further or submit a report containing negative findings;
it is not sent to repair unchanged source documents. Failed report assertions
still enter the report-repair loop. Coding review rejections still enter
implementation repair.

After a failed report assertion, a read-only review may submit
`{"report":<complete corrected report>}` immediately. The original source and
report checks run again; this path preserves the read-only document boundary.
The existing diagnosis/change/replan path remains available for deeper work.
Failure feedback distinguishes a tool failure from a successful check whose
stdout differs. It names the tool and query arguments with the actual result;
the caller's expected `stdout` is withheld. Review guidance keeps conditions for
the requested decision separate from limits on broader claims.
Its shared native function lives in
[`form-cli-review-guidance.bml`](../form/form-stdlib/bml/form-cli-review-guidance.bml).
An explicit textual requirement can also have a necessary presence check:
`{"tool":"jq","arguments":[".answer | contains(\"?\")"],"stdout":"true\n"}`
checks for question punctuation in the answer field. This does not establish
that the question is relevant or advances the enquiry. Preserve the original
checks and assess those semantic requirements in the actual answer.

The next action follows the supported current decision; broader evaluation
becomes a prerequisite only when the supplied policy requires it. The guidance
asks for source-backed requirements and thresholds.

Initial review uses `fcrg-substance` to ask for the complete enquiry, relevant
source values and identities, an explanation of preserved paths, and an immediate
action whose prerequisites follow the supplied rule. Broader research remains a
separate optional follow-up. Review after native repair keeps its narrower
instruction about the paired evidence and verified proposal. These are generation
instructions; their effects must be checked in the returned answer and are not
established by passing report-field checks.

The JSON review door now normalizes exact `"yes"`, `"no"`, `"true"` and
`"false"` string literals when a caller-owned check expects a boolean result
from one root field. `form-cli-boolean-report.bml` uses only that result type;
it never chooses a truth value from the expected assertion. All original source
and report checks run again. A contrary decision stays failed, and native repair
history attributes accepted normalization as `caller-native-repair`. Other
fields and source documents retain their values. Missing values, numeric 0/1,
null, ambiguous text, duplicate root keys and unsupported query shapes are
left alone. This changes representation, not the answer's prose or reasoning.
`fcaq-live-checker` installs this behavior in the ordinary JSON request path;
the four-element `fcaq-checker` contract remains available to embedding callers
that attach their own repair. Type-mismatch feedback separately names expected
and actual JSON types when both outputs are valid JSON, while keeping expected
values hidden. Neither mechanism establishes semantic quality.

Caller-owned native repair can restore an exact source quotation when a model
has changed only its ASCII whitespace. The pure helper
`fcqe-restore(source, quote, maxBytes)` in
`form/form-stdlib/bml/form-cli-quote-evidence.bml` returns
`[available, sourceExcerpt, startByte, endByte, reason]`; endByte is exclusive.
Exact quotations remain unchanged. A unique whitespace-normalized match yields
the original source bytes and offsets. Changed text, ambiguous normalized
matches, empty quotations and caller byte-limit violations yield no candidate.
The caller retains source identity and these transitions, proposes the restored
report through its existing native repair callback, and the controller reruns
the same complete checker. This helper does not enable automatic report edits
or add a report schema. Quote membership establishes an excerpt's provenance;
interpretation, topic coverage and answer quality require their own observation.
The quotation band witnesses both restoration and the existing checked-repair
boundary, including a changed-word candidate that remains in repair.

For a caller explicitly reading a single-level Markdown blockquote,
`fcqe-blockquote-restore(source, quote, maxBytes)` also recognizes line-leading
`>` markers followed by whitespace, with up to three preceding spaces. It
maps a unique whitespace-folded match back to the original source bytes,
including intervening markers. Inline `>`, joined words, changed words and
ambiguous projected matches remain unavailable. This is an explicit reading
view, not a complete Markdown parser or a change to the default quotation
checker. The returned source excerpt must still pass that original checker.

Observation prefill uses the existing sliced batched route when the admitted
context records a scratch width greater than one. Each outer slice is bounded
by that actual width and the normal admission slice limit. Legacy contexts and
width-one contexts retain the per-position barrier route. Both routes keep the
same resident state and validate position, pending-token range and settled GPU
state. A failed submission is not retried: the existing caller preserves the
last completed counters and retains ownership for release, without claiming
that physical state was rolled back.

The effectful comparison `observe/form-cli-observation-prefill-compare.bml`
compares the retained barrier reference with the actual serving dispatch on the
registered local Q8 model. It uses fresh stream states, identical prefixes,
nonzero observation positions and 16 continued predictions for each pair.
Recorded scratch widths are 64, 4 and 1; cases include short, boundary-crossing
and multiple-slice observations. It reports timing, prediction agreement,
settled GPU state and release. The probe does not alter dispatch configuration
or establish equality outside its samples. Run it without another GPU owner.

At each model admission, including checkpoint resume, the controller records
the exact resident documents supplied in that context. A successful single-file
`read` still executes. When its output equals a supplied document byte-for-byte,
the model observation carries `stdout_reference` with its id, path, byte count
and context scope. The original content remains in the model context and the
caller-owned documents. A changed read may carry `stdout_patch`: one exact
line splice against that original admission document, with the current changed
lines in full. The controller reconstructs the current read from that base and
splice, checks byte equality, and uses it only when its serialized observation
is smaller. Every splice refers to the admission snapshot, never to a previous
splice. Errors, absent snapshots and splices that save no space return the full
result. `cat` always returns full current text; focused queries remain available.
Source/report verification continues to use actual document bytes; it does not
consume these model-context references or splices.

Coding and read-only reviews may opt into `"document_context":"catalog"`. The initial
prompt contains source IDs, paths, byte counts and current `resident_sha256` identities; native tools retain all
original source text. `read`, `cat` and focused queries retrieve that text on
demand. The admission snapshot starts empty, so a catalog cannot produce a
reference or patch against source text omitted from the prompt. Reads continue
returning full results in this mode. Resume starts a fresh catalog context;
previous tool reads do not implicitly establish visibility there.

Catalog requests can also supply `source_queries` to put selected native source
observations into the initial model context:

```json
"source_queries": [
  {"tool":"rg","arguments":["observed","evidence.txt"]},
  {"tool":"sed","arguments":["-n","10,20p","notes.md"]}
]
```

Each row names a read-only native tool and a string arguments array. Form runs
the query over the actual resident documents and includes its output, exit
status and diagnostics. Caller-supplied `input` or `stdout` fields are rejected,
including empty or null values. Other row metadata is not admitted as an
observation. A failed query remains a failed observation. The request chooses
the queries, including on resume; omission clears old query selections.
Checkpointing retains the commands and recomputes observations from the current
resident sources at admission. An excerpt does not establish that the full
document was supplied, so subsequent reads still return full source text.

This is caller-directed retrieval. It does not choose relevance automatically,
replace original documents or source/report checks, or establish semantic
quality. With no queries, the existing prompt is unchanged.

The default is `"full"`. The request selects the admission mode, including on
resume, without changing its source/report assertions or read-only boundary.
Catalog mode accepts coding and review requests. A smaller initial prompt is a
measurable resource change; answer quality and total session time still need
observation, including subsequent source reads.

Bounded read-only provider synthesis (`form-cli-response-synthesis.bml`) also
honors `document_context="catalog"` and these `source_queries`. Form retains
and checks the complete original request. The provider receives the document
catalog, actual native query outputs with exit/status evidence, and an explicit
partial-visibility statement. It has no further tool access in this lane, so
the caller must select enough evidence for the enquiry; missing facts remain
missing. Query failures are preserved, never promoted into successful reads.
Full-document requests keep their existing packet and replay identity. Catalog
requests bind replay identity to the exact provider prompt, so an older
full-document admission cannot masquerade as a selected-source run. New
admissions retain that exact prompt alongside the original manifest.

For composition claims, `fccw-observe()` in
`bml/form-cli-composition-witness.bml` returns an actual native observation:
same-composition interning, changed recipe identity, retained original children,
and forward/reverse tree patches. The public door runs the observation and its
organ-health response:

```sh
form-run ./fkwu observe/form-cli-composition-witness-run.bml
```

The report records `[2,3]`, its `[2,4]` revision, and the reconstruction using
retained information. Collection of unreferenced cells, durability after process
exit, automatic response-text persistence and the identity of an absent reply
remain explicitly unobserved. A caller can retain this report as a source
document and select it with an ordinary native source query. Such an addition
changes the model's evidence and belongs in the retained request. Its execution
result does not itself establish response quality.

Native embedding callers can request one follow-up before releasing a review's
model through `bml/form-cli-review-followup.bml`:

```text
followed = fcrf-admit(model, initialReviewState, checker, context, turns, feedback)
outcome = head(followed)
```

The first review must complete before the caller's feedback is injected into
the same live context. The same checker, read-only boundaries, context capacity
and total turn allowance govern the follow-up. There is one model admission
and a final release. Ordinary review entry remains unchanged.

The returned list contains the ordinary outcome, the first report, its check
count, whether the follow-up observation was admitted, and the selected route.
Keep the first report alongside the final one. Exhausting the shared turn
allowance leaves the requested follow-up incomplete; a previously completed
state without its live context cannot satisfy a new follow-up. Empty feedback
requests no additional pass. The caller owns the feedback; this API does not
provide an independent reviewer or infer semantic quality from a second pass.

`bml/form-cli-review-bindings.bml` checks the evidence links in a retained
review. Call `fcrb-check(draftObject, reportObject, sourceDocuments)` with parsed
JSON objects and caller-selected native documents `[id, path, text]`. Exclude
candidate answers from the evidence list unless they are explicitly the source
being assessed. Each `review_findings` row carries:

- `draft_field` and an exact, nonempty `draft_quote` in that original field;
- `source_path` and an exact, nonempty `source_quote` in that supplied document;
- `explanation`, and `status` equal to `applied` or `unresolved`;
- `replacement_quote`: an exact, nonempty quotation in the changed report
  field when applied, or an empty string when unresolved.

An applied removal uses explicit `operation="delete"` with an empty
`replacement_quote`. Its original quotation must be absent from the changed
field. Missing `operation` means `replace`; an empty replacement alone never
counts as a deletion. Both operations retain the original and source quotes.

The result is `[passed, observation, appliedCount, unresolvedCount]`. A failed
check identifies the first invalid link; its counts do not summarize partial
success. Keep applied findings so the correction remains inspectable. The
checker preserves exact whitespace and rejects an unchanged field claimed as
applied. Callers retain their original source and report checks alongside it.
An empty findings array establishes no correction. Valid links establish
neither entailment nor completeness: the reviewer can quote a real source and
still reason incorrectly. This boundary is covered by a counterexample in
`tests/form-cli-review-bindings-band.bml`.

`bml/form-cli-review-spans.bml` lets a caller make review participation explicit.
`fcrs-spans(text)` returns `[startByte, endExclusive, exactText]` rows covering
the input. It splits after ASCII `.`, `?` or `!` followed by whitespace or the
end, absorbing following ASCII whitespace. A final unpunctuated suffix is kept.
This punctuation heuristic preserves UTF-8 bytes; it does not parse sentences
or enumerate every proposition. Empty text has no rows, and whitespace-only
text has one row.

Use `fcrs-complete?(text, rows)` to verify an exact, uninterrupted partition.
Use `fcrs-ids-complete?(rows, ids)` to require one integer index for every row,
from zero in order. Missing, repeated, reordered, extra and noninteger indices
fail. Callers associate their review judgments with those indices and retain
the original source/report checks. Complete participation establishes no
semantic verdict, and a span may contain multiple claims or expressive language.
The ordinary review path is unchanged. `tests/form-cli-review-spans-band.bml`
checks partition and participation behavior, including malformed rows.

Review callers can let the model select source lines instead of reproducing
quotation bytes. `bml/form-cli-review-source-lines.bml` supplies
`fcrl-propose(sourceDocuments, reportText, maxSourceBytes)` and the controller
callback `fcrl-repair([sourceDocuments, maxSourceBytes], subject)`. A finding
uses its existing `source_path`, adds `source_lines: [first, last]`, and leaves
`source_quote` absent or empty. Indexes are one-based inclusive integers into
the original caller document. Native `rg -n` observations expose those indexes.
The resolver retains Markdown, UTF-8 bytes and original line terminators;
a trailing newline creates no extra physical line.

The proposal has the ordinary native repair shape
`[available, candidateText, "native-source-lines", evidenceOrReason]`.
Evidence identifies each changed finding, its source path, selected lines,
source byte length and half-open byte offsets. Keep the raw report and source
snapshot alongside that candidate. The existing controller records the native
repair and reruns the full caller checker. Use
`fcrl-check(draftObject, reportObject, sourceDocuments, maxSourceBytes)` in that
checker: it validates coordinates even when a supplied literal quote already
passes, then calls the unchanged `fcrb-check`. Install the repair callback
explicitly when constructing the six-element checker. It does not
automatically replace a caller's existing repair callback.

Conflicting explicit quotations, duplicate object keys, ambiguous source paths,
invalid or blank ranges and source byte-limit violations yield no candidate.
Literal quotations remain unchanged, and resolution is all-or-nothing across
findings. The pure `fcqe-lines(source, first, last, maxSourceBytes)` helper in
`form-cli-quote-evidence.bml` exposes the same byte selection separately.
This resolves citation representation only. The selected passage can still be
irrelevant, the interpretation incorrect, or the answer unhelpful.

### Amend a retained report

After a JSON report fails review, the repair observation includes a
`report_amend.base` SHA-256 of its exact retained bytes. The model can submit
only the changed values:

```json
{"report_amend":{"base":"<observed hash>","replacements":[
  {"path":["answer"],"value":"Corrected answer"},
  {"path":["review_findings",0,"source_quote"],"value":""}
]}}
```

For a small correction inside a string, `text_edits` replaces exact text
without regenerating the whole field:

```json
{"report_amend":{"base":"<observed hash>","text_edits":[
  {"path":["answer"],"old":"exact unique text","new":"corrected text"}
]}}
```

Each selected value must be a string. `old` is nonempty and must occur exactly
once, including overlapping occurrences; an empty `new` deletes it. Unchanged
edits are refused. Use either `text_edits` or `replacements` in one amendment.
Text edits follow the same path, identity and atomicity checks, and rerun the
original source and report checks. The model chooses their meaning.

Paths address existing object keys and zero-based integer array indexes.
An amendment has 1–32 edits at nonoverlapping paths, each 1–32 path segments
deep. Missing paths, stale bases, duplicate object keys, overlapping paths
and malformed edits refuse the whole amendment and preserve the retained
report. This door replaces values; it does not insert or remove array items.
Unselected JSON values remain unchanged; emitting the candidate can change
JSON formatting.

If all report values remain equal under the native JSON comparison, the
amendment returns an explicit unchanged diagnostic. It retains the candidate,
failed-check evidence and check count, without adding an amendment record.
Changing object key order alone is not a value correction.

The retained report is supplied when a fresh model context opens in repair.
Later repair observations carry its current hash and amendment instructions,
without repeating the report text.

A caller can also request revision after the original assertions passed. Keep
that feedback attributed to the caller; it is not an automatic check failure.
For a released session, build a fresh repair context with the original request,
documents and exact retained report:

```bml
let docs = fat-wire-value(fat-wire-documents(request));
let state = fcap-begin-context(fcap-rework(
  fcap-with-report(fcaq-initial(request,docs),retainedReport),callerFeedback));
```

Check `fcap-amendable(state)` before model admission. Merely setting the report
with `fcap-with-report` leaves the state outside repair, so the fresh bootstrap
does not supply it as an amendment target. Open the new session using
`fcac-bootstrap-budget(state,turns)`, retain the original
`fcaq-live-checker(request)`, and let `fcac-resident` complete and release it.
This reconstructs context from retained evidence; it does not recover old KV.
Read the revised answer after its checks: the observed dialogue revision fixed
an unsupported guarantee and draft placeholders while leaving other caller
feedback unresolved. [Revision receipt](../receipts/2026-09-17-dialogue-revision-and-generation.md).

The complete candidate runs through the original checker and any configured
native repair callback. Source documents and the caller's contract stay
unchanged. `report_source="model-amendment"` identifies model-authored edits
applied by the native carrier; a subsequent native repair has its own
attribution. `model_amendments` retains each before report, submitted message,
candidate and actual checker result, including failures. A successful edit
operation establishes no semantic quality. The explicit native-review
continuation still requires its complete report submission.

Each completed model reply emits `form-code-reply` metadata: JSON validity,
wrapper/tool presence, report type, before/after role and check/repair counts.
It includes no answer text, prompt text or document content. These observations
separate protocol handling from the candidate's semantic quality.
The actual completed model reply is retained separately beneath the private
hearth's `code-memory/replies/` directory. `reply_evidence` points to it, or is
null when retention failed. This is diagnostic evidence, never a verified
training target. Reply contents are not placed in the framebuffer.

`model` defaults to `qwen38-q8`, `context` to 8192 positions, and `turns` to 64
model replies. Both Qwen registry rows require actual artifacts, seals, tokenizer
indexes, tokenizer crystals and a wired native session lane. Actual digest verification happens on
admission. Missing resources never select another provider. Context or caller
budget exhaustion returns `attention` with candidate values retained; partial
generation is never accepted as a completed action. After the last permitted
reply, the controller checks that allowance before injecting another model
observation. It retains the candidate and uses the existing stop path. An
already-complete result still completes normally. This door releases its
model on completion; it is not a long-lived shared hearth service.

## Roles and tools

### Optional initial reasoning

Add `"initial_reasoning_tokens":4096` to a coding or review request to open
the model's reasoning channel for the first reply of that admission. The
positive integer limits **all generated tokens in that reply**, including
reasoning and the final answer. The maximum is 8192; the caller's context
capacity still applies. When `max_reply_tokens` is also supplied, the smaller
ceiling governs this initial reply, including its reasoning. Omit
both reasoning fields to keep the ordinary entry.

Form separates the final channel at the actual closing token. Only that final
response enters the existing tool, edit, report and verification loop. Later
replies use the ordinary controller. A missing boundary or unfinished initial
generation returns `attention` and releases the model; a partial answer does
not become an action or a successful report.

The result reports `requested_initial_reasoning_tokens`. Actual initial
generation counts, completion, boundary presence and a private evidence path
appear in `form-code-reasoning` metadata. Reasoning text stays outside the
framebuffer and tool actions. Evaluation still excludes its answers from
training. The model, source checks, document constraints and release owner are
unchanged. This optional local path adds no provider or model-server dependency.

To reserve a separate answer stage, also supply `reasoning_answer_tokens`:

```json
{"initial_reasoning_tokens":512,"reasoning_answer_tokens":1024}
```

Both allowances are positive integers at most 8192. The answer reserve requires
the initial allowance. `max_reply_tokens`, when present, caps each stage
separately. Both stages must fit the context; the controller also checks room
for its observation before opening the answer stage. If the model completes
its final response naturally during the initial stage, that response is used.
Otherwise Form adds an explicit runtime observation and opens one final-response
stage with the reserved allowance. It supplies no new task evidence. An
incomplete or refused final stage cannot submit a report or execute an action.

Generation completion and JSON validity are separate. Within the controller's
reserved final stage, a stop predicted inside an unfinished JSON object string
can yield to the highest finite non-stop logit. The model continues in the same
stream using only the remaining answer allowance. The original prefix is
retained; `form-code-json-prefix` and `form-code-json-continuation` identify it
and record each stop selection and token count. The organ observes and rechecks
this care through `json-string-completion` health events. Complete JSON,
other syntax failures, private reasoning and ordinary dialogue retain their
original stop behavior. A non-finite or unavailable head leaves the original
response intact. No missing answer text or JSON closer is supplied by this path.

The ordinary parser, tool permissions, review and caller assertions still
apply. An exhausted final stage cannot execute its partial action. This bounded
completion capability does not establish answer quality; compare the returned
answer and the original task checks.

The result includes `requested_reasoning_answer_tokens` (0 when omitted).
`form-code-reasoning-reserve` metadata reports the effective allowances, both
generation counts, observation token count, answer origin, completion and
private evidence paths. Original single-stage behavior remains available by
omitting this option. The reserve improves access to a completed answer; it
does not establish that extra reasoning improves that answer's quality.

More reasoning has an actual local cost; passing fields still leaves broader
answer quality to be assessed. The earlier native experiments are retained in
[`../receipts/2026-09-16-matched-session-and-turn-budget.md`](../receipts/2026-09-16-matched-session-and-turn-budget.md).
The public request's incomplete and completed executions are recorded in
[`../receipts/2026-09-16-public-native-reasoning.md`](../receipts/2026-09-16-public-native-reasoning.md).

### Reasoning across the task

Use `reasoning_tokens` instead of `initial_reasoning_tokens` to open bounded
reasoning on every generated reply, including tool continuations, failed-check
repair and context renewal:

```json
{"reasoning_tokens":512,"reasoning_answer_tokens":1024}
```

This mode requires the separate answer allowance. Both limits are positive
integers at most 8192; `max_reply_tokens` caps each stage separately. The caller's
reply count remains unchanged. The task retains these allowances in its owned
state, opens the actual thought boundary after feedback and uses the same
generation, final-channel selection and incomplete-output handling as the
initial reserve. Only a complete final response enters tools and checks.

Before each reasoning reply, admission also tokenizes and reserves the current
final-stage controller request, including its crossing control IDs. A completed
action followed by insufficient room selects the existing context renewal on
the same admitted weights. A fresh context that cannot fit the full reservation
retains the candidate and reports capacity. This prevents spending a reasoning
allowance on an answer stage that the current task handoff cannot reach.

The result's `reasoning_tokens` names the continuing allowance. Each
`form-code-reasoning-reserve` event identifies its reply number and actual stage
counts without exposing reasoning text. The existing initial-only option and
ordinary continuation remain available. Reasoning availability is a runtime
capability; improvement in the resulting work needs its own observation.

### Controller roles

| Role | Qwen supplies | Form applies |
|---|---|---|
| Refine | `{"brief":"..."}` | Preserve the original goal alongside the refined brief |
| Plan | `{"plan":"..."}` | Retain approach and verification intent in the same session |
| Split | `{"tasks":["first task","next task"]}` | An ordered work queue; no rented subagents |
| Implement | Tool calls, then `{"task":"done"}` | In-memory edits and advancement to the next task |
| Review | `{"verdict":"accept","reason":"..."}` or `reject` | Rework on rejection; retain acceptance findings and run caller verification |
| Repair | A document tool action, or `{"diagnosis":"observed cause","change":"different approach","next":"implement"}` / `next:"plan"` | Check an applied repair immediately, or retain the diagnosis and selected route |

Every role may inspect documents. Implementation and repair may edit caller-writable
documents; repeated counterexamples require replanning before further edits. Native calls use
`{"tool":"rg","arguments":["-nF","enabled","config.json"],"input":""}`.
Other examples:

```json
{"tool":"read","arguments":["config.json"]}
{"tool":"jq","arguments":[".enabled","config.json"]}
{"tool":"verify","arguments":[]}
{"tool":"edit","arguments":["config.json","\"enabled\":false","\"enabled\":true"]}
{"tool":"write","arguments":["notes.md"],"input":"New document bytes\n"}
```

Coding acceptance requires a nonempty string `reason`. An absent, blank or
non-string reason stays in review without running final verification. The
result's `report` retains a `native-coding-review-v1` JSON string containing the
supplied reason, goal hash and reviewed document identities; checkpoint encoding
preserves it. These findings belong to the submitting reviewer. Their presence
does not establish their adequacy, and passing caller checks establishes only
those assertions. Coding review reads prose requirements as well as source
behavior. Historical completed checkpoints keep their existing evidence; this
requirement applies when a fresh acceptance crosses the review boundary.

Successful document mutations return the accepted document's `id`, `path`,
`bytes` and `resident_sha256`, plus `documents_changed:1` and `before_sha256`
(null for creation). This acknowledges the accepted bytes; caller verification
has its own result. A later whole-document edit uses this new hash. Stale hashes
still fail without changing the document. Full admission and context renewal
include each current document's identity beside its text, so continued work
can use that identity without another read.

The full existing native tool set is reused: `rg`, `jq`, `read`, `cat`, `head`,
`tail`, `wc`, `sort`, `uniq`, `tr`, `cut`, `awk`, `sed`, `edit`, `write`. Their
existing supported subsets and error behavior remain unchanged. An unavailable
tool or invalid edit becomes a tool observation, not a shell fallback. Malformed
model responses receive a correlated native revise action and a format reminder.
Closing the last implementation task also runs the caller's existing source
checks before entering review. A failed check enters repair with the current
diagnostic and checked document hashes. A passing check supplies that evidence
to review; final acceptance runs the caller checks again. This consumes one
model reply and one actual check, with no invented tool call. Intermediate tasks
continue without this whole-candidate check. Read-only report work keeps its
report-aware submission boundary.

A failed coding source check stays open through diagnosis, planning, a return
to implementation, context renewal and checkpoint resume. Each accepted changed
candidate in that repair runs the original source checks immediately, including
when the model chose `next:"implement"`. A passing source check closes this
condition; review and final verification still follow. Failed or unchanged tools
retain the condition without running another source check. Before any source
failure, ordinary implementation edits retain their existing behavior.

The coding runtime saves a completed action before loading its feedback into
the model context. Interrupted feedback loading can therefore resume from that
action. Context references become admitted only after loading completes; the
next checkpoint retains those references as well. A refused checkpoint write
stops advancement with the candidate retained in the returned state.

When completed work fills the model context, the coding owner uses the existing
independent-stream renewal cell. It keeps the admitted weights, prefills a fresh
bootstrap from the current candidate and retained task, and checks retirement
of the old stream. The returned session becomes the sole owner. Role and failure
references reset so the new stream receives their full evidence. Source documents,
permissions, caller checks and the absolute reply limit remain unchanged.

Budget feedback includes `completion_replies_minimum`, a lower bound from the
current phase and pending tasks. Unfinished edits, reads, failures and rejected
reviews can require more. Coding guidance names the actual completion path:
each implemented task needs `task=done`, then a separate review verdict. Closing
the last task already runs the caller source checks, and acceptance checks again;
an extra `verify` call is available for diagnosis. The metadata reserves no
extra replies, changes no phase and grants no acceptance.

Renewal requires a completed reply in the departing context and remaining reply
allowance. A bootstrap that leaves no usable space therefore stops instead of
repeating admission. When `max_reply_tokens` is set, the controller reserves
that many usable positions before beginning the next ordinary reply. A short
remaining context selects renewal before any reply token is generated; the
configured allowance stays unchanged. A fresh bootstrap that cannot leave the
requested room stops without repeating admission. An omitted token ceiling
retains the ordinary context-bound generation behavior.

Renewal checks the encoded bootstrap against the caller's next-reply capacity
before allocating or prefilling a replacement stream. Coding uses its existing
reply reservation, including the final-stage handoff for reasoning requests.
A `prompt-caller-capacity-refused` result preserves the original owner and its
counters for release. The model renewal API exposes this check through
`fcmr-replace-with-profile-checked`; existing unchecked callers retain their
ordinary prompt-window check. This avoids a full prefill that cannot admit even
one complete reply; it does not shorten or weaken the caller's requested reply.

Partial generations, empty encoded feedback and failed
prefills retain their failure paths. Capacity is checked before feedback touches
the stream; only that nonmutating failure can retain its earlier live owner.
An unsuccessful renewal ends the call, releases the returned owner and retains
any incomplete stream release as a failed release. Initial reasoning and answer
reserve routes use this same continuation after their initial stage.

`form-code-context` records actual renewal, ownership replacement, reason,
capacity trigger, remaining positions, reply allowance, context prompt size
and accumulated generated/injected IDs. Ordinary progress
frames retain their current-context counters. Final generated IDs, injected IDs
and completed-observation counts sum all contexts in the coding call. Those
counters do not include all prefill work or establish response quality.

The coding-loop `verify` tool invokes the caller's existing source checks in
any role. It accepts an empty argument array and no input; the model cannot
provide a program, command, path or replacement checker. Its actual outcome
enters the same tool observation and repair loop. Report checks run on submission.

Repair is an active role, not a stopped job. A native tool error, review finding,
or failed check enters repair. Read-only inspection stays available; its output
does not overwrite the separately retained failure evidence. Qwen can apply a
guarded `edit`, create with `write`, or call `repair-bml` directly. When a tool
changes documents successfully, the controller records the actual action and
original failure, then runs the caller's source checks in the same transition.
This record explicitly supplies no model diagnosis. A failed check returns to
repair; a pass returns to implementation and still requires task completion,
review and final verification. Failed or unchanged edits do not advance.
Qwen may instead name what failed, what it will change, and its next route.
Replanning preserves original documents,
constraints, candidates, completed-task counts and repair memory. If the exact
same counterexample survives a repair, the next route must be `plan` before
further edits. An `rg` no-match result is an observation, not a tool malfunction.
Malformed JSON and unavailable tools receive correction in the current role.

The result returns `repair_attempts`, `check_runs` and `repair_notes` containing
each supplied model diagnosis or explicitly attributed controller observation,
the change, selected route and retained failure evidence.
These are model reflections, **not proven causal explanations or weight
training**. The JSON door now persists native binary checkpoints beneath
`.hearth/code-memory/` after complete model/tool transitions. A completed repair
also becomes a lesson. Retrieval requires the exact original goal, documents,
writable paths and checks. For coding, the prior completed candidate must pass
the current checker again; only then does the new job return that candidate
as complete without model admission. The result names
`source=verified-native-code-reuse`, the prior checkpoint and the actual current
check in its reason. New model replies, tool calls and generation counters stay
zero; the current check is counted. The sealed lesson keeps the original
attempt history. This is explicit reuse of verified work, not fresh generation
or a weight update.

A failed or malformed current check leaves the new job's original source and
work phase intact and retains the refused check result in its observation.
Changed goals, original documents, writable paths or check
contracts cannot select that lesson. Read-only review recall remains guidance
for a fresh report; it does not return a prior report as new work. Evaluation
continues to exclude recall. Other tasks load none of this private context.

Budget/context exhaustion returns `checkpoint_id`. To continue, send the same
original request with `"resume":"<checkpoint_id>"`; `turns` is the additional
reply budget and `context` sizes the new local admission. Pending tasks, brief,
plan, candidate documents, tool outcomes and failure evidence survive. Complete
checkpoints are rechecked without reopening Qwen. Incomplete generated text is
discarded, not executed. This is explicit resumability, not automatic KV
compaction or a persistent model resident across jobs.

The disk format is FORMBIN2 native nodes, decoded directly. Writes go to a
process-specific temporary file, are read back, then atomically renamed. A
digest detects corruption; it is not authentication against someone who can
rewrite local files. Checkpoint contract changes are refused. Source files
remain caller-owned: persistence here stores private task state and lessons,
not a repository edit. Nothing is sent to an external service. Each completed
JSON request also contributes to the private native session learner: verified
documents teach the caller contract, and unsuccessful requests teach observed
status only. `session_learning_example` and `session_learning_worker` identify
that work; `session status` distinguishes queued, failed, learned and promoted.
`evaluation: 1` starts a fresh run **before continuity lookup**: no recalled
repair notes, checkpoint resume/write, session LoRA proposal or weight training.
Combining it with a nonempty `resume` is refused. Assessment experience may still
be retained privately, excluded from gradients. This does not make a previously
seen task an unseen test or remove knowledge already present in the base model.
Review reports, including ordinary non-evaluation reviews, are retained only as
assessment experience, never as verified implementation targets. Ordinary review
checkpoints retain their report and mode; resume reruns the report-aware checker.

Three identical native tool/argument/result observations with unchanged
documents select repair and require replanning. A single `rg` miss is still a
normal negative result. These bounded signals survive checkpoint restoration;
they never become successful checks. Glass receives counters only, including
`lessons-recalled`. Coding token progress publishes every four generated IDs
instead of 32; actual time between updates still depends on local inference.

For a public-data, separate-process local witness:

```text
./fkwu observe/form-cli-code-memory-witness.fk
```

Its three stdin lines are an isolated memory directory, `seed` (write a known
failing checkpoint), and `qwen38-q8`. Run it again with the returned checkpoint
ID on line two to resume with local Qwen; then run with an empty second line to
start a fresh job and observe `lessons-recalled`. The seeded defect is fixture
data, never attributed to Qwen. `form-cli-code-memory-band.fk` and
`form-cli-code-progress-band.fk` under `form/form-stdlib/tests/` exercise binary
round trips, contract mismatch, corruption, verified recall, and replan routing
without admitting a model or publishing live telemetry.

## What a successful result proves

Coding completion requires an actual document change and all caller-owned checks
passing. Read-only review completion instead requires unchanged documents and a
nonempty report passing the caller's source and report checks.
The Qwen path also completes its task/review loop; the earlier session
LoRA proposal is checked directly. Checks require exit zero, empty diagnostic
text, and exact stdout. The JSON door supports **read-only native tool assertions**
and executable arithmetic checks through the existing native definition grammar.
A `jq` check establishes a configuration property, not that an
arbitrary application compiles or runs. Tests and acceptance criteria do not
come from the model. Failed verification returns its actual result to Qwen's
repair role, followed by a changed implementation or plan, another review, and
verification. A model's claim that it repaired something never substitutes for
rerunning the caller's checks.

For an executable source check, supply a document such as
`module calc { fn bump(x) = add(x,1); }` and this immutable check:

```json
{"kind":"definition","path":"calc.form","function":"bump","cases":[[0,1],[1,3],[21,43],[-2,-3]]}
```

Qwen must repair the function to produce those outputs. Form decodes the whole
module with its existing definition grammar, verifies the allowed expression
tree, lowers the function into a resident NodeID and executes each input. This
lane accepts one unary function using pure integer arithmetic/comparisons,
without effects, recursion, division or unknown bindings. It is not a general
repository compiler. Invalid syntax, changed function identity, and failed cases
return concrete observations to Qwen.

For richer executable behavioral tests, an embedding Form cell calls:

```text
fcac-run(model, goal, documents, writable,
         list(native-checker, immutable-contract), context, turns)
```

The callback receives `(immutable-contract, candidate-documents)` and returns
`list(passed, actual-observation)`. It runs in-process. The model cannot replace
that callback or its contract. A caller claiming native-only behavior must keep
its callback native too. No generic shell test runner is implicitly provided.

An embedding cell can run a fresh read-only review with:

```text
fcac-review(model, goal, documents,
            list(report-checker, report-contract, source-checker, source-contract),
            context, turns)
```

`report-checker` receives `(report-contract, list(original-documents, report))`
and returns `list(0-or-1, actual-observation-string)`. The optional last two
checker elements bind `verify` to a native source-only callback, which receives
`(source-contract, original-documents)`. Without them, `verify` reports its
absence; submission still runs the report checker. This lower-level door does
not recall lessons, write checkpoints or offer training examples. The JSON door
provides ordinary continuity unless `evaluation` is 1. Neither door grants an
arbitrary filesystem or compiler surface: richer native tests belong to the
caller-bound callback.

### Execute a proposed function during review

`bml/form-cli-review-execution.bml` lets a caller's report checker evaluate an
expression-bodied BML function against explicit native bindings. Use it when
the report proposes code and a field assertion cannot establish its behavior:

```text
bindings = list(list("combine", 2, native-combine-callback), ...)
prepared = fcre-prepare(proposed-source, expected-function-name,
                        expected-parameter-count, bindings)
result = fcre-run(prepared, arguments, bindings)
```

Each binding callback receives one argument list and returns
`list(ok, value, observation-string)`. `fcre-ok(value)` and
`fcre-no(observation)` construct those results. Successful `nothing()` is
distinct from a refused evaluation. The caller owns each callback's behavior,
argument validation, purity and execution cost. Preparation retains the binding
contract; execution refuses changed bindings.

The existing Form BML grammar must consume the entire source as one expression
definition with the expected name and arity. Every branch is checked before
execution: names must be parameters or explicit bindings; calls must match
their arities. Literals, parameter references, bound calls and lazy
`if … then … else …` expressions are supported. Extra definitions, unknown
names, recursion and dynamic callees are refused. Source is limited to 65,536
bytes and admitted expression depth to 64. This is an expression lane, not a
general BML module compiler or an isolation boundary for effectful callbacks.

Admission refusals retain `proposal-outside-caller-bound-expression-lane` and
append the first concrete cause, such as
`binding-arity-mismatch name=and expected=2 actual=5`, an unbound call or
parameter, or excessive depth. This diagnostic walks the rejected syntax;
it never executes proposed callbacks. The same validator still decides which
expressions may run. A native repair can therefore act on the actual mismatch
while the caller keeps the original behavior checks.

Compare the returned values with caller-owned expected behaviors inside the
report callback, retaining the original source and report assertions. Pass that
combined callback to `fcac-review` or `fcac-admit`. It then runs on **every report
submission, including repair submissions**. Supplying a failure once at repair
entry and subsequently checking only report fields leaves the failure outside
the acceptance contract. An explanation of a counterexample is not its repair.

The evaluator makes no filesystem, process or model calls. The normal review
controller carries failed observations into its existing repair and diagnostic
flow. `tests/form-cli-review-execution-band.fk` checks actual return values,
lazy evaluation, successful absence, callback errors and whole-source refusal.

`bml/form-cli-review-trace.bml` adds `fcre-trace-run(prepared, arguments, bindings)`.
It returns `[ordinary-result, invoked-binding-names]`. The trace records actual
evaluation order: arguments before their enclosing call, only the selected
branch, and no later calls after an argument fails. Each callback runs once.
Changed bindings and rejected source produce no calls. The ordinary evaluator
remains available without trace allocation.

These are expression call sites. Supplied callback bodies remain opaque; the
trace does not infer their internal calls or effects from their names. A caller
can compare original, rejected and verified candidates using the same inputs
and bindings, then return those observed traces alongside the paired checks.
Use the source to explain internal effects, and preserve the trace's scope.

`fcre-trace-detail(prepared, arguments, bindings)` returns
`[ordinary-result, events]`, where each event is
`[binding-name, evaluated-arguments, execution-result]`. This retains the inputs
and result at each actual call boundary, including a failed call. Calls skipped
after failure have no event. `fcre-trace-run` projects names from the same
execution; it does not invoke callbacks again. The trace itself emits no values
to the framebuffer. The caller owns retained data and any public projection.

When supplying native observations as initial context, keep the original source
documents and place the derived evidence in its own document. Name the controlled
inputs, observed values and scope. A final counter alone does not attribute its
change to a particular call; use the corresponding arguments and results for
that attribution. An accurate observation still requires checking the generated
answer against the source.

### Search a small native repair before another model call

`bml/form-cli-review-search.bml` searches one explicit repair family: replace
one existing call with a call that forwards the function's original parameters
to a compatible caller-owned binding. Compatibility comes from caller-supplied
result roles and matching parameter count. Roles describe the caller's domain;
they are not inferred types or a guarantee that a callback accepts every value.

```text
roles = list(list("finish", "continuation-result"),
             list("continue", "continuation-result"))
result = fcrs-search(source, name, arity, bindings, roles,
                     behavioral-checker, immutable-contract, check-budget)
```

The checker receives `(immutable-contract, prepared-expression)` and returns
`list(passed, actual-observation)`. It can call `fcre-run` for each case. The
search first checks the unchanged function, then reparses and checks each
generated candidate. It never invents expected outputs or changes the contract.
Candidates use existing parameters and bindings; no reference implementation
is required. Callers must supply safe, appropriately validated callbacks.

The result is `[status, source, checker-runs, observation]`. Status is
`unchanged`, `repaired`, `exhausted` or `refused`. Failed or exhausted search
returns the original source. The budget counts actual checker invocations,
including the original. No model, filesystem or process call is made by the
search itself. Attribute its output as native structural repair, separately
from model generation. A passing result establishes the supplied behaviors;
inspect and independently execute the generated source before a broader claim.

This is a targeted search strategy, not unrestricted program synthesis.
`tests/form-cli-review-search-band.fk` exercises a separate two-argument task,
additional input values, budget exhaustion, malformed checks, missing roles,
unchanged source and escaped/unicode source rendering.

### Attach native repair to the review loop

An embedding caller may append a native repair callback and its immutable
contract to the four-entry review checker:

```text
checker = list(report-checker, report-contract, source-checker, source-contract,
               native-repair, repair-contract)
```

After a valid failed report check, the controller retains that failure and calls
`native-repair(repair-contract, list(documents, report, actual-failure))` once.
The callback returns `list(available, proposed-report, method, observation)`:
`available` is 0 or 1; the other entries are strings, with a nonempty method.
It may use `fcrs-search` or another caller-owned native capability. The caller
must update any explanation or related claims made stale by a changed proposal.

When a verified proposal still needs the resident's full explanation, return
`list(available, proposed-report, method, observation, "review")`. After the
unchanged checks pass, this leaves the task in review with event status
`accepted-for-review`. The same resident receives the exact rejected and verified
reports with their respective check results. It must complete the original
caller report; that submission runs the same checks again. A passing native
candidate alone does not finish this continuation. Existing turn and context
budgets still apply, and no additional model admission is requested by this hook.
Other fifth-element values are malformed.

A changed, nonempty report runs the original complete checker again. Passing
is required for completion. A failed native candidate returns to ordinary
repair without recursively invoking the callback; declining or returning the
same report preserves the existing failure. Malformed checker/proposal results
cannot approve a report. The hook does not run for passing reports or coding
mode. Model JSON cannot install or replace it; ordinary JSON requests retain
their existing behavior.

Results expose `report_source` and `native_repairs`. Each native event records
method, outcome, caller observation and controller check count. A later submitted
report resets its own attribution while preserving the native attempt history.
Each new event also pairs the exact prior report with its original failed check
under `before`, and a changed native report with its own check under `after`.
A declined or unchanged proposal has no `after` check. Older events expose null
evidence instead of reconstructing a history they did not retain. Malformed
checker output is explicitly marked, with no valid check asserted. These records
are returned to the caller and preserved in continuity. The four-element callback
does not add them to model feedback. The explicit `"review"` continuation sends
the newest pair under `native_review` once per resident context. Only a completed
observation marks it delivered; later feedback may use `native_review_reference`.
A fresh or resumed context receives the full pair again. Historical failed checks
stay attached to the rejected report instead of appearing as a current failure.
When the live controller matches the exact rejected report to the resident's
just-generated submission, `before.report_reference=preceding-submission` replaces
that duplicate report text in feedback. Its check still travels in full. Without
that match the complete report travels. Bootstrap always includes the full pair,
and renewed contexts clear the submission attestation. Returned provenance keeps
the exact report in every case.
Context renewal and binary continuity preserve those records. Nested search
checks remain the native callback's observations, distinct from the controller's
whole-checker count. Callback execution and its side effects are caller-owned.
Native work remains inside the submitted turn and does not consume another
model-turn allowance; its verification runs and provenance are counted separately.

`tests/form-cli-native-review-repair-band.fk` exercises successful and failed
repair, declines, unchanged proposals, malformed results, source preservation,
attribution, result projection, continuation, observation admission and binary
continuity without loading a model.

Review guidance follows the caller's requested report fields and value types.
Decision-first guidance applies when the task asks for a decision. Originals,
rejected candidates, proposed changes and verification results keep their own
attribution when the model explains the evidence. These instructions guide
generation; inspect the returned answer to establish whether it followed them.
The shared guidance also keeps a format check's scope separate from behavioral
verification, and a total attempt count separate from the final observed result.

Review currently shares the same Qwen and context: **not independent-model
validation**. Qwen weights remain unchanged; the shared native Llama adapter
learns asynchronously from observed outcomes. The loop does not assert rented-model
parity, broad coding quality improvement, or `voice-home=1` from a passing example.

## Observe

Each movement publishes a fresh `qwen.coding.<pid>` Glass snapshot with the
current role, actual native tool-call count, cumulative generated-ID count,
repair attempts and caller-check runs. Check runs count whole callback calls,
not individual assertions. A failed check remains counted after a later pass.
Generation refreshes these counts every four IDs, preserving the pending ID
exactly once and decoding the complete reply only after generation ends.
Terminal metadata adds injected IDs, position and elapsed milliseconds.
Repair observations initially carry complete failure evidence. Notes and
pending tasks exactly equal to that evidence point to the same message's
`failure_evidence` field. After completed delivery in the current model context,
that field may contain `{reference: <failure_evidence_id>, scope:
"current-model-context"}`. The full message supplies the identity; the reference
is used only when shorter. Changed failure bytes and fresh/resumed bootstrap
contexts receive the full evidence. Failed or partial feedback loading cannot
admit a new identity. Distinct text and retained controller state stay intact.
Prompt, response and source content stay out of the diagnostic framebuffer.
The JSON result carries candidate document content and, for review, the report
back to its caller. Rechecks on recall/resume count as actual checker calls.

Regression doors (preflight each FK band first):

```text
form/form-stdlib/tests/form-cli-code-policy-band.fk   -> 65535
form/form-stdlib/tests/form-cli-code-request-band.fk  -> 255
form/form-stdlib/tests/form-cli-code-memory-band.fk -> 65535
form/form-stdlib/tests/native-session-code-band.fk -> 511
form/form-stdlib/tests/form-cli-code-session-band.fk  -> 31
form/form-stdlib/tests/form-cli-code-telemetry-band.fk -> 7
form/form-stdlib/tests/form-cli-code-definition-band.fk -> 127
form/form-stdlib/tests/form-cli-code-dispatch-band.fk -> 7
form/form-stdlib/tests/form-cli-code-repair-band.fk -> 262143
form/form-stdlib/tests/form-cli-code-documents-band.fk -> 255
form/form-stdlib/tests/form-cli-agent-tool-wire-band.fk -> 131071
form/form-stdlib/tests/form-cli-agent-tools-examples-band.fk -> 32767
form/form-stdlib/tests/qwen38-sliced-head-band.fk -> 7
```

The existing policy, request, memory and session-code bands also contain
fail-fast review guards: immutable sources, report-aware checking, caller-bound
verification, report checkpoint round trips, fresh evaluation before recall,
and exclusion of review reports from supervised implementation examples.

For an explicit real-model recovery exercise, run
`form-run ./fkwu observe/form-cli-code-retry-witness.fk` and send `qwen38-q8`
on stdin. The fixture deliberately supplies a wrong candidate for `x*x+x+1`.
The native checker fails it before Qwen is admitted; Qwen must then diagnose,
repair, review and pass six arithmetic cases plus preservation of a second
document. The initial wrong edit and six initial policy turns are **fixture
inputs, not model-generated behavior**. The witness reports newly generated
model replies separately. It admits real weights, publishes actual Glass
metadata, and releases its model; it is not a simulated model test.

`observe/form-cli-code-first-reply-witness.fk` accepts the same model-name line
and checks a bounded first reply on public two-document data. It requires a
completed refinement reply advancing to `plan` and successful release. Its
128-ID generation bound is a probe bound, not a production reply limit.
The 2026-09-09 probe found that sliced prefill drained its concurrent batch,
then submitted the gather/head with no concurrent batch armed. Re-arming
before the head changed the identical prompt's first response from unrelated
54-ID JavaScript to a valid 70-ID refinement. Generated text was never executed
as host code. The structural band guards this batch boundary; the live probe
checks actual first-role behavior.

The native tokenizer parity witness is
`observe/form-cli-code-tokenizer-witness.fk`. On this host, its 34 observation
IDs were identical across the scanner and indexed paths: 19,328 ms versus
1,124 ms. This is one encoding measurement, not a whole-task speedup claim.

Live local Qwen observations on 2026-09-09:

- Configuration edit: 11 model replies, 3 native tool calls, 3 completed tasks,
  481 generated IDs, all three native `jq` checks passed, release verified.
- Executable Form edit: changed `add(x,1)` to `add(mul(x,2),1)` in a resident
  definition module. Four behavioral cases passed, including zero and a negative
  input. 11 model replies, 3 native tool calls, 3 tasks, 579 generated IDs,
  887 newly injected IDs, 10 same-session observations, release verified.

Both runs recovered from a first reply that did not satisfy the refinement
contract. These are observed small tasks, not a held-out coding benchmark.

On 2026-09-11 the read-only JSON example above completed with real local Qwen:
8 replies, 2 native calls (read and caller-bound verification), 2 check stages,
249 generated IDs, 643 injected IDs, no repairs or recalled lessons, and verified
model release. The source remained unchanged and all three returned report
fields passed. Total admission-to-release time was 287,493 ms under other local
workload. Its assessment record is excluded from training. This is a small
functionality witness, not an unseen review benchmark or latency guarantee.

## What we learned from open-source agents

Qwen-Agent's [function-call loop](https://github.com/QwenLM/Qwen-Agent/blob/main/qwen_agent/agents/fncall_agent.py)
feeds executed tool results back into the model's conversation and continues
when tools were requested. We use that interaction pattern with Form functions,
without importing its Python runtime or provider transports.

Qwen Code's [engineering prompt](https://github.com/QwenLM/qwen-code/blob/main/packages/core/src/core/prompts.ts)
emphasizes inspecting existing project conventions and reporting actual
verification outcomes. Its [role-specific agents](https://github.com/QwenLM/qwen-code/blob/main/docs/users/features/sub-agents.md)
demonstrate restricted tool sets and specialized contexts. This implementation
uses restricted roles but deliberately reports its shared-context review seam.
These are design references, not evidence that this local model has passed a
coding benchmark. No framework source was copied into Form.
