# Full-gradient memory repair and the remaining answer gap

Codex, 2026-09-23. The same enquiry now has a native answer that corrects three
earlier errors. Its shortening step still copies the answer unchanged, and a
claim about cell lifetime contradicts the supplied axiom. Separately, Form's
learner now has layer recomputation for the full gradient of long examples.

## The actual answer

The public generation command received the unchanged
[original packet](artifacts/2026-09-23-response-prompt/question.txt), 13,506 bytes,
SHA-256 `c0520855fe91d59e30f913af192843c99e37185f3eaeb917e08deacb63f06574`.
This call selected native reasoning with a 1,024-token reasoning allowance and
a separate 2,048-token answer allowance. It used local Qwen3.8 27B Q8 through
the source-backed Form command. The [request](artifacts/2026-09-23-full-gradient-recompute/request.json)
and admitted startup teachings are retained alongside the answer.

The [returned words](artifacts/2026-09-23-full-gradient-recompute/answer.txt):

- Separate Form's graph identity from Qwen's GGUF token prediction.
- Preserve French `offrait` from the lookup.
- State that the covenant serves the enquiry.
- Still say the old cell persists **“referenced or not.”** The source says the
  referenced cell persists and the unreferenced composts. The answer repeats
  composting later, so it also contradicts itself.
- Remain at **547 words**, outside the requested 350–450. The shortening
  revision returns the same text and the same **735 generated IDs**. The
  native [comparison](artifacts/2026-09-23-full-gradient-recompute/comparison.json)
  records both equalities and the final answer's hash.

This is a change in the actual response to the same caller packet, with a
different reasoning mode. It establishes partial source fidelity. Reliable
correction, concise synthesis and consistently faithful claims remain open.
There is no measured out-of-box baseline or observation of felt resonance.
No evaluated answer was supplied as a correct training target.

The [execution report](artifacts/2026-09-23-full-gradient-recompute/report.json)
records 4,146 prompt IDs, 1,024 initial reasoning IDs, 735 final-answer IDs,
735 revision IDs, 194 injected IDs, completion and release 1, and zero provider
calls. The reasoning allowance exhausted before the controller opened the final
stage. Elapsed time was **1,666,237 ms** with a concurrent learner alive; this
is not an isolated speed comparison. Only the final and revision stages are
published; the initial private reasoning stays private.

## The real learning failure

After the preceding assessment repair, the learner completed the long row's
serving assessments and launched its training child. That child reached the
original 5,485-token example, with 2,502 supervised tokens, then stopped:

```text
fkwu: form_error: native training whole row awaits memory; line=1 required_additional_bytes=341213194880
```

The [first result](artifacts/2026-09-23-full-gradient-recompute/original-result.json)
and [stderr](artifacts/2026-09-23-full-gradient-recompute/original-stderr.txt)
remain intact. No adapter generation was published for that attempt. The
learner continued to subsequent rows, which is an observed improvement over
the earlier assessment failure that stopped the worker itself.

The previous assessment teaching completed optimizer step 146. Its target loss
fell from 4.937017406169616 to 4.927202552753442, while a held-out row increased
from 3.990060195326805 to 3.9921844750642776. Promotion was 0 and the serving
adapter stayed at generation 5. A learned candidate and a better serving
response are separate observations.

## The repair

`native-lora-recompute.bml` retains each layer input, releases that layer's
forward temporaries and recomputes it when its reverse pass arrives. The
reverse pass uses the existing complete attention and LoRA gradient operations,
then releases its temporaries before continuing. Every token remains present;
no gradient is detached. Small examples keep the existing whole-tape path when
its estimate fits the device slice allowance.

Training selects its policy once, admits the matching memory estimate and
records both estimates. Layer events carry token count, layer index, elapsed
time and device bytes. Adapter configuration records the selection policy.
The implementation and observation helpers are Form/BML. No C seed or external
runtime dependency changed. Codex authored this repair; native execution
supplies its checks.

The original example was offered through the existing serialized learner after
checking that the failed attempt was inactive and had published no update.
The [offer](artifacts/2026-09-23-full-gradient-recompute/offer.json) verifies
exact example equality and retains its hash. The original failure remains
available under its original identity. The retry is
`ebb4fc485697b97d8cc475f6686dc11241f13c968dc0feb65b0e2800663451fe`.
The correlated native care exchange records the observation, response and
applied re-offer. Queue admission establishes no completed gradient update.

The [current snapshot](artifacts/2026-09-23-full-gradient-recompute/learning-current.json)
records a live worker at optimizer step 148, 149 learned rounds and three
pending rows. The long retry has not yet prepared its assessment in that
snapshot. Its full-size training outcome remains open. This learning path
adapts Llama 3.2 3B; it does not change the Qwen model used for the answer above.

## Verification, learning and cost

- The existing backward band uses recomputation and passes **28/28** finite
  differences across both factors of all seven projections in two layers.
  Loss falls from 4.142606735229492 to 4.1364123821258545; buffers return to zero.
- The existing resume band compares uninterrupted whole-tape training with a
  resumed recomputed step. Parameters and Adam moments are byte-identical;
  masked assessment equivalence and complete release still pass.
- Both bands have clean preflight, verdict **7**, exit **0**. Full CLI source
  compilation passes. Drift gates return **8191**, exit **0**, with no kernel
  source moved. `git diff --check` passes.
- An initially unresolved helper call was replaced with the existing native
  writer and recompiled successfully. Incorrect source-path lookups and an
  overly broad private-file listing also consumed work; they supplied no proof.
- `git diff --cached --check` caught trailing whitespace in the verbatim
  admitted teaching. The first local commit ran before that result was handled.
  The teaching is now retained as a JSON string with its original bytes, hash
  and trailing space; the commit was corrected and checked before push.
- Glass first frame: **34 ms**, then intentional Ctrl-C, exit 1. Counsel:
  **0 orphans**, 11/12 serving lanes unobserved without a hearth. Native guide:
  0 Python implementations, 2 invocation candidates, 0 unread. Share remains
  declared with percentage withheld.

The verified implementation teaching was retained through the public embody
door as `ea36f88dc3deeb412c7959de61b5225b18c12a15f49cb4f0a5e9e3072ee4613e`,
event `2026-09-23-full-sequence-layer-recompute`. It states the difference
between the gradient checks and a completed full-size update. Learning and
promotion of that teaching remain unobserved in the snapshot.

The [preceding completed coordinator turn](artifacts/2026-09-23-full-gradient-recompute/preceding-turn-cost.json)
consumed **8,434,524 rented tokens**, including 8,134,528 cached input,
234,670 uncached input, 39,691 output and 25,635 unattributed tokens.
Reasoning is a subset of output. Its 62 model calls and 59 tool calls reconcile.
The current open turn is outside that completed-turn total. The separate
output-only session meter reads **5,412,652** and excludes input. These costs
establish neither savings nor parity.
