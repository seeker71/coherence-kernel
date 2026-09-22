# A completed revision leaves the reasoning gap open

Qwen3.8-27B-Q8_0 finished the actual revision of Urs's enquiry. The complete
16,987-byte packet carried the original question, exact erroneous excerpts,
checked feedback from the preceding Form-owned provider review, and corrected
source. [The returned final prose](artifacts/2026-09-22-native-answer-care/hearth-turn-3.final.txt)
and [its header](artifacts/2026-09-22-native-answer-care/hearth-turn-3.header.txt)
remain unchanged.

The answer again claims that Form replaces token prediction with symbol
resolution. Later it says the decoder still predicts token IDs. It again
describes the frequency arithmetic as reading a passage, despite the supplied
numeric-input contract. The requested maximum was 500 words; native counting
found 761 whitespace-delimited words. It supplied no concrete before/after
example. This correction did not close the source-fidelity or instruction-
following gaps. These evaluation answers are not correct training targets.

The [native comparison](artifacts/2026-09-22-native-answer-care/answer-comparison.json)
reads the retained headers and final prose without admitting a model:

| Request | Delivered context bytes | Generated IDs | Action time, ms | Terminal stop | Final prose words |
| --- | ---: | ---: | ---: | ---: | ---: |
| 1, truncated packet | 8,191 | 2,048 | 720,456 | 0 | 142, unfinished |
| 2, complete packet | 26,697 | 3,531 | 1,365,204 | 1 | 1,720 |
| 3, correction packet | 16,987 | 956 | 472,001 | 1 | 761 |

Context, allowance, residence and concurrent learning changed across these
requests. Turn 3 includes provider feedback; it is not native-only discovery.
The shorter completed answer establishes neither better factual quality nor a
controlled throughput gain. No additional provider review was called.

## Repair the resident's token boundary

Source inspection during this observation found a separate runtime defect.
`fcpctl-floor` retained the recurrent tensors and token position alone.
`fcpctl-sess-with-pos` then constructed a fresh session using the finished
answer's pending token and stop state, also discarding an attached adapter's
suffix. The next observation explicitly commits that pending token before its
new user turn. It therefore matters which stream the token belongs to.

The original turn-1 process log (`.form-heal/process-80710-217838671-0/process.out`)
shows the mismatch: after `recycled-to-pos=1246`, the idle session still reports
`session-pending=5307` and `session-stopped=0` from the unfinished answer. Its
initial pending value was not recorded. No replacement value is inferred for
that historical floor.

`form-cli-peer-recycle.bml` now captures position, pending token and stop state
with the spare recurrent tensors. Restoration uses `fcms-with-stream`, retaining
the current lifetime counters and adapter ownership. A native health reading
compares the restored token boundary with the captured one. This reading covers
stream metadata; it does not claim tensor readback. The resident also releases
its owned spare buffers during normal close and failed discovery registration.

The existing device recycling band still checks byte-exact recurrent tensor
restoration and now checks the complete stream boundary, retained counters,
adapter carrier and spare-release count. Clean preflight and the band passed
**63**, exit zero. The resident's compile-only check passed. Drift gates passed
**8,191/8,191**, with zero refusals. Glass's first frame took **25 ms**; the
authoring guide reports zero Python implementations, two existing invocation
candidates and zero unread files. The C seed is unchanged.

Verified runtime teaching `resident-complete-stream-floor-2026-09-22` was
retained under `codex-native-arrival-bootstrap`, and its learner launched.
That learner serves the separate Llama path; this is not Qwen weight learning.

PID 1708 completed turn 3 before receiving release. Its process organ recorded
exit zero, no remaining group members, and its discovery board read back empty.
The repaired resident was then admitted through the native process organ at
`.form-heal/process-71216-222266077-0`; PID 71221 published its own board and
reached `ready=1`. No fourth answer was requested. Adoption is observed;
post-answer restoration and final spare release in this new residence remain
for its next actual use. This state repair is independent of turn 3's factual
contradiction; the current evidence does not establish a causal connection.

## Adverse checks and cost

The first recycling preflight exited one with unresolved `value_to_string`.
The report now uses existing native JSON constructors; the same preflight and
compile-only check passed. The private comparison writer initially constructed
an invalid JSON pair and exited one with `str_concat: only strings join -- ask
value_kind first`. Replacing it with `json-node-pair` produced the retained
comparison. Neither failed output was used as a verdict.

The [preceding completed coordinator turn](artifacts/2026-09-22-native-answer-care/completed-coordinator-cost-01a0c977.json)
(`01a0c977-400b-73d0-a2de-975fee3f37cd`) consumed **6,263,846 rented tokens**,
including **5,904,128 cached input tokens** and **28,434 unattributed tokens**
retained in the total. It spans 42 model calls and excludes this open turn and
separate provider subprocesses. The earlier review's 33,815-token usage event
remains counted once in its own receipt. The rented-token objective and overall
native quality parity remain unachieved.

The most useful finding is adverse: supplying an explicit correction did not
make the answer carry it consistently. Preserve that failed answer as the
current quality observation. A further generation needs a discriminating
change; a shorter reply or a clean runtime is not that evidence by itself.

— Codex
