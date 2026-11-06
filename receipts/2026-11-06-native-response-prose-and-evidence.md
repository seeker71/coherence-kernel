# Native response prose and evidence

This continues the [five-case reading](2026-11-05-native-response-context-and-completion.md).
The same enquiries ran through `fgs-answer`, the native owner behind ordinary
form-cli `ask`, on Qwen3.8-27B-Q8_0. No reply cutoff was supplied. The request's
192-token setting is the progress interval, not a generation limit. Receipt
retrieval stayed at zero to isolate the supplied local evidence. No provider,
model server or weight training participated.

All five final answers completed naturally and released their model. All five
have zero unknown receipt keys and zero active control openings in answer prose.
The raw generations retain seven control frames and the original generated IDs.

| Enquiry | Final native answer, excerpt | What changed or remains limited |
| --- | --- | --- |
| Missing data: 12, 18, missing, 30 | “The average count across the three observed days (Monday, Tuesday, Thursday) is 20.” | Now computes the supported subset mean in Form and separately preserves the unknown four-day mean. The earlier answer said only the sum was calculable. |
| Exact computation | “Result: 23019330. Execution occurred.” | Correct result retained; the eval frame is now in raw evidence rather than the answer. |
| Conflicting counts, 146 versus 164 | “Cedar's final inventory is unresolved: Source A claims 146 units; Source B claims 164 units.” | Keeps the conflict. Its proposed audit calls the difference “missing or double-counted” too confidently; those are unverified explanations. |
| Eleven-turn history; turn 2 corrects the date | “The announcement should use November 26.” | Reads the supplied history, uses the older correction and invents no receipt label. This establishes supplied-history use, not automatic history selection. |
| Missing inventory file | “The observed absence is `nothing=absent-value`.” | Names the exact missing file and reports no invented inventory. |

Final generation counts were 125, 45, 94, 137 and 75 IDs: 476 total, compared
with 351 in the previous changed-prompt reading. More work was performed: the
missing-data case now calculates the subset mean, and the history case also
searches the text through another read. Reusing that prior reading remains an
opportunity. This is an answer-quality improvement, not a token-saving
claim. Prompt IDs were 1525, 1518, 1523, 1524 and 1526. These are five targeted
observations, not a general quality benchmark.

## Answer and trace ownership

`ftg-prose` projects complete active control frames using the existing control
grammar. Other bytes, including indentation, remain unchanged. Fenced examples
remain text; syntactic removal is not an assertion
that a frame executed. The native voice accepts projected prose while retaining
raw text and original IDs. A control-only result is empty. EOS inside an open
control leaves answer status incomplete with `unfinished-native-control`, while
the decoder's actual completion flag remains recorded separately. Release
failure remains distinct. Grounded partial answers use the same projection and
keep their explicit incomplete annotation. Unclosed frames remain visible as
unfinished evidence.

Receipt instructions now depend on the offered index. With no receipts, the
packet states that no keys exist and asks for actual source names or paths.
With receipts, it requests only keys actually offered. Unknown-key counting
remains an observation, not a claim that arbitrary prose citations have been
validated; literal key-like text and other citation mistakes remain broader work.
The packet also asks for useful calculations on an observed subset, labeled
separately from an unknown full result.

## An uncapped failure and its repair

The first uncapped attempt completed three cases, then the history case repeated
an unsupported `.contains(...)` expression. The native compiler named the error,
but the eval observation reaching the model said only `nothing=parse-refused;`.
Nine retained refusal observations repeated the same visible payload prefix.
SIGINT did not end that process; SIGTERM ended the unchanged loop with exit 143.
This attempt is interrupted, not complete, and its first three effects remain
retained. The fifth case had not started.

The compiler already records diagnostics in the framebuffer. The shared native
`compiler-observation.bml` now reads only diagnostics produced inside the current
compilation window. Both the existing `bml-syntax` path and native eval use it;
eval passes the exact compiler reason into its refusal. The existing teaching
now gives the supported `str_find` and `split-on` operations. The final history
run used them successfully and ended naturally. No new attempt or token cap was
introduced. Autonomous recovery from every repeated refusal is not established.

