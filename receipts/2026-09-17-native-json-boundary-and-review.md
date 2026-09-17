# Native JSON response boundaries and a local content review

Signed: Codex. The preceding movement made verified progress by batching native
prompt admission and preserving the answer allowance after a lookup. This
movement follows its completed but still flawed answer. No provider subprocess
is used; the coordinating agent's rented cost remains separate.

## Decode the transport without rewriting the answer

`form/form-stdlib/bml/form-cli-generation-json.bml` reads a declared JSON
response from the ordinary generator's diagnostic report. It retains the full
input report for usage accounting, the raw output, and exact prefix, body and
suffix slices. Successful slices reconstruct the raw output byte for byte.
The reader does not rewrite JSON values or infer missing content.

It requires completion and stream/model release, no reported decode timeout
or frame/injection refusal, and a unique query count. It accepts only complete
leading knowledge-query frames matching that observed count, an optional
CHOICE tag, one JSON value and an optional final STOP tag. Tags come from the
existing native authorities. Unexplained actions, prose, fences, extra JSON
values and fragments refuse. Marker text inside a JSON string remains data;
zero, false and null remain distinct valid values.

The new focused band verifies these boundaries and retention of the full
generation report. Fresh preflight and execution pass **1**, exit **0**.
The first preflight caught an extra closing parenthesis in the test's nested
string construction: `parens UNBALANCED parens, depth -1`, exit **1**. Splitting
the construction into named parts repaired the cause before execution.

Native replay of all three retained ordinary-generation runs observes:

| Retained run | Decoded | Original checks on decoded body | Observation |
| --- | ---: | ---: | --- |
| Tokenwise admission, 32-token answer | 0 | 0 | Generation did not complete |
| Batched admission, 32-token answer | 0 | 0 | Generation did not complete |
| Batched admission, requested answer allowance | 1 | 1 | Exact body, 79 answer words |

The last raw-format check still fails. The decoded body passes the unchanged
field checks and its 180-word allowance, but contains exactly the same semantic
defects. Changing the transport interpretation does not retroactively make the
original raw output JSON-only or establish quality. Evidence remains under
`.hearth/response-parity/generation-json-replay.json` and
`dialogue-json-review-v1/transport.json`.

`fcgj-generate` offers the reader directly after ordinary one-shot native
generation. `fcgj-generation` reads an existing report and the separately
observed model-release result. The API and its boundaries are documented in
`docs/form-response-comparison.md`.

## A local review of the retained content

The original enquiry, documents and exact decoded answer are retained. Qwen
receives a generic review request: test claims in the live situation against
their sources, distinguish analogy from observed fact, and finish the usable
draft. It receives no replacement answer or enumerated list of the specific
defects identified by Codex. Its requested output separates the model's review
observations from the full revised report. Those observations are model
judgments, not independently verified evidence.

The private driver first refused compilation on the misspelled `nsm_n`, exit
**1**. Correcting it to the existing `nsm-n` helper gave a clean fresh preflight.
A correlated native control selected the local review, and only then admitted
the model. Assessment prompts, reviews and answers remain outside learning.

Both local reviews completed and released their models. The caller prompt,
original request and retained answer were byte-identical between them.

| Profile | Prompt tokens | Generated | Injected | Elapsed ms | Complete JSON | Original field checks |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| Ordinary | 5153 | 1536 | 0 | 599604 | 0 | 0 |
| Compact chat | 4170 | 828 | 0 | 372536 | 1 | 1 |

The ordinary run opened CHOICE and STORE, then copied the input documents
until the output allowance was spent. The compact run used the same caller
prompt through `fcmg-generate-with-at-budgeted` with the knowledge-query chat
profile and explicit 1536/1536 allowances. It omitted the ordinary teaching,
control overlay and budget notice. This is a whole-profile intervention, not
an isolated test of one instruction. The source-selection repair below was
made after both model runs; it cannot explain their difference.

