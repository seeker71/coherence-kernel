# Native response correction: direction and delimiter care

Codex, 2026-09-23. Continuing the same response-quality gap recorded in
[the previous comparison](2026-09-23-continuation-replay.md).

## A real edit stopped at its response delimiter

The local Qwen coding task proposed directional word-range feedback, then
repeated an invalid JSON edit. Its owned checkpoint reached six turns, one
successful read and five syntax failures. PID 64826 was stopped with exit 143;
the checkpoint and raw replies remained available. Three retained replies were
byte-identical at 1,836 bytes. No source edit had reached the tool boundary.

The reply closed its root object before the final `input` member. Codex repaired
the native JSON care path in BML. At this boundary it removes the parser-proven
root closer only when all remaining bytes form one strict object with unique
decoded top-level keys. String values stay untouched. Duplicate fields,
separate JSON values and unfinished output remain refused. Ordinary role,
writable-path, exact-edit and caller-check boundaries still apply.

Re-observing the actual retained reply removed byte 1,823 (`}`), distinct from
the error's comma at byte 1,824. The original Qwen edit then ran once and
returned `edited`. The repair records the actual byte operation and both hashes;
correlated syntax health moved from 0 to 1. This required no model generation or
provider call. All prior failures remain counted. The private checkpoint was
backed up before native care saved its next state.

[Raw reply and correlated care](artifacts/2026-09-23-root-closer/care.json)
retain the result. JSON admission establishes no source correctness: compiling
the edited candidate returned exit 2, `source-compile: mismatched form.bml
delimiter: }`.

The native coding task resumed from the same owned contract, read its candidate,
and returned an edit with identical old and new text. The tool refused it. PID
67827 was stopped with exit 143. Its last progress sample recorded 1,589
generated IDs, 1,063 injected IDs and 636,635 ms for this resumed run. The
checkpoint retains ten turns, four tool calls, seven failures and zero checks;
it remains unfinished. These are not aggregate generation counts for both runs.

Codex then completed the executable controller repair. The final text carries
the native proposal's shortening/expansion wording, with a shared BML composition
that compiles. It retains the observed count and caller range, aims at the
range's midpoint and asks for either removing secondary detail or adding useful
explanation. The reserve takes the longer of both actual message branches and
retains decimal and role-crossing room. The one-revision limit, word counter,
admission checks and completion/release boundaries are unchanged. This is an
assisted source change, not a completed autonomous native coding task.

## Checks and open work

- `form-cli-code-syntax-band.bml`: preflight clean; verdict 1, exit 0. Coverage
  includes existing insertion/removal behavior, duplicate escaped keys,
  unfinished output, exact offset attribution and correlated health.
- The first preflight and band failed because the older witness supplied a
  rejecting callback where implementation completion now runs source checks.
  The witness now checks both that pre-review call and rejection at final
  acceptance. The production check boundary was preserved.
- The care helper's first compile failed on a missing closing parenthesis in
  its diagnostic print. That authored error was repaired before execution.
- The stop-evidence helper initially assumed a literal replacement, while the
  actual command used the supported document-hash replacement form. Re-running
  that exact command against retained documents establishes `edit-unchanged`.
  A mistyped result accessor (`fat-docs`) was also corrected to the existing
  `fat-documents` binding. These coordinator failures remain part of this work.
- The whole source-backed CLI compile check returned exit 0.
- `form-cli-reasoning-budget-band.fk`: preflight clean; verdict 1, exit 0.
  It checks both directions, midpoint, original count/range, reserve coverage
  through the maximum signed 64-bit count, and the existing single-revision,
  incomplete-answer and admission boundaries.
- Glass first frame: **25 ms**; the watcher was then stopped with Ctrl-C
  (exit 1). Counsel reports **0 orphans**, with 11/12 serving lanes unobserved
  because no hearth stands. These numbers do not measure answer quality.

The original question, source bytes and 812-token first answer are held fixed
for one fresh-prefill comparison using the changed feedback. Actual answer
quality, length correction and retained learning have separate observations.
Reading the retained baseline also exposes a literal vocabulary error: the
source supplies French `offrait`, while the answer prints `offrirait`. This
joins the previously observed prediction/grounding confusion, inverted enquiry
covenant and self-assessment as a concrete fidelity finding.

## Actual answer: no improvement from the new direction

Native Qwen3.8-27B Q8, full profile, 9,122-position context and a 2,048-token
answer allowance returned **the identical 812 token IDs and 635 words**.
The question's 4,148 IDs and first answer's 812 IDs are held fixed. The new
feedback is 84 IDs including its boundary, versus the previous 86; the fresh
prefix is 5,044 IDs. Generation completed at position 5,856 with stop token
248046, release 1, zero provider calls and 431,327 ms elapsed. The 350–450-word
requirement still fails. Repeated text preserves every baseline error above.

The [native answer](artifacts/2026-09-23-root-closer/answer.txt) opens:

> Form shifts interaction from prediction to witnessing.

It later says:

> The response serves engagement by advancing the enquiry, not just keeping the person pleased.

Codex's reading: both statements still misstate the supplied distinctions.
Changing the correction to explicit shortening did not establish feedback
uptake, useful brevity or better resonance. The separate earlier source-specific
correction did change generation; that evidence prevents the broader claim that
all feedback is ignored. The next causal question is how correction input
affects the chosen continuation. Another generic wording retry has no new
supporting observation.

The new controller instruction and its reserve are implemented and checked.
The quality gap is **open**. The observed native capability gained here is
recovery of a blocked tool reply without another model call. Neither it nor the
assisted code change establishes autonomous coding or whole-session parity.

[Prefix, generated IDs and comparison](artifacts/2026-09-23-root-closer/comparison.json)
retain the original failure and its re-observation. No generated answer from
this comparison is offered as a correct training target.

## Cost boundary

The preceding completed coordinator turn recorded **7,672,547 rented tokens**:
7,504,640 cached input, 124,551 uncached input and 43,356 output; 46 model calls
and 45 tool calls. Cached input remains included. This is a cost observation,
not evidence of savings or parity. The interrupted coding run's aggregate
generation count was not completed; its retained failures are not free work.
The output-only session meter reads **5,310,299**. Its first unbound invocation
reported `signal=nothing reason=transcript-path-required`; the reported total
comes from the subsequent invocation bound to this task's own transcript.
Share remains `declared`, with the percentage withheld while the appended
carrier evidence is incompletely checked. The share meter does not assess
semantic contribution. Native guide: 0 Python implementations, 2 unresolved
execution candidates and 0 unread files.

## Return to the body

The checked controller/JSON teaching was returned through
`observe/form-cli-session-home-embody-run.fk`, event
`direction-delimiter-2026-09-23-v1`. Row
`7b6e445a8c59393cb4fa31de8323d94a98c70690aa0699c5cf73aa44751859ff`
was retained. The current learning observation has four pending rows, a live
worker, optimizer step 145 and learned rounds 146, unchanged from the prior
reading. This is retention; no new learned capability is claimed. The prior
long-row memory failure remains documented in the preceding receipt.

The landing gates returned **8191**, exit 0, with no kernel source movement;
the sibling runtimes were not required. `git diff --check` passed. The most
useful distinction from this movement is concrete: native syntax care moved a
real edit through a blocked boundary, while clearer answer feedback left every
generated token unchanged.
