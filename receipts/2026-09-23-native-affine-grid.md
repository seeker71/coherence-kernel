# Restore the long example's output projection

Codex, 2026-09-23. The gap is useful native learning and completed native
answers. This repair follows a numerical contradiction in the actual retained
example, while the separate Qwen answer-editing request continues.

## Observation

The original 5,485-token example completed forward and backward execution and
contributed to checkpoint 149. Its [training row](artifacts/2026-09-23-native-affine-grid/training-events.jsonl)
reported loss **11.761782428249717**; the same parent model's sliced
[assessment](artifacts/2026-09-23-native-affine-grid/learning-gradient.json)
reported **0.040271283876650146**. The other long row had the same discrepancy;
the 175-token row agreed exactly at **0.051170041334792415**. Execution and
checkpoint completion had not established correct long-row computation.

The [native reader](artifacts/2026-09-23-native-affine-grid/learning-head-coverage.bml)
isolated the actual model's output projection: 5,485 tokens, width 3,072 and
128,256 vocabulary outputs. One dispatch requested 703,484,160 cooperative
groups, or 22,511,493,120 threads. It projected embeddings of the original
token IDs and compared five output rows with independent one-row projections
using the same immutable weights. This isolates the projection; it does not
repeat the preceding transformer layers.

| Inspected row | Before: maximum absolute difference | After |
| --- | ---: | ---: |
| 0 | 0 | 0 |
| 1,023 | 1.307617425918579 | 0 |
| 2,047 | 1.0201318264007568 | 0 |
| 3,000 | 1.251286506652832 | 0 |
| 5,484 | 0.8104193210601807 | 0 |

Before repair, the four later inspected rows were all zero while their
independent references were nonzero. [Before](artifacts/2026-09-23-native-affine-grid/head-coverage-before.json)
and [after](artifacts/2026-09-23-native-affine-grid/head-coverage-after.json)
retain the observations. Both released every buffer. The exact host limit or
driver cause remains unestablished.

## Repair and re-observation

`native-affine-metal.bml` bounds each cooperative dispatch at 16,777,216 groups
and carries its global output offset into 64-bit shader indexing. All five
inspected positions now agree exactly. The observed run took 121,858 ms,
versus 24,565 ms before; the earlier operation omitted observed outputs and
both overlapped other GPU work. This establishes correctness at the inspected
positions, not a throughput improvement.

`native-lora-train.bml` independently projects the final normalized activation
before backpropagating a large output. A mismatch emits an organ-health
observation, selects abstention, releases the forward state and refuses the
gradient update. The check covers that final output row. It does not validate
all outputs or earlier activations. The existing original-example retry remains
active; its actual loss and guard event are still owed. No duplicate learner,
provider call, external runtime or C-seed change was introduced for this repair.

The [completed checkpoint observation](artifacts/2026-09-23-native-affine-grid/learning-checkpoint.json)
and updated assessment retain the adverse result: checkpoint 149 was **not
promoted**, held-out loss rose from 3.37489495575428 to 3.3768423721194267, and
serving generation 5 remained selected. Later candidate ancestry is not a
validated learning result merely because it descends from this checkpoint.

## Checks, failures and cost

Clean preflight and successful execution: affine file **15**, affine adjoint
**63**, resumed LoRA **7**, backward **7** with **28/28** finite-difference
checks. A deliberately zeroed finite final row is refused by the equality
check, then agrees after restoration. Resumed parameters and Adam moments
remain byte-identical; all checks release their buffers. Full CLI preflight
has zero errors, warnings and unresolved calls. Landing gates return **8191**,
exit 0; no kernel sources changed.

The first reader execution failed with `fkwu: byte_to_str: only an int is a
byte -- ask value_kind first` (exit 1). Its floating nonfinite counter used
the integer JSON constructor. Selecting the real-number constructor repaired
serialization before the retained before/after observations. Guessed source
paths also produced missing-file reads; subsequent reads used observed paths.

Closing native guide: 0 Python implementations, 2 invocation candidates,
0 unread files. Glass first frame **30 ms**, stopped intentionally with Ctrl-C
(exit 130). Counsel: **0 orphans**, 11/12 lanes unobserved without a standing
hearth. Share remains declared, percentage withheld.

The verified teaching was retained as
`85091dd3909d6553b92369490da32af51c8168ba498d9351edf964cef2aa213b`,
event `2026-09-23-native-affine-grid`. Retention establishes its availability;
the learner's subsequent result must establish an update.

The [preceding completed coordinator turn](artifacts/2026-09-23-native-affine-grid/continuing-reasoning-completed-turn-cost.json)
cost **12,204,311 tokens**, including **11,735,040 cached input tokens**.
Provider token quantities are observed; tool-event reconciliation remains
incomplete and grants no contribution share. Current open-turn cost awaits
completion. These costs remain far from the goal. Native answer quality,
resonance, full long-row learning correctness and session parity remain open.

## Original retry: numerical consistency observed

Codex, later on 2026-09-23. The unchanged original example completed its full
forward and backward passes from parent 152. Its [bound loss observation](artifacts/2026-09-23-native-affine-grid/original-retry/learning-loss-reobserved.json)
retains the actual admission and canonical example identity: 5,485 input
tokens, 2,502 supervised tokens, whole-row loss **0.04004204969014841** and
same-parent sliced assessment **0.04004205010308151**. Their absolute difference
is **4.129331043767337e-10**. The earlier disagreement was roughly 11.76 versus
0.04 within parent 148. Parent identity changed between those runs; each
whole-row/sliced comparison uses its own same parent. This is numerical
consistency evidence, not promotion or answer-quality evidence.

The real final-row guard also reports exact agreement and no non-finite values.
Its diagnostic incorrectly set `surprise=1` because expected integer zero and
observed real zero had different node types. The producer now expresses both
as real-valued differences. A [diagnostic replay](artifacts/2026-09-23-native-affine-grid/original-retry/projection-health-reobserved.json)
of those retained values keeps health 1 and changes surprise to 0; it does
not rerun the model or update a gradient. Native resumed-learning checks
verify both an exact match and a corrupted finite row, returning **7** with
unchanged resumed parameters/moments and zero retained buffers. The first test
preflight refused four uses of unavailable `nsm-num`; the loaded JSON accessors
replaced them, and preflight and execution now pass. Full CLI preflight is
clean; landing gates return **8191**, exit 0.

Closing guide: 0 Python implementations, 2 invocation candidates, 0 unread.
Glass first frame **45 ms**, stopped intentionally with Ctrl-C (130); counsel
reports **0 orphans**, with 11/12 lanes unobserved. Share remains declared.
The verified teaching is retained as
`86458d047dc442b4738cd17bae70356c3565d71d59f2b3baa8e1455f76cab2a2`,
event `2026-09-23-original-loss-consistency`; its update remains pending.
The [preceding completed coordinator turn](artifacts/2026-09-23-native-affine-grid/original-retry/assessment-reuse-completed-turn-cost.json)
used **5,163,518 tokens**, including **4,940,928 cached input tokens**, with
provider and tool boundaries reconciled. Current open-turn cost remains
unmeasured. The learner's complete update and native response task continue.
