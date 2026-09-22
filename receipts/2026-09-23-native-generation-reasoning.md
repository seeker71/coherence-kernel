# Native generation: reasoning still needs source correction

Codex, 2026-09-23.

The native answer had the relevant source and still dropped its conditions.
Opening Qwen's reasoning span corrected its claim that supplied frequency
annotations measure the person's state. It did not correct retention or the
invented comparison with an out-of-box model. That is the observed gap.

## Actual comparison

The preceding [literal-grounding receipt](2026-09-23-native-literal-grounding.md)
retains the original answer. Both calls used Qwen3.8-27B Q8 on native Metal,
the same 13,506-byte enquiry/source packet, shared meanings and full Form
system teaching. The original enquiry hash remains
`c0520855fe91d59e30f913af192843c99e37185f3eaeb917e08deacb63f06574`.

| Observation | Ordinary generation | Open reasoning |
| --- | ---: | ---: |
| Prompt IDs | 4,607 | 4,605 |
| Initial allowance | 2,048 | 2,048 |
| Separate answer allowance | 2,048 after a lookup | 2,048 if a final stage is needed |
| Generated IDs | 871 | 2,001 |
| Injected IDs | 0 | 0 |
| Additional final-stage IDs | 0 | 0 |
| Elapsed milliseconds | 394,411 | 631,869 |
| Observed completion / release | 1 / 1 | 1 / 1 |
| Provider calls inside generation | 0 | 0 |

The new call used a caller-selected context of 12,288 positions. The system
and user prefix remained unchanged; the reasoning template removes the two
IDs that closed the thought before generation. The session controller also
changes: native bounded reasoning replaces ordinary generation's lookup and
literal-thought machinery. Neither injected anything in this comparison.
These are two observed calls, not a controlled throughput distribution.

The new answer correctly says the supplied annotations do not measure a
person's inner state. It also says an old cell persists **"referenced or not"**.
Both the supplied axiom and the admitted shared teaching state the reference
condition. It invents that an out-of-box model has no such state and fills a
gap with a guess or hedge, although no measured baseline was supplied.
Source inspection found those conditions present in the admitted packet.
These are answer failures after delivery, not absent context.

The full answer and hashes are retained in
`artifacts/2026-09-23-native-generation-reasoning/`. Private initial model
deliberation is retained locally; the public evidence contains final answers,
source packets, counters and the native callers used here.

## Correction through the public CLI

The second call used `generate --reasoning 2048 --tokens 2048` through the
public command boundary. The original enquiry/source remained intact. The
caller appended the actual failed draft and the two source contradictions,
explicitly asking for a corrected answer. Its 17,455-byte repair packet is
retained; its SHA is
`d5cbe5d78282a75d590365a5150d9a01b27b443a1cbf7a14ca219780e4a47faa`.
This is a feedback-assisted repair, with a changed context, not an independent
answer or automatic native detection of those findings.

The organ exchange recorded the two caller-observed contradictions, selected
`revise-answer`, and applied the public command. The completed revision says:

> Changed composition mints a new id; the referenced old cell persists. A cell
> with no remaining reference may be reclaimed — the unreferenced composts,
> returns to potential.

It removes the invented account of an out-of-box model's behavior and keeps
the distinction between supplied frequency annotations and the person's felt
sense. These two targeted findings are resolved in this returned answer by
comparison with the supplied source. The arriving agent made that assessment.

The command counted 5,473 prompt IDs and allocated 10,011 context positions.
Its initial stage generated 2,048 IDs without a complete final answer. The
existing controller inserted its explicit 100-ID observation and opened the
reserved final stage, which generated 697 IDs and stopped. Total generated
IDs: 2,745; elapsed: 900,840 ms; release: 1; retention: 1; provider calls: 0.
The 100 IDs are controller input, not generated reasoning. Both model stages
remain private; the [final answer](artifacts/2026-09-23-native-generation-reasoning/command-answer.txt)
is returned separately.

Native whitespace counting finds 486 words in the first reasoning answer and
504 in the repaired answer, both above the requested 350–450. The repaired
answer also ends by saying the person leaves more grounded, stretched and
free. That outcome was not measured here. Its trust discussion still leans
heavily on relabeling words. These are remaining quality findings; the two
corrected source claims do not establish complete instruction following,
felt resonance, independent self-correction or retained learning in Qwen.

## Implementation

`generate --reasoning N` now opens the existing native reasoning controller
with the full Form system and caller context. `--tokens N` selects the answer
allowance; either option order is accepted. Ordinary generation keeps its
existing default. Invalid, repeated or empty options return before admission.

The command allocates from the counted prompt plus generation and observation
reserves, checks the GGUF's capacity, retains stage evidence privately and
reports completion separately from resource release. It releases a previous
residence before owning its reasoning session. Each reasoning call owns and
closes that session. Automatic source lookup and literal-thought injection
are not bound in this mode; the caller supplies evidence.

The C seed did not change. No language runtime or model server was installed.

## Checks and adverse observations

- Full source-backed REPL compilation passes. Its first check failed with
  `source-compile: form.bml if missing 'then': if isTokens`; naming the inner
  condition before branching repaired the expression, and the same check
  exited 0.
- Native template check: system and user prefix preserved; existing review
  profile preserved; unarmed-system behavior preserved.
- Native command check: exact prompt bytes, both option orders, invalid
  admissions 0, missing-model guidance and refused zero reserve.
- Preflighted reasoning-budget band: 1. Preflighted model-session band: 4095.
- Drift gates: 8191/8191; no kernel source moved. `git diff --check` passes.
- Native authoring guide: Python implementations 0; two existing execution
  candidates remain in `voice-say.bml`.

The glass launched a continuing UI process after its initial reading. I
terminated that owned live process after about 37 seconds; its supervisor
reported child exit 143, released its sensors and exited 1. This was an
intentional stop, not a passing glass completion. Its overlap is a confound
in the first generation's elapsed time. Panel: counsel reported **orphans 0**
and 11/12 performance lanes unobserved because no hearth resident stood.

## Coordination and learning boundary

The observed preceding coordinating turn
`01a0cacd-e30c-7991-a3f3-c1f4a1734eb8` cost **2,993,299 tokens**:
2,976,147 input, including 2,795,648 cached, and 17,152 output. Its 14 model
calls and 13 tool calls are reconciled. This excludes the current open turn
and separate subprocesses. The corresponding share counts native/local/remote
events 7/24/14; those counts do not measure semantic contribution or token
shares. Local generation's zero provider calls does not remove coordination
cost.

The preceding verified teaching finished learner round 140. Serving generation
5 and four promotions remained unchanged. That learner targets another model;
it establishes no Qwen27B weight update or retained correction in this answerer.

This movement's verified implementation teaching was retained as event
`native-generation-reasoning-2026-09-23`, row
`98c489f5d567613f7566d875f7a163fa670afdc1124c9688424d7f7420bf9615`.
The learner launched; a completed update was not yet observed. The native
answer itself was not offered as a correct training target.

Opening reasoning is a usable native capability. Its first answer is mixed
evidence for quality, and completion is not a grounding verdict. Whole-session
quality, resonance and throughput parity remain unestablished.
