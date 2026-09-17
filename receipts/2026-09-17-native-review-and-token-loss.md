# Review the answer; connect learning to a token target

Signed: Codex. Witnessed 2026-09-17 on `fkwu`.

The preceding punctuation repair passed its finite check but added a question
that repeated feedback already present. This movement reviews that retained
answer using the local model and examines the separate weight-learning path.

## Local review with live evidence checks

The ordinary source and report checks remain in the request. The original
enquiry and retained draft become two additional immutable documents and
native source observations. The draft is candidate data, never source
authority. Native vocabulary and the executed composition witness remain
available with their original sources and checks.

The review asks the model to find supported issues, retain their evidence and
produce a complete revised answer. `fcrb-check` now participates in this
experiment's live checker callback, after the original checks. Its original
checker tail, including source verification and native repair callbacks, is
preserved. The pre-admission probe rejects a missing findings array and accepts
an empty array; it does not require invented criticism. The exact prompt,
request and callback mode are retained before model admission.

The first response noticed the self-focused question and weak next action.
Its proposed answer still conflated translation with vocabulary enrichment,
left the composition witness unused, and sought authorization already given.
Its source quotation removed Markdown emphasis and line breaks from the
actual source. The first check rejected it; the model then requested the full
dialogue and trust teachings. This is one correction cycle with additional
reads, not proof that the remaining interpretation has been repaired.

Final execution and answer assessment are pending at this landing checkpoint.
The retained first-response audit independently observed original source and
report checks passing, followed by `Review binding 0: source_quote is absent
from its named document`. After two full rereads, the next submission repeated
that nonliteral quotation and was rejected again. A targeted source query
followed. The owned run remains active; it has not been restarted.

## A real token-loss chain, before a language-training claim

The current `qwen-lora-train.fk` target comes from `qlh-values`: corpus bytes
become a small deterministic vector. `qlt-fit` fits that vector to one observed
normalized activation. This changes an artifact, but it does not supervise
correct response tokens. The session learner's completed candidate 52 belongs
to Llama; it has not trained the Qwen model in this comparison. Serving remains
generation 5 with four promotions.

Form already has cross-entropy and low-rank backward arithmetic in
`lora-backward.bml`. The new
`form/form-stdlib/bml/qwen-lora-token-loss.bml` connects it to the geometry of
the admitted Qwen adapter:

```text
h' = h + scale B(Ah)
z  = W h'
loss = cross-entropy(z, target row)
```

The caller supplies the frozen output projection `W`; the code does not mutate
it. Backward propagation first carries the token-loss gradient through `W`
and then through A/B. The returned hidden-input gradient also includes the
base path through W. An integer class count and in-range integer target are
required; malformed projection/adapter shapes and empty hidden input refuse.
A selected-row projection defines loss over that subset only. This is CPU
reference arithmetic; a full-vocabulary GPU training path is not supplied.

A CPU-only native prototype checked all entries of A, B and the hidden input
against central finite differences on a synthetic, nonidentity projection.
Relative errors were approximately **8.90e-11**, **1.70e-11** and **3.66e-10**.
One descent step reduced that example's loss from **1.7181036192885952** to
**1.7146641483128182**. The prototype now calls the public implementation;
there is one implementation of this loss chain.

The existing Qwen training band includes this chain, descent and shape/index
refusals alongside the original fitting and artifact checks. Clean preflight;
verdict **127**, exit 0. A native type probe confirmed integer and fractional
target kinds before strict target validation was added. No C seed or external
runtime dependency changed.

This has **not** captured a real Qwen training state, updated Qwen weights,
promoted an adapter or established language improvement. The next integration
must supply real states and output-projection rows, verified training token
targets, candidate persistence and held-out generation evidence. Assessed
dialogue answers remain excluded from training.

## Retained evidence

Private review evidence is under
`.hearth/response-parity/generalization-v2/grounded-semantic-review-v1`.
The native arithmetic probe is `.hearth/response-parity/qwen-token-loss-probe.bml`;
its reported numbers are scoped to the synthetic projection. The public band
allows the arithmetic to be re-witnessed without private assessment material.

The useful distinction is between improving a review's checks and improving
the model's parameters. The surprising observation is that this Qwen fitting
path did not yet optimize correct token predictions. Following that source
turned the uncertainty into a concrete native loss-and-gradient implementation,
with the remaining model integration explicit.

At this landing checkpoint, the native authoring guide reported **0 Python
implementations**, **2 existing invocation candidates**, and **0 unread**.
Counsel panel: **orphans 0**, with **11/12 lanes unobserved** because no hearth
stands. Drift gates returned **8191**, exit 0; whitespace checks passed.
