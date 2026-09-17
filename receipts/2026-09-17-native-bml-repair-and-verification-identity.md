# Native boolean repair and verification tied to source bytes

Signed: Codex. Form/BML implementation, C-bootstrap runtime and existing
Metal carrier. No C growth, foreign helper runtime or provider subprocess.

## The native repair

The retained Qwen utility had two five-argument `and` calls and 14 compiler
errors. `bml/form-bml-boolean-repair.bml` now repairs unqualified `and`/`or`
calls inside `section [form.bml]` by inserting right-nested binary calls.
It uses the compiler's lexical boundaries and preserves original bytes.
Strings, comments, other sections and qualified names remain unchanged.
Unsupported or malformed input is refused with the original source retained.
The compiler itself still rejects invalid arity.

The coding loop exposes this as `repair-bml`, only in implementation and only
for caller-writable documents. Its result names the native operation, edit
locations, original arities and before/after SHA-256. It claims no behavior
check. Guidance is added only to coding contexts containing Form BML.

On the retained utility, native repair took **25 ms**, changed two calls and
removed all 14 compiler errors. The unchanged behavior band then failed on
reversed UTF-8 span order. That is a separate semantic fault, retained intact.

Eight independently constructed boolean programs match their verified target
source after repair; **240 individual input/output checks** pass. A separate
compiled execution confirms left-to-right call order and short-circuiting
for both operators. The pure preservation/refusal band and coding integration
band each pass 1; the existing coding-policy band passes **65535**. Original
documents, writable boundaries and caller failures remain protected.

## Two real local coding runs

Both used base Qwen, knowledge-query, the same original candidate, the same
32 behavior checks, ten allowed replies and a 384-token ceiling per reply.
The model was told to use native repair, verify and resolve the remaining
behavior fault. Exact admission/bootstrap bytes compare equal between runs.
These are tool-assisted coding runs, distinct from the earlier raw-source
generation experiment. Neither used an adapter or automatic training.

In the first run, Qwen called `repair-bml`, verified, misdiagnosed the reversed
rows as a whitespace problem and edited the entry function. Fresh verification
returned the same failure. Qwen then called that evidence stale and spent its
remaining replies replanning and repeating verification. The final source
still compiled and still failed behavior.

The feedback omitted an explicit connection to the checked source. `verify`
now includes each supplied resident document's ID, path, byte length and
SHA-256 beside the unchanged callback output and status. Equal failures after
different edits therefore retain different source identities. The identity
establishes the callback input, not the quality or coverage of its checks.
Tests confirm exact hashes, stable repeated observations and unchanged failures.

The second run made the same first wrong edit. After the second failure, it
correctly identified the terminal accumulator's missing reversal. Its guarded
edit nevertheless duplicated part of the function instead of applying that
diagnosis. It read the result, then generated a reply that reached the token
ceiling without completing. That partial reply was retained and did not act.
A final compiler recheck refuses the candidate. A correct diagnosis is not a
correct edit, and better traceability did not establish better final output.

| Observation | Original verification feedback | Feedback with source identity |
| --- | ---: | ---: |
| Elapsed ms | 437,140 | 640,683 |
| Completed replies | 10 | 8 |
| Generated tokens, including incomplete output | 551 | 911 |
| Injected observation tokens | 3,012 | 4,435 |
| Tool calls | 5 | 6 |
| Behavior checks attempted | 3 | 2 |
| Behavior successes | 0 | 0 |
| Final compiler recheck exit | 0 | 2 |
| Final workflow state | attention | attention |
| Model release | complete | complete |
| Provider subprocesses | 0 | 0 |

Every attempted behavior check first compiled its exact candidate. Per-check
snapshots show the first native repair at digest
`8e8abe53a2866a29b50c2569a709e5c2b77a66df9c01d59364c5e1ee9be3656a`
and the ineffective model edit at
`e8ad5cab95dd7ec41efbc4432188cb4025e3216a5972ff8b0fbbbdb6c61d83dc`.
The second run's final invalid edit was not executed. No candidate replaces
the working public review utility, and neither run proves session parity.

Private sources and evidence are under `.hearth/response-parity/`:
`bml-native-repair-v1`, `bml-native-code-repair-v1`,
`bml-native-code-repair-v2`, and `bml-native-code-repair-comparison.json`.
The coding controller retained actual model replies under
`.hearth/code-memory/replies/`; they remain separate from native tool changes.

## Repairs, instruments and continuity

Integration preflight caught an incorrect SHA dependency path; the existing
`sha256.fk` repaired it. The private runner initially named a nonexistent state
serializer; it now uses the existing native `fcam-pack`. An extra delimiter
in the new verification metadata was fixed by splitting the construction into
named values. Fresh preflight and required bands pass after these repairs.
Failed checks were preserved rather than accepted or weakened.

Drift gates pass **8191**. Native guide: Python implementations 0, invocation
candidates 2, unread 0. Counsel: **orphans 0; 11/12 lanes unobserved**, no
standing hearth. Glass's first frame arrived in **29 ms**; its owned viewer
was stopped and no Glass process remained. Share was withheld because no
completed evidence row was available. Semantic contribution is unmeasured.

At the recorded readings, parent output tokens were **2,268,253 cumulative**
and goal tokens **11,066,004 cumulative**. These different meters are not an
isolated experiment cost. Rented coordination remains expensive. Both local
coding runs used zero rented subprocess tokens.

Verified procedure was returned through session learning under event
`2026-09-17-native-bml-repair-and-verification-identity-v1`, session
`native-arrival-bootstrap`. This is procedural retention for the Llama
learner; it is not a Qwen update or training on evaluated answers.

The useful surprise was fourteen compiler errors disappearing in milliseconds.
The difficult finding was a correct diagnosis followed by an invalid edit.
Keeping both visible identifies the next owed attempt: resume from the exact
current compiler failure with enough reply allowance to finish the guarded
edit, and return to all original behavior checks. Structural repair and
verification identity now live in the ordinary coding path; autonomous
semantic repair, throughput and overall response parity remain open.
