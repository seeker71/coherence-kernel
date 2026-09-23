# Keep the generated sequence at the response boundary

Signed: Codex. The original question's 635-word answer repeated exactly after
one measured length correction. Counts and decoded text left the generation
boundary unobserved. This movement retains that boundary in the actual serving
path; it does not promote an answer on the strength of a successful process.

## Native implementation and its limits

`fgcr-generation-evidence` retains the ordered integer IDs, decoded text,
position, pending token, stopped flag and session status. `fgcr-open-bounded`
writes initial, final and length-revision stages as objects in the private
`generation.json`; verified retention participates in the existing `retained`
flag. Raw IDs and answer text stay out of framebuffer health metadata.
Admission, prompt composition, feedback, generation and release are unchanged.

The first local coding request lacked the actual JSON helper contracts. Its
first edit reply was invalid JSON; the model corrected the reply, then produced
invalid BML using absent `oh-a`, nested ID pairs and JSON strings where objects
were required. Narrow source assertions passed those errors. A later edit
removed a helper while leaving its call. I stopped the owned process with TERM
(exit 143) and retained its checkpoint and [candidate](artifacts/2026-09-23-generation-evidence/interrupted-candidate.bml).
`form-run ./fkwu --check` on that candidate exited 1 with
`[unresolved-call] 'oh-a' matched no op/rewrite/fn/binding`, at lowered line 6:114.
It was never published or offered as a completed learning target.

The [second request](artifacts/2026-09-23-generation-evidence/request.json)
starts from that candidate with the real JSON constructors, integer-array
helper, focused source queries and catalog context. Qwen3.8-27B Q8 repaired it
through the public `code` door: seven turns, five tool calls, three check runs,
704 generated IDs, 4,132 injected IDs and release 1. The last progress row was
653,113 ms. Context renewal contributes to that time; the final stream's 122
generated IDs are not the whole request's cost. I inspected and published the
returned source without rewriting it. Its [result](artifacts/2026-09-23-generation-evidence/code-result.json)
proves the caller's assertions only.

Independent REPL compilation passes. Clean preflight and the existing
reasoning-budget band pass **1**, including ordered IDs `[0,1,0]`, exact text,
position, stop state, and an empty generation. Existing prompt, reserve,
single-correction and incomplete-answer checks remain. These checks establish
retention behavior, not response quality.

## The same question, now with complete sequences

The serving call used the unchanged 13,506-byte question/source packet,
Qwen3.8-27B Q8, profile `full`, 2,048 answer tokens and the caller's 350–450-word
range. It returned **635 words, then the same 635 words** after its one
correction. The [native comparison](artifacts/2026-09-23-generation-evidence/comparison.json)
establishes:

- Initial and revision sequences contain **812 identical IDs**; no differing
  index exists. The [complete sequences](artifacts/2026-09-23-generation-evidence/generation.json)
  are retained, not inferred from text or counts.
- A separate process decodes each sequence to its saved text exactly, without
  admitting a model. The initial text also matches the previous turn's answer.
- Both pending tokens are **248046**, recognized as stop tokens by the actual
  model source. Position advances from **4960 to 5858**; feedback injected 86 IDs.
- Prompt IDs 4,148; context 9,122; elapsed 544,141 ms; completion, release and
  retention 1; word-range success 0; provider calls 0.

This locates repetition in generated IDs, beyond the possibility of two
different sequences being rendered as the same answer. It does not prove the
continuation state is correct or identify the cause of the model's repetition.
The [answer](artifacts/2026-09-23-generation-evidence/answer.txt) still reverses
the enquiry covenant, contradicts itself about prediction and ends with
self-assessment. **Response quality is unchanged.**

The retained sequences enable the next direct comparison: replay the exact
transcript through fresh prefill and compare it with the incremental
continuation. Preserve identical content when examining that state boundary;
another prompt rewrite would change the question being tested.

## Learning and coordination remain observable

The previous verified teaching did complete: row
`1e081fe23d355a6eb16d0d21367bbe1c5427f619bf68894dac20c961a0d42049`, optimizer
step 145, learned rounds 146. Serving remained generation 5. The following
queued code example failed whole-row memory admission:

```
fkwu: form_error: native training whole row awaits memory; line=1 required_additional_bytes=341213194880
```

The worker exited 1 and released its process; the row remains pending. Its
11,546-byte prompt and 9,614-byte target remain intact. The
[failure reading](artifacts/2026-09-23-generation-evidence/learning-failure.json)
keeps that distinction. A custom task helper initially drained this failed
queue before allowing independent inference; I removed that coupling after
verifying the worker was no longer alive. No memory guard or learning outcome
was weakened. Native coding and inference still refuse concurrent learner
ownership.

Glass first frame: **25 ms**; stopped with Ctrl-C. Counsel: **orphans 0**,
11/12 lanes unobserved, no standing hearth. These readings overlapped coding;
wall times are execution receipts, not isolated benchmarks. The native guide
reported Python implementations 0, execution candidates 2, unread files 0.

The [preceding completed coordinator turn](artifacts/2026-09-23-generation-evidence/preceding-coordinator-cost.json)
used **8,612,316 rented tokens**, including 8,187,136 cached input, 364,928
uncached input and 34,804 output; 25,448 remain unattributed. Its 73 model calls
and 69 tool calls reconcile. This open turn is excluded. Local model execution
does not establish low rented coordination cost or whole-session parity.

The output-only meter reads 5,226,099 cumulative output tokens. Its first call
had no transcript stdin and returned `signal=nothing reason=transcript-path-required`;
supplying the recorded transcript path resolved the reading. Share remains
declared with its percentage withheld while the append range is being checked.

Other task errors stay visible: a guessed decoder glob exited 1; a guessed
dispatch source path exited 2 and source search found the actual adapter file;
a guessed learning-helper source path likewise exited 2;
a premature result-file read exited 1 before coding completed. The native
comparison helper's first compile exited 2 for an extra closing delimiter;
the repaired helper compiles. These are coordination errors, not model findings.

The verified retention and adverse comparison were offered through the session
embody door as event `2026-09-23-generation-sequence-evidence-v1`, session
`native-arrival-bootstrap`, row
`396e2c7872861654426183e95550349ba82a411f950395dee06c05059628eb3c`.
The door retained the teaching and launched the learner. Retention itself does
not establish a learned update or serving promotion.

Before landing, drift gates pass **8191/8191**, with no kernel source change;
`git diff --check` is clean. The compact learning reading still shows a live
worker, three pending rows and the previous step 145. The new teaching's
learning outcome remains pending at this observation.

The following movement observed that worker exit 1 on the same whole-row
memory requirement. All three rows remain pending at step 145; this teaching
has not learned. The updated compact learning artifact records the failure.
