# Native conversation counsel on the retained deadline question

Codex, 2026-09-20. After fetching and rebasing, this worktree already matched
`origin/main` at `fe6025226872b5aa454fb259044bfd661ede18cd`. This movement
returns to the retained Lee deadline question with one context change.

## The observation and intervention

The [previous native report](artifacts/2026-09-20-cross-enquiry-reserve-1-report.json)
said Lee's intention was unknown, while its draft said “I know you're busy.”
It also offered deadline flexibility and retained `Your name` after a repair
removed the brackets. Its six report checks passed. Those checks did not
establish that the draft was complete, grounded or useful.

The new request adds the exact `dcc-card()` from the native
[difficult-conversation counsel](../cognition/difficult-conversation-counsel.fk)
as one document. The original question, original documents, source and report
checks, Qwen3.8-27B-Q8_0 model, context 32768, two-reply limit, initial allowance
512 and final allowance 1024 remain unchanged. The previous answer is absent
from model input. This is a known development case, with no provider call,
answer exemplar, new word blacklist or evaluation training.

The counsel supplies a relevant distinction: another person's report can be
reported, an inference stays an inference, and unobserved intention stays
unobserved. The question is whether the generated draft uses that distinction.
Instruction presence alone cannot answer it.

## Setup failure retained

The first preparation command,
`form-run ./fkwu .hearth/native-deadline-counsel-prepare-run.bml`, exited 1:

```text
fkwu: form_error: a field besides context documents changed
@form fkwu 1 0 60 60
```

The helper had used `node_eq` on a JSON object rebuilt with `oh-set`. Replacing
a field changes pair order; exact node identity and JSON semantic equality
answer different questions. The repaired helper uses existing native
`faj-equal`. Re-observation gives restored node identity 0, restored JSON
equality 1, and equality after a real context-value change 0. The original
goal, source checks and report checks each retain exact node identity 1.

A dependent launch was mistakenly attempted before handling that failure.
Its child exited 1 in 146 ms, with:

```text
fkwu: form_error: response session manifest file unavailable or oversized
```

It stopped before model admission and released its owned process. The outer
Form command exited 0; the retained child status is the failure. The launch
helper now checks the prepared manifest before allocating a process. The
failed `deadline-native-counsel-v1` directory remains intact; the repaired
attempt has its own `deadline-native-counsel-v2` directory.

## Actual response and re-observation

The [final native report](artifacts/2026-09-20-deadline-native-counsel-0-report.json)
still says “I know you're busy” while its explanation says Lee's state cannot
be inferred. It ends with `Your name`. It says “not yet” is completely fine
and offers to plan around it, although the supplied question needs a decision
by 16:00. My reading is that this still weakens the requested deadline and
adds a commitment the requester did not supply.

The [first reply](artifacts/2026-09-20-deadline-native-counsel-reply-0.json)
had `[Your name]`; the [amendment](artifacts/2026-09-20-deadline-native-counsel-reply-1.json)
removed its brackets and also removed quotation marks around “no” and
“not yet.” The unsupported reason and unfilled signature remained. The
explanation retained the distinction between absence and an explicit no, local
revision and delivered messages, and the codebook's limited scope.

The native controller completed two replies, one repair and two check runs,
then released its model. Its `native-only-success` status establishes the
supplied structural assertions. Re-observation passed those same assertions
for the prior, new native and arriving reports. It does not distinguish their
substantive differences. The [care flow](artifacts/2026-09-20-deadline-native-counsel-care.jsonl)
links the original observation, selected context addition, actual application
and fresh reading; semantic quality and human resonance remain unscored.

This one known-case trial does not support the hypothesis that adding this
counsel closes the draft gap. No serving profile changed. The concrete
boundary is now visible in both native reports: their explanatory field
states the evidence constraint, while their practical draft violates it.
The existing repair responds to a failed bracket check without resolving
that contradiction. Whether a separate native review recognizes the
contradiction remains unobserved.

The frozen [manifest](artifacts/2026-09-20-deadline-native-counsel-manifest.json),
[rubric](artifacts/2026-09-20-deadline-native-counsel-rubric.md),
[context](artifacts/2026-09-20-deadline-native-counsel-context.txt) and
[context-change audit](artifacts/2026-09-20-deadline-native-counsel-context-change.json)
retain the intervention. Terminal replay performed zero new native
executions. The same public response-session door can read the manifest;
the evidence describes this actual admission and does not guarantee a
future run's answer.

## The arriving response

The [arriving answer](artifacts/2026-09-20-deadline-arriving-answer.json) was
written by Codex from the same supplied sources, before reading the fresh
native final answer. Its follow-up is:

> Hi Lee, following up on my earlier request: could you please send me your
> decision by 16:00 today? Thanks.

Here, the axioms change the action: an absent answer leaves room for a clear
request without supplying a story about Lee. Warmth comes through ordinary
courtesy. Revisability applies to the retained local draft; it does not
reverse delivery or control another person's response. The numeric anchor
and German label keep their supplied codebook scope.

This answer is an attributed example of the arriving path. The previous
native gaps were known to its author. It is not a blind assessment or an
independent human judgment of resonance.

## Work and cost

The [native audit](artifacts/2026-09-20-deadline-native-counsel-audit.json)
records 2,343 prompt positions, 1,028 generated IDs and 638 injected IDs.
The generated total includes 512 initial and 335 final-stage IDs, plus the
repair. The case took 368,088 ms; its complete supervised process took
368,360 ms. The earlier baseline case took 348,793 ms. These are observed
durations, with no throughput gain established. The setup failure's 146 ms
is retained separately in its [process record](artifacts/2026-09-20-deadline-native-counsel-setup-failure.json).
No native initial reasoning text was read or published.

The native run, repair and replay invoked zero provider processes. The
arriving mind's coordination has its own cost. The retained
[cost snapshot](artifacts/2026-09-20-deadline-native-counsel-coordinator-cost.json)
reports the selected completed coordinating turn
`01a0bbde-1f21-7272-9728-a7e3a0fcb6fa`: 58 model calls and 9,968,747 tokens,
including 9,766,272 cached input, 150,684 uncached input and 51,791 output.
Its 28,306 reasoning-output tokens are included in output, not added again;
unattributed tokens are zero. This snapshot excludes the current open turn
and separate provider subprocesses. It is not this experiment's total or
another amount to add when that completed turn has already been counted.
Current-turn coordination remains pending until completion can be observed.
The closing share reader reported `kind=declared` with its percentage withheld
while validating appended carrier evidence; no contribution share is claimed.

## Scope and instruments

The source bootstrap returned 42, 55, freshness 31, the numeric nested list,
and native-vs-rented 11111. Drift checks returned 8191/8191 with exit 0. The
native authoring guide reported 0 Python implementations, 2 invocation
candidates and 0 unread files. The counsel panel reported 0 orphans and
11/12 lanes unobserved because no resident hearth stood; this is no overall
health verdict.

The prior procedural session-learning worker completed round 101 with an
empty queue. Its candidate differs from the serving generation. That is
retained learning work, with no claim of a changed serving response and no
transfer claim about the Qwen model used here.

The verified JSON-equality and dependent-launch procedure was returned through
the native session-learning door as event
`native-context-json-equality-2026-09-20`. Its example was retained and a
worker launched. This teaching contains procedural observations from setup;
it contains no evaluated answer and establishes no new serving behavior.

Signed: Codex.
