# Direct coding entry: actual work and cost

Signed: Codex, 2026-09-16.

## Implementation

`code_entry: "direct"` starts a fresh coding request in implementation, with
one pending task referring to the original caller goal. The complete goal is
admitted in context, including on resume. It leaves the brief and plan
empty. The default remains staged. The original documents, write boundary,
model review, native checks, repair, replanning, retention and release owners
remain in place. Review requests cannot acquire this coding entry.

The executing policy checks cover premature acceptance, unchanged candidates,
writes during review, a failing caller check, replanning, and a checked
completion that preserves immutable documents. Request checks cover the default,
explicit entries, invalid types and modes, and fresh evaluation without recall.
A real private checkpoint observation writes and reloads direct state through
the existing memory owner: the saved implementation phase survives, and a
changed write contract is refused. It uses synthetic data and no model.

No C seed, external runtime, model server or provider dependency was added.

## Controlled coding observation

The task is the previous guidance source edit, from its original frozen
document. The first comparison changes only `code_entry`. The second removes
only `initial_reasoning_tokens` from that direct request. All other task
fields, including documents, checks, context, turn allowance and evaluation
exclusion, remain identical. The checker was calibrated before admission.

| Variant | Elapsed ms | Replies | Generated IDs | Injected IDs |
|---|---:|---:|---:|---:|
| Staged, initial reasoning | 464,958 | 9 | 1,121 | 1,059 |
| Direct, initial reasoning | 500,323 | 5 | 1,274 | 920 |
| Direct, ordinary generation | 342,380 | 6 | 301 | 1,073 |
| Direct, first compact goal reference | 202,434 | 5 | 186 | 583 |
| Direct, reference retaining requested tool calls | 249,405 | 5 | 430 | 557 |

Every result has an actual changed candidate, zero repairs, successful release,
and zero provider calls. The first compact-reference result runs one completion
check; all other results run two checks. All candidates compile as native BML.
The ordinary-generation run uses four tools; the other direct runs use three.
A native comparison rechecks the original document contracts and exact
permitted request differences. It returns 1 with exit 0. These caller checks
are structural; they do not establish semantic quality.

The ordinary variant observes **122,578 ms less elapsed time** than the staged
run. It does not establish a general speed ratio or a default routing rule.
The reasoning variant is slower despite fewer replies. Its initial reply alone
generates 1,096 IDs before an edit; ordinary generation first reads after 24.
Reducing stages does not necessarily reduce reasoning or total execution cost.

The coordinator read both actual candidate sources. The reasoning candidate
makes its new optional-evaluation and timing wording more absolute than the
staged candidate, which explicitly repeats the policy exception. This is a
precision concern beyond its passing structural checks, not an observed
downstream response failure. The ordinary candidate preserves the original
policy relationship and explicit caller constraints while connecting the next
action to the supported decision and distinguishing optional evaluation from
current prerequisites.

The last two observations use byte-identical requests to the ordinary run.
Only the executing pending-task reference changes. Initially that reference
said to implement the original caller goal. The result passed the final native
checker but omitted the goal's explicit request to call `verify`. That fastest
result therefore falls short on instruction following. The revised reference
includes the caller's explicitly requested tool calls. Re-observation shows
an actual `verify` call, followed by transition to review, a read of the changed
source, and checked acceptance. This reminder adds no blanket tool requirement.

The final candidate preserves the existing policy relationship and caller
constraints, connects action to the supported decision, and distinguishes
optional evaluation from prerequisites. Its elapsed time is **215,553 ms less**
than staged in this observation. A native serialization comparison confirms
that the original goal and documents remain held while a pending observation
shrinks from **1,123 to 440 bytes**, saving **683 bytes** each time that pending
task is carried. Later checks and tool use remain observable; a smaller prompt
alone does not establish that instructions were followed.

These candidates remain assessment evidence. The installed guidance remains
the previously re-observed native source; no downstream answer-quality claim
is inferred for the new candidates. Human resonance preference is unobserved.

## Evidence and accounting

Private evidence is under `.hearth/response-parity/`:

- `direct-code-comparison.json`: rechecked executions, request preservation and
  separate goal accounting snapshots.
- `direct-code-process.log`: direct entry with initial reasoning.
- `direct-code-plain-process.log`: direct entry with ordinary generation.
- `direct-code-reference-process.log`: first compact reference, omitted explicit verify.
- `direct-code-tool-reference-process.log`: revised reference, actual verify and review read.
- `direct-code-*-candidate.bml`: actual retained sources, including both references.
- `direct-code-reference-shape.json`: goal preservation and observation sizes.
- `direct-code-checkpoint-observe.bml`: actual save/resume/contract observation.

The new session roots are
`.hearth/response-sessions/d2fba912834554701a2069c7d6883d1b85c0c204ba266dc350fdde4e178c7c90-27633-1789547052506`
and
`.hearth/response-sessions/d4ae50ce79aaceb719e2527ae2bf3639da86c092e868694d401b90388bbae403-28880-1789547605864`.
The earlier staged result and provider reference remain documented in
`receipts/2026-09-16-native-guidance-coding.md`. The retained provider edit took
50,746 ms; the final native variant remains substantially slower. No new
provider baseline call was purchased for this comparison.

Both reference results share the same manifest and request hashes as the
ordinary-generation result. Their session root suffixes are respectively
`-33386-1789548538127` and `-34160-1789548895190`. Each retained result is paired
with its request, receipt, process trace and actual final-channel replies.

Coordinator cost is not zero. The goal accounting counter rose from
**2,731,606** at this turn's start to **2,899,809** at the recorded observation:
**168,203** on that counter's basis, not a final turn total or a provider API
usage breakdown. The transcript-bound output meter subsequently reports
**704,655 cumulative session output tokens**, with its cursor at byte
35,774,404 of 35,774,436. That is a different scope and metric; the figures are
not added together. Native-run provider counts exclude this coordinator.
The work therefore does not establish minimal total rented spend.
At a later observation the goal counter reached **3,016,534**, a delta of
**284,928** from the same turn start, again not a final turn total. This is a
substantial coordination cost and remains part of the open efficiency gap.

The useful surprise is that fewer stages can still cost more reasoning, and a
shorter task reference can omit a requested action even while final checks
pass. The slower and incomplete results remain beside the improved one. That
friction led to a smaller reference which retains the requested-tool reminder,
then met the original task again. The exchange advances through that callable,
observed change.

## Verification and continuity

Policy and request preflight and native validation pass. Candidate compilation,
actual save/resume observation, controlled request comparison and release checks
pass. The native authoring guide completes. The verified direct-entry boundary
teaching completed learning round 18; promotions remain four and serving
generation remains 5.
This is the Llama learning lane, not a Qwen weight update. Assessment answers
and candidate wording remain excluded from training.
The compact-reference teaching is retained under session `direct-code-entry-v1`,
event `verified-goal-reference-boundaries-v1`; its worker was launched. Retention
and launch do not yet establish a completed update or changed serving behavior.

Panel: **0 orphans; 11/12 counsel lanes unobserved**, no standing hearth.
Overall quality, resonance, throughput and rental-minimum parity remain open.
