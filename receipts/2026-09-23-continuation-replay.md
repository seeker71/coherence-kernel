# Fresh prefill retains the same failed revision

Signed: Codex. The preceding turn established that the correction repeated all
812 generated IDs. This movement compares that continuation with fresh prefill
of the same transcript, using Qwen3.8-27B Q8 through the native C-seeded runtime.

The [native replay](artifacts/2026-09-23-continuation-replay/continuation-replay.bml)
checks its composed prompt against the retained serving prompt byte for byte.
It retains the indexed prompt IDs, original generated IDs and exact correction
IDs in [prefix.json](artifacts/2026-09-23-continuation-replay/prefix.json):
4,148 + 812 + 86 = **5,046**. It verifies the model seal, tokenizer completion,
positions, feedback bytes and context capacity before admission. Reading the
actual correction IDs back as text confirms the assistant close, user request
and fresh assistant opening with the ordinary empty thinking section.

The original continuation advanced through generated steps and appended
feedback. The replay feeds the complete retained prefix through fresh span-64
prefill; this also changes batching boundaries. It holds the model, profile,
content, context capacity and 2,048-token answer allowance fixed.

[Result](artifacts/2026-09-23-continuation-replay/comparison.json): **812 IDs,
all identical to the incremental revision; 635 words; range success 0**.
Both end at position 5,858 with stop ID 248046. Completion and release are 1;
elapsed time is 439,836 ms; provider calls 0. The full answer and generated
sequence remain alongside the receipt. Rebuilding state did not improve this
answer. This comparison does not prove every inference operation correct.

The observation changes the next action: supply concrete source corrections
for the answer's prediction/witnessing contradiction, engagement reversal and
self-assessment. Repeating the same generic length request adds no distinction.

## Concrete source correction changes the answer

The [next native call](artifacts/2026-09-23-fidelity-repair/fidelity-repair.bml)
retains the original prompt and answer IDs and replaces only the feedback.
Codex supplied three source-directed corrections: keep prediction distinct
from grounding, serve the enquiry, and replace self-assessment with a practical
consequence. This is added caller guidance, not autonomous native discovery.
Its complete [request](artifacts/2026-09-23-fidelity-repair/request.json) remains
available. The prefix is 5,201 IDs, including 241 feedback IDs; the same model,
context capacity, prefill route and answer allowance remain in use.

Qwen now opens: “Form does not replace Qwen’s prediction. The model still
encodes prompt bytes and predicts output token IDs.” It states the enquiry
covenant correctly and explains the difference from keeping someone talking.
These are observed corrections to two source errors. The
[complete answer](artifacts/2026-09-23-fidelity-repair/answer.txt) also expands
from **635 to 756 words** (964 generated IDs), remains outside the requested
range, and ends with an overbroad assurance about releasing what is no longer
held. The axiom preserves referenced cells; it does not guarantee retention
of unreferenced cells or external effects. Self-assessment is reduced but
“The distinction ... is clear” remains. Overall answer quality is not closed.

The [result](artifacts/2026-09-23-fidelity-repair/result.json) records changed
IDs and text, completion 1, release 1, range success 0, provider calls 0 and
488,334 ms. This establishes that specific content feedback can change this
response. It does not support a universal claim that native feedback is ignored.
The next implementation addresses the word-range controller's direction:
shorten an overlong answer by removing repetition; expand a short answer with
useful missing explanation. Its reserve must cover both instructions.

## Cost and learning boundary

The [preceding completed coordinator turn](artifacts/2026-09-23-continuation-replay/preceding-coordinator-cost.json)
used **12,948,583 rented tokens**: 12,520,704 cached input, 350,360 uncached
input, 51,063 output and 26,456 unattributed. Its 98 model calls and 95 tool
calls reconcile. The present turn is excluded. These costs remain part of the
gap; zero provider calls inside native inference does not erase coordination.

The previous teaching command ended with exit 1. Its worker reported:
`native training whole row awaits memory; line=1 required_additional_bytes=341213194880`.
Worker alive is 0, pending rows 3, optimizer step 145 and learned rounds 146.
The preceding sequence-evidence teaching has not learned. Inference waited
for actual process release and did not change or drain that failed queue.

Glass first frame: **26 ms**, then an explicit Ctrl-C. Counsel: **orphans 0**,
11/12 lanes unobserved, no standing hearth. Bootstrap freshness is 31; ground,
recursion and numeric-list checks return their expected values. The native
guide reports Python implementations 0, execution candidates 2, unread 0.
These reads overlap inference; wall times are not isolated speed comparisons.
A guessed layout-contract source path returned exit 2; source search supplied
the actual native path. No runtime patch was made from that failed lookup.

The output-only meter reads 5,263,113 cumulative output tokens; it is separate
from the input-inclusive turn cost above. Share remains declared, with the
percentage withheld during append-range validation. The new native coding
request's source assertions were changed to literal matching before model
admission so function parentheses are not treated as unsupported regex syntax;
the original request is retained locally. A guessed definition-helper glob also
returned no match; the documented bounded definition checks were read instead.

Both observation helpers compile, and their real executions retain completion
and release. Drift gates pass **8191/8191** and `git diff --check` is clean.
The native controller coding request is running with its owned checkpoint;
these observations land independently while that implementation continues.
Its publication, behavioral checks, real response comparison and verified
closing teaching remain part of the open coding movement.
