# Pin the original answer; compare actual review outcomes

Codex, 2026-09-20. Continued from `885d14b68`. The preceding turn was
progress: it repaired auxiliary-model accounting and retained a complete
native answer whose conclusion still overreached. This movement repaired
the comparison reference and observed native and provider review on the same
supplied task packet. The overall goal remains open.

## The reference had grown beyond the measured answer

The guided comparison read a living receipt that had accumulated later
experiments. Its 35,183 bytes and two dated-receipt citations were being
presented as the first guided answer's structure and volume.

The [original guided answer](artifacts/2026-09-18-guided-first-answer.md)
is recovered byte for byte from commit `1736af3c8`. `git rev-parse` on that
commit's path and `git hash-object` on the retained artifact both returned
`ee9f9b9c0bb6d46f562aa5e14d12a3732c4cf5fc`. Its native SHA-256 is
`2fd9864ac6ebb10326b387ba835619f49e66b8c7d0b5a931919446bef5355ac0`.

The native comparison and contest now share that original reference.
`fnc-guided-text` checks its hash before admission; absence or changed content
stops comparison. Output-volume rows distinguish readable empty artifacts
from absent or unreadable ones. They measure original artifact bytes rather
than substituting rented-token counts for output size.

The [native reading](artifacts/2026-09-20-original-output-measures.json) gives:

| Original artifact | Bytes | Scope |
| --- | ---: | --- |
| Direct | 5,837 | Returned answer text |
| Bounded | 5,561 | Returned answer text |
| Guided | 12,520 | First guided receipt, including measurement and implementation account |

The guided structural reading is **9 axis labels, 0 dated receipt citations,
1 limit marker**. The citation measure is the existing dated-`.md` path
scanner, with its narrow coverage. This corrects the measured reference;
it does not establish semantic quality or relax the comparison predicate.
The qualitative reading remains pending in newly emitted comparison rows.

## Native review: complete, but worse overall

The [native review packet](artifacts/2026-09-20-native-review-packet.json)
contains the original enquiry, the previous complete candidate, refreshed
all-reported-model usage, original output sizes, native concept definitions,
and an attributed excerpt of the original author's observations. It requests
up to three findings and a complete revised answer. Expected findings and
replacement answers were kept outside the prompt; the rubric was frozen.

This is a changed context, with additional evidence, a review task and a
2,048-token allowance. It does not isolate the effect of review alone.
Qwen used the existing compact `knowledge-query` profile and 12,288 positions.
The [raw review](artifacts/2026-09-20-native-review-raw.json),
[extracted answer](artifacts/2026-09-20-native-reviewed-answer.md), and
[generation result](artifacts/2026-09-20-native-review-result.json) are retained.

It completed, decoded as JSON, and released the model: **3,537 prompt IDs,
1,724 generated IDs, 502,294 ms, zero provider calls**. Its useful correction
was the output-byte comparison. Its findings also called correct route counts
incorrect and then repeated those same counts as evidence. Its answer claimed
the identity seam was repaired, ranked guided least sovereign and direct
intermediate, and treated guided as necessary for complex work. Those claims
were not established. It omitted the supplied broader token totals and their
coverage, cited no actual paths in the revised answer, and amplified qualitative
rankings. This review remains a failed quality attempt and is not promoted.

The packet itself has a coverage limit: it did not carry every source from the
old candidate's original grounding. The 13,222-token and local-oracle material
was absent from this review's supplied subset. Absence there does not establish
fabrication in the original candidate. The review's stronger verdict crossed
that boundary. The selected excerpt also refers to a seam described earlier in
the receipt; the full original says its repair was named, not made. Future
review grounding needs that antecedent, and a source omission stays unresolved
unless contradictory evidence is actually supplied.

The profile inspection confirmed closed reasoning. Earlier
[reasoning observations](2026-09-17-native-reasoning-profile-and-review.md) and
[reserved-answer observations](2026-09-17-native-reasoning-answer-reserve.md)
already showed that opening reasoning was not a reliable quality repair.
No unchanged reasoning experiment or default profile switch was added here.

## One Form-owned provider comparison

The existing synthesis organ received the **same domain packet**, verified
byte for byte by the native retention helper. The provider wrapper adds its
ordinary task-envelope instructions and native source-verification status;
the carriers are therefore different. The task documents include the old
candidate to review, but exclude Qwen's new review and expected findings.
The synthesis wording now accurately allows caller-supplied draft documents
while keeping expected report assertions local.

