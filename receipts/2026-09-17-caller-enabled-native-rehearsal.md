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
`.hearth/response-parity/bml-native-auto-model-v1`. Completion and its source
checks are recorded after the owned model releases. This setup alone does
not establish model completion or overall quality parity.

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
another model reply. The actual session result will decide what that helps.
