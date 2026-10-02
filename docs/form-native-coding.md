# Qwen coding inside Form

Native substrate lookup, retrieval, calculation and executable recipes lead
the task flow. An observed native answer takes precedence over model prose;
local models contribute where the substrate does not yet carry the task.
Form's choices, care, checkpoints, original checks and release own both paths.

`code` edits caller-supplied source values through local Qwen and Form tools. Token IDs, KV state and tool observations remain in the native process; filesystem loading/publication belongs to the caller and telemetry uses shared memory. The controller offers no model-selected shell, network or provider call. Metal is admitted dynamically. Source bootstrap still lowers cold BML through a native compiler process; warm images reuse that work.

The north star is compact, behavior-preserving native refactoring with retained context, reusable compiled abstractions and locally verified learning. `form-lift.bml` supplies the current engineering guidance at BML admission: lexical lambdas, stdlib reuse, named domain values, explicit ownership and live observations. Classes group APIs; templates describe parameters and members, without implying instance accessors, interface checking or specialization. See [BML admission](native-bml-admission.md#local-lift).

An evaluated session LoRA may offer a proposal before Qwen unless the request names a model, selects evaluation or requests review. The original checks decide whether it suffices. The optional learner uses native workers. Explicit `model` and `weight_training: 0` keep ongoing quality assessment on the selected local model with checkpoints and no answer gradients. [Session learning](native-session-learning.md), [healing](form-cli-healing.md), [provider repair](form-response-resource.md) and [provider synthesis](form-response-synthesis.md) describe their separate interfaces; provider paths are not implicit `code` behavior.

## Native surface lifting

Mixed modules use the same native lifter: raw Form definitions become BML while
embedded BMF and BML sections remain authored source. Existing enclosing scopes
remain in place. Admission compares ordered loader dependencies and complete
lowered bodies, then verifies the collection and operator refinements. This
deterministic transformation requires no model generation.

`form/form-stdlib/bml/form-source-lift.bml` provides `form-source-lift(source, owner)` for Form and executable BML. It returns `[equivalent, candidate, definitionCount, lowered]`. `form-source-lift-batch([[source, owner], ...])` returns the same results in order and owns one discovery map for the batch. A dependency's exact source and home bindings must match before its local facts can be reused; imports, contract order and observations are resolved for each owner. Every other discovery in the process shares one map, so a lowering child reads each file of its closures once; `fsc-bml-reading-counts` answers how many files were read and how many readings were reused. Form lifting preserves grouping and comparison contracts while emitting BML definitions, blocks, lazy conditionals, list literals, ranges and infix operators. Repeated exact-value comparisons of one atomic subject become `match` arms. Operator spelling and precedence come from the grammar's shared table. Its proof compares the lowered trees, decoding string escapes, the runtime's boolean spellings and closed literal string construction. This includes the compiler's semicolon encoding; calls with variables or effects retain their structure. Only the single module wrapper is removed, so nested blocks retain scope. Added local bindings or shadowed literal/constructor names produce `equivalent = 0`; that candidate is not ready for publication. Malformed source follows the compiler's diagnostic path.

Existing BML travels through a BMF cursor over balanced argument spans. Native
admission parses the completed candidate. Calls become `[]`, `[0]`, `[1:]`,
constant nonnegative indexes, ranges, `len`, `.str()` and infix operators while
classes, functions, comments and quoted bytes retain their places. Ordered
prelude directives become explicit module imports. Computed legacy `nth` calls
remain explicit: negative indexing has a different meaning on the new surface.

`form-bml-surface-equivalent?(source, candidate)` compares exact compiler recipes
before type resolution, including declared comparison metadata and lexical
bindings, together with the surrounding source. Identical recipes receive the
same imported context, so a surface migration can prove its change without
re-reading an unchanged dependency graph. This narrow proof does not produce a
compiled image; ordinary source admission still resolves imports and validates
execution.

The lifter turns unshadowed `empty()` into `[]` and `not(value)` into `!value`,
retaining local constructors and bindings. Identical source bytes discharge
the identity comparison directly; changed candidates still carry the proof.
Operator providers retain the primitive operations they implement, so lifting
an operator cannot introduce a call back through its own implementation.
The lifter uses `form-bml-semantic-equivalent?` for the extended surface. It
additionally normalizes the admitted string-length, standard value-rendering,
list-first/remainder and explicitly contracted operator forms. This comparison
preserves literals and bindings, rejects a locally shadowed renderer, and checks
ordered dependencies. These are migrations of the existing valid operand domains;
they do not assert identical behavior for invalid primitive inputs. Native value
execution and both BML readers witness the broader sequence/operator surface.

Imports remain loader directives outside the executable section, and admission also compares ordered dependencies. This deterministic step uses no model tokens and writes no files. Keep the original source, check it is still current before publishing, and observe affected callers. Keep the compiler's bootstrap closure in its bootable source form. Structural lifting does not establish that every algorithm or abstraction is optimal.

## Call without knowing Form syntax

The resident `lift-bml` tool takes `[path]` and empty input. It applies the native
BML surface and import lift to a writable resident document, proving the recipe
structure before adopting it. No source text is generated by a model and the
tool makes no filesystem, process or network crossing. Parser refusal leaves
the original intact; operator providers and local bindings retain their meaning.
Its observation reports source hashes, `changed`, `equivalent` and `checks_run:0`;
the caller's execution checks still decide completion. An unchanged result means
no supported lift remains, so another identical call adds no work.

Use `code <JSON>` or `code @request.json` in the source-backed form-cli. The standalone door is `form-run ./fkwu observe/form-cli-code-run.fk`, with the JSON or `@request.json` on stdin. Use the file form for a large packet: the host line reader admits at most 8,191 bytes per line. Documents are resident values, not filesystem permissions. Results return candidate values; the caller checks stale source and publishes them.

```json
{"goal":"Enable native coding; preserve provider and remote.","documents":[{"id":"config","path":"config.json","text":"{\"enabled\":false,\"provider\":\"native-qwen\",\"remote\":false}\n"}],"writable":["config.json"],"checks":[{"tool":"jq","arguments":[".enabled","config.json"],"stdout":"true\n"},{"tool":"jq","arguments":["-r",".provider","config.json"],"stdout":"native-qwen\n"},{"tool":"jq","arguments":[".remote","config.json"],"stdout":"false\n"}],"model":"qwen38-q8","code_entry":"direct","weight_training":0,"context":8192,"turns":16}
```

### Request fields

`code help` reads the contract from `bml/form-cli-code-request.bml`. Mode-specific fields retain their validation.

| Field | Contract |
|---|---|
| `goal` | Nonempty original task, retained beside refinements |
| `documents` | `[{id,path,text}]`; supply callers, helpers and ownership contracts |
| `writable` | Paths implementation/repair may change; `[]` for review |
| `checks` | Nonempty caller-owned source assertions, described below |
| `mode` | `code` (default) or `review` |
| `report_checks` | Required, nonempty in review; read-only assertions on the report |
| `model` | Registry name; default `qwen38-q8`; explicit selection skips the LoRA proposal |
| `context` | Positions per admission; default 8192 |
| `turns` | Reply allowance; default 64; additional allowance on resume |
| `max_reply_tokens` | Optional positive integer ceiling per reply |
| `code_entry`, `review_entry` | Respective mode only: `staged` (default) or `direct` |
| `document_context` | `full` (default) or `catalog` |
| `source_queries` | Catalog-only `[{tool,arguments}]` read-only queries; no supplied input/output |
| `evaluation` | Integer 0/1; 1 excludes recall, resume, checkpoint writes, LoRA proposals and training |
| `weight_training` | Integer 0/1, default 1; 0 excludes answer gradients while preserving ordinary continuity |
| `resume` | Returned checkpoint ID with the same original request |
| `feedback` | Coding resume only: exactly `{id,text}`, both nonempty |
| `native_rehearsal_checks` | Coding-only integer 0–64, default 0 |
| `initial_reasoning_tokens` | Positive integer ≤8192; first reply of admission |
| `reasoning_tokens` | Positive integer ≤8192; every reply; requires answer reserve and excludes initial-only option |
| `reasoning_answer_tokens` | Positive integer ≤8192; requires one of the reasoning options |

Tool checks are `{tool,arguments,input?,stdout,exit?}`. They require exact stdout, empty diagnostics and the requested exit (default 0, otherwise a canonical nonnegative integer). For example `{"tool":"rg","arguments":["-F","obsolete claim","answer.md"],"stdout":"","exit":1}` expects a normal no-match. Missing source remains a failure. Failed assertions retain actual output/status and checked source identities; source text stays in the resident catalog.

`bml-syntax` with `arguments:[path]` parses the resident document's executable
BML sections through the native compiler, preserving documents without publishing
candidate files or invoking a process. Success returns `bml-syntax: admitted\n`. A refused grammar
publishes its cause through the live framebuffer and returns the current attempt's
compiler diagnostic to the tool caller. This checks syntax; imported bindings
and behavior still need the original execution checks.

`bml-api` with `arguments:[symbol names]` and empty input returns exact current
declaration headers, source paths, lines and hashes from resident documents and
the core, line-grammar and string-join libraries. It supplies names and argument
order without asking a model. Empty declarations state this lookup's coverage;
they do not establish universal absence. The three local library reads are
reported as crossings. Documents stay unchanged; no model or process is opened.
Imports and behavior still need execution evidence. Tool availability and coding
permissions derive from the same native catalog.
Declaration scans and hashes are reused only for byte-identical source in the
current process. Changed source replaces its reading; sources outside the current
resident catalog and floor libraries leave the memo. Each lookup publishes
its elapsed time, declaration count and file crossings into the live framebuffer.

Word checks are exactly `{kind:"word-range",path,minimum,maximum}` for a document, or `field` instead of `path` for a top-level report string. Inclusive nonnegative bounds use the shared ASCII-whitespace counter, including headings. Missing text differs from empty text. Failure supplies observed count and repair direction; count agreement does not establish content quality. Report checks take the report on stdin, have no document access and accept no `input` override. The additional `{kind:"provider-usage-sequences"}` report assertion takes no other field; `form/form-stdlib/bml/form-cli-code-request.bml` (`fcaq-replay-check`) owns its meaning.

### Direct implementation entry

`code_entry: "direct"` begins one pending task referring to the full original goal. It avoids generated refinement, planning and splitting, while retaining review, verification and the requirement for an actual change. Resume retains the saved phase. `review_entry: "direct"` selects the corresponding read-only entry.

### Resident instruction context

Every admission receives the shared Form meanings in `bml/form-cli-qwen-meaning.bml`. BML coding also receives `form-lift.bml` guidance and the available repair instructions. A later role instruction may reference exact text already delivered in that resident context; incomplete observations never mark it delivered. Renewal/resume supplies the current instructions again. Delivery establishes context availability, not that the generated work followed it.

Coding retains the latest native source verification with its exact document
identities. Admission reports whether those identities still match the current
candidate. A source change leaves the older reading attributed to its own
version. Caller feedback remains earlier evidence: a repaired condition must be
checked against current source before it is reported as still present. Passing
assertions and a review finding retain their separate scopes.
Resuming completed work reruns the caller's checks and retains that fresh
reading, including the exact cause when execution needs repair.
Its checkpoint and exact lesson share one payload digest; each durable envelope
keeps its own kind and verified write.

### Resident context and source queries

Full admission includes original document text and current identities. A successful unchanged `read` may use `stdout_reference`; a changed read may use one `stdout_patch` against the admission snapshot after exact reconstruction and size comparison. Patches never depend on earlier patches. `cat` returns the current text, bounded beyond the observation cap (below). Checks always consume actual document bytes.

Catalog admission supplies IDs, paths, sizes and SHA-256 identities while native tools retain all text. Optional `source_queries` execute actual read-only queries before generation, for example `{"tool":"sed","arguments":["-n","10,20p","notes.md"]}`. Results retain exit and diagnostics. Excerpts do not establish full visibility; catalog reads return source text up to the observation cap and a bounded note beyond it. Resume recomputes selected queries; omission clears prior selections. Supply enough source for the task and use the native tools for a specific missing fact.

### Read-only review

Use `mode: "review"`, empty `writable`, source checks and report checks. Sources remain byte-identical in every role. Submit the requested report JSON directly, or `{"verdict":"accept","report":...}` with a string, object or array report. Here acceptance submits evidence to checks; negative source findings belong in the report. `reject` with a reason continues inspection. After a failed assertion, `{"report":...}` immediately rechecks a complete corrected report. Feedback names actual results and result types without disclosing expected assertion values.

The JSON door can normalize exact yes/no/true/false strings when a caller's root-field assertion expects a boolean. It changes representation only, rechecks the full contract and attributes native repair separately. Missing/ambiguous values, numbers, duplicate keys and unsupported query shapes remain unchanged. Review uses the same model and context; it is not independent validation.

### Edit a retained response

Supply an existing answer as a writable document with `mode: "code"`, direct entry, the original enquiry, immutable sources and checks. Use guarded `edit`; `write` creates an absent document. Read the changed answer after verification. Keep assessed answers out of gradients with `weight_training: 0`, or use `evaluation: 1` for fresh evaluation without continuity.

### Amend a retained report

Repair observations carry `report_amend.base`, the exact retained report's SHA-256. Submit either `replacements:[{path:["answer"],value:...}]` or `text_edits:[{path:["answer"],old:"unique current text",new:"replacement"}]` inside `report_amend`, with that base. Paths address existing object keys and zero-based array indexes. There are 1–32 nonoverlapping edits, each path 1–32 segments. Text edits select strings; `old` is nonempty and occurs exactly once, including overlaps. Empty `new` deletes text. Missing paths, stale bases, duplicate keys, overlap and unchanged values preserve the retained report. The operation replaces values rather than inserting/removing array entries.

Every complete candidate reruns original checks. `model_amendments` retains before/message/candidate/result, and `report_source` attributes the current output. For caller-directed revision of a released review, build repair state with `fcap-rework(fcap-with-report(fcaq-initial(request,docs),retainedReport),feedback)`, then `fcap-begin-context`; check `fcap-amendable`. Admit it with `fcac-bootstrap-budget`, the original `fcaq-live-checker` and `fcac-resident`. This reconstructs retained evidence rather than recovering released KV.

## Roles and tools

### Controller roles

| Role | Model reply | Native effect |
|---|---|---|
| Refine | `{brief}` | Retain beside original goal |
| Plan | `{plan}` | Retain approach |
| Split | `{tasks:[...]}` | Ordered queue |
| Implement | Tool calls, then `{task:"done"}` | Edit resident values; advance task |
| Review | `{verdict:"accept",reason}` or reject | Retain findings; check or repair |
| Repair | Tool action, or `{diagnosis,change,next:"implement"}` / `next:"plan"` | Recheck changed candidate or choose a new approach |

Native tools are `read`, `cat`, `rg`, `jq`, `head`, `tail`, `wc`, `sort`, `uniq`, `tr`, `cut`, `awk`, `sed`, `balance`, `arity`, `edit` and `write`, plus controller `verify` and the BML repair/rehearsal doors below. These names select Form implementations, not shell commands. Their supported subsets remain defined by `bml/form-agent-tools.bml` and its native search/JSON helpers.

Calls use `{tool,arguments:[strings],input?:string}`. `verify` takes empty arguments and no input; the model cannot supply a replacement checker. `edit` takes `[path,old,new,...]` for exact replacement pairs, or `[path,"sha256",observedHash,newText]` for whole-document replacement. A batch may instead use `[path,resident_sha256,old1,new1,...]`: the initial hash is checked once, then the same atomic replacements run. Each old span must be unique; stale identities or a failed pair leave the document unchanged. `fat-edit-guidance()` owns the complete argument contract. Successful mutations return identity, byte count, current hash and before hash. `write` creates an absent writable path. Review is read-only; implementation and repair can change only the caller's writable paths.

Closing the last task runs source checks before review; acceptance with a nonempty string reason runs them again and retains a `native-coding-review-v1` report. An open source failure survives diagnosis, replanning, renewal and resume; each accepted changed repair rechecks it immediately. Unchanged/failed edits do not manufacture a verification run. Three identical tool/argument/result observations with unchanged documents select repair and replanning. An ordinary search miss alone remains a normal result. Malformed model messages enter the correlated framebuffer revise flow.

### Optional initial reasoning

`initial_reasoning_tokens` bounds the first reply's generation, including reasoning and final output. Optional `reasoning_answer_tokens` reserves a separate final stage. Each allowance is capped independently by `max_reply_tokens` when present. Form uses the actual final-channel boundary; incomplete generation never acts. Within a reserved final stage, a stop inside an unfinished JSON string may yield to a finite non-stop prediction using the remaining allowance. It retains the prefix and stop-selection evidence; it invents no closing text. Other stop/syntax behavior is unchanged.

### Reasoning across the task

`reasoning_tokens` plus `reasoning_answer_tokens` applies that flow to every reply. Before generation, Form reserves the current final-stage task handoff as well as both allowances. Only complete final output reaches tools/checks. Reasoning and partial output stay in private evidence, outside the framebuffer. Metadata reports actual stage counts and completion separately from response quality.

Each generated reply consumes `turns`, including tool reads and retained no-action attempts. The model sees remaining replies and the minimum completion protocol. An unfinished local reply at a generation boundary retains its exact private bytes, preserves completed effects and enters the existing repair flow with a request for one smaller complete JSON action. The caller's reply ceiling stays unchanged. Feedback uses the same stream while room remains; a full context can renew on the same admitted weights, retaining the task and counters. Failed prediction, admission, evidence retention and partial prefill do not select this retry. Exhausted turns settle release with the candidate retained. The returned session owns release even after failure; incomplete release remains a distinct outcome.

### Local retry and changed choices

`form-cli-code-live.bml` owns reply completion; `form-cli-code-policy.bml` owns repair and replanning. A truncated reply never reaches tool admission. Its failure travels with the original goal, current candidate, checks and private evidence path. The same cause, role and reply ceiling remain the same counterexample even when the private path or generated count changes. A checked candidate first retries its review role with a shorter response; retained native identities need not be copied into prose. An unchanged failure after that changed instruction requires a new plan. A stopped reply re-observes completion through the framebuffer; JSON validity and task quality retain their separate checks.

When an observation exceeds the current decoder capacity before prefill, its
exact bytes are retained privately and the live stream receives a compact
signal requesting focused source queries. The native cursor stops after the
first ID that exceeds available capacity, before changing KV; exact fits and
cursor failures remain distinct. Original documents, pending tasks,
completed effects and checks stay in the same checkpoint. A checked candidate
stays in review. The signal reports actual admission, with no weight admission
or context replacement; if that signal also cannot fit, the existing renewal
choice handles the retained state. Partial prefill and other generation
failures keep their own release handling. A repeated capacity issue against the
same source identities invites a new plan, independent of its receipt path.

#### The observation cap, the renewal reserve and the repeat across contexts

A native tool result whose stdout is larger than the **observation cap** does not enter the decoder whole, though it
would fit: the cap is a fifth of the window, at least 1024 IDs, counted in bytes at 3 per ID (host-walk.bml's whole read
measured 2.84), so 3276 IDs and 9828 bytes at 16384 positions. The stream takes the bounded note instead, through the
same focus path as the capacity signal above: the read's exit, byte and line counts and sha256, its first 12 and last 6
lines, and the window to read next, `{"tool":"sed","arguments":["-n","A,Bp",path]}` with at most `window_lines` lines so
that one window fits under the cap, plus an `rg -n -F` form to find a symbol. The exact observation stays in private
evidence, the document stays whole in the tools, and a read that worked counts no repair (a window refusal is a failure
the lane repairs, and carries the same excerpt). A verification or check note is never bounded: failed-check evidence
stays complete. The bounded state is saved before the whole read is, so no checkpoint holds it, and a checkpoint that
does (a lane out of replies saves what it has) is bounded again when a lane opens on it.

A renewal prompt must leave room for a reply and the next observation, the **renewal reserve**: an eighth of the window,
at least 1024 IDs and at most a quarter, whether or not the caller sets a reply ceiling. A refusal is a choice point,
not an exit: the trigger note is bounded and the renewal asked once more, and when nothing in the prompt can be
bounded the lane ends with `native-context-renewal-prompt-caller-capacity-refused` after one tokenization. It never
renews into the overflow that stopped it. The `form-code-context` row names `renewal_attempts`.

A fresh context is sight regained, except where the call's bytes never reached a context. An observation that was
refused or bounded marks the swerve record **unsighted**, and the next context keeps its context number and the
policy's count of the same call in a row, so the same read over the same result keeps counting across renewals:
the policy's no-progress route at the third, the swerve at the third, the replan at the fourth, the end at the fifth.
Without the mark each renewal restarted both counts and the same read stayed silent however often it came back. The
mark clears as soon as a context has observed anything.

`form-cli-code-bounded-note-band` (8191) replays the 12:30 day option's retained replies and form-code-context rows
(`form/form-stdlib/tests/fixtures/form-cli-code-day-option.jsonl`) through these paths with the decoder and the renewer
scripted. What it cannot show is the clock: the seconds returned are read on the next physical option.

This applies the distinction between truncated tool-call recovery and execution in [Hermes's truncation controller](https://github.com/NousResearch/hermes-agent/blob/main/agent/turn_truncation.py), and the focus on meaningful repeated outcomes in [OpenClaw's loop detection](https://docs.openclaw.ai/tools/loop-detection). Form uses its own owned state, live signals and caller allowances. [OpenClaw's agent loop](https://docs.openclaw.ai/concepts/agent-loop) also separates finished tool results from an unfinished continuation; Form's checkpoints preserve completed actions before admitting feedback. These are native BML flows in the existing process.

### The local lane's stages

Each stage below is a BML organ with its own band; none of the bands opens a model.

- **Rung 0, the check witness** (`form-cli-check-witness.bml`, called by `form-cli-code-request.bml`): every
  caller check meets a witness before any model opens. A check no candidate could meet (a dialect the tool
  does not speak, a read-only document it would have to change) is refused at rung 0 with reason
  `caller-check-unmeetable`, zero generated IDs and the blocking check named; a check the original fails but a
  candidate could meet reads open and is never refused. `form-cli-check-witness-band` 1023.
- **The edit ladder** (the `edit` tool in `form-agent-tools.bml`): an old text that drifted (a whitespace run,
  for one) lands where exactly one place matches and names its rung; two places are refused; a miss carries
  the nearest resident line window. `form-agent-edit-ladder-band` 8191.
- **The turn guard** (`form-cli-code-turn-guard.bml`, read by `form-cli-code-policy.bml`): the lane reads its
  own tool turns as a stillstreak. At 2 the observation names the repeat, at 3 a replan is required, at 4 the
  attempt ends with the candidate retained. `form-cli-code-turn-guard-band` 131071.
- **Swerve** (`form-cli-code-swerve.bml`, stepped by `form-cli-code-live.bml`): a reply whose content was
  already decoded decodes again seeded from that content, the lane's phases speak on the stage bus, and the
  last replies are kept for the deliverable. A context that has observed nothing is fresh sight unless the record is
  unsighted (an observation that never entered a context). `form-cli-code-swerve-band` 4095,
  `form-cli-code-bounded-note-band` 8191.
- **The circle** (`form-cli-code-circle.bml`): the local satsang for code. Candidates are published over a toy
  copy of the gap and their band runs in a `./fkwu` child under a deadline; voices other than the writer's
  judge, execution vetoes consensus, refusals are evidence, and exactly one pointing comes back when nothing
  passes. `form-cli-code-circle-band` 1048575.
- **Lesson review** (`form-cli-code-lesson-review.bml`): a lesson learned under one contract is recalled for
  another of the same family, a local lesson-review voice speaks after every rung, and every rung packet
  becomes a verified or failed corpus row. `form-cli-code-lesson-review-band` 1048575. The circle and lesson
  review stand with their bands; today only lesson review calls the circle, and no lane stage calls either.
- **The remote guide** (`form-cli-guide-packet.bml`, read by `local-flow-reading.bml`, `session-rent.bml` and
  the review doors): one light remote guide is earned only from rung rows that show distinct failed swerves,
  saturation and a second local guide, the last rungs repeating earlier outcomes; a green rung never earns
  it. A row without digests reads swervecount 0 and is never saturated, so the ledger's rows written before the
  native-turn ladder, which carry no ladder fields, earn nothing; a row the ladder writes carries `rung`,
  `approach_digest` and `outcome_digest`. `form-cli-guide-packet-band` 16383.

### Checkpoints, resume and feedback

`observe/native-turn-run.bml` offers six local attempts by default; stdin
`attempts` selects a positive allowance. Each admission allows 64 local replies
by default, twice the earlier allowance; `turns` selects another amount.
A gap is walked as a ladder of
distinct approaches (`form-stdlib/bml/native-turn-ladder.bml`), each an option of one backtrack walk. First a
coherence check: a gap whose band reads above its `full`, or whose own `; Verdict n` head names another number,
cannot be met, so the turn lands one `contract` row with zero options and the gap waits on its contract (its
checks, its documents, its band and its `full`). Then the native rung (rung 0, no model, no local ID): the
body's boolean-syntax repair and one-value wrapper search against the band, with `native_rehearsal_checks`
on every model request after it. Then the model ways in order: `first` (the supplied task), `resume` (the same
checkpoint with identified execution feedback under the unchanged original goal, documents and checks),
`context` (a fresh checkpoint carrying the finding as text), `lens` (a smaller document view) and `voice` (the
next model of the write-code chain). A clinamen gate stands before every way: an option whose approach
digest already stands in the gap's history under the same contract with a non-green outcome is refused with
no option and no GPU (`[native-turn:clinamen] ... refused`), and the walk moves on; a `resume` is admitted
again only while the outcomes move. A row the walk stopped (the deadline, a lane that never launched, a
refused publish) judged nothing and is no history. Queue `sources` adds read-only
caller/helper context alongside `writable`. Catalog admission is the default;
focused native reads supply source as needed, and `document_context:"full"`
requests the complete text. Missing checkpoints and source
conflicts retain the evidence without repeating an unchanged admission. Only a
completed, released candidate may be published, against its original source
snapshot, and its execution check decides whether the edit stays. Failed
candidates remain in the coding checkpoint; published failures are restored.
Each ledger row records local generated/injected IDs, repairs and checkpoint
identity. Coordinator attribution is not token measurement: `rented_tokens`
remains null until transcript usage is supplied. Remote access is unchanged.

### Direction toward zero rented coding

The goal is verified local completion and quality with zero required remote
prompts and tokens. Native substrate lookup, retrieval, calculation and
executable recipes lead. An observed native answer takes precedence over model
prose; local models contribute what the substrate cannot yet complete. Form
owns tools, choices, care, original checks, checkpoints and release throughout.

Local tokens never have a penalty. Encourage useful local guidance, review,
repair, prompt A/B on the same tasks and checks, multiple perspectives and
changed attempts. Prefer a local guide; after repeated local guidance failures,
carry the retained finding into light remote guidance. Available local models
may serve different tasks; this guidance does not supply automatic routing.

Optional remote review may accompany each independently successful step. It
does not count toward required-remote dependence, while its actual spending
stays visible. Remote implementation, diagnosis, guidance or acceptance needed
for success is required assistance. Classify by dependence rather than timing.
Keep local-only evidence distinct when a remote suggestion changes a candidate;
re-observe that candidate locally. Missing attribution remains unknown.

`fl-session-rent()` carries the caller's guide; coding and review admit
`fl-local-work()`. The native lifter and authoring tools operate before model
admission where their observed capability serves the task. The coding
checkpoint retains original goals, source identity, checks and completed
effects while changed local choices repair failures.

The existing task loop offers work through `learn/native-turn-queue.jsonl`.
`observe/native-turn-run.bml` runs the original checks, walks the ladder above (the native rung before any
model way, each model way offering the task to the native coding lane), publishes a released candidate against
unchanged source, and observes its checks again. An unsuccessful choice retains its finding and candidate before
restoring source; the next way is a different approach under the same contract.
The current scheduled doors are `observe/day-turn.bml` and
`observe/scheduled-walk.bml`.

Each option records local generated/injected IDs, attempts, checks, release and
elapsed time in `receipts/native-turn-ledger.jsonl`, with the ladder's fields: `rung`, `approach`,
`approach_digest`, `outcome_digest`, `clinamen` (what differs from the nearest earlier model approach),
`finding`, `stillstreak_max` and `remote_dependence`; the native rung's row carries no `wall_ms` and a known 0
local IDs. `local_verified` requires
actual native coding, complete status, observed release and a passing changed
candidate. Dry, test, supplied-candidate and `publish_from` observations cannot
establish local coding success. A legacy green band remains its own observation
and does not become verified local completion.

`execution_required_remote_prompts` and `execution_required_remote_tokens`
describe the provider-free native coding invocation only. Caller/coordinator
guidance, arbitrary band execution and the whole task are outside that scope.
Whole-task `required_remote_*` and `optional_remote_review_*` remain null until
attributed. Coordinator identity does not measure usage.

Open and close a session with `observe/local-flow-review.bml`. It reads current
work and ledgers, reports verified local completion separately from green bands
and unmeasured verification, and follows unresolved work with `split`, `lane`,
`queue` or `turn`. Throughput remains diagnostic; it does not select the next
repair. Local token share does not judge success.

A Claude Code transcript goes to `observe/session-rent-run.bml`, one count per
`requestId` including its subagents. Codex uses
`observe/rented-turn-meter-run.fk` at opening and closing: cumulative input,
cached input, output, reasoning and total, with the comparable delta. A first
reading, different transcript or decreasing counters starts a baseline.
Cached input belongs to input; reasoning belongs to output. Neither is added
twice. Absent or incomplete usage is never zero.

For Codex work sessions, `observe/rented-work-run.bml` separates required work,
optional review and unclassified usage. Send one JSON line with `action`,
`session` and, at `begin`, `transcript` and `role`. `mark` settles the current
phase before changing its role; `read` reads the retained sample; `close`
settles the last phase and publishes an idempotent session row in
`receipts/rented-work-sessions.jsonl`. Roles are `work`, `review`, `unknown`.
Guidance, diagnosis and implementation needed for completion are work.
An independent optional review stays separate. Roles are caller declarations,
not an inference from token volume, model identity or a tool call. The row's
`from` and `through` delimit its measured interval. Missing historical role
evidence invites recovery from the retained task transcript. A retrospective
declaration keeps its source packet, timestamps and rationale; required work
may be conservatively charged in full, without inventing recorded phase
transitions or optional review. Until recovered, unclassified usage remains
visible and the human reading does not present its work or review as zero.

```json
{"action":"begin","session":"native-source-wave","transcript":"/absolute/path/to/rollout.jsonl","role":"work"}
```

`local-flow-review.bml` reads the latest published phase rows alongside local
completion evidence. Codex cumulative counters are the authoritative quantity;
repeated call-local usage events are not summed as separate expenditure. A
complete invalid sample stays unknown, and an unfinished row waits for its
newline. State publication verifies its bytes and role totals reconcile to the
cumulative delta. Local tokens carry no penalty.

The sovereignty milestones are complete local source lookup and task planning,
locally authored edits that pass the original execution checks, local repair
and review within the same owned checkpoint, and local publication with
observed release. Advance these on representative repository tasks, retaining
failed attempts as well as completions. Zero required rented work is established
for a task only when its full local path completes without required remote
guidance or repair; a provider-free model invocation alone does not establish
that outcome. Optional remote review remains available and separately measured.

Compare comparable tasks using actual quality, verified completion, required
remote prompts/tokens and the functions/lines changed. Retain unsuccessful
attempts and optional review spending beside them. Generated and injected local
IDs and live phase time guide resources without penalizing local use. The next
movement follows the unresolved cause in its retained context. Zero is observed
when the scoped work succeeds without required remote assistance; remote access
remains available.

Ordinary JSON jobs save native binary checkpoints under `.hearth/code-memory/` after completed transitions. Resume uses the same original request and returned checkpoint ID; `turns` adds replies and `context` sizes the admission. Candidate documents, tasks, findings and failed checks survive. Incomplete output never becomes an action. A complete candidate is rechecked before reuse, with zero new model/tool counts and its earlier provenance retained. Exact original goal/documents/writable/checks select that lesson; changed contracts do not. Review recall supplies guidance rather than presenting an old report as new work.

An observed failure beyond a completed coding job's checks can reopen that checkpoint with `feedback:{id,text}`. Repeating identical event bytes is idempotent; reusing an ID with changed text is a conflict. The original contract, review and candidate remain. Keep `weight_training:0` on continuation while assessing quality. `evaluation:1` starts fresh before continuity lookup and cannot resume. Review reports remain assessment experience, not verified implementation targets. Checkpoint digests detect corruption; filesystem publication remains caller-owned.

## Native BML boolean repair

`repair-bml` takes `[path]` and empty input during implementation. It repairs n-ary boolean primitive calls into binary nesting within writable `form.bml` sections, preserving other bytes and reporting spans/hashes. It does not compile or establish behavior. The pure `fbr-repair(source)` returns `[ok,candidate,edits,reason]` without IO; run the original checker afterward.

## Rehearse a small BML repair

`rehearse-bml` takes `[path]` or `[path,decimalCheckBudget]` and empty input. `fbvs-search(source,checker,contract,budget)` wraps one existing value expression in a unary call already in that source. Each candidate starts from the original and runs the unchanged checker. The 1–64 allowance counts the original check. A passing original stays unchanged; exhaustion preserves it. Results retain spans, digests and observations. This is one repair family; the caller owns checker cost, purity and any process supervision.

### Caller-enabled rehearsal after a failed action

`native_rehearsal_checks` offers that search after the applicable failed/unchanged model action. The source contract, writable boundary, review and final verification remain. A passing rehearsal supplies a verified candidate and its evidence in the same owned task; it does not establish arbitrary synthesis.

## What a successful result proves

Coding completion requires a changed candidate and passing caller checks. Review requires unchanged documents and a nonempty checked report. Exact output, syntax, returned failure and answer quality remain distinct observations. A `jq` property or balanced delimiters does not establish that an application runs. Preserve execution order, absence, ownership and release in the actual behavior checks.

A definition check is `{kind:"definition",path,function,cases:[[input,expected],...]}` for one unary integer function in the existing definition grammar, for example `module calc { fn bump(x) = add(x,1); }`. It admits pure arithmetic/comparisons without recursion, division, unknown bindings or effects. It executes each case as native nodes; it is not a general repository compiler.

Embedding callers can use `fcac-run(model,goal,documents,writable,[checker,contract],context,turns)`. The callback receives `(contract,candidateDocuments)` and returns `[passed,observationString]`. `fcac-review` takes `[reportChecker,reportContract,sourceChecker,sourceContract]`; the report callback receives `(contract,[documents,report])`. Source-only `verify` needs the optional last pair. These lower-level calls are fresh/unmanaged; the JSON door supplies continuity. Callback behavior and effects belong to the caller.

A native application can retain the managed request flow through
`fcaq-managed-with(request,residentDocuments,[checker,contract])` after validating
its request and admitting its documents. It shares checkpoint preparation,
feedback, model choice, local repair, learning and release with the JSON door.
The application supplies the callback; model output cannot select it. The
callback preserves the original assertions and scope while observing the actual
operation. Its failure enters the same repair movement at `verify`, implementation
completion and final submission. Executable snapshots and their effects belong
to that caller; an observed candidate does not publish the source files.

### Attach native repair to the review loop

Append `[nativeRepair,repairContract]` to the four-entry review checker. On a valid failed report check, Form calls it once with `(contract,[documents,report,failure])`. Return `[available,proposedReport,method,observation]`, optionally followed by `"review"`. A changed nonempty proposal reruns the complete original checker. Failure returns to ordinary repair without recursively invoking the hook. The optional continuation keeps the task in review after a passing proposal so the same resident can complete its explanation. Update every claim affected by the repaired code.

`native_repairs` retains paired before/after reports and actual checks; `report_source` names authorship. A completed context observation permits later references, while fresh contexts receive the full pair. The hook cannot be installed by model JSON, does not run for coding or passing reports, and grants no additional model turns. Callback checks/effects remain caller-owned and separately counted.

## Explicit Qwen adapter sessions

`fcms-open-adapted(modelPath,prompt,context,profile,adapterPath)` or immediate `fcms-attach-adapter(session,path)` attaches a rank-one head adapter before generation/observation. Always use the returned owner, including on refusal; inspect `fcms-live?`/`fcms-reason` and finish with `fcms-release-ok?`. Renewal transfers ownership and must not double-release the old stream. `fcmd-carrier`, `fcmd-path`, `fcmd-digest` and `fcmd-owned` expose admission evidence. The 5,120-wide finite float32 A/B contract and tensor-byte digest do not establish training-base compatibility or answer improvement. JSON coding does not select this adapter automatically.

## Observe

Shared-memory publisher `qwen.coding.<pid>` reports tools, generated IDs, repairs, checks and recalled lessons. Generation updates every four IDs; admission/prefill emit separate native stages. Read stage time, work counters and real tool outcomes. Private prompts, answers and reasoning stay outside the public framebuffer. Failures retain the exact observation in the checkpoint and enter the same local repair/choice flow; [live diagnostics](live-dynamic-diagnostics.md) describes correlation and care.

The final `form-native-code-v1` result includes status/reason, report, candidate documents, checkpoint, turn/tool/check/repair counts, generated/injected IDs, release, native repair/amendment provenance and learning evidence. `release_ok` is independent of work quality. Read the result and execute the original behavior before publishing a candidate or teaching it as verified. Corpus fixtures and passing field assertions establish their own scope, not general coding quality.
