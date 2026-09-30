# Qwen coding inside Form

`code` edits caller-supplied source values through local Qwen and Form tools. Token IDs, KV state and tool observations remain in the native process; filesystem loading/publication belongs to the caller and telemetry uses shared memory. The controller offers no model-selected shell, network or provider call. Metal is admitted dynamically. Source bootstrap still lowers cold BML through a native compiler process; warm images reuse that work.

The north star is compact, behavior-preserving native refactoring with retained context, reusable compiled abstractions and locally verified learning. `form-lift.bml` supplies the current engineering guidance at BML admission: lexical lambdas, stdlib reuse, named domain values, explicit ownership and live observations. Classes group APIs; templates describe parameters and members, without implying instance accessors, interface checking or specialization. See [BML admission](native-bml-admission.md#local-lift).

An evaluated session LoRA may offer a proposal before Qwen unless the request names a model, selects evaluation or requests review. The original checks decide whether it suffices. The optional learner uses native workers. Explicit `model` and `weight_training: 0` keep ongoing quality assessment on the selected local model with checkpoints and no answer gradients. [Session learning](native-session-learning.md), [healing](form-cli-healing.md), [provider repair](form-response-resource.md) and [provider synthesis](form-response-synthesis.md) describe their separate interfaces; provider paths are not implicit `code` behavior.

## Native surface lifting

`form/form-stdlib/bml/form-source-lift.bml` provides `form-source-lift(source, owner)` for Form source. It returns `[equivalent, candidate, definitionCount, lowered]`. The cursor preserves grouping and comparison contracts while emitting BML definitions, blocks, lazy conditionals and infix operators. Its proof compares the lowered trees, decoding string escapes and the runtime's boolean spellings. Compiler-added construction or local binding, and bindings shadowing boolean literals, produce `equivalent = 0`; that candidate is not ready for publication. Malformed source follows the compiler's diagnostic path.

This deterministic step uses no model tokens and writes no files. Keep the original source, check it is still current before publishing, and observe affected callers. Keep the compiler's bootstrap closure in its bootable source form. Structural lifting does not establish that every algorithm or abstraction is optimal.

## Call without knowing Form syntax

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

Word checks are exactly `{kind:"word-range",path,minimum,maximum}` for a document, or `field` instead of `path` for a top-level report string. Inclusive nonnegative bounds use the shared ASCII-whitespace counter, including headings. Missing text differs from empty text. Failure supplies observed count and repair direction; count agreement does not establish content quality. Report checks take the report on stdin, have no document access and accept no `input` override. The additional `{kind:"provider-usage-sequences"}` report assertion is documented in [usage observation](form-provider-usage-observation.md).

### Direct implementation entry

`code_entry: "direct"` begins one pending task referring to the full original goal. It avoids generated refinement, planning and splitting, while retaining review, verification and the requirement for an actual change. Resume retains the saved phase. `review_entry: "direct"` selects the corresponding read-only entry.

### Resident instruction context

Every admission receives the shared Form meanings in `bml/form-cli-qwen-meaning.bml`. BML coding also receives `form-lift.bml` guidance and the available repair instructions. A later role instruction may reference exact text already delivered in that resident context; incomplete observations never mark it delivered. Renewal/resume supplies the current instructions again. Delivery establishes context availability, not that the generated work followed it.

### Resident context and source queries

Full admission includes original document text and current identities. A successful unchanged `read` may use `stdout_reference`; a changed read may use one `stdout_patch` against the admission snapshot after exact reconstruction and size comparison. Patches never depend on earlier patches. `cat` returns full current text. Checks always consume actual document bytes.

Catalog admission supplies IDs, paths, sizes and SHA-256 identities while native tools retain all text. Optional `source_queries` execute actual read-only queries before generation, for example `{"tool":"sed","arguments":["-n","10,20p","notes.md"]}`. Results retain exit and diagnostics. Excerpts do not establish full visibility; catalog reads return full text. Resume recomputes selected queries; omission clears prior selections. Supply enough source for the task and use the native tools for a specific missing fact.

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

Each generated reply consumes `turns`, including tool reads. The model sees remaining replies and the minimum completion protocol. Exhausting a token ceiling preserves the candidate and unfinished bytes without executing them. When a completed action leaves insufficient room for feedback or the next full reply, Form can renew the stream on the same admitted weights, retaining completed work and counters. Partial generation, failed admission and exhausted turns do not select renewal. The returned session owns release even after failure; incomplete release remains a distinct outcome.

### Checkpoints, resume and feedback

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

### Search a small native repair before another model call

`fcrs-search(source,name,arity,bindings,roles,checker,contract,budget)` in `form-cli-review-search.bml` replaces one call with a compatible caller binding forwarding the original parameters. Roles are caller data, not inferred types. It checks the original, then each candidate with the same contract. Result: `[unchanged|repaired|exhausted|refused,source,checkCount,observation]`; failure/exhaustion returns original source. Search itself makes no model, file or process call. Attribute native structural repair separately from generated code.

### Attach native repair to the review loop

Append `[nativeRepair,repairContract]` to the four-entry review checker. On a valid failed report check, Form calls it once with `(contract,[documents,report,failure])`. Return `[available,proposedReport,method,observation]`, optionally followed by `"review"`. A changed nonempty proposal reruns the complete original checker. Failure returns to ordinary repair without recursively invoking the hook. The optional continuation keeps the task in review after a passing proposal so the same resident can complete its explanation. Update every claim affected by the repaired code.

`native_repairs` retains paired before/after reports and actual checks; `report_source` names authorship. A completed context observation permits later references, while fresh contexts receive the full pair. The hook cannot be installed by model JSON, does not run for coding or passing reports, and grants no additional model turns. Callback checks/effects remain caller-owned and separately counted.

## Complete text review with explicit edits

These native helpers keep review evidence distinct from its judgment:

| Helper source under `bml/` | Interface and scope |
|---|---|
| `form-cli-review-followup.bml` | `fcrf-admit(model,state,checker,context,turns,feedback)` admits one follow-up after a completed review in the same live context, retaining first and final reports under the same total budget/checker; final release is owned |
| `form-cli-review-bindings.bml` | `fcrb-check(draft,report,sourceDocs)` checks each finding's exact `draft_field/draft_quote`, `source_path/source_quote`, explanation and applied/unresolved status. Applied replacement quotes must appear in changed text; explicit delete requires an empty replacement and disappearance of the original. Returns `[passed,observation,applied,unresolved]`; provenance does not prove entailment |
| `form-cli-review-spans.bml` | `fcrs-spans(text)` produces exact byte spans using ASCII punctuation/whitespace. `fcrs-complete?` checks partition; `fcrs-ids-complete?` checks one ordered integer index per row. This is participation, not sentence parsing or semantic coverage |
| `form-cli-quote-evidence.bml` | `fcqe-restore(source,quote,maxBytes)` restores only a unique ASCII-whitespace-normalized quotation to original byte offsets. `fcqe-blockquote-restore` explicitly reads single-level Markdown quote markers. Changed/ambiguous words remain unavailable. `fcqe-lines(source,first,last,maxBytes)` selects one-based inclusive physical lines with exact bytes |
| `form-cli-review-source-lines.bml` | `fcrl-propose(docs,report,maxBytes)` resolves findings with `source_path` and `source_lines:[first,last]`, leaving explicit conflicting quotes unchanged. Bind `fcrl-repair([docs,maxBytes],subject)` explicitly and use `fcrl-check` to check coordinates plus original evidence links. Selection is all-or-nothing; it does not establish relevance |

`form-cli-review-edits.bml` supplies `fcre-packet(reportText,fields)` → `[ok,packet,diagnostic]` and `fcre-apply(reportText,fields,message)` → `[ok,candidate,diagnostic]`. Select unique existing top-level string fields. The message carries the packet's exact base hash and one ordered decision for each span: `{id,status,action,reason,text?}`. Status is supported/inference/unsupported/style/action-gap; action is keep/replace/remove. Replacement must be nonempty and different; only replacement carries text. Joining adds no punctuation. Stale/missing/repeated decisions preserve the original. Retain packet, decisions and candidate, then rerun original checks and read the whole answer.

### Explicit Qwen adapter sessions

`fcms-open-adapted(modelPath,prompt,context,profile,adapterPath)` or immediate `fcms-attach-adapter(session,path)` attaches a rank-one head adapter before generation/observation. Always use the returned owner, including on refusal; inspect `fcms-live?`/`fcms-reason` and finish with `fcms-release-ok?`. Renewal transfers ownership and must not double-release the old stream. `fcmd-carrier`, `fcmd-path`, `fcmd-digest` and `fcmd-owned` expose admission evidence. The 5,120-wide finite float32 A/B contract and tensor-byte digest do not establish training-base compatibility or answer improvement. JSON coding does not select this adapter automatically.

### Full-vocabulary Qwen head learning

`qwen-lora-full-loss.bml` provides `qlfg-open(rows,cols)`, `qlfg-live`, `qlfg-loss(owner,logits,target)` and `qlfg-gradient(owner,projection,logits,target)` → `[ok,loss,hiddenGradient]`. It computes full-vocabulary cross-entropy and `W^T(softmax-onehot)` using the immutable packed Q8_0 projection. Caller-owned handles, geometry and whole blocks must agree. `qlfg-owned` reports scratch buffers; `qlfg-close` synchronizes/releases once, including partial admission. Invalid/nonfinite results remain absent. This neither updates nor publishes an adapter by itself.

### Cached head batches

`qwen-lora-head-batch.bml` reuses frozen normalized features `[float32Bytes,target]`. `qlhb-gradients(ctx,lossOwner,adapter,samples)` returns token count, mean loss and mean A/B gradients; `qlhb-loss` omits gradients. `qlhb-optimizer(adapter)` borrows A/B and owns six buffers; `qlhb-step(state,batch,rate,step,maxNorm)` performs checked/clipped Adam updates. Release optimizer buffers with `nlb-state-close` (success flag); adapter/loss owners remain separate. Refresh the session head before generation because these calls overwrite scratch. Caller-owned masks, training/validation separation and held-out answers establish learning quality.

## Observe

Shared-memory publisher `qwen.coding.<pid>` reports tools, generated IDs, repairs, checks and recalled lessons. Generation updates every four IDs; admission/prefill emit separate native stages. Read stage time, work counters and real tool outcomes. Private prompts, answers and reasoning stay outside the public framebuffer. Failures retain the exact observation in the checkpoint and enter the same local repair/choice flow; [live diagnostics](live-dynamic-diagnostics.md) describes correlation and care.

The final `form-native-code-v1` result includes status/reason, report, candidate documents, checkpoint, turn/tool/check/repair counts, generated/injected IDs, release, native repair/amendment provenance and learning evidence. `release_ok` is independent of work quality. Read the result and execute the original behavior before publishing a candidate or teaching it as verified. Corpus fixtures and passing field assertions establish their own scope, not general coding quality.
