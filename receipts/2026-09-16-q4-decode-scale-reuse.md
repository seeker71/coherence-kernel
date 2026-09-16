# Q4's slower session exposed repeated scale decoding

Signed: Codex, 2026-09-16.

## Observation and decision

The registered Q4 model was substituted for Q8 in two retained native requests.
All other request fields were held. Provider assistance stayed disabled and
evaluation content stayed excluded from training. At the stop decision,
601,037 ms had elapsed and Q4 was still implementing the first task. The
successful Q8 pair had completed both tasks in 561,664 ms.

The last Q4 progress row reported 128 generated IDs, 145 injected IDs, one
tool, zero checks, and 596,744 ms. A correlated metadata-only control exchange
selected branch action 2. Only the owned trial process was terminated; its
terminal exit was 143. Answer quality and native graceful release remain
unmeasured. The review case never started. Q8 remains the selected model.

An OS sample found the carrier waiting for Metal. That observation did not
identify a particular kernel as the cause. Source inspection then found a
specific avoidable cost: Q4/Q6 decode used serial kernels that decode a block's
scales again for every weight. Form already carried a scale-hoisting kernel.

## Executing change and re-observation

`qwen38-decode-quant.bml` admits that existing kernel with exactly one
partition. It keeps the original descending row sum and writes directly into
the existing output buffer. No combine pass, temporary buffer, C seed change,
external runtime dependency, or relaxed numerical tolerance is introduced.
The two pipelines append after the existing slots. Q8 and prefill keep their
current dispatches.

The first isolated 257-row, 5,120-column probe matched bytes and padding:
five Q4 dispatches took 26 ms on the original path and 9 ms with scale reuse;
Q6 took 11 ms and 10 ms. These short measurements are not general speed ratios.

A whole-model Q4 comparison alternated original and changed dispatches over
eight steps from token ID 42, with independent recurrent/KV states and one
shared weight admission. **Every generated ID and all output-logit bytes
matched at all eight positions.** Original steps totaled **16,038 ms**;
changed steps totaled **7,326 ms**. Position zero includes a larger cold-path
cost on the original side; positions one through seven remained approximately
1,854–1,858 ms versus 914–922 ms. The complete comparison released every
allocated buffer. It is a numerical and timing witness, not an answer-quality
assessment or a completed replay of the interrupted session.

The existing quantization band now also checks actual decode against the
original serial kernels at 3 × 512 and 257 × 5,120, with both fixture blocks,
output sentinels, and release counts. Prefill checks retain their byte or
previously named-bound requirements. Native validation returns 31, exit 0.
The dense pipeline band returns its pin, 2,147,483,647, exit 0; its stale count
of 55 now accounts for the existing Q8 slots 55/56 and new Q4/Q6 slots 57/58.
Both changed boundaries passed preflight.

## Failed attempts remain visible

The first request-comparison helper rejected the model-only substitution
because JSON pair ordering changed. Comparing the objects with only `model`
removed repaired the observation; all other fields still had to match.

The first isolated probe exited 1 with
`fkwu: form_error: hoist probe dispatch/read failed`. Its diagnostic showed a
valid pipeline and buffers but enqueue result 0. The helper had omitted
`metal_batch_concurrent`. Adding the required batch entry made the existing
dispatch/read and exact-equality checks pass. Neither failure weakened a check.

## Evidence, cost and remaining distance

Private evidence lives under `.hearth/response-parity/`: `q4-pair-*` retains
the frozen manifest, partial run and stop decision; `q4-hoist-probe.*` and
`q4-hoist-model.*` retain the numerical/timing observations; the preflight and
validation logs retain their exits. No assessment answer was used as a teaching.

The lane panel still reports **11/12 lanes unobserved**, with no standing
hearth. The share reader withholds a percentage. The goal counter moved from
3,344,132 to 3,471,467 at the recorded intermediate observation: **127,335**
additional coordinator tokens, computed by native Form. This is not final-turn
usage or a provider API breakdown. The local probes called no provider; overall
minimal rented spend remains unproven and this coordinator cost is substantial.
The transcript meter separately records 821,999 cumulative output tokens,
decided through byte 41,600,535 of 41,600,567. These scopes are not added.

The verified implementation result was retained for native learning as session
`q4-decode-scale-reuse-v1`, event `verified-one-partition-hoist-v1`. At this
observation the worker is running with one pending item; completed learning
remains at round 21 and the serving generation remains 5. Retention is observed;
an updated adapter or response-quality gain from this teaching is not yet observed.

The useful surprise was that an exact improvement was already expressible by
an existing native kernel. The disappointing Q4 trial became an observed repair
when attention moved to its dispatch. This movement kept the exchange alive by
turning a failed resource choice into a checked change. Whole-session quality,
resonance, and parity with the context-equipped provider remain open.
