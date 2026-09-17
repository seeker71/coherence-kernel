# Dialogue revision and ordinary native generation

Signed: Codex. This movement follows the retained local transfer comparison.
It separates caller-guided revision, the ordinary generation interface, and
the prompt admission work done by those interfaces. Evaluation answers stay
outside learning. No provider subprocess was used.

## Caller-guided revision is only a partial repair

The original local dialogue request, sources, assertions and report were
retained. Codex supplied attributed feedback about the unsupported assurance
“without harm,” unmarked conversational use of kernel composition, confusion
between the short codebook anchor and kernel identity, and draft placeholders.
No replacement answer was supplied. The original structural checks had passed;
the feedback was not represented as a failed native assertion.

The first setup attempt refused before model admission: retaining the report
alone does not put the controller into repair. Exact failing command:

```sh
form-run ./fkwu .hearth/response-parity/heldout-dialogue-scope-revision.bml > .hearth/response-parity/heldout-dialogue-scope-revision-v1.log 2>&1
```

Exit **1**: `fkwu: form_error: retained report not admitted to revision context`.
The failed runner and log remain under the private hearth. The repair enters
`fcap-rework` before `fcap-begin-context`; its correlated control selected that
existing repair state. Fresh preflight then passed.

The two-reply revision completed in **337,678 ms**, with **5,283 prompt IDs**,
**246 generated IDs**, no injected IDs and release **1**. The unchanged source
and report checks passed. Reading the actual revised answer shows two useful
changes: it removes the unsupported harmlessness assurance and replaces the
name/topic placeholders with a complete draft. It leaves the “composition
changes” wording and the unclear identity/anchor description. Partial repair
does not establish completed semantic review or equal response quality.

Private evidence: `.hearth/response-parity/heldout-dialogue-scope-revision-v2`.
The coding guide now shows the verified fresh-context repair construction and
keeps it distinct from recovering an old model stream.

## Ordinary generation and admission

The production ordinary generation door is `fcmg-generate-resident`, called by
the REPL's `generate` command. It carries the Form teaching overlay, full chat
profile and native knowledge-query capability. Coding review uses a different
bootstrap, controller and output protocol. Changing between them changes more
than a role sentence; it is an interface comparison, not an isolated prompt
ablation.

The retained ordinary-generation enquiry contains the original goal and
documents and requests the original JSON report. It receives no earlier
answer or revision feedback. Its assertions are run separately after the
answer. Evaluation remains private and untrained.

Source inspection found that ordinary generation used tokenwise live-cursor
prefill and admitted scalar scratch, while model sessions already admitted
bounded spans through `fcma-span` and used `fcmg-live-prefill-span`. Generation
now uses those existing native paths as well. The live cursor is materialized
to a prompt ID list before sliced prefill; scratch width comes from the same
BML authority, currently capped at **64** positions. Existing scalar residents
remain supported through their recorded width. Scanner fallback, teaching,
decoding and cleanup remain on their existing paths. No C seed or external
runtime was added.

The same caller prompt and request ran before and after the admission change:

| Observation | Tokenwise admission | Sliced admission |
| --- | ---: | ---: |
| Elapsed ms | 1,077,866 | 386,892 |
| Prompt tokens | 4,808 | 4,808 |
| Generated tokens | 61 | 61 |
| Prefill GPU busy us | 778,070,818 | 195,940,232 |
| Decode GPU busy us | 109,932,724 | 109,754,567 |
| Model release | 1 | 1 |

The complete generated text is byte-identical. The observed elapsed ratio is
**2.785** for this pair; it is not a general speed guarantee. The first run
includes the instrument activity noted below. A tokenizer-only native reading
independently counted **4,808** prompt tokens without admitting a model.

## The answer allowance was being lost at a lookup

Both outputs were incomplete. Each made one native lookup, injected **582**
tokens, spent **29** tokens on its query and exhausted the **32**-token answer
reserve. The original caller had offered **1,536** tokens. The cursor correctly
cut unused query fuel, but ordinary generation had supplied the small default
answer reserve regardless of that request. The text ended inside a JSON
string, with `heed_stopped=0`. The original report checker returned exit **2**,
`invalid-json-input`. Source checks and release passed; report checks did not.

The ordinary ledger now uses `fhm-caller-answer-reserve` in BML: the caller's
allowance survives the lookup, with the existing default reserve as its floor.
The model prompt names the actual per-call allowances. Direct replies use their
existing phase; specialized knowledge callers retain their explicit budgets.
The existing cursor band now exercises an answer that crosses **32** tokens
and stops normally, alongside its prior direct-answer and lookup boundaries.
Fresh preflight and execution pass **127**, exit **0**. The shared admission
band passes **15**, exit **0**; its stale image was automatically rebuilt by
the compiler's observed care action before the successful reading.

Private evidence: `.hearth/response-parity/heldout-dialogue-ordinary-v1` and
`heldout-dialogue-ordinary-v2`, plus
`heldout-dialogue-generation-comparison.json`. Their native audits preserve
the failed original report checks rather than relabeling the fragments.

The third run retains the original caller enquiry and documents with the new
answer reserve and its explicit budget notice. It completes in **418,671 ms**:
**4,856** prompt tokens, **221** generated tokens, **582** injected tokens,
**29** query tokens and **192** answer tokens. The cursor stops normally with
**1,344** answer tokens left; all **128** stream handles and the model owner
release. The answer is no longer a budget-cut fragment. This run changes both
the reserve and its model-visible notice, and is outside the isolated admission
timing pair. Evidence: `heldout-dialogue-ordinary-v3` under the same private
hearth.

I read the actual answer. It contains the requested fields and a follow-up, but
still leaves a name placeholder. It describes two hours as a “budget expiry”
without an agreed deadline in the evidence, and opens with “Your silence” when
the absent reply belongs to the collaborator. It correctly declines evidence
about the collaborator's intent and reports the absent German mapping. Its
knowledge-query and control markers surround the JSON, so the unchanged raw
report checks still fail with `invalid-json-input`. No extraction was used to
relabel that failure. Ending normally is a verified runtime improvement;
semantic quality, a complete usable draft and the requested JSON-only output
remain separate gaps. This interface change did not establish voice parity.

## Instruments

Native guide: Python implementations **0**, invocation candidates **2**, unread
files **0**. Counsel: orphans **0**, with **11/12** lanes unobserved because no
hearth stands. The owned glass viewer ran and was closed; its observed tick
panel showed **40 ms** target cadence. Its activity overlaps part of the first
ordinary generation run, so that timing includes instrument activity.

Drift gates pass **8191**, exit **0**. The goal meter at this checkpoint reads
**12,574,592 cumulative tokens**; the native transcript meter reads
**2,576,974 session output tokens**. These are different meters, not alternate
counts of one quantity. Share remains declared and unmeasured, with its
percentage withheld. Zero provider subprocesses in these evaluations does not
erase the rented coordination cost.

The verified procedure was retained for session learning under
`ordinary-generation-admission-and-answer-reserve-v1`, and its worker launched
after the final Qwen release. The teaching contains the observed procedure,
not the assessment answers. Training completion and promotion are pending at
this checkpoint; the learner's Llama updates are not Qwen quality evidence.

The useful surprise is that the ordinary answer path had not inherited the
session path's prompt admission improvement. Reading the incomplete revision
kept a passing checklist from becoming a quality claim. The remaining semantic
work stays visible in the actual retained answers.