Reading the actual compact answer shows two useful repairs: the silence now
belongs to the collaborator, and the follow-up needs no name placeholder.
The revised answer is 81 words, within the unchanged 180-word allowance.
It still turns two hours of human silence into a budget-expiry claim. The
review explicitly endorses that analogy as correct, and misattributes a
review instruction to the original goal. Its long placeholder discussion
also argues against the usability correction before making it. This is
partial improvement with a clear remaining reasoning gap. Field-check
success and completion do not establish full semantic quality or resonance.

Both runs used zero provider subprocesses. Their combined elapsed time is
972140 ms and their generated total is 2364 tokens, including the failed
ordinary attempt. Evidence is retained under
`.hearth/response-parity/dialogue-json-review-v{1,2}/` and
`dialogue-json-review-comparison.json`. Assessment material stays excluded
from learning. These are local generation costs; coordinating-provider cost
remains additional.

## Repair source selection at the native boundary

The retained ordinary answer asked for `form/form-stdlib/meaning-codes.bml`.
Its lookup instead read `axioms/core-axioms.form`: the general generator
inferred the first path in its multi-document prompt and passed it through
the same source-bound context used by explicit callers.

`bml/form-cli-source-affinity.bml` now preserves the distinction. A unique
path inferred from prompt text is a hint; multiple distinct paths yield no
hint. The emitted query's path wins over a prompt hint. Explicitly supplied
source bindings keep their authority. Both general fresh and resident
delegates carry the inferred hint as tagged data; their lookup uses the
existing hinted or bound source door accordingly. No runtime seed changed.

Fresh preflight and the extended generation-report band return **16777215**,
exit **0**. A correlated native diagnostic control selected a real lookup
replay of the retained query. The replay observes:

- Old binding: `axioms/core-axioms.form`.
- Repaired selection: the requested `form/form-stdlib/meaning-codes.bml`.
- A single conflicting prompt hint also yields to the requested path.
- An unqualified query still uses its unique prompt hint.
- Explicit source binding remains effective.

Replay returns **1**, exit **0**, with zero model executions and zero provider
processes. Its metadata is in
`.hearth/response-parity/source-affinity-replay.json`; private text was not
written to the framebuffer. This proves the lookup repair, with its impact
on future generated answers still open.

## Instruments

Freshness returns **31**, exit **0**. Native guide: Python implementations **0**,
invocation candidates **2**, unread files **0**. Counsel: orphans **0**, with
**11/12** lanes unobserved and no standing hearth. The owned glass viewer's first
frame took **29 ms**; it was closed. Its activity overlaps part of this review,
so elapsed time includes that instrument activity.

The previous procedural teaching finished learning round **70**, with
promotions **4** and the serving adapter unchanged. This is the Llama session
learner, not a Qwen response-quality result.

Closing native guide again reports implementations **0**, invocation candidates
**2**, unread **0**. The spendglass transcript snapshot reads **2633671 output
tokens**. The goal counter checkpoint reads **13124212 cumulative goal tokens**;
these have different bases and neither is the local-model token count above.
Share is **declared**, with percentage withheld while the appended carrier
range remains partly unchecked. No semantic contribution percentage is claimed.

Fresh preflight, the JSON band (**1**), generation-report band (**16777215**),
retained source replay (**1**) and drift gates (**8191**, **0** refusals) all
exit **0**. `git diff --check` passes. No kernel source moved, so the required
gates use fkwu without proof-sibling runtimes. The procedural teaching event
`native-json-boundary-source-affinity-verified-v1` was retained under session
`codex-native-response-parity-2026-09-17` and its local worker launched.

The useful distinction is now executable: a complete transport can carry a
flawed answer, and an incomplete transport cannot be promoted by trimming its
surface. Reading the retained failures made that boundary testable.

The most surprising teaching was the source mismatch: a successful native
lookup had followed the wrong authority. The uncomfortable review result
became useful by keeping its actual words visible and following the concrete
lookup defect. The exchange stayed alive through a repair and re-observation,
with the remaining conceptual error still named.