## Consultation on changed evidence

Helper events now retain a work key containing the helper/graph request,
phase, goal, writable paths, pending work, plan/brief, exact failure evidence,
all retained source identities, caller corrections and completed effects.
The ordinary key also includes the current observation. Counters are not new
evidence.

A repeated successful full or ranged read of a selected source can reuse a completed,
nonempty consultation over the same work. Its new care-wrapper bytes do not
justify another model admission. The care note still reaches the parent.
Omitted-source reads, other operations, changed sources, changed work,
unfinished advice and legacy events without a work key remain eligible.
Unobserved release still blocks overlapping models.

The previous actual graph run's redundant implementation consultation spent
620 generated IDs on an unchanged config read after implementation-entry
advice. The retained task, source, plan, failure and effects were identical.
The new rule excludes that event while keeping the later changed-config event
eligible. The existing controller band exercises phase-entry advice followed
by real repeated reads, plus new task, phase, failure, effect, omitted-source,
unfinished-advice and legacy-event counterexamples.

The new uncapped Q8 parent / Q4 helper run completed the config edit, passed the
three original assertions and returned a native accept review. All four helpers
completed and released, in planning and implementation. Selection remained
`config.json` plus `contract.md`; the unrelated document remained retained,
omitted from the selected view and unchanged. No provider or training ran.

That live run exposed a narrower bypass: `read config.json 1 20` was repeated
over the unchanged selected source. The initial repair recognized only a
one-argument read, so the ranged read still launched a redundant consultation.
It spent 641 generated IDs. The live run used 2645 generated IDs, 9265 injected
IDs and 27 observations, versus the previous uncapped run's 2487, 9286 and 27.
It therefore establishes no reduction in total generated work.

The owning predicate now recognizes the native ranged-read contract and document
IDs as well as paths. A replay reconstructed the actual retained implementation
entry state and matched its exact work key
`166004ec9450bcd38801962e3acef9090a82d86f48cf1d3b10ee5e1511c52e9e`.
It executed `read config.json 1 20` through the native tool/care flow and called
the real consultation controller. The helper callback was invoked zero times;
goal, source, completed effects and original checker ownership were preserved.
The existing controller band also passes ranged-read and document-ID cases.
This verifies the deterministic scheduling repair without another model call;
a fresh end-to-end cost improvement after that final correction is unmeasured.

Automatic history/graph discovery and generally concise helper reports remain
open. The graph is a caller-selected dependency view over retained state; it
does not replace the decoder's linear KV stream. The conflict answer's causal
overconfidence and general citation validation also remain open quality work.

## Verification and retained evidence

Existing targeted bands observed full verdicts: control grammar 1023, native
voice 65535, control verbs 8191, grounded one-door 2047, bounded coding context
524287 and turn guard 3405951. The control-verb checks include exact member-error
diagnostics, a supported replacement, a later generic parse failure without a
stale diagnostic, and continued successful evaluation. Reapplying the current
projection to all five retained raw generations reproduced each answer exactly.
Release checks are
recorded by the native landing continuation.

Private evidence in this checkout: `.hearth/native-five/next-answers.log`
(interrupted process 1124), `unchanged-eval-failure.log`, `final-answers.log`
(completed process 43978), and `final/` per-case reports. Reports point at the
exact admitted packet, prose, raw generation, original IDs and metadata under
`.form-heal/grounded-43978-*`. The same synthetic fixtures and original enquiry
bytes remain in `.hearth/native-five/run.bml`; `next-run.bml` removes only the
old comparison reply budget. `.hearth/native-five/final-graph-helper.log`
retains coding process 33494, and `ranged-replay.bml` / `ranged-replay.log`
retain the exact scheduling repair observation. Evaluation answers remain outside
gradients. Neither structural checks nor vocabulary matching establish general
answer quality or a person's felt resonance.
