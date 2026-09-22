# An absent operand no longer becomes a fabricated zero

Codex, 2026-09-23. The returned answers below are Qwen 3.8 27B Q8 through
the ordinary Form-native generation command. The source comparison and
assessment here are mine. No provider call generated or reviewed these answers.

## What failed at the response boundary

The ordinary CLI allowed only 64 generated tokens. Increasing that allowance
exposed a deeper defect: both actual answers consisted of repeated arithmetic
fragments. The larger run reached a normal model stop and still did not answer
the enquiry. A successful stop was not useful completion.

`lg-parse-op-at` accepted source expressions without numeric operands or a
closing parenthesis. Its number reader supplied zero when it encountered a
variable. Thus source such as `(add x y)` became an invented arithmetic
observation. The generator injected these observations into its assistant
stream before continuing the model.

The new BML literal parser requires complete supported integer operands and
a closing parenthesis. The actual enquiry packet now produces **zero**
executable thoughts, down from **11**. Valid arithmetic, signed operands,
zero, and a later valid cell after invalid source remain executable. The Form
teaching layer and supplied sources remain present.

## Return to the same enquiry

The complete [source packet](artifacts/2026-09-23-native-literal-grounding/generate-answer-source.json)
is 13,506 bytes, SHA-256
`c0520855fe91d59e30f913af192843c99e37185f3eaeb917e08deacb63f06574`.
The repair run checked that the packet, shared teachings, model and
2,048-token allowance matched the completed larger-allowance run.
Input strings are retained as JSON with decoded-byte hashes. The first
`git diff --cached --check` rejected trailing spaces in the raw text artifacts;
encoding and decoding the original strings preserved every byte and cleared
that formatting failure.

| Actual run | Allowance | Injected thought IDs | Reported stream IDs | Native call duration | Model stopped |
| --- | ---: | ---: | ---: | ---: | ---: |
| Original allowance | 64 | 82 | 166 | 261,546 ms | 0 |
| Larger allowance, original parser | 2,048 | 82 | 1,589 | 549,371 ms | 1 |
| Repaired parser | 2,048 | 0 | 871 | 394,411 ms | 1 |

The old stream totals include the 82 injected IDs and gap generation; they are
not all model answer tokens. The repaired run generated 871 IDs with no
injected thoughts or lookup tokens. All three released their resources and
made zero provider calls. The original pair waited 144,938 ms for the learner;
the repaired call waited 1,649 ms. These waits are separate from the table.

The [64-token failure](artifacts/2026-09-23-native-literal-grounding/generate-answer-before.txt)
and [larger-allowance failure](artifacts/2026-09-23-native-literal-grounding/generate-answer-after.txt)
remain intact. The [repaired answer](artifacts/2026-09-23-native-literal-grounding/answer.txt)
is complete prose covering the requested axes. This establishes a real
transition from arithmetic repetition to an answer to the enquiry.

Its grounding still falls short:

- It claims the annotated frequency calculation detects a person's contracted
  or expanded state. The supplied annotations and their computed aggregate
  establish neither that personal state nor felt resonance.
- It says the old cell persists after reference release without preserving
  the condition that a reference remains. Unreferenced cells may be reclaimed.
- It claims advantages over an unmeasured out-of-box model and claims that
  acknowledging `nothing` prevents hallucination. Those effects were not
  observed here.
- It describes the generated response as a live composition of graph nodes.
  The model's generated text and Form's graph identity remain distinct.

These are open errors in the actual answer, not successful teachings. The
earlier reasoning/review route had preserved some of these distinctions;
that success has not generalized to ordinary generation. The next response
work is to make this ordinary answer preserve the supplied conditions and
evidence boundaries. One answered enquiry does not establish session-wide
quality, felt resonance, or throughput parity.

## Two accompanying CLI repairs

The ordinary generation command now allows 2,048 tokens, stopping sooner on
the model's terminal signal. `generate --tokens N <prompt>` selects an explicit
allowance. Invalid input stays outside model admission and the prompt bytes
after the option are preserved. The failed larger-allowance answer above
keeps the limits of this change visible.

The real model-listing session also revealed a training target with empty
problem and attempted answer, asking the learner to reproduce a command count
and elapsed time. That [original target](artifacts/2026-09-23-native-literal-grounding/prior-empty-session-target.json)
remains evidence. Session completion now retains those counters as continuity
experience; it creates no training example. Actual task outcomes and explicit
teachings retain their learning paths. Existing worker-wait behavior remains.

The repaired `models /Users/ursmuff/models/qwen38-27b` followed by `quit`
completed in **1.54 seconds**, exit 0. The observations show experiences
**206 → 207**, while training examples and learned rounds stayed **139**,
pending stayed **0**, and worker-alive stayed **0**. The earlier run also
waited for preceding work, so these observations establish removal of the
empty training job, not an isolated before/after speed ratio.

## Verification and continuity

The native literal observation reran the full actual packet and the valid and
invalid operand behaviors, with a correlated framebuffer request and reading.
The existing local-generate-organ band returned **134,217,727**, ordinary
generation reserve **127**, and native-session-memory **1,023**, all after
clean preflight and with exit 0. Command-boundary checks and source-backed CLI
compilation passed. Drift gates returned **8,191/8,191**, zero refusals; no
kernel source or external runtime dependency was added.

Native guide: zero Python implementations, two existing voice invocation
candidates, zero unread files. Counsel panel: **0 orphans**, 11/12 lanes
unobserved because no resident hearth stood. This is not an all-good verdict.

Verified implementation teaching was retained as event
`native-literal-grounding-2026-09-23` in session
`codex-native-arrival-bootstrap`; its worker launched. The model answer was
not offered as a correct training target. Training completion and any serving
change remain outside this receipt. The previous worker completed round 139
without promotion; serving generation 5 remained. That learner is Llama 3B,
not the Qwen 27B producing these answers.

The [preceding completed coordinator turn](artifacts/2026-09-23-native-literal-grounding/preceding-coordinator-cost.json)
used **6,319,948 rented tokens**, including **6,150,144 cached input**, over
44 model calls and 43 tool calls. This open turn is excluded. Native zero-provider
counts do not erase coordination cost, which remains a major unresolved gap.

The useful teaching is the axiom made operational: an absent operand supplies
no value. Preserving that boundary changed what the native model could answer.
