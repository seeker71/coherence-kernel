# Native answer reserve: completed execution, semantic improvement still open

Signed: Codex, 2026-09-17.

The public `code` JSON request now accepts `reasoning_answer_tokens` alongside
`initial_reasoning_tokens`. The existing bounded controller opens one explicit
answer stage when the initial generation has no complete final response. Both
generations and its observation remain private; only a complete answer enters
the ordinary action, report and verification controller.

Both allowances are caller-selected positive integers within the existing
hearth limit. `max_reply_tokens` caps each stage separately; context capacity
also applies. A natural completed answer uses no second stage. An incomplete
or refused stage cannot act. Omitting the new field preserves existing behavior.
The new module is `bml/form-cli-code-answer-reserve.bml`. All implementation and
experiment helpers are Form/BML; no C seed or foreign runtime is added.

## Same prompt, more reasoning

The prior independent source/intent review prompt was tested with the reasoning
profile: 512 initial tokens and a 1024-token final reserve. Both prompt texts,
enquiries, sources, report bodies and checks are byte-identical or structurally
identical to the compact trial, as appropriate. Candidate provenance and expected
judgments remain outside the model prompt. These are retained cases, not unseen
transfer or training.

| Candidate | Prompt IDs | Initial IDs | Answer IDs | Controller IDs | Elapsed ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| Provider control | 1741 | 512 | 401 | 98 | 263910 |
| Native reviewed draft | 1661 | 512 | 350 | 98 | 216960 |

Both complete through `controller-final-stage`. Both pass strict JSON decoding,
indexed edit application and original report checks without fence recovery.
Both keep every original span. The native audit verifies unchanged reports and
inputs. The sound control survives; the missed concession remains. Added
reasoning does not establish a semantic improvement on this pair.

Total **480933 ms**, **1775 generated IDs**, **196 controller-prefill IDs**,
one admission, fresh streams, successful final release, **zero provider
subprocesses**, **zero training**. The compact pair took **283203 ms**, generated
**927 IDs**, and required explicit fence recovery. The reasoning profile stays
optional. Better format compliance does not establish better quality or speed.
Evidence: `.hearth/response-parity/intent-reasoning-v1` and its native audit.

## Public request execution

The original fresh-case request, documents and checks were reused through
`fcaq-command("@...")`, adding only an initial allowance of **32** and final
reserve of **512**. The ordinary controller completed in **one reply**, with
**one verification**, **zero repairs**, **zero tool calls**, and all source and
report assertions passing. Documents stayed identical and release succeeded.

The run took **161077 ms**, generated **322 IDs** (32 initial, 290 final), and
injected **95 controller IDs**. The model used the explicit reserved answer
stage. Evaluation remained enabled; no answer was used for training and no
provider subprocess ran. These are execution and checked-report results, not
a semantic-quality or resonance verdict. Private evidence:
`.hearth/response-parity/reasoning-reserve-cli-v1`.

Reading the actual draft reveals two concrete defects despite its checked
fields passing: it invents that the recipient is busy and offers to adjust the
deadline without that permission in the enquiry. The explanation separately
says no reason for silence was observed. The native completion path therefore
works while the generated draft remains unsuitable without correction. This
answer is not a correct training target or evidence of response parity.

The retained public-route audit passes **1**, exit **0**. Stage counts reconcile,
the retained controller observation exactly matches the native protocol, both
private generations exist, and replaying only the final generation through the
original controller reproduces the same checked report. Release and original
assertions are rechecked. No additional inference is used by this audit.

## Checks, failures and instruments

Fresh request-band preflight is clean; execution passes **255**, exit **0**.
Checks cover dependent and typed budgets, reply ceilings, final-only selection,
and withholding action text from incomplete or unstopped output. The existing
reasoning-budget band passes **1**, exit **0**, covering natural completion,
one handover, insufficient context, failed generation, refused handover, partial
final output and stage accounting.

The public-route harness initially fails compile-only checking with four
unresolved `fcre-string` calls, exit **1**. Adding its existing defining prelude
repairs the chain; fresh compilation passes before execution. A panel extraction
also exits **2**, `unclosed character class`; a corrected pattern reads the
retained glass output. An initial receipt patch is refused for a missing patch
line marker and creates no file; the corrected patch succeeds.

Native guide: Python implementations **0**, execution candidates **2**, unread
**0**. Counsel: orphans **0**, **11/12** lanes unobserved, no standing hearth.
Glass: `13:57:06.844Z #0 dt=0/50ms`, **3M nodes / 114K cons**; its owned viewer
is closed. The preceding procedural learner completes round **82**, pending
**0**, promotions **4**, serving generation **5** unchanged. That learner targets
Llama 3.2 3B, not the Qwen evaluated here.

Drift gates pass **8191**, freshness **31**, whitespace checks clean. The
semantic defects above remain open independently of those successful checks.

The verified procedure is retained under event
`native-public-answer-reserve-procedure-verified-v1`, session
`codex-native-response-parity-2026-09-17`; its learner is launched. Completion
or promotion is not claimed, and no evaluated answer is supplied as a target.
The previous completed turn's share reconciles as observed: native **10%**,
local tool output **42%**, remote model calls **48%**, using the reader's
`observed-boundary-event-counts-v1` basis. These are counts of boundary events,
not semantic contribution, token shares or the current open reply.

The preceding completed coordinating turn
`01a0af84-acab-72c2-af85-31d7d286b876` records **52** model calls: **4473964**
input tokens (**4399616** cached, **74348** uncached), **24684** output including
**9288** reasoning, and **26080 unattributed** tokens; reported total
**4524728**. The unattributed quantity stays visible. This excludes the current
open turn and separately launched provider processes. Native zero-provider
counts do not erase coordinating cost.

The useful surprise is that the same prompt can become easier to consume
without becoming more correct. The extra cost changes the next choice: retain
the optional completion mechanism, decline a default reasoning upgrade, and
assess future quality gains against actual outcomes.
