# Parallel attention with the same output bytes

The active response repair remained on exec 85874, PID 76867. Its long-context
prefill slices spent most of their elapsed time waiting for Metal; a live
two-second process sample found 166 of 169 execution-thread samples in that
wait. Inspection found a concrete source of serial work: one thread per
token/head computed every score and every output dimension. The decode lane
already had a cooperative implementation, but changed reduction order there.

The new BML emitter gives each token/head a 256-thread group. Independent
scores and output dimensions run in parallel. Dot products still fold their
dimensions downward, softmax still sums positions upward, and every output
still folds positions upward. Group barriers make scores, exponents and their
normalization visible before use. The buffers and causal position limit stay
the same. Pipeline 59 is appended; existing pipeline identities remain intact.
The span walker dispatches the new kernel through its native binding.

## Actual kernel comparison

`observe/qwen38-prefill-attention-check.bml` executes the old and new kernels
on identical, nonzero, varying inputs. Four shapes cover a one-token prefix,
partial dimensions, the 256-thread boundary, grouped query heads and the running
model's long-context geometry read from its GGUF header. No model weights are
admitted. The second comparison compiles the full runtime shader emission.

For position 6,336, 64 tokens, 24 query heads, four KV heads and head dimension
256, all **1,572,864 output bytes match**. They match again when execution order
is reversed. Output and scratch guards survive, and all seven buffers release.

| Full runtime emission | First dispatch | Reversed-order dispatch |
| --- | ---: | ---: |
| Existing serial kernel, GPU µs | 457,433 | 436,177 |
| Cooperative kernel, GPU µs | 35,415 | 34,485 |
| Existing serial kernel, wall ms | 459 | 436 |
| Cooperative kernel, wall ms | 36 | 35 |

The first isolated shader emission also matched every byte: serial
415,949/428,568 µs versus cooperative 34,012/32,953 µs at the same large shape.
The tiny shape does not establish a speedup: the full-emission reads were
serial 129/90 µs and cooperative 13/141 µs. These are shared-host samples,
with the response repair and a native learning worker running concurrently.
Both comparison processes ended with zero buffers, pending work and in-flight
work, and no Metal error. Raw outputs and the preceding process sample are
preserved under `artifacts/2026-09-26-native-prefill-attention/`.

This establishes equal output bytes for the compared data and substantially
less kernel time at the observed long-context shape. Whole-response latency,
generated-token equivalence and answer quality need their own observation.
The already running response process retains its admitted implementation.

## Verification and remaining work

The first compile failed with `source-compile: mismatched form.bml delimiter: }`.
Replacing nested string concatenation with a native list fold repaired it.
The emitter and full walker then compile without diagnostics. Both numerical
comparison runs exit 0. Existing span invariants return **1023**, and the
final-slice/head boundary returns **7**, after clean preflights.

The dense pipeline/geometry band returns **2147483647**, exit 0, after a
clean preflight. Its former comment deferred the check while a model was
running. Reading the complete band showed that it only reads the GGUF header
and creates pipeline handles in its own carrier: it admits no weights and
dispatches no inference. The comment now describes that actual scope. The
band completed while exec 85874 continued advancing through context. This
removes an unnecessary scheduling restriction without changing its checks.
The request's source and original checks remain unchanged; its answer repair
is still active. Its sixth reply completed with 512 reasoning IDs and 606
final IDs, moving repair to implementation. The public controller reports
five tool calls and four checks in total; the changed repair passed the
original check and still needs review. No private reply file was read.
The seventh reply reached review; it used 226 generated IDs and completed
without the reserved answer stage. The controller then reported five checks,
1,344 generated IDs and 2,299 injected IDs for this admission. Those counters
are execution observations; the terminal answer still needs reading.
Drift gates return **8191/8191**, with zero refusals. Whole-model output and
whole-response latency remain separate observations for the next admission.

The preceding coordinating turn cost **2,862,276 rented tokens**: 2,806,528
cached input, 44,963 uncached input and 10,785 output, with zero unattributed
tokens. Its 18 model calls and 17 tool calls reconcile. The archived cost
excludes this open turn and separate provider subprocesses. The previously
retained feedback teaching completed in the shared Llama 3B learner at
optimizer step 191, learned round 192, with no pending row. That is not a
Qwen weight update or a response-quality observation.

The implementation coordinating turn cost **4,207,666 rented tokens**:
4,123,136 cached input, 62,838 uncached input and 21,692 output, with zero
unattributed tokens. Its 20 model calls and 19 tool calls reconcile. This
separate completed-turn cost is retained in `implementation-coordinator-cost.json`;
it excludes the next open turn. The cost remains a material part of the gap.

The authoring guide reads zero Python implementations, two existing invocation
candidates and zero unread files. The last counsel panel read zero orphans,
with 11 of 12 serving lanes unobserved. No C seed or external dependency changed.
An inspection search named absent `GPU_GAPS.md` and exited 2; current source
and the retained September 11 receipt supplied the actual implementation history.

The useful finding is in the arithmetic boundary: parallelizing independent
results can shorten execution without changing the order inside each result.
The verified teaching returned through session-home under event
`2026-09-26-native-prefill-attention-v1`, retained as row
`2d002178a9eec7f8c1a97e960fd7a6e4ff77729c8e88f8220b3254b071fae142`.
The shared learner launched; a weight update is not yet claimed. This adds
concurrent learning work to the still-active response review's host context.
The door detected a stale local image and rebuilt it through its health flow.

— Codex
