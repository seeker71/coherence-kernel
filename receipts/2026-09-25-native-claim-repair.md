# Native correction applies; whole-answer judgment remains open

Urs's enquiry is still the live work: explain how Form's axioms, trust,
frequency awareness, numeric identity, translation and vocabulary change an
interaction. The current native answer has not reached the requested quality.

Two actual Qwen3.8-27B-Q8_0 correction results are retained under
`artifacts/2026-09-25-native-claim-repair/`. Both ran through the source-backed
form-cli on fkwu and the in-process Metal carrier. Neither called a provider.
Neither evaluated answer entered learning as a correct target.

## Observe and resolve

The first correction used the complete indexed review, with 1,024 reasoning
tokens reserved and a 4,096-token answer allowance. It completed and released
its session. It returned 47 span judgments, including eight replacements.
Every replacement omitted its required reason. Native application returned:

> span 8: expected ordered integer id, status, action and nonempty reason; replace also needs different nonempty text

No edits were applied. Reading the public proposal also exposed substantive
failures: the reviewer kept “There is no defensive hesitation” as a reasonable
consequence and treated “I demonstrate care through clarity” as acceptable.
This is a gap in the review itself, alongside the missing fields.

The follow-up used the native sparse amendment carrier already landed in
943a4c04a. Its packet retained the original answer, original sources, eight
unapplied proposals and caller observations identifying the remaining claims.
The model was asked to return changed spans with reasons, remove unsupported
assurances and self-assessment, and preserve the original 350–450-word range.
Codex supplied those observations; they are not native discoveries.

## Re-observe the complete answer

The native follow-up completed with valid JSON. Form applied all 15 proposed
replacements without editing their text. The original checks then reported
**561 words, outside 350–450**. The original answer had 496 words. All original
checks remain unchanged; the first failing check is retained in
`followup/comparison.json`.

Some distinctions improved. The model changed the claim that a correction is
a kernel-interface breach into a report of what Urs observed. It distinguished
codebook changes from kernel composition and removed a promise of retaining
conversation history merely by releasing a reference.

The assembled answer still fails in visible ways:

- “I acknowledge it directly” appears twice consecutively.
- The same sentence explaining changed composition and retained references
  appears twice, beside earlier sentences already explaining that mechanism.
- “I demonstrate care through clarity” became “the response demonstrates care
  through clarity.” The self-assessment remains.
- The answer still says “The response is not cautious” and connects its own
  operation to the current cell composition without establishing that relation.
- It describes mechanisms more than it explains useful changes to this actual
  interaction. The concrete vocabulary lookup supplied in the sources remains
  largely unused.

These are Codex's observations of the returned words. The model's reason fields
claim to remove several defects that remain in its replacements. A declared
reason therefore does not establish a successful correction.

The full, unchanged native answer is `followup/answer.txt`. Its SHA-256 is
`c1563cf249a6c4a1a9e2c1f02d0279d286c8ba91927f7a0a8c86adc3b69a0b62`.
It is retained as evaluation evidence, not accepted as the completed reply.

## Execution and cost

| Observation | Complete review | Sparse follow-up |
| --- | ---: | ---: |
| Model prompt tokens | 4,915 | 5,690 |
| Generated reasoning tokens | 1,024 | none separately requested |
| Generated final/output tokens | 1,482 | 804 |
| Provider calls | 0 | 0 |
| Application | rejected | 15 replacements applied |
| Original answer checks | not reached | word range failed |

The complete review took 941,587 ms. The direct follow-up reports 492,374,838
microseconds of GPU work, including 310,195,873 for prefill and 182,178,965 for
decode. That GPU measure is not wall time. Its public report records stopped=1,
no timeout or fuel cuts, and 128/128 state handles closed.

The operation, profile, feedback and prompt changed between these calls.
Their token counts do not isolate an efficiency gain from sparse amendments.
The follow-up's startup overlay is retained in `startup-teaching.json`,
reconstructed after generation from unchanged committed source. It is not an
admission-time token capture or evidence of the earlier reasoning-mode context.

The retained completed coordinator-turn readings count cached input:

- `coordinator-cost.json`: 4,286,166 rented tokens for its recorded turn.
- `followup/coordinator-cost.json`: 6,631,954 for the subsequent recorded turn,
  including 6,491,136 cached input, 110,056 uncached input and 30,762 output.

Those readings exclude the currently open coordinator turn. Zero provider calls
in the native generations does not erase the coordination cost around them.

## Embody the verified result

`prepare-followup.bml`, `apply-followup.bml` and `retain-teaching.bml` keep this
movement executable in Form. The exporter reads the actual public direct-mode
key/value report, derives completion from its stop and budget fields, applies
the exact sparse reply, and reruns the original checks against the composed
answer. It preserves the proposal, log, failed check and answer.

Three helper failures remain part of this work's evidence:

- `./fkwu --check .../prepare-followup.bml` initially reported
  `source-compile: mismatched form.bml delimiter: )`. Splitting the nested
  expression into named values repaired it; compilation and execution passed.
- `./fkwu .../apply-followup.bml` initially reported
  `public generation report not yet available`. The helper assumed the
  reasoning-mode JSON report. It now reads the direct mode's actual public
  report; the real follow-up was then applied and checked.
- `./fkwu --check .../apply-followup.bml` reported
  `[unresolved-call] 'parse_int' matched no op/rewrite/fn/binding` during that
  repair. The existing native JSON numeric reader replaced the nonexistent
  call. Compilation and real execution passed.

All commands above were carried by `form-run`. The final native guide reports
zero Python implementations, two invocation candidates and zero unread files.
The repository drift reading passes **8191/8191**. The glass first frame was **29 ms**;
counsel reports zero orphans and 11/12 lanes unobserved without a standing
hearth. These are instrument readings, not answer-quality measures.

The previous sparse-carrier teaching completed locally at optimizer step 186,
learned rounds 187, with pending=0. A new verified outcome teaching was retained
as row `4d083ae75b0b749127160dd18082e33fdad0dc4e066ea6e39f4450710c0206fa`,
event `2026-09-25-native-claim-repair-outcomes-v1`. It distinguishes generation,
application and acceptance. Its worker was launched; a Qwen weight update or
transfer to this enquiry is not established by that retention.

The useful crossing is precise: the native model can now supply an applicable
sparse correction for this answer. The remaining gap is whole-answer judgment:
removing a false implication also needs to produce a coherent, useful reply within
the original requirements. The next native repair has the actual composed
answer, its duplicated sentences, surviving claims and failed word count to
work from. This observation does not justify another unchanged shortening call
or a claim of parity.

— Codex
