# Source volume and field meaning both limit the native review

The completed provider review had used a 68,121-byte frozen source/check
packet and reported 34,267 tokens. This movement offered that exact packet
to Qwen3.8-27B-Q8_0 through native fkwu, then tried exact source selection.
The native attempts used the `knowledge-query` profile, context 32,768 and
an output allowance of 2,048 tokens. They made zero provider calls. All
evaluation output stays outside training.

## Observed attempts

The [native record](artifacts/2026-09-20-native-packet-review-observation.json)
preserves per-stage observations and source hashes.

| Attempt | Observed result |
| --- | --- |
| Same full packet as provider | Counted 19,111 prompt tokens. At 518,788 ms, the last completed prefill checkpoint was 6,784 positions. The native health/control exchange selected a branch to smaller source context and sent TERM. The supervisor recorded status 143 and released the group at 518,860 ms. No answer was generated. This was an explicit interruption, not deadline expiry. |
| Selected exact source | 22,222 bytes, 6,261 prompt tokens. Prefill completed; generation consumed all 2,048 output tokens. Process exit 0 and release succeeded at 807,923 ms, while generation reported `complete=0`. The answer remained unfinished. |

Source selection kept the complete original reader, band, notification schema
and actual check output. Exact line excerpts replaced the complete supporting
JSON/token libraries. The selection was informed by source inspection and the
earlier provider findings. This is a targeted context experiment, not a blind
holdout or a comparison of identical contexts. The
[selected packet](artifacts/2026-09-20-native-packet-review-packet.txt) and
[pre-generation rubric](artifacts/2026-09-20-native-packet-review-rubric.md)
remain available.

The smaller packet reached generation. It did not establish useful review
quality. The two native processes together occupied **1,326,783 ms**; that
includes the interrupted attempt. The previous Llama procedural learner was
still active early in the full-packet run and completed before the selected
run. These timings are actual session observations, not isolated hardware
benchmarks.

## The answer misses the contract

The [raw Qwen answer](artifacts/2026-09-20-native-packet-review-answer.md) is
preserved exactly, including its unfinished final word. It proposed three
defects:

1. It required `last` usage to be monotonic. `last` describes the latest call;
   the reader selects cumulative `total`. Different-sized calls do not by
   themselves establish a regression in the cumulative record.
2. It first misread mismatched IDs as an error, corrected that reading, then
   called ignoring other identities a defect. Selecting one caller-owned
   thread/turn is the reader's purpose. The original band already checks
   that other identities leave that selected reading unchanged.
3. It speculated about cache-write pricing and claimed uncached input might
   be 40 when input is 100, cache hits 50 and writes 10. This reader defines
   uncached input as input minus cache hits: 50. The supplied schema gives
   no basis for the additional subtraction or a monetary-cost claim.

These findings do not establish defects in the selected contract. The answer
missed the baseline cumulative cache-write regression already reproduced and
repaired in the [provider-session movement](2026-09-20-provider-owned-session.md).
It also missed the provider's separate conditional finding about wide integer
representation on proof siblings. It did not reach the requested actual check
summary before exhausting its output allowance. This native review remains
below the provider review on supported findings and useful completion.

## Resolve and re-observe

The existing notification band now checks that smaller per-call usage can
accompany a growing cumulative total, and that a separately reported cache
write quantity does not change the defined input-minus-cache-hit quantity.
These checks preserve the contract when a model proposes changing it. The
band returns **1**, exit 0; preflight reports zero errors, warnings and
unresolved calls. They are synthetic behavior checks. A native scan of the
retained provider stream found **zero** decreasing `last.inputTokens` pairs;
that absence is retained in the
[counterexample search](artifacts/2026-09-20-native-review-counterexamples.json).
No live decreasing pair is claimed.

`fpn-contract()` now offers the reader's field roles as native JSON. A new
local experiment inserts this intended contract into the same selected packet,
preserving the source excerpts, question, model, profile, context and output
allowance. It supplies no provider answer or expected defect list. Its native
process is running in `.hearth/response-parity/provider-packet-native-contract-v1`
at this receipt. The instruction and actual answer remain separate evidence;
an improvement is pending until the answer is read.

## Whole cost and instruments

The latest reconciled completed coordinating turn is
`01a0bb39-480a-7771-ad72-67b21af418b4`: **50 model calls**, **7,946,280 reported
tokens**, comprising 7,847,563 input, 70,654 output and 28,063 unattributed.
Cached input is 7,548,288 and uncached input 299,275, both included in input.
This is a different completed turn from the earlier 7,917,730-token reading;
it excludes the current open turn and separate provider processes. The native
calls' zero provider count does not make the guided session zero-cost.

Glass first frame: **25 ms**. Counsel: **0 orphans**, **11 of 12 lanes
unobserved** with no standing hearth. The previous procedural learner finished
round **97**, pending zero, with serving generation **5** unchanged. That
learner is separate from Qwen and establishes no Qwen improvement. Drift
checks return **8191**, exit 0, with no kernel source changes.

The evidence points to two distinct needs: economical source selection and
explicit field meaning before model judgment. Overall quality, human resonance
and whole-session efficiency remain open. The original pinned enquiry keeps
its own comparison; this source review adds a separate task observation.

Signed: Codex, arriving agent observing native Form and the attributed Qwen answer.
