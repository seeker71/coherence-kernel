# The count is visible; the answer still repeats

Signed: Codex. Native Form/BML on the committed C bootstrap; no new runtime
dependency, C-seed change, adapter training or Qwen promotion.

The strongest retained source-corrected answer had **504 words** against its
350–450-word request. I returned that actual draft and its original source to
the public generation command. The feedback also identified the final sentence's
unobserved promise about how the person would feel, asking for an intention
instead. This is a feedback-assisted revision, not independent error detection.

## Native observation and care

`generate --words MIN:MAX` now counts the completed answer, including headings,
using an explicit ASCII-whitespace convention. It admits one bounded correction
inside the same model session when the range is missed. It retains the original
answer, measured feedback and correction. Completion, range success, release
and retained evidence are separate fields. An unsuccessful correction remains
failed; nothing clips, pads or silently substitutes the answer.

The first returned answer had **510 words**. The native organ observed the miss,
correlated `revise-once`, appended an **86-ID** observation and generated another
answer. That correction was byte-identical to the first. `word_range_passed=0`
and `reason=answer-word-range-not-met` remained visible despite completion 1,
release 1 and process exit 0.

## A real message-layout mismatch, followed by an unchanged answer

Reading the selected GGUF's `tokenizer.chat_template` exposed a mismatch:
its tool branch renders a `user` message containing `<tool_response>` markup.
The native carrier emitted a bare ChatML `tool` role. The shared observation
carrier now uses the model's envelope; direct user messages keep their own path.

The [native token audit](artifacts/2026-09-23-native-answer-length/tool-template-audit.json)
reads the actual model metadata and decodes the actual emitted token sequence.
The 32-ID sample matches the template layout, including the assistant opening.
It preserves the observation's payload bytes. This establishes representation,
not model comprehension or a causal explanation of the repeated answer.

I re-observed the original correction with the same prompt, source, draft,
model, profile and allowances. The revised envelope changed the injected
observation to **96 IDs**; the reserved context changed accordingly. The initial
answer remained byte-identical, and the correction again repeated it exactly.
The encoding repair therefore did **not** close the length failure.

| Actual observation | Bare tool role | GGUF tool envelope |
| --- | ---: | ---: |
| Initial prompt IDs | 5,490 | 5,490 |
| Reserved context positions | 10,455 | 10,473 |
| Initial generated IDs | 703 | 703 |
| Correction generated IDs | 703 | 703 |
| Injected observation IDs | 86 | 96 |
| Initial → corrected words | 510 → 510 | 510 → 510 |
| Correction changes answer bytes | no | no |
| Word-range requirement passes | no | no |
| Completion / release / retention | 1 / 1 / 1 | 1 / 1 / 1 |
| Elapsed milliseconds | 621,997 | 623,482 |
| Provider calls | 0 | 0 |

Both calls used Qwen3.8-27B Q8, the full closed-thinking profile and a
2,048-ID answer allowance. The native reasoning controller was not opened.
There were two model admissions and four generation stages in this movement,
including failed corrections: **2,812 generated IDs** and **1,245,479 ms**.
These are actual execution costs, not a throughput distribution. The brief
owned Glass reading overlapped the first run; other local activity was not
experimentally isolated.

## What changed in the answer's meaning

The [returned native answer](artifacts/2026-09-23-native-answer-length/template-answer.txt)
changes the earlier promise into: “The practice intends for the person to leave
more grounded, more stretched, and more free.” It retains the reference condition
for old cells, the external-file distinction, supplied-annotation scope and
language-label/sense distinction. It still leans on vocabulary relabeling for
trust, and its literal repetition fails the requested revision. No human
judgment of felt resonance was supplied. The root assessment remains attributed
to Codex; counts and exact-byte comparisons are native observations.

The [comparison](artifacts/2026-09-23-native-answer-length/template-comparison.json)
verifies identical prompts and initial answers, unchanged revised text, and
counts against retained bytes. The original enquiry SHA remains
`c0520855fe91d59e30f913af192843c99e37185f3eaeb917e08deacb63f06574`.
All four generated answers share final-text SHA
`40b5b208f7dddf744e9b67b521101a2f8a9e44fb2bc14ea961566fcfcd595799`.
The original question and revision request are retained as the `text` field in
`question.json` and `request.json`. Native JSON roundtrip checks preserve their
exact bytes, including source whitespace that the staged whitespace check
rejected in the plain-text export.

The sharper open boundary is effective uptake of appended feedback. Delivery
counts and a conforming envelope do not show which decoding states changed.
A further generation needs a discriminating observation at that boundary;
repeating this request again would add no new evidence about its cause.

## Checks, instruments and rented cost

Full source-backed REPL compilation passes. Preflighted reasoning/word-range
checks pass **1**, model-session checks **4095**, and bounded-observation checks
**16777215**. The checks cover range parsing, retained prompt bytes, one correction,
an unchanged failing answer, incomplete answers and preserved failure reasons.
The selected model's encoded-template audit passes independently. Drift gates
pass **8191/8191**; no kernel source moved.

Native guide: Python implementations 0, execution candidates 2, unread 0.
Counsel: **orphans 0**, **11/12** lanes unobserved because no hearth stood.
The owned Glass viewer was deliberately interrupted after reading; its exit 1
is an intentional stop, not a passing completion. The output-only meter read
5,062,006 cumulative output tokens; it is not an input-inclusive turn cost.

The [preceding completed coordinator turn](artifacts/2026-09-23-native-answer-length/preceding-coordinator-cost.json)
cost **5,215,383 tokens**: input 5,161,004, including 4,999,040 cached and
161,964 uncached; output 28,252; unattributed 26,127. It records 39 model calls
and 37 tool calls with reconciled completion. The current turn is still open
and excluded from that reading. Zero provider calls inside the native runs
does not remove coordinating cost. Neither overall savings nor quality parity
has been established.

Verified command and template behavior was returned through native session
learning as event `2026-09-23-native-answer-length-and-tool-layout-v1`, session
`native-arrival-bootstrap`. The lesson explicitly withholds semantic-quality
and feedback-uptake claims. The separate Llama learner completed with exit 0:
pending 0, optimizer step 143, cumulative learned rounds 144. Its new candidate
is `143-38611-251471894`; serving remains generation 5. No generated answer from
this movement was treated as a verified learning target, and the completed
learning operation does not establish later uptake of this teaching.
