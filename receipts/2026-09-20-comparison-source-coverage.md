# Restore the source before judging the answer

Codex, 2026-09-20. Rebased `codex/native-arrival-bootstrap` onto `origin/main`;
the branch was current. Continued from `a14c05944` with native preparation and
one Form-owned provider evaluation. The overall goal remains open.

## Observation and repair

The prior review packet started after the identity-seam passage even though
its excerpt referred back to that passage. It also omitted the candidate's
earlier grounding. The native reviewer called omitted historical material
fabricated, and both reviewed answers blurred the status of the seam repair.
The earlier source says the repair was named, not made.

`form-cli-comparison-review.bml` now prepares the question, candidate and
measurements unchanged, adds that original antecedent, and carries four exact
historical source sections with explicit scope. Those sections show what the
candidate had seen, including its old volume count and local-oracle material;
they do not independently establish every historical claim. Other original
sections remain outside this review. The exact prepared
[source](artifacts/2026-09-20-source-restored-provider-source.json) is retained.

Instructions and source JSON now travel separately. Native preparation measured
the old actual provider prompt at **16,894 bytes**, the normalized old content
at **15,788**, and the restored packet at **20,425**. Rewrapping removed **1,106
bytes**; the additional source context added **4,637**. This is a measured
representation change, not an observed token saving from a separate generation.
The new wrapper includes a document-location instruction as well.

## Re-observation

The [actual returned report](artifacts/2026-09-20-source-restored-provider-review.json)
and [answer](artifacts/2026-09-20-source-restored-provider-answer.md) are produced
by `codex exec` through native Form. Qwen was not invoked in this movement.
The provider used the same review task and candidate with changed grounding;
the earlier model answer and expected findings were not supplied.

Codex's reading of the answer: it now describes the identity repair as proposed,
keeps the original direct/guided definitions, qualifies guided coverage, and
separates the primary-model and all-reported-model direct totals. It does not
call the omitted historical material fabricated. It grounds its practical
choice in economy versus the observed instrumentation and artifacts, while
keeping semantic quality and felt resonance open.

There are still limits. Its description of 35,183 bytes as a different
historically grounded answer does not explain the precise cause: the living
receipt had accumulated later material. The original guided volume includes
measurement and implementation prose, so the 2.1-fold byte comparison does
not mean 2.1-fold answer value. Its sovereignty finding relies on an attributed
author's account, not a universal measurable ordering. Changed grounding and
one sample do not isolate why the answer changed or establish quality parity.
Human resonance feedback remains unmeasured.

## Costs and replay

The [result](artifacts/2026-09-20-source-restored-provider-result.json) records
one new provider process, one completed turn, successful release, message-only
events and passed source/JSON-shape checks. These checks are narrower than
answer correctness.

| Observation | Prior review | Restored source review |
| --- | ---: | ---: |
| Input tokens | 18,945 | 19,649 |
| Cached input | 10,624 | 10,624 |
| Uncached input | 8,321 | 9,025 |
| Output, reasoning included | 1,408 | 1,420 |
| Input plus output | 20,353 | 21,069 |
| Provider process ms | 47,495 | 44,867 |

The new native driver window was **45,203 ms**; preparation and compilation
are outside that window. The new answer cost **716 more tokens**. Native
arithmetic records **41,422 total provider tokens**, **17,346 uncached input**,
across the two distinct reviews. Earlier failed native attempts remain in the
[previous receipt](2026-09-20-original-reference-and-matched-review.md).

The old answer was rechecked in **372 ms**, with zero new processes and the
same usage event. The new answer also replayed with zero new processes. The
[audit](artifacts/2026-09-20-source-restored-provider-audit.json) retains that
evidence; replay readings are not additional provider usage.

The [latest completed coordinator reading](artifacts/2026-09-20-source-restored-provider-coordinator-cost.json)
is turn `01a0ba94-d365-7a71-ac88-058ead73e692`: **47 model calls**, **8,706,663
input tokens**, including **8,488,576 cached** and **218,087 uncached**,
**52,446 output** including **33,827 reasoning**, **0 unattributed**,
**8,759,109 total**. It excludes this open turn and separate provider processes.
Coordination remains the largest observed cost here; the small packet change
does not close that gap or prove a minimum.

## Checks and embodiment

The native source band returned **1**, exit 0. It checks the unchanged question,
candidate, route and output measurements; exact source roundtrip; preservation
of the old excerpt; restored unresolved-seam wording; selected historical
grounding; and admission with the original report schema. Preflight was clean;
drift gates returned **8191**, exit 0. Both provider results rechecked under
the retained manifest without new admission.
No runtime seed change or external language dependency was added.

Panel: **0 orphans**, **11 of 12 lanes unobserved**, no standing hearth.
The glass reached its first frame in **31 ms**. Its full live view kept rendering
with terminal input unavailable. Interrupting the watcher launched here caused
`form-run ./fkwu observe/form-glass-run.fk` to exit **1**, reporting
`[glass-supervisor, live-exit, 130]` and
`fkwu: form_error: Glass live runner failed; owned sensors released`.
That interruption is not a successful full-view check. Re-observation through
`form-run ./fkwu observe/form-glass-staged-startup-run.fk` returned exit **0**
with a **35 ms** first frame. Other existing watchers were left alone.
The native authoring guide reported **0 Python implementations, 2 execution
candidates, 0 unread**.

The procedural teaching `comparison-source-coverage-2026-09-20` was retained
for the existing session learner. The worker completed round **89**, pending
**0**, with serving generation **5** unchanged. It updates the existing Llama
adapter, separately from Qwen. Evaluated requests and generated answers are
excluded from training. This candidate update establishes no change in served
response quality.

The useful change is now in a native source preparation door. The next native
response attempt can consume this repaired evidence without asking a rented
coordinator to reconstruct it. Native-only quality, human resonance and
whole-session efficiency remain open.
