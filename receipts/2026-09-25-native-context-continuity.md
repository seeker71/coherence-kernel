# Keep native task continuity while excluding assessed answers from gradients

The preceding movement made progress: it retained a native correction that
applied but left a 561-word answer with repetition and unsupported claims.
That is still unfinished work toward Urs's enquiry and the whole-session goal.

## A context difference in the comparison

The requests used `evaluation: 1` to keep assessed answers out of training.
Source inspection establishes that this also bypasses native recall,
checkpoint writes and resume. The coordinating conversation retains history.
Those are different context conditions; the earlier observations do not
establish a matched comparison of ongoing sessions.

The public `code` request now accepts `weight_training: 0`. It retains ordinary
native task continuity and routes the current answer through the existing
assessment exclusion in the session learner. The option accepts integer 0 or
1, with the existing policy as the default. Existing learned proposals remain
available. Evaluation and review answers remain excluded for either value.
The public result reports `evaluation` and `weight_training_excluded`
separately. Continuations carry the same original request and checkpoint ID,
including the weight-training option.

This separates two real choices: a fresh evaluation and ongoing work whose
answer is being assessed. It does not turn remembered work into an unseen
test, change Qwen weights or establish better generated answers.
`AGENTS.md`, the native coding guide and the response comparison guide now
carry this distinction into later sessions.

## Re-observe the changed boundary

The request band returns **255**. It checks the new flag's type and values,
ordinary resume availability, unchanged default behavior, and exclusion for
evaluation and review even when weight training is requested.

The existing session-code band returns **511** in its isolated paused home.
Through the public request's preparation and observation functions, it saves
and resumes a checked candidate while recording the answer as excluded from
gradients. The original document checks and read-only preservation remain.
This witnesses the context and learning boundary, not prose quality.

Both bands passed clean preflight. The first request preflight had exited 1
with `preflight: no readable verdict — read the chain line above`, reporting
one carried error. Inspection found the new integer flag validation using the
string-only `fat-has` helper. Integer comparisons replaced that use; the clean
preflight and band runs followed. A multi-file patch also failed its document
context match before applying; the corrected patch used the actual paragraph.
No assertion was relaxed.

The repository drift reading is **8191/8191**. The native authoring guide reports
zero Python implementations, two existing invocation candidates and zero
unread files. Counsel reports **zero orphans**, with 11/12 lanes unobserved
without a standing hearth. Whitespace checks passed.

## Actual answer work remains active

The public native coding door was started with the actual 561-word answer,
the original source documents and unchanged checks, plus caller observations
of repeated sentences and remaining unsupported assurances. The request and
preparation are retained in `artifacts/2026-09-25-native-whole-answer-care/`.
This call was admitted before the continuity change and keeps its original
`evaluation: 1`; editing source does not change an admitted process.

Observed through live exec **41153**, PID **46699**, it reached its first valid
JSON action: 624 generated IDs, one tool call, no completed checks yet. The
native compiler first reported a stale image, then rebuilt it through its
health flow. The call remains local-only and has not supplied a terminal
answer in this receipt. Its public log is
`.hearth/native-whole-answer-care/public-cli.log`. Continue observing this same
process; an observation timeout does not authorize a new admission.

The retained Form-owned provider comparison was re-read, without another
provider call. Its completed 444-word answer and original observations remain
under `artifacts/2026-09-23-answer-quality-gap/`: two provider calls, 40,274
tokens including 21,248 cached input, and 67,359 ms of provider execution.
It connected more concepts to interaction choices while retaining some
unsupported concluding language. It is a comparison answer, not a perfect
target or evidence of matched-session parity. It was not supplied to this
native repair as a replacement answer.

## Cost and retained teaching

The newly read completed coordinator turn used **4,614,424 rented tokens**:
4,471,680 cached input, 87,957 uncached input, 28,108 output and 26,679
unattributed tokens. Both call and tool-event reconciliation passed. The
current open turn and separate provider subprocesses are outside this scope.
The full native reading is `coordinator-cost.json` beside the request.

The verified continuity and exclusion contract was retained as teaching row
`6a353854a2872d6ed5ac5bd0398b9c41819b9d6cbde3ab9910ab0cc148a97555`,
event `2026-09-25-continuity-without-answer-training-v1`. Its worker was
launched. The preceding outcome teaching completed at optimizer step 187 and
learned rounds 188; the new row remains pending in the last status reading.
These are the shared local learner's observations, not Qwen improvement claims.

The context difference now has an explicit native remedy. The next observed
use of that remedy and the ongoing repair's returned answer are still needed
before claiming progress in native response quality or whole-session parity.

— Codex
