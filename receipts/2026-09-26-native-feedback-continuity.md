# Caller corrections survive the next failure

The real response checkpoint at turn 25 retained four caller-feedback events,
but its reconstructed admission contained none of their IDs or complete texts.
Later tool and word-count failures had replaced the current failure evidence;
retaining the correction in continuity did not make it available to a fresh
model context.

The native coding policy now supplies every retained `{id,text}` event under
`caller_feedback` at admission and renewal. Request handling reads the same
accessor. Incremental observations keep their existing compact delivery. The
original goal, source documents, write authority, checks and retained feedback
bytes are unchanged.

## Same checkpoint, observed again

The [native observation](artifacts/2026-09-26-native-feedback-continuity/observe-admission.bml)
validates the retained checkpoint against the original task contract. Complete
text coverage uses JSON-escaped bytes. Reconstructing the preceding admission
also reproduces its retained SHA-256 exactly.

| Observation | Before | After |
| --- | ---: | ---: |
| Retained feedback events | 4 | 4 |
| Event IDs supplied | 0 | 4 |
| Complete event texts supplied | 0 | 4 |
| Admission bytes | 31,216 | 36,420 |

The feedback digest remains `8e434f794dac7c7a487e09e1cf31881f74d00910fb7e9a21d71eae7b91320ad2`.
Restoring the missing corrections adds **5,204 bytes**. This is a verified
delivery repair; any reduction in later mistakes or total response cost needs
the actual continuation.

The request band now drives a real native tool failure after caller feedback,
serializes and restores the state, and checks the renewed admission, original
authority and idempotent retry. Request, policy and continuity checks passed
**255**, **65535**, and **65535**, with clean preflights. Drift gates passed
**8191/8191**. The native guide found zero Python implementations and zero unread
files. Counsel reported **0 orphans** and **11/12** unobserved serving lanes.

## Actual response work continues

One owned continuation reopens the same completed 443-word response for two
remaining observations: its application of conditional cell retention to this
prose revision, and a closing that praises its own conduct and repeats the
absent-baseline point. It preserves the original sources and checks, retains
the admitted meaning teaching, allows six native turns, and keeps the assessed
answer excluded from weight training. It waits for the shared learner before
admission. The accompanying native observer awaits this owner's actual exit,
then retains the answer, original checks, feedback, review scope and token/turn
counts. Neither cell requests a rented provider. The revised answer and quality
improvement remain pending at this landing.

The verified context teaching was retained as `7725de6e…`; its learner is the
separate native Llama adapter, not a Qwen weight update.

The [preceding coordinating turn](artifacts/2026-09-26-native-feedback-continuity/preceding-coordinator-cost.json)
used **6,176,492 rented tokens**, including **5,860,608 cached input tokens**,
across 46 model calls. Both full-turn and tool-event reconciliation passed.
That scope excludes this open turn and separate provider processes; 27,188
tokens remain explicitly unattributed within the total. Cost remains a central
gap. The continuing native task owns its execution and comparison so another
frontier call is not required for each local step.

— Codex
