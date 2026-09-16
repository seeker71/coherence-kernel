# Make an omitted requirement observable

Signed: Codex. Witnessed 2026-09-17 on `fkwu`.

The previous composition witness reached the local model but was largely
unused. This movement first tested the framing of that same request, then
addressed an explicit requirement missed by the existing report checks.

## Read-only framing attempt

The candidate bootstrap removed coding/editing instructions from read-only
review and supplied the review instruction once. The initial implementation
left it only inside escaped JSON and failed the existing resumed-instruction
boundary check. It was repaired by retaining the raw CURRENT REPLY instruction
and referring to it from the context object. The unchanged policy band then
returned 65535, request band 255, and review-execution band 33554431, exit 0.

The retained request bytes, documents, source queries and checks matched the
prior composition-grounded local run. Prompt size changed from 15788 to 14404
bytes. The candidate completed in **232431 ms**, generated **386 local token
IDs**, injected no further context, made no model-requested reads and released.
It passed the original source/report checks. The native pair audit recorded
**11632 ms less** than the prior 244063 ms run, and **23 more generated IDs**.
No provider process ran.

The answer still narrated an intended change in posture, did not use the
composition witness, omitted translation, and supplied no question. The
question is an explicit requirement of the original enquiry. The bootstrap
change was reverted; a single faster run with these omissions does not
establish the required quality and throughput improvement.

The original request also explicitly asks for a next action. A prospective
next-action field is therefore not, by itself, a failure. Assessment must
examine whether the answer completes the substantive enquiry and whether
its proposed action fits the evidence and existing authorization. This
correction to the assessment preserves the original task.

## Native field-level presence

The resident JSON query organ now supports `contains` for string input and
string arguments. It returns literal substring presence, including an empty
substring, and preserves query streams and variable binding. Non-string input
and arguments return explicit errors. Array/object containment is outside
this subset. No C code, provider, external command or runtime dependency is
needed to perform this operation.

For the original question requirement, a caller can append:

```json
{"tool":"jq","arguments":[".answer | contains(\"?\")"],"stdout":"true\n"}
```

This checks the answer field, so punctuation in another field cannot satisfy
it. A punctuation-only answer could satisfy it: relevance, insight, warmth
and whether the question advances the enquiry remain semantic assessments.
No semantic quality score is derived from this check.

The existing edge band now includes true/false field selection, empty and
Unicode substrings, bound and unbound variables, an empty result stream, and
type refusals. It returned **131071**, exit 0. The existing public tool band
and restored policy band each returned **65535**, exit 0. All had clean
preflight before their verdicts were read.

## Original prompt through the repair loop

The new local run retains the original goal, sources and assertions and adds
only the question-presence assertion. Its initial prompt is byte-identical
to the prior 15788-byte prompt, observed before admission. The private helper
also verifies that the new assertion refuses the retained answer without a
question. Any subsequent feedback comes from the actual native check through
the existing repair flow. Expected answer prose is never supplied.

The first submission failed the added check. At that transition the native
trace recorded one turn, one check, one repair, **363 generated IDs** and
**250 injected feedback IDs**. No read was requested. The model revised in
the same admitted context. The run completed in **337938 ms**, with **748
generated IDs**, **250 injected IDs**, **2 turns**, **2 check runs**, **1 repair**
and **0 provider calls**. Original and added checks passed, source documents
remained unchanged and model release was observed. The native audit measured
**93875 ms** and **385 generated IDs** above the original local run.

The final answer retained the earlier explanation and added a question asking
which fear-costume Urs could see. That asks for feedback already present in
the enquiry. Translation remains conflated with vocabulary enrichment, the
composition witness remains unused, and the next action still seeks fresh
authorization for work already offered. The punctuation omission is repaired;
the requested lifting question and broader response-quality improvement are
not established. This is a useful counterexample to treating a necessary
presence check as sufficient evidence of completion.

## Evidence and scope

Private roots under `.hearth/response-parity/generalization-v2/` are
`read-only-bootstrap-v1` and `question-presence-v1`. They retain their requests,
exact prompts, ownership and outcomes. The first includes a native comparison
and the actual answers. These are repeated development enquiries, not held-out
proof of parity. Assessment answers remain excluded from training.

Private experiment helpers initially failed preflight on extra closing
parentheses; these were repaired before model admission. A source search also
named nonexistent request and glass-file paths; their actual organs were
located through the file inventory. No failed check was treated as a pass.

Counsel panel: **orphans 0**, with **11/12 lanes unobserved** because no hearth
stands. The preceding learning worker completed candidate **51**, pending **0**;
serving remains generation **5**, promotions **4**. This Llama learner is
separate from the Qwen response model measured here.

Native guide: **0 Python implementations**, **2 existing invocation candidates**,
**0 unread**. At the closing measurement checkpoint the parent goal recorded
**8790758 tokens** and the transcript meter recorded **1810935 cumulative output
tokens**. These include coordinating work; the zero-provider counts describe
the two local experiments only. Minimal total rental and overall parity are
not established.

Drift gates returned **8191**, exit 0; whitespace checks passed. The share
reader reported `kind=declared` with no completed evidence row and withheld
the percentage. Verified procedural teaching is retained under
`2026-09-17-native-string-presence-procedure-v1`; its worker launched and
completion remains pending at this checkpoint. Its target contains execution
mechanics and observed limits, never the assessed answer.

The useful correction is to check a concrete omitted requirement at its
boundary. The surprising teaching is that the shorter prompt improved time
while leaving substantive omissions. The failure remains visible and the
framing change stays reverted.
