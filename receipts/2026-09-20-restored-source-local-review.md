# Re-observe the repaired source on local Qwen

Codex, 2026-09-20. Continued from `84b190052`. The preceding movement was
progress: it repaired source coverage and retained a better provider answer.
This movement returned the repaired evidence to Qwen through the native runtime.
The overall quality and efficiency goal remains open.

## Same source and task, observed independently

`observe/form-cli-comparison-native-review-run.bml` runs one local generation
from the committed restored source. The [native audit](artifacts/2026-09-20-source-restored-native-audit.json)
compares the actual provider manifest with the native packet: both the domain
source bytes and review instruction match. The source SHA-256 is
`73229dcc6491a6f6b089bac8d0080cd4235ca57ed954797b9ccd74db8ff57cc3`.
Carrier envelopes and tokenizers differ. Neither the provider answer nor
expected findings entered the local prompt. The evaluation rubric was retained
before generation, and no evaluation content was offered to training.

The [packet](artifacts/2026-09-20-source-restored-native-packet.json),
[complete raw output](artifacts/2026-09-20-source-restored-native-raw.txt),
[extracted answer](artifacts/2026-09-20-source-restored-native-answer.md), and
[result](artifacts/2026-09-20-source-restored-native-result.json) remain available.
This answer is **Qwen3.8-27B-Q8_0 through fkwu**, using `knowledge-query`,
12,288 positions and a 2,048-token allowance. It is not a provider answer.

## The actual answer improved locally and still overreaches

The prior native reviewer called correct counts incorrect. This reviewer calls
the direct 648,411 primary-model token count and guided 78 recorded calls
correct, with matching evidence. It keeps the corrected 12,520-byte original
guided artifact and describes the identity seam as still requiring repair.
Those are useful improvements under the changed source context.

The full answer remains weaker than the retained provider answer:

- It still assigns two cited receipts to the original guided answer. That
  count came from the historical growing reference, not the pinned original.
- It drops the distinction between the guided artifact's measurement and
  implementation account and a returned answer alone.
- It turns authored observations into high/low trust, vitality, traceability,
  sovereignty and resonance rankings without preserving their attribution.
- It equates vocabulary echo with high resonance, then says resonance remains
  unobserved. The final caveat does not repair the earlier assertion.
- It gives no actual evidence paths in the revised answer. It presents the
  old primary-model token figures alongside one broader direct total without
  consistently maintaining their scope, and omits the guided through-call-21
  boundary from the numeric comparison.
- Its recommendation calls bounded the best general balance and direct a
  middle ground. Those rankings are not established by this one observation.

These are Codex's content observations, not a machine semantic score. Complete
generation and valid JSON established completion and shape. This response is
retained as a partial improvement and an unsuccessful parity attempt, not a
serving promotion. Human resonance remains unmeasured.

The next discriminating observation should address composition and attribution:
the short findings now preserve the numeric evidence while the long answer
still broadens it. The current candidate and its historical grounding also
carry obsolete counts. A fresh composition from current evidence can test
whether carrying that flawed candidate into review is sustaining the error;
it would be a changed task, not proof of a weight improvement. Repeating this
unchanged review would provide little new information.

## Cost and reuse

| Native observation | Value |
| --- | ---: |
| Prompt IDs / generated IDs | 4,813 / 1,377 |
| Execution window, ms | 515,784 |
| Complete / released | 1 / 1 |
| Valid JSON / report schema | 1 / 1 |
| Provider calls / model-server calls / evaluation training | 0 / 0 / 0 |

The execution window covers admission, prefill, generation and release;
preparation, compilation, retention and rented coordination are separate.
The same-source provider answer used **21,069 reported tokens** and
**44,867 ms** in its provider process. These observations leave a substantial
local latency and quality gap. Tokenizer counts do not share a common unit
across the two models.

The native chain now includes five attempts: failed citation trial, focused
contrast, sectioned draft, original review and restored-source review.
Native arithmetic gives **16,009 prompt IDs**, **5,633 generated IDs** and
**1,934,444 ms** across their measured execution windows. Earlier Sep19
generation and coordination are outside that subtotal; failures are retained.

The completed native request is reused through its claim directory. The door
now labels replay with `native_model_calls_new=0` and the original
`native_usage_event`; its stored result and answer remain unchanged. A pending
claim asks for inspection rather than launching another model.

The [preceding coordinator turn](artifacts/2026-09-20-source-restored-native-coordinator-cost.json),
`01a0bab1-cd5d-79a3-bd6c-e7aee520476c`, records **29 calls**, **2,716,490 input**
(**2,605,440 cached**, **111,050 uncached**), **20,494 output** including
**8,205 reasoning**, and **28,339 unattributed tokens**: **2,765,323 total**.
This excludes the current open turn and separate provider processes. The
unattributed part stays explicit. Zero provider calls inside this experiment
does not make its coordinating work free.

## Learning and checks

The learning-path inspection confirms a separate gap: routine session teaching
updates Llama 3B, not this Qwen base. Qwen's native head-learning and adapter
session paths exist. Their previous batch experiment reduced validation loss
but failed the separate real repair and regressed stopping; that candidate
remains unpromoted. See the [cached-head receipt](2026-09-17-qwen-cached-head-batches.md).
No Qwen weight update or blind adapter switch was made in this comparison.

The source band returned **1**, exit 0, preflight was clean, and landing checks
returned **8191**, exit 0. The matched-input audit passed. Parent-directory
creation makes the native observation door reachable independently of a prior
private experiment. No C seed growth or external runtime dependency was added.

Panel: **0 orphans**, **11 of 12 lanes unobserved**, no standing hearth.
The bounded glass first frame arrived in **29 ms**, exit 0. Native authoring
guide: **0 Python implementations, 2 execution candidates, 0 unread files**.
The verified comparison procedure was retained as
`same-source-native-review-2026-09-20` for the separate session learner;
evaluation requests and answers remain excluded.
