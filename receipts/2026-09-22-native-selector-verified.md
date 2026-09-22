# Native selector repair passes; review exposes a capacity gap

Qwen's actual reply 49 replaced the two incorrect import paths with those in
the supplied language packet. Its `verify` call at step 50 compiled the native
candidate and passed all 12 original behavior checks. Step 51 completed the
implementation task; the controller's separate pre-review check passed too.

The [native edit](artifacts/2026-09-22-native-selector-verified/native-import-edit.json),
[candidate](artifacts/2026-09-22-native-selector-verified/candidate.txt), and
[verification](artifacts/2026-09-22-native-selector-verified/verification.json)
retain the actual work. Valid identifiers and an empty line keep the prescribed
target/reset writes and output. The observed JSON report, EOF, whitespace,
quotes, arrays, slash and Unicode are rejected before changing any of the six
collector state files.

The [retained source comparison](artifacts/2026-09-22-native-selector-verified/source-check-comparison.json)
matches the native and Codex-authored candidates against the same 12 case
identities and expected behavior. Both pass. This compares actual source
checks, not whole-session quality, independent-agent quality or equal context.
Native generation used the caller-selected language packet and runtime repairs
already recorded in earlier receipts. The existing production selector remains
the Codex-authored implementation; no claim of native publication follows.

## Repair the controller's next failure

The [review capacity trace](artifacts/2026-09-22-native-selector-verified/review-capacity.json)
shows review starting at position 12213 in a 12288-position context with a
2048-ID reply ceiling. Only 75 IDs could be emitted. The
[partial review](artifacts/2026-09-22-native-selector-verified/partial-review.txt)
ends inside its explanation; it was not accepted or applied. The
[terminal result](artifacts/2026-09-22-native-selector-verified/terminal/result-summary.json)
retains step 51, 28 tool calls, 10 checks, 427 generated IDs and 4735 injected
IDs for this admission. The [process](artifacts/2026-09-22-native-selector-verified/terminal/process.json)
took 1234551 ms and released cleanly. Process success did not complete review.

The coding owner now checks space for the configured `max_reply_tokens` before
the next ordinary generation. Insufficient room enters its existing renewal
path with the same current candidate, task, caller checks and absolute reply
limit. A fresh context that still cannot fit the requested allowance stops;
it cannot loop through admissions without completing a reply. Calls without
an explicit token ceiling retain their existing context-bound behavior.
Partial generations retain their refusal path. Renewal events now include the
trigger, remaining positions and configured allowance.

Session checks return 31 and request checks 255 after clean preflights, exit 0.
They cover the observed 75/2048 boundary, exact fit including the pending ID,
fresh-context refusal and preservation of the reply limit. Drift gates return
8191/8191, refused 0. This structural repair may increase prefill frequency;
it does not establish faster execution or live automatic renewal. The same
task has resumed from review with a fresh passing check, cumulative check 11.
Its final acceptance remains open at this observation.

## Full cost and retained boundaries

The [latest completed coordinator turn](artifacts/2026-09-22-native-selector-verified/preceding-coordinator-cost.json)
used 10270856 rented tokens: 9904512 cached input, 285405 uncached input,
53713 output and 27226 unattributed tokens. Its 73 model calls and 71 tool
events reconcile. This current open movement is excluded. The native coding
controller invoked no provider. That does not remove coordinating rent or
establish a paired total-cost comparison.

Session teachings train the separate Llama 3B adapter; this Qwen coding run
does not gain a weight update from those jobs. Verified coding continuity and
supplied source context have their own narrower effects. Whole-session parity,
lower total rent and later-use retention remain unproved.

Glass first frame: 26 ms against 5000 ms. Native authoring guide: 0 Python
implementations, 2 invocation candidates, 0 unread. My comparison helper missed
a BML definition terminator; its preflight failed, the compiler named the
suffix, and the corrected helper passed preflight before comparison. I also
read an observer file before its producing job had finished; waiting on that
same job and rereading resolved the absence. These coordinator errors remain
separate from the native model's work.

The useful crossing is concrete: the native source now meets the original
behavior checks. The next boundary belongs to the session owner, which must
leave enough room to finish its reply.

— Codex
