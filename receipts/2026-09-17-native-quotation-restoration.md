# Restore source quotations natively and keep answer quality visible

Signed: Codex. Observed 2026-09-17.

## Capability carried into the body

`form-cli-quote-evidence.bml` provides the pure
`fcqe-restore(source, quote, maxBytes)` operation. It returns availability,
the exact source excerpt, start and exclusive end byte offsets, and the reason.
An exact quotation remains unchanged. When only ASCII whitespace differs,
a unique normalized match restores the original source bytes. Changed text,
multiple normalized matches, empty quotations and caller size limits produce
no candidate. It never edits a source or changes a word to make a claim fit.

The existing caller-owned native repair callback can propose the restored
report. The controller then reruns the complete unchanged checker. The new
18-case band witnesses that boundary: exact source repair completes in the
same submitted turn after two checks, while changed words remain in repair.
This is an available primitive, not a new default report schema or automatic
editing policy. `docs/form-native-coding.md` describes its contract.

## Two actual dialogue trials

First, a native source projection exposed every JSON literal path and explicit
statement line in the supplied documents. It kept zero, false, null, empty
containers and nested values distinct, with source hashes. Three of the eight
documents had these forms. The derived observation was added alongside all
original documents; no expected answer was supplied.

| Native dialogue | Elapsed ms | Generated IDs | Injected IDs | Provider calls |
| --- | ---: | ---: | ---: | ---: |
| Prior complete-response guidance | 360563 | 561 | 0 | 0 |
| Added native literal observations | 437586 | 558 | 0 | 0 |
| Model source notes with strict checks | 951602 | 2588 | 1156 | 0 |

The literal trial now names composition-based identity and retains the concrete
anchor. It overstates when old shapes persist, omits the short-anchor versus
full-content-identity distinction and still does not explicitly name the absent
German mapping. The added context costs time. This projection remains private
and experimental; it has not earned a default change.

Its effective request passed all checks and released the model. The unextended
request correctly rejects the additional document path. A separate native audit
confirms that every original document and every other request field is intact,
and the added document exactly matches the native source projection. These
observations are retained separately, without renaming the extended request
as byte-identical to the original.

The second trial used only the original documents. Its goal preserved the
original enquiry and added a workflow: produce source notes before the answer,
with at least five substantive exact quotations. A caller-owned checker verified
each quotation's membership in its named source, alongside the original checks.
It does not infer topic coverage or semantic entailment from quote membership.

The first submission had six notes. Three changed source line wrapping; three
were already exact. The unchanged strict check rejected the submission. The
local model made six source reads and eventually returned six valid quotations.
The completed run passed the original and additional checks and released Qwen.

The native identity audit found that **the final answer and every other non-note
field were identical to the first submission**. The prose now names the absent
German mapping and anchor, but still does not explain composition identity or
the anchor's narrower identity scope. Its proposed next action is a codebook
edit beyond the original explanatory enquiry, without supplying a grounded
translation value. Valid quotations have not resolved the whole answer.

## Native repair of the same rejected submission

The new primitive was applied to the retained first submission. It restored
the three whitespace-only quotations, kept the other three unchanged, retained
source hashes and before/after hashes and offsets, and passed the **same strict
checker**. All other report fields stayed identical. This replay took **118 ms**
with zero model or provider calls. The public band additionally verifies use
through the existing native repair boundary.

This is a measured replay of a rejected report, not a measured replacement
end-to-end session. It demonstrates an avoidable generative formatting task;
it does not assign the entire 951602 ms session cost to that task or establish
prose quality. No assessment response is used as a training target.

## Verification, cost and continuity

Preflights were clean. Source projection checks passed **127**; strict-note
counterexamples passed **63**; the public quotation band passed **262143**
(18 checks), including Unicode offsets, ambiguity, changed words, punctuation,
empty input, caller bounds and controller rechecking. Executions exited 0.
Diff checking passed; drift panel read **8191/8191**. Counsel still reports
**11/12 lanes unobserved** with no standing hearth. Native authoring guide was
read at arrival and closing. No C seed or external dependency was added.

The preceding teaching completed learner round **36**, pending **0**, with
serving generation **5** and **4** promotions unchanged. The new verified
mechanics teaching is retained as `2026-09-17-native-source-quote-restoration-v1`;
its worker was launched after Qwen released. This learner trains the Llama
adapter, not the assessed Qwen. Retention is not a serving-quality claim.

The coordinator goal meter rose from **6026220** to **6199087** at the
post-implementation reading: **172867 tokens**, excluding closing work. This
rented development cost is separate from the native trials' zero provider
calls. The retained provider baseline was reused; no new baseline was called.
Minimum expenditure and overall quality, resonance and throughput parity remain
open. Human resonance judgment remains pending.

Private evidence is under `.hearth/response-parity/source-literal-dialogue/`,
`source-notes-dialogue/`, `source-notes-native-repair-receipt.json` and
`source-notes-answer-identity.json`, with the corresponding native drivers.
The first rejected reply is retained at
`.hearth/code-memory/replies/24960-1789578322388.txt`.

The surprising teaching was that the long model repair left its answer exactly
unchanged. The useful movement through difficulty was making that distinction
executable: source-byte restoration became native work, while the unresolved
meaning and action stayed visible. The bootstrap's competing brevity guidance
and escaped-JSON document layout were also observed; their effects have not
been measured or changed in this movement.
