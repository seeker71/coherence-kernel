# Qwen's prefill reads the quantization it was given

The previous local autonomy attempt emitted 124 exclamation marks before Codex
cancelled it. Its model admission and token-flow receipts did not establish
valid arithmetic. This movement traced that failure to `q38-mv-batch-y`: every
prefill matrix was dispatched as Q8_0, including Q4_K and Q6_K weights. The
existing scalar decode path already selected those formats correctly.

The native BML dispatch now selects Q4_K, Q6_K, Q8_0 or F32 from the tensor's
GGUF type and retains its output pitch. It reuses Form's existing eight-token
batch fold and quantization decoders. Unsupported types refuse explicitly.
No C seed growth, Python, new model download or base-weight mutation was involved.
Codex supplied the diagnosis and implementation; the measured model execution
used local Form and Metal only.

The [bounded comparison](../observe/qwen38-prefill-quant-diagnostic.bml) used the
same Qwen3.8-27B-Q4_K_M weights and five public prompt tokens, with fresh recurrent
state for each route. Its diagnostic-only pipeline substitution reproduces the
old dispatch before selecting the typed route through a correlated framebuffer
control. The final comparison below ran after rebasing onto `a31a89c2` and
rebuilding its dynamic Metal carrier. [Current raw output](evidence/2026-09-10-qwen-native-prefill/q4-diagnostic-after-rebase.txt):

| Route | First token | Non-finite logits / 248,320 | Prefill wall time |
|---|---:|---:|---:|
| Scalar reference | 11751 | 0 | 18,026 ms |
| Previous Q8-only batch, reproduced | 0 | 248,320 | 885 ms |
| Typed batch | 11751 | 0 | 1,126 ms |

The typed batch was about 16 times faster than scalar prefill on this single
short probe. Model opening took another 24,886 ms. This is a shared-host wall-time
sample, not a hardware-floor measurement or a general quality score. All 993,280
logit bytes were read; token agreement does not claim full logit equality.
After release: zero live buffers, zero pending dispatches, no Metal error.

Artifact: `/Users/ursmuff/models/qwen38-27b/Qwen3.8-27B-Q4_K_M.gguf`,
16,810,716,128 bytes, existing seal SHA-256
`14dd37b54fb4ac3240dde91549a6ac20ed00cf2716d6c542553f40662ead55b3`.
The probe used the base model, no LoRA. Its first draft accidentally treated a
word-index reader as a byte-index reader; those partial counts were discarded.
The table is from the corrected complete scan and retained rerun.
The [initial comparison](evidence/2026-09-10-qwen-native-prefill/q4-diagnostic.txt)
also remains available: scalar 20,349 ms, typed batch 1,757 ms, with the same
token and non-finite counts. The rebase freshness gate refused the old binary
with 15; after the prescribed rebuild it returned 31. The new four-format
regression passed again, and the final carrier reported `metal_carrier=dynamic`,
55 source compilations, zero archive reads/writes, and zero live buffers.

The same previously unresolved `positive-boundary` case was then rerun through
`heal eval|native|qwen38-q4|positive-boundary`. Result: **repaired**, original
checker **31**, exit **0**. The base Qwen model consumed **766 input tokens**,
emitted **113 output tokens**, and ended at EOS without intervention. Its patch
changed `ge` to `gt` and retained the rest of the function. Retrieval, repair
memory, other models, remote inference and training were excluded from this
evaluation. This is **one targeted retest**, not a fresh general-autonomy score.
That complete repair evaluation preceded the upstream carrier rebuild; the
final full-model numerical comparison above verifies the rebuilt stack.

The [summary](evidence/2026-09-10-qwen-native-prefill/summary.json),
[candidate](evidence/2026-09-10-qwen-native-prefill/repair/eval-model.candidate),
[checker result](evidence/2026-09-10-qwen-native-prefill/repair/check-eval-model.out)
and [timing/choices](evidence/2026-09-10-qwen-native-prefill/repair/timing-report.md)
are retained with raw events and the original public prompt. The case worker
took **790,966 ms**, including its isolated snapshot. The model process took
**553,882 ms**: admission/state **32,408 ms**, prefill **137,275 ms**, then
decode/release **384,199 ms**, measured at parent event boundaries. The direct
checker process took **127 ms**. Token capture retained all **115 events**
(admission, 113 decode events, EOS); timing reconciliation is **1**. Model
supervision used dynamic progress, budget **0**, with normal process cleanup.
These timings leave substantial performance work; no hardware floor is claimed.

The new Metal regression compares every output byte against independent
per-token lane dispatches for all four formats, across nine distinct inputs
(including the tile tail), with padded output rows. All padding retained its
sentinel. Verdict **31**. Existing embedding **31**, span invariants **1023**,
sliced-head boundary **7**, and dense pipeline/geometry **2147483647** also passed
after clean preflights. Drift gates: **8191/8191**, refused **0**.
Source/document whitespace review is clean. The raw `eval-model.request` retains
its final empty adapter field; the whole-evidence whitespace diagnostic flags
that required blank line. Raw protocol bytes have been preserved.

Panel: truthful first frame **59 ms**, kernel operations **33**. Counsel still
finds **11/12** serving lanes unobserved because no hearth stands. The share
reader reports `kind=declared`; no percentage is claimed.

The local RAG query `quantvoice Qwen prefill quantization` returned a miss. Its
current index still has eight rows, 11,196 bytes, and no repair receipt in its
source inventory. Local files and git history supplied this diagnosis's
references. The native RAG writer currently refuses mutation until its substrate
resolver can construct the complete REF/CTOR binding; this movement does not
claim that teaching has become searchable in that index.

After model execution and the checker finished, the verified diagnostic method
was returned through `form-cli-session-home-embody-run.fk`. The native session
learner completed one real LoRA round, optimizer **3 → 4**, and promoted generation
`4-51391-280730080`. The worker completed with **pending 0** and **retained
buffers 0**. Training/assessment wall time was **159,838 ms**. The
[learning result](evidence/2026-09-10-qwen-native-prefill/learning/result.json)
records all four before/after losses; the method row changed
**5.16089081 → 5.12889412**. The repair examples were excluded. This is the
Llama 3.2 3B session dialogue adapter; it was not used by the base Qwen evaluation
and is not credited with that repair. Generation quality after this LoRA update
has not been retested.

The surprising teaching was that the decoder needed for this repair was already
local. The missing connection, rather than missing model knowledge, made every
logit non-finite. The uncomfortable repeat-only output became a useful numerical
comparison instead of another longer generation attempt. This extends the
`quantvoice` observation in the September 1 kitgate receipt, corpus row 1216.

Signed: Codex. The exchange stayed productive by turning a failed local answer
into a reproducible native repair and keeping the remaining quality question
separate from arithmetic correctness.
