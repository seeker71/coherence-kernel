# Native quote repair works; source selection still misses the full enquiry

Signed: Codex. Form now offers an explicit repair for a quotation reflowed
across Markdown blockquote lines. The original exact-source checker passes
after repair. The response experiment remains incomplete; source selection
is not enabled as a default response policy.

## Observed response failures and native repair

The original dialogue goal, documents and caller assertions remain unchanged.
Direct answering and source selection use the same compact `knowledge-query`
profile, 12,288 context positions and 768 output allowance. Fresh streams share
one Qwen admission. The rubric was frozen first; neither stage receives an
earlier answer or expected verdict.

The direct answer completes and passes the field assertions, but includes a
name placeholder and an unsupported assertion that the proposed action resolves
uncertainty. The selector completes with fenced JSON, which the existing JSON
reader declines. Removing the explicit fence exposes four quotations; one
reflows Markdown line breaks and markers and fails exact-source membership.
The batch releases without composing from those unverified excerpts.

`fcqe-blockquote-restore` adds an explicit single-level blockquote reading view
to the existing quotation helper. It recognizes a line-leading marker followed
by whitespace, with at most three preceding spaces, then maps a unique
normalized match back to the original byte offsets. Returned excerpts include
the original intervening markers and line breaks. Default quotation and JSON
transport behavior remain unchanged.

Native recovery records the fence removal separately, restores the one quote,
then repeats the original exact-source checker on all four passages. Three
quotes remain byte identical; the changed quote maps to source bytes
**651..773**, end exclusive. Raw output, hashes and transitions remain private.
Repair takes **46 ms**, with no model call.

The repaired-excerpt answer completes but confuses a missing language mapping
with the codebook anchor and retains its placeholder. Exact quotations did
not preserve enough context for correct interpretation.

A further rule was frozen before generation: when a selected source is a valid
JSON object or array, supply its complete original record; preserve the other
selected passages. This adds neighboring source fields, not a corrected answer.
The resulting answer distinguishes the anchor and missing mapping correctly,
but still has a placeholder and does not clearly apply revisability. This is
one observed correction, not full response parity or unseen transfer.

## Complete execution evidence

| Stage | Prompt IDs | Generated IDs | Elapsed ms |
| --- | ---: | ---: | ---: |
| Direct answer, all sources | 3,886 | 221 | 227,300 |
| Source selection, all sources | 3,959 | 221 | 193,551 |
| Composition after native quotation repair | 423 | 156 | 64,720 |
| Composition with complete selected JSON records | 584 | 188 | 74,202 |

The first two stages share one admission. Each later composition has a separate
admission, included in its timing. The native audit reconciles **3 admissions**,
**4 generations**, **8,852 prompt IDs**, **786 generated IDs**, every stream
transition and every model release. Total observed execution, including failed
and recovery attempts: **559,976 ms**. Provider subprocesses and training during
these comparisons: **0**. Source-selection cost prevents claiming a faster
complete response from the smaller composition prompt alone.

All three actual answers pass the original field assertions and word allowance;
their content still has the defects above. Human resonance remains unassessed.
Private evidence is in `.hearth/response-parity/source-selection-v1`: frozen
rubric, prompts, actual answers, repair transitions, receipts and native audit.

## Checks, instruments and learning

Fresh preflight is clean. The quotation band passes **262143**, exit **0**,
including Unicode byte offsets, CRLF, ambiguous matches, changed words, inline
greater-than signs, excessive indentation, joined words, empty input and byte
limits. Drift gates pass **8191**, freshness **31**, whitespace clean.

Native experience recall with `skill=review` examined **372** records and
returned **0**. Removing that filter found **279** related records and selected
**20** session observations, with no model call; none entered the evaluation.
A guessed read of `.hearth/response-parity/span-review-v1/run.bml` exited **1**
with `No such file or directory`; the actual `span-review-run.bml` was then
read. One documentation patch was rejected for a missing context line and
reapplied at the observed source location.

Native guide: Python implementations **0**, invocation candidates **2**,
unread **0**. Counsel: orphans **0**, **11/12** lanes unobserved, no standing
hearth. Glass first frame: `12:40:22.813Z #0 dt=0/50ms`, **3M nodes / 128K
cons**; its owned viewer was closed.

The preceding learner completed round **79**, pending **0**, promotions **4**,
serving generation unchanged at **5**. Verified quotation procedure was retained
as `source-blockquote-restoration-verified-v1`, session
`codex-native-response-parity-2026-09-17`; its worker launched after Qwen
released. Evaluation answers were excluded. That learner targets Llama 3.2 3B;
this retention is not a Qwen weight update or a claimed promotion.

The preceding completed coordinator turn
`01a0af3e-10d2-7072-84bf-4b51a4dad001` used **39** model calls:
**5,504,381** input tokens (**5,429,376** cached, **75,005** uncached),
**30,611** output (**18,252** reasoning included), and **25,484** explicitly
unattributed tokens. Reconciled total: **5,560,476**. This excludes the current
open turn and separate subprocesses. Native zero-provider counts do not erase
the substantial coordination cost.

The useful surprise was that exact evidence became misleading after selection
removed neighboring fields. Keeping that regression visible led to a concrete
source-context repair. Form carries a verified quotation repair; the wider
quality, resonance, throughput and rental-minimization objective remains open.