The provider was invoked once through `frsy-run` inside `fkwu`, in the existing
empty-directory process boundary. No provider tool execution was observed.
Source equality and JSON field-type checks passed; these are narrow checks.
The [actual provider report](artifacts/2026-09-20-form-owned-provider-review.json),
[answer](artifacts/2026-09-20-form-owned-provider-answer.md), and
[native result](artifacts/2026-09-20-form-owned-provider-result.json) retain
the separate answer source and usage event.

| Provider observation | Value |
| --- | ---: |
| Provider process time / native driver time, ms | 47,495 / 47,912 |
| Input tokens | 18,945 |
| Cached / uncached input | 10,624 / 8,321 |
| Output, including 239 reasoning tokens | 1,408 |
| Reported input plus output | 20,353 |
| Completed turns / new provider processes | 1 / 1 |
| Process released / message-only event check | 1 / 1 |

Codex's reading: the provider keeps the original direct/guided distinction,
uses the corrected output sizes, distinguishes primary-model figures from
all-reported-model totals, marks guided coverage partial, attributes authored
vitality observations, and avoids turning visibility into automatic
sovereignty. Its conclusion gives a useful conditional choice. These are
content observations, not an automatic quality score.

Remaining issues are visible. Its phrase about guided exposing and repairing
“that ambiguity” can refer to the route-mapping error; it should not be read
as proof that the session-identity seam was repaired. “Briefly” blurred
attribution supplies a duration the excerpt does not establish. Its first
finding does not fully explain the old two-citation count's changed reference.
Its out-of-scope finding is bounded to the supplied packet and does not prove
that the omitted old material was fabricated. Human depth, warmth and resonance
feedback has been requested and remains unmeasured.

## Full attempt costs and what changed next

The [native chain accounting](artifacts/2026-09-20-native-review-chain-cost.json)
includes the failed citation trial, focused contrast, sectioned draft and this
review: **4 native generations, 11,196 prompt IDs, 4,256 generated IDs,
1,418,660 ms** across their measured execution windows. All four used zero
provider calls. The provider comparison adds the distinct 20,353-token usage
event above. Compilation, source preparation, retention and coordination sit
outside those native generation windows; this is not a total wall-time claim.

The preceding completed coordinator turn
`01a0ba87-3d09-7753-afb9-ed711299dd55` used **25 model calls**, **2,616,724 input
tokens** (2,444,800 cached and 171,924 uncached), **24,362 output tokens**
(9,741 reasoning included), and **0 unattributed**: **2,641,086 total**.
The meter excludes this open turn and separate provider processes. Provider
assistance inside Form remains much smaller than that observed coordination
cost; this does not prove a global minimum or native-only parity.

The next response path can use the measured Form-owned provider resource where
its stronger reasoning earns the cost, while native response development
continues separately. The failed native review changes the local work toward
complete source coverage and checked claim relationships. Repeating a prose
review is not itself a correctness check.

## Checks, instruments and learning

The source band returned **1**, exit 0: exact reference hash, original sizes,
byte-size agreement, rejected appended/missing references, unobserved missing
or unreadable output, observed empty output, matching emitted reference and
pending qualitative reading. The existing synthesis boundary band passed
**21 cases**, exit 0. Native preflight was clean for both public paths and
the retained audits; drift checks returned **8191**, exit 0.

The native-review preflight first failed with unresolved `faj-valid`; loading
its existing JSON-query prelude repaired it before inference. The provider
retention preflight found one unmatched parenthesis; its repaired native
helper passed before copying the artifacts. No model call was repeated for
either repair. Native cache warnings rebuilt changed helpers and retained
their observed/applied events.

Panel: **0 orphans**, **11 of 12 lanes unobserved**, no standing hearth.
The authoring guide reported **0 Python implementations, 2 execution
candidates, 0 unread files**. C-bootstrap sufficiency is preserved.

The preceding procedural learner completed round **87**, pending **0**,
serving generation **5** unchanged. This movement retained
`immutable-comparison-reference-2026-09-20`; its learner subsequently completed
round **88**, pending **0**, with serving generation still **5**. That learner
targets the existing Llama adapter, separate from Qwen. No evaluated request
or answer was used as a training target, and no new served quality is claimed.
