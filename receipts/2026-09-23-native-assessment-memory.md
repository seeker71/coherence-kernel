# Complete assessment after the whole-row memory refusal

Codex, 2026-09-23. The response-quality gap remains open. This movement repairs
an observed blocker in the native learning path and preserves the unchanged
response comparison.

## Correction execution: unchanged answer

The retained 84 correction IDs decode byte-for-byte to the intended shortening
request and its user/assistant boundaries. Replaying those IDs through the
reference, per-position observation path produced the same **812 generated IDs
and 635 words** as the optimized sliced path. Completion and release were 1;
provider calls were 0; elapsed time was 475,372 ms. The 350–450-word requirement
and source-fidelity findings remain unresolved.

[The comparison](artifacts/2026-09-23-assessment-recovery/reference-comparison.json)
does not support changing the optimized correction lane to fix this answer.
The earlier source-specific correction did change generation, so this is not a
claim that all feedback is ignored. The reference helper is retained alongside
the comparison; its question and prefix remain in the preceding receipts.

## Actual learning failure and repair

The preceding learner stopped before writing its assessment baseline:

```text
fkwu: form_error: native training whole row awaits memory; line=1 required_additional_bytes=341213194880
```

Worker status was `failed-exit-1`, with four pending rows and optimizer step
145. The failure occurred while assessing the serving adapter, before the
training child existed. Assessment had reused training's whole-row memory
admission and retained the entire forward tape.

Codex changed the native BML implementation to retain the complete KV history
and assess memory-admitted forward slices. Each slice releases its tape. Loss
uses every supervised target and is weighted by the supervised-token count;
prompt-only slices still advance the complete context. Validation and session
grading use the same operation. The model's training entry point still requires
position zero and its complete gradient memory. No C seed or dependency changed.

The executing organ emits a correlated `assessment-memory` observation,
`slice-assessment` response and applied action, followed by a fresh coverage
reading. Per-row scores retain input tokens, supervised tokens, slice count and
peak estimated slice bytes. Counts and successful execution establish coverage;
they do not establish answer quality.

Re-observing the exact failed row with the same serving Llama 3.2 3B adapter
completed in **271,297 ms**: **5,485 input tokens**, **2,502 supervised tokens**,
**15 slices**, loss **0.04537703881265798**, zero retained buffers and zero
provider calls. The peak estimated slice was **6,943,217,540 bytes**, excluding
the resident model and complete KV allocation. This estimate and the old
training admission estimate describe different memory scopes.

[Before](artifacts/2026-09-23-assessment-recovery/learning-before.json) and
[after](artifacts/2026-09-23-assessment-recovery/assessment-after.json) retain
the failed worker state and actual completed assessment. Optimizer step stayed
145: assessment does not train. The later prompt-only optimization uses the
existing last-row head path where the supervision mask is zero; the timed run
above preceded that optimization. Its numerical equivalence is checked below.

Session learning here adapts Llama 3.2 3B. The response comparisons use Qwen
27B; this repair does not update Qwen's weights or establish response parity.
The remaining whole-row training memory requirement stays visible.

## Verification and cost

- `native-lora-resume-band.fk`: clean preflight, verdict 7, exit 0. One-token
  and uneven slices agree with whole-row masked loss within
  `7.947285940446136e-8`, including zero-mask prompt slices and a partial final
  slice. Adam parameters and moments remain byte-identical after continuation;
  all buffers release.
- `native-llama-backward-band.fk`: clean preflight, 28/28 finite-difference
  checks, verdict 7, exit 0; loss decreases after its update and buffers release.
- Whole source-backed CLI compile: exit 0. Drift gates: 8191, exit 0; no kernel
  source moved. `git diff --check` is clean.
- The first compile refused unsupported infix arithmetic in the byte estimate;
  replacing it with native arithmetic calls resolved that authored error.
  Several mistyped source lookups and a rejected patch context also cost work.
- Glass first frame: **28 ms**, followed by intentional Ctrl-C, exit 1.
  Counsel: **0 orphans**, 11/12 serving lanes unobserved without a hearth.
  Native guide: 0 Python implementations, 2 invocation candidates, 0 unread.
  Share remains declared with its percentage withheld.

The preceding completed coordinator turn consumed **7,927,281 rented tokens**:
7,614,720 cached input, 244,560 uncached input, 42,061 output and 25,940
unattributed tokens. Reasoning tokens are a subset of output. Its 56 model calls
and 53 tool calls reconcile in the
[cost record](artifacts/2026-09-23-assessment-recovery/preceding-turn-cost.json).
Current open-turn cost is not substituted with that completed-turn reading.
The separate output-only session meter read **5,355,604**; it excludes input.
This is paid Codex implementation work, with native execution supplying the
checks and actual assessment. No savings or quality parity is claimed.

## Return to learning

The verified teaching was retained as
`335ea4c7c95a900eb7f368d390137a8763e1a236e8a4d52580c7cd451ab1d5f9`
through `observe/form-cli-session-home-embody-run.fk`, event
`2026-09-23-assessment-memory-recovery`. The worker was launched on the repaired
implementation. Retention alone does not establish a learned or promoted row;
the queue's subsequent state must supply that observation.
The [closing snapshot](artifacts/2026-09-23-assessment-recovery/learning-current.json)
has a live worker, five pending rows, optimizer step 145 and 146 prior learned
rounds. The long row has not yet completed preparation in that snapshot.
