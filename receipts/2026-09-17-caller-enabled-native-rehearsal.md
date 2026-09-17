# Caller-enabled native rehearsal after an unchanged edit

Signed: Codex. The previous movement made progress by landing native semantic
search and truthful unchanged-edit feedback. Its actual Qwen admission did
not adopt the search tool. This movement connects those observations in the
native coding policy.

## Native work inside the coding movement

A caller can now allocate `native_rehearsal_checks` in a coding request, or
use `fcap-with-rehearsal-budget` in native code. The allowance is an integer
from **0** through **64**, with **0** as the default. Model messages cannot
allocate it. Read-only review cannot enable the request option.

An actual unchanged edit on a caller-writable BML source retains its failed
observation and can trigger native semantic rehearsal in the same turn.
The policy uses the caller's existing source checker. Every actual invocation
spends the allowance, including the original-source check and a malformed
callback response. Each path and source digest is attempted once in the
retained state; renewing the allowance does not erase that history.

The search can change only the passing candidate's writable document. Failed
or exhausted search preserves the source. The model receives explicit native
attribution, the final check and exact current source or reconstructed source
patch. Full attempts, the original unchanged-edit failure and remaining
allowance survive checkpoint encoding and are returned to the caller.
Native work does not add a model reply or model tool call. It has its own
check count. Task completion, review and final verification remain required.

This is caller-enabled assistance, not a claim that the model chose the
search. The allowance bounds check count; the caller also bounds each
candidate's execution. There is no C growth, provider subprocess or additional
language runtime in the implementation.

## Verification

Fresh preflight and execution pass:

- Native value-search policy band: **1**, including automatic repair,
  allowance consumption, original/writable preservation, checkpoint roundtrip,
  one attempt per source, model inability to allocate checks, read-only/default
  exclusion, malformed callback accounting and final review/verification.
  Its checker is a mock; this establishes policy behavior.
- Coding request band: **255**, including allowance typing, range and review
  exclusion. Its first new run failed because fractional input became zero
  through the numeric accessor. Canonical integer spelling is now checked
  before admission; the unchanged assertion passes after the repair.
- Existing coding policy and memory bands: **65535** each. Drift gates:
  **8191**, exit 0. Whitespace check is clean.

The private model runner uses the same retained task and candidate as the
preceding explicit-tool admission, with the same prompt, context **12,288**,
reply allowance **1,536** and cumulative turn limit **28**. The saved bootstrap
files compare byte-identical. The changed conditions are the truthful
unchanged-edit boundary and the caller-enabled native allowance. The checker
retains the original 32-case behavior body, separate bounded native workers,
exact checked source snapshots and owned-process release evidence.

Actual admission evidence is retained under
`.hearth/response-parity/bml-native-auto-model-v1`.

## The actual local session completed

Qwen first read the candidate, then emitted the same byte-identical edit as
the preceding run. The policy retained that failed action and performed
**28** native checks. The last candidate passed all **32** original behavior
cases. Its exact bytes match the earlier successful native-only repair.
Qwen then marked its implementation task done, read the current source in
review, and accepted it with a correct explanation: both terminal paths now
reverse the accumulated spans. The final caller verification independently
executed the unchanged 32-case band again and passed. This is same-model
source review plus repeated executable behavior evidence, not an independent
model review.

| Observed admission | Explicit tool available | Automatic native assistance |
| --- | ---: | ---: |
| Final workflow state | attention | complete |
| Elapsed ms | 674,474 | 534,842 |
| Generated token IDs | 1,373 | 511 |
| Injected observation token IDs | 2,004 | 1,750 |
| Actual generated replies | 7 | 5 |
| Model tool calls | 4 | 3 |
| New source checks | 0 | 29 |
| Full behavior passes | 0 | 2 |
| Provider subprocesses | 0 | 0 |
| Model released | yes | yes |

Native assessment reconciles the original goal, original documents and
writable boundary, byte-identical saved bootstrap, all **28** native attempt
digests against executed candidates, and **36** checks left from the native
allowance. It calculates **862** fewer generated IDs and **139,632 ms** less
elapsed time. These are observations from this paired retained case, not a
general throughput claim. This case changed from unsuccessful to complete.

The independent native stage audit accounts for **29** check snapshots,
**87** released stages, **9** failed timeout stages and **1** deferred output
drain. Both passing sources equal the final retained candidate. Every
executed behavior body retains the original band with only its candidate
prelude path changed. The final source digest is
`9c787aad7e6c67eeaa12546dfbbfff6ae1dbe236a602b8eb08c0e381aa010807`.
Private audits are `bml-native-value-audit.bml` and
`bml-native-auto-assessment.bml` under `.hearth/response-parity/`; both pass
**1**, exit 0. The final actual model reply is retained at
`.hearth/code-memory/replies/56773-1789627960993.txt`.

Attribution stays explicit: native search authored the repair; Qwen reviewed
its actual effect and finished the workflow. No desired expression was added
to the prompt. No evaluated answer was trained. This run admitted the base
Qwen model once with no adapter and no automatic learning. The public working
utility was not replaced by this private candidate. Broad response quality,
frequency/resonance and provider-baseline session parity remain unproved.

## Instruments and retained learning

The prior procedural learner completed: learned rounds **65**, promotions
**4**, pending **0**, worker no longer alive. Its candidate changed while
the serving generation stayed unchanged. This is the Llama session learner,
not an update to the Qwen model used here. Qwen was admitted after release.

Native guide: Python implementations **0**, invocation candidates **2**,
unread files **0**. Glass's first observed frame arrived in **29 ms**;
its owned viewer was closed after reading. Hearth returned
`signal=nothing reason=no-standing-hearth`.

The useful surprise is that an unchanged edit can be both an honest failed
action and a precise trigger for available native help. The movement keeps
the original failure visible while letting the body do useful work before
another model reply. Here that changed the session's actual outcome.

After Qwen released, verified procedural teaching was retained under event
`2026-09-17-caller-enabled-native-rehearsal-v1`, session
`native-arrival-bootstrap`. Its learner launched; a completed new weight
update is not claimed. The teaching excludes the utility's desired repair
and evaluated replies.

Parent output meter: **2,408,159 cumulative tokens**. Goal meter:
**11,665,759 cumulative tokens**. These are different accounting boundaries,
not isolated native-run costs. Both compared native admissions used zero
provider subprocesses; rented design and coordination remain substantial.
The full efficiency and parity objective remains active.
