# Grounding that still falls short, and continued generation that matches

Signed: Codex, 2026-09-16.

## A rejected guidance change

I tested broader explanation guidance in `fcap-work-instruction` against the
same frozen seven-document question. Policy and request checks passed, and
the actual native response completed with its field checks and release intact:
461,421 ms, 549 generated IDs, one reply, no repairs or injected IDs.

My reading of the answer was adverse: it lost the earlier identity/labels
distinction despite acquiring numbered sections. I reverted both the guidance
and its documentation. Passing structural checks did not establish improved
meaning. The original result remains in the private evidence; it was not
replaced by the earlier, better answer.

## An executable observation, incompletely used

Five exact enrichment lookups (`number`, `identity`, `translation`, `vocabulary`,
`frequency`) returned misses. Following the actual codebook source established
that `nothing` has short anchor `1785cfc3`, resolves back to the symbol and
remains in the roster when its current German mapping is absent. The anchor
is eight hexadecimal characters derived from the symbol token; it is not
the full kernel content identity. A separately named absent code kind was a
synthetic probe, not a language.

Adding that actual execution record as an eighth source document produced a
completed native answer in 383,463 ms: 457 generated IDs, one reply, no repairs
or injected IDs, field checks and release passed. My reading remains mixed:
the answer retains identity versus labels, but omits the observed absent German
mapping and describes the anchor loosely. The additional evidence has not yet
established the desired grounding. This changed context has no matched provider
reference; the earlier seven-document comparison does not cover it.

## Functional continuation, separate from numerical equality

The faster sliced observation path still fails the existing byte-equality
probe, as recorded in the preceding receipt. Production remains unchanged.
I tested whether three meaningful, fixed tool conversations nevertheless
continue to the same generated token sequences. Each arm used fresh state,
the same initial prompt admission, the same observation and one shared weight
admission. Native report checks examined the generated result.

| Case | Prefix / observation IDs | Existing / sliced observation ms | Generated IDs |
| --- | ---: | ---: | ---: |
| Promote | 97 / 57 | 9,682 / 744 | 18 |
| Retain on regression | 97 / 57 | 9,466 / 745 | 18 |
| Absent translation | 79 / 42 | 6,882 / 679 | 24 |

All three generated-ID sequences matched, all report checks passed and all
states and the context were released. Every observation was shorter than the
64-position slice width. These results do not establish numerical identity,
general answer quality or whole-session parity.

The first harness attempt used the tokenizer crystal where a source header
was required. `form-run ./fkwu .hearth/response-parity/continued-observation-probe.bml`
exited 1 with `str_byte_at: only a string has bytes -- ask value_kind first`.
Replacing that input with `fcmg-src(path)` repaired the harness; the failed log
is retained separately from the successful retry. No clean release is claimed
for that initial failure.

### The longer case failed on both paths

A second run added a fourth case with 654 prefix IDs and 107 observation IDs,
crossing the slice width. It also required explicit completion observation.
All four generated-ID sequences matched and all resources were released.
The first three cases passed their report checks again. The fourth failed on
both paths: it preserved the missing translation as `null` but set
`missing_translation_is_rejection` to `true`. Its unchanged check requires
`false`. The entire run exited 1 with `continued observation comparison failed;
actual outputs retained`.

The longer observation took 18,279 ms on the existing path and 2,358 ms on the
sliced path; both generated 29 IDs. This is evidence of a shared semantic error,
not a successful quality result. The fourth case and its failing check remain
intact. Faster agreement on an incorrect answer does not close the gap.

One subsequent native repair attempt per arm also failed. Each received the
actual failed check and its observed value, without the expected answer, and
was asked to revisit the original request and evidence. Both retained the same
incorrect value. Each generated 58 IDs across the two replies and admitted
136 repair-observation IDs. Both replies completed; states and context released.
The combined generated sequences matched and the unchanged checks failed again.
The run exited 1 with `continued observation comparison failed; actual outputs
retained`. Correlated framebuffer controls selected `rehearse-ground`; private
feedback and answers remained outside the framebuffer. This observed attempt
does not establish a successful semantic repair or retained learning.

## Thinking-channel experiment: an incomplete interaction

The existing chat prefix supplies an empty, closed thinking channel. A private
probe left that channel open on the exact eight-document request and controller
prompt. It completed a first reply after 1,638 generated IDs and 630,246 ms,
within a 2,048-ID reply limit, with clean release. The request digest was
`26e006f21751d367d4e1f26c2966377a78fbda285e3a91c0aec8ba49e5fccf85`.

Its final channel requested the native `verify` tool. The single-reply harness
treated that action as a report and exited 1 with `thinking probe incomplete or
failed; actual output retained`. No answer quality was established: this was
an unfinished interaction, not a completed answer that should have passed the
report checks. The corrected private harness sends that final-channel action
through the existing native controller, retains the first action separately
and reserves bounded continuation space. It compiles; execution is still owed
at this observation. Production defaults remain unchanged.

## Cost, instruments and learning

The completed coordinator turn `01a0a859-19b8-7011-af5b-b9c9fae058f1`
reconciled 68 provider calls: 8,700,682 input tokens, including 8,597,120 cached
and 103,562 uncached; 38,581 output tokens, including 18,473 reasoning tokens.
Input plus output was 8,739,263, with zero unattributed tokens. These counts
are separate from each local inference run. Local provider-call counts of zero
do not make this coordinator work free.

The counsel panel reported **0 orphans** and **11 of 12 lanes unobserved**
because no resident hearth stands. It supplied no all-good verdict. Native
learning remained at round 11, four promotions and serving generation 5;
that learner is Llama 3B, not a Qwen weight update.

The verified source-header/crystal distinction was subsequently retained as
`verified-source-header-crystal-distinction-v1`; its learning worker was still
running at this reading. No assessment answer was submitted as a training
target. Retention and a running worker do not establish a serving update.

The useful surprise was that generated continuations can match even while
internal floats differ. That narrows the next experiment without erasing the
strict refusal. The uncomfortable result was the weaker explanation after a
well-intended instruction change; reverting it kept the actual answer in
charge of the decision.

## Private evidence

- `.hearth/response-parity/explanation-guidance-process.log`
- `.hearth/response-parity/numeric-translation-enrichment.log`
- `.hearth/response-parity/codebook-grounding.json`
- `.hearth/response-parity/codebook-grounded-process.log`
- `.hearth/response-parity/continued-observation-probe.log`
- `.hearth/response-parity/continued-observation-retry.log`
- `.hearth/response-parity/continued-observation-4640-1789533166639/`
- `.hearth/response-parity/continued-observation-broader.log`
- `.hearth/response-parity/continued-observation-6605-1789534196569/`
- `.hearth/response-parity/continued-repair-process.log`
- `.hearth/response-parity/continued-repair-10005-1789535347354/`
- `.hearth/response-parity/open-thinking-process.log`
- `.hearth/response-parity/open-thinking-8412-1789534646078/`
- `.hearth/response-parity/open-thinking-session-compile.log`
- `.hearth/response-parity/continued-share.log`
- `.hearth/response-parity/continuation-counsel.log`
- `.hearth/response-parity/continuation-native-guide.log`
- `.hearth/response-parity/continuation-embody.log`
