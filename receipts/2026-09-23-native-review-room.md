# A complete native review preserves two repaired distinctions

The shared-meaning review had ended mid-finding at its 1,536-ID final ceiling.
This movement returned the same request with initial, final and per-reply
allowances of 3,072. The [native comparison](artifacts/2026-09-23-native-review-room/delivery.json)
verified that every other request field stayed identical, including context
12,288, the enquiry, candidate, source packet, checks and evaluation exclusion.

## Actual answer and cost

Qwen 27B returned a [complete report](artifacts/2026-09-23-native-review-room/report.json)
and [answer](artifacts/2026-09-23-native-review-room/answer.txt). It used
**3,072 IDs in the initial stage and 1,773 in the final stage: 4,845 total**.
The controller supplied 100 observation-prefill IDs between stages. There was
one completed reply, one combined checking step, zero tool calls, zero
provider calls and successful resource release. The checks establish their
source and report-shape assertions; they do not grade the report's meaning.

The [native call](artifacts/2026-09-23-native-review-room/summary.json) took
**1,394,002 ms** (23.2 minutes), after **470,242 ms** (7.8 minutes) waiting for
the learner. That worker completed [round 134](artifacts/2026-09-23-native-review-room/learning-round-134.jsonl)
without promotion; serving generation 5 remained. Native waiting and initial
generation are part of the cost even though the final stage supplied the report.

Reading the actual returned words against the earlier native answers gives:

| Distinction | Earlier completed review | Teaching delivered, short allowance | Current complete review |
| --- | --- | --- | --- |
| Referenced retention and unreferenced reclamation | Unconditional retention claimed | Correct condition then contradictory claim | Condition and reclamation preserved |
| Language surfaces and semantic senses | Labels called alternate senses | Both descriptions used | Language surfaces distinguished |
| Numeric frequency inputs | Supplied rows described | Supplied rows described | Supplied rows and caller annotation described |

This is Codex's source-based comparison. The current answer improves the two
target distinctions. It establishes neither lasting uptake nor whole-session
quality, frequency/resonance or throughput parity. The earlier completed native
review took 809,467 ms and 1,746 IDs. The current answer is slower and uses more
local generation. The assisted review's 67,745 ms and 21,301 rented tokens
describe a different execution system; those windows exclude coordination.

## The remaining finding and its repair

Finding `f1` still misattributes a claim. The
[original candidate](artifacts/2026-09-23-direct-answer-template/hearth-turn-7.final.txt)
describes first-class concepts, then explicitly states the three states as
`0, 1, and nothing`. The finding adds “as the three states” to its description
of the preceding phrase. That added interpretation is not the candidate's
assertion. More generation room did not resolve this review error.

The shared native review guidance now asks for findings grounded in actual
assertions, neighboring-sentence context, a distinction between exact wording
and interpretation, and consistency between recommended corrections and the
final answer. Its existing policy preflight was clean and band returned
**65,535**.

The [focused native request](artifacts/2026-09-23-native-review-room/finding-context-request.json)
returns the complete candidate, actual `f1` and the states-axiom excerpt for
assessment of that finding. It asks whether to retain, revise or remove it;
no expected decision is supplied. It uses the retained
[review guidance](artifacts/2026-09-23-native-review-room/review-guidance.txt),
keeps evaluation exclusion, and waits for learning before admission.

Qwen's [actual correction](artifacts/2026-09-23-native-review-room/finding-context-report.json)
returned `decision: remove`. It quoted the two neighboring sentences and
correctly distinguished the thematic description from the formal state names.
The original full report remains unchanged above, including its false finding.
This resolves the disputed finding in the focused re-observation. Both the
review guidance and task context changed; the result does not isolate the
guidance's effect or establish that a future full review will avoid the error.

The [focused call](artifacts/2026-09-23-native-review-room/finding-context-summary.json)
took **384,894 ms** (6.4 minutes), after **288,464 ms** (4.8 minutes) waiting
for learning. It generated **1,357 IDs**: 1,024 in the initial stage and 333
in the final stage, with 100 controller observation-prefill IDs. One reply
completed, one checking step passed, zero tools and zero provider calls ran,
and resource release succeeded. The answer's source comparison supports the
specific correction; the passing checks establish their narrower assertions.

The [enrichment heading repair](2026-09-23-enrich-language-surfaces.md) landed
separately. Its later wording was not in the running review's frozen packet.
Verified formatter and claim-context teachings completed through learning
round 136, with zero pending rows and no new promotion. Serving generation 5
remained. That learner uses Llama 3B; this does not establish a change to the
reviewing Qwen 27B weights. The evaluated model answer was not used as a
correct training target.

## Accounting and observation boundary

The [preceding completed coordinator turn](artifacts/2026-09-23-native-review-room/preceding-coordinator-cost.json)
used **6,464,807 rented tokens**, including **6,276,992 cached input tokens**,
over 49 model calls. That is turn `01a0ca43-e883-7881-94c6-37ae8d335163`;
the current open turn and separate provider subprocesses are excluded.
This cost remains substantial and is not hidden behind the native call's
zero-provider count.

The share door returned `kind=declared`, with the percentage withheld because
the appended carrier range was not fully checked. It does not measure semantic
contribution.

Glass first frame was **29 ms**. The native guide reported zero Python
implementations, two existing invocation candidates and zero unread files.
The drift gates returned **8,191 / 8,191**, zero refusals, exit 0; no kernel
source moved.
The completed answer was offered for Urs's assessment of grounding, warmth
and useful direction. That felt assessment has not been inferred from a
successful process, the source corrections or these counters.

— Codex
