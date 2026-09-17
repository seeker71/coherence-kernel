# Native intent review: a judgment needs an observable consequence

Signed: Codex, 2026-09-17.

The preceding source review removed an invented reason for silence, yet kept
wording that accepted a nondecision despite the caller needing a decision by
a stated deadline. This movement tests two native interventions against the
same retained pair: a provider-produced control and the locally reviewed
draft. Neither candidate's provenance nor the expected judgment reaches the
model. These are diagnostic replays, not unseen transfer or training.

## Independent source and intent labels

The experimental prompt asks for source support and intent preservation
separately on each indexed span. It asks whether following the wording would
obtain the requested outcome, checks the whole report, and discourages stylistic
rewrites. Only the follow-up field may change. The original enquiry, documents,
report checks and both report bodies are retained unchanged.

One Qwen Q8 admission, compact `knowledge-query` profile, context 8192,
1024 output-token allowance per candidate, fresh independent streams:

| Candidate | Prompt IDs | Generated IDs | Elapsed ms | Original JSON decoding |
| --- | ---: | ---: | ---: | --- |
| Provider control | 1743 | 505 | 167439 | Refused |
| Native reviewed draft | 1663 | 422 | 115706 | Refused |

Both generations completed but returned complete Markdown JSON fences.
The strict transport reader correctly refused those bytes. An explicit native
recovery removes only the enclosing fence, retains and hashes the raw output,
and reruns the unchanged indexed edit carrier and caller's report checks.
Both recovered messages pass. Their five and four edits all keep the original
text; exact original report identity is verified for both. Format recovery is
not a semantic repair. The native draft's intent defect remains.

Batch total: **283203 ms**, **927 generated IDs**, **one model admission**,
successful stream transitions and final owner release, **zero provider
processes**, **zero training**. The private carrier boundary witness also passes
all eight checks, including duplicate/missing fields, wrong base, incomplete
coverage, and preservation of unselected fields. That validates the carrier,
not the model's judgments.

## Concrete recipient counterexample

A second prompt asks for a concrete recipient reply that the draft explicitly
permits but that misses the caller's requested outcome. It requires an exact
permission quote, excludes mere refusal to follow instructions, and permits an
empty counterexample when no such permission exists. Indexed edits follow the
counterexample. The same retained inputs and local model profile remain in use;
the allowance is 1408 output IDs per candidate.

On the control, the model claims a response after the deadline is permitted by
a question asking for a response by then. The surrounding draft explicitly
requires the decision by the deadline. Its quote matches source bytes, but
does not establish the claimed permission. The model also emits `replacement`
where the carrier requires `text`; application refuses span 2. We retain the
raw failure instead of normalizing it into an accepted edit.

The control therefore defeats promotion of this candidate, independently of
what the other draft produces. An adversarial prompt can invent a loophole;
matching quoted bytes cannot prove the entailment attributed to them.

The native draft's counterexample also fails to isolate the actual concession.
It interprets the explanation of silence as permission for silence and proposes
removing that explanation while retaining the explicit acceptance of a
nondecision. It also emits `replacement`; application refuses span 1. No
proposed edit is applied on either candidate.

| Candidate | Prompt IDs | Generated IDs | Elapsed ms | Strict JSON | Applied |
| --- | ---: | ---: | ---: | --- | --- |
| Provider control | 1737 | 671 | 197723 | Pass | Refused |
| Native reviewed draft | 1657 | 537 | 138716 | Pass | Refused |

Second batch: **336499 ms**, **1208 generated IDs**, one admission, both
stream transitions and final owner release successful. Across both experiments:
**619702 ms**, **2135 generated IDs**, **two admissions**, all releases
successful, zero provider subprocesses, zero training. A successful process
exit records lifecycle completion; both semantic experiments are unsuccessful.

Private evidence lives in `.hearth/response-parity/intent-review-v1`,
`intent-review-recovery-v1`, and `intent-counterexample-v1`, with the native
trial, prompt construction, recovery and carrier checks beside those roots.
No experimental prompt is installed in the production review path.

## Failed preparation and current instruments

`form-run ./fkwu .hearth/response-parity/intent-counterexample-prepare.bml`
first exits **1** with `str_len: nothing has no length -- ask nothing? before
measuring`. Its new-file guard measured a missing file. Testing `nothing?`
repairs the guard; preparation and compile-only checks then exit **0**.
Three guessed lookup paths also fail with `No such file or directory` (exit 2):
`source-selection-recovery.bml`, `form-cli-json-repair.bml`, and
`form-cli-action-progress.bml`. The actual retained recovery is
`source-selection-recover.bml`; the diagnostic control lives in
`form-cli-code-policy.bml`. These lookup mistakes are coordination cost,
not native runtime defects.

Native guide: Python implementations **0**, invocation candidates **2**,
unread **0**. Counsel: orphans **0**, **11/12** lanes unobserved because no
hearth stands. Glass first frame: `13:32:45.041Z #0 dt=0/50ms`, **3M nodes /
126K cons**. Its owned viewer is closed (130).

The native retained-output audit passes **1**, exit **0**: identical inputs,
original source/report checks, strict JSON on the second batch, exact
reproduction of both application refusals, successful lifecycle boundaries,
zero provider subprocesses and zero training during evaluation. The diagnostic
response retains the default review and declines experimental promotion.
Drift gates pass **8191**, freshness **31**, and whitespace checks are clean.
Share is **declared**, percentage withheld while appended carrier evidence is
still being reconciled; semantic contribution is unmeasured.

The verified checking procedure is returned under event
`native-intent-review-boundaries-procedure-verified-v1`, session
`codex-native-response-parity-2026-09-17`. It is retained and the learner is
launched. This is procedural learning after model release, separate from the
evaluation batches; no evaluated answer is supplied as a correct target.
Completion and serving promotion are not claimed. The previous learner round
**81** completed with pending **0**, promotions **4**, serving generation **5**
unchanged. That learner targets Llama 3.2 3B, not the Qwen evaluated here.

The preceding completed coordinating turn
`01a0af69-d035-7c12-b7d0-d7bbe3a3cf27` used **48** provider model calls:
**9322438** input tokens, including **9123328** cached and **199110** uncached;
**46002** output, including **31699** reasoning. Total **9368440**,
unattributed **0**. This excludes the current open turn and separately
measured provider subprocesses. No new provider subprocess is used in these
experiments. Reusing a paid control adds no new synthesis charge; the
coordinator's substantial cost remains visible.

The surprising teaching is that exact quotes and separate judgment fields can
still support an incorrect interpretation. The useful movement from the
uncomfortable result is to retain the sound control and reject the proposed
review, rather than spend its apparent confidence as evidence of progress.
