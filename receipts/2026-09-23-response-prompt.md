# Ordinary answers receive the contract they execute

Signed: Codex. The preceding feedback-state work left answer quality and
correction failure open. This movement repairs a concrete prompt mismatch in
the bounded response path and returns to the original question.

## The executing boundary

`fgcr-open-bounded` called `fcmg-teach`, which carried the shared query overlay:
automatic knowledge-query dispatch, a 48-token query and a 32-token answer
reserve. This path actually calls `fcms-generate` with the caller's allowance
and binds no query handler. The shared overlay remains appropriate to its
query-capable callers.

The new `fgcr-response-prompt` carries `fqt-meaning-prefix`, asks for an answer
from supplied context in the requested format and length, and names runtime
revision requests. It appends the caller's bytes unchanged. The caller-budget
suffix, admission, reserves, generation, correction, retention and release stay
unchanged. This is a repair to the supplied contract; answer improvement must
be observed separately.

## Local code that became the implementation

The [public code request](artifacts/2026-09-23-response-prompt/code-request.json)
supplied the full generation source and meaning helper, one writable path,
the task and three caller-owned source assertions. It used Qwen3.8-27B Q8,
`code_entry=direct`, full document context, context 8192 and at most 12 turns.
No replacement implementation was supplied in the request.

The [returned result](artifacts/2026-09-23-response-prompt/code-result.json)
completed in six turns, four tool calls, three check runs and zero repairs:
411 generated IDs, 1,955 injected IDs, release 1. The final progress row
reported 430,568 ms. Its checkpoint is
`d18bbb6d86c7c707d4894b734e71578852441278409bab1f2f0ade7eafb27115-49907-1790123105410`.
The assertions established the selected source-call boundary only. I inspected
the actual returned source, checked the disk still matched the request's
original, and published that candidate without rewriting it.

The source-backed REPL compiles. The existing reasoning/word-range band passes
1 after clean preflight. Its added checks establish shared meaning retention,
absence of the unbound query protocol, exact caller-byte preservation even when
the caller quotes a query marker, and retention of the query-capable overlay.
Existing reserve, one-correction, incomplete-answer and release behavior checks
remain in that band.

## The actual answer comparison

Both calls use the original 13,506-byte question/source packet, base Qwen3.8-27B
Q8, profile `full`, 2,048 output tokens and 350–450 requested words. The packet
contains no earlier answer or correction. Each call owns and releases its own
model session. The question and complete answers are retained beside this
receipt. The prompt artifacts record the caller-side composition; the shared
model system teaching is unchanged by this patch.

Before: 481 words, then the same 481-word answer after one correction;
628 initial and 628 correction IDs, 86 injected IDs, 4,647 prompt IDs,
9,621 context positions, 515,049 ms. Completion, release and retention are 1;
word-range success is 0. Provider calls are 0.

The baseline calls supplied valence annotations a “numeric identity” for
emotional tone, moves between spectrum arithmetic and felt resonance too
loosely, and describes its own answer as precise and warm. Its trust discussion
stays mostly at the level of terminology. These are answer-quality findings,
separate from the word-range failure. The packet explicitly excludes invented
comparison responses; no unobserved out-of-box baseline is required or claimed.

After: **635 words, again repeated byte-for-byte after correction**;
812 initial and 812 correction IDs, 86 injected IDs, 4,148 prompt IDs,
9,122 context positions, 557,948 ms. Completion, release and retention are 1;
word-range success is 0. Provider calls are 0. The native
[comparison](artifacts/2026-09-23-response-prompt/comparison.json) checks both
repetitions and confirms the before/after answers differ.

Read the [baseline](artifacts/2026-09-23-response-prompt/before-answer.txt) and
[repaired-path answer](artifacts/2026-09-23-response-prompt/after-answer.txt).
The latter separates language surfaces from dictionary senses more clearly
and drops the emotional-tone “numeric identity” claim. It also opens with
“Form shifts interaction from prediction to witnessing,” despite later stating
that graph lookups do not replace model prediction. It says the response
“serves engagement,” reversing the supplied covenant, and ends with an extended
self-assessment of completeness and usefulness. Length adherence worsened and
these errors prevent a response-quality improvement claim.

The repair remains narrowly justified by matching the prompt to the executing
capabilities. It is not a promotion of this answer or evidence that the native
voice has improved. Removing the mismatched overlay saved 499 prompt IDs but
increased generated work from 1,256 to 1,624 IDs in this comparison. The smaller
prompt therefore established no overall throughput gain.

Full generated-ID sequences and stop-token identities were still not retained
by this serving door. Counts and decoded text do not resolve that boundary.
The next repair belongs in the actual response evidence: retain those sequences
at initial and correction completion, then use them to distinguish repeated
generation from repeated decoded output. Another prompt-only change cannot
answer that question.

## Failures and coordination cost remain visible

The first `form-run ./fkwu --check .hearth/ordinary-response-run.bml` exited 2:
`fkwu: form_error: source-compile: mismatched form.bml delimiter: )`.
Removing the extra closing delimiter made compilation pass.

The first `form-run ./fkwu .hearth/ordinary-response-prepare.bml` exited 1.
Its native `rg` returned exit 2 and `unsupported-rg-option` for `-o`, followed
by `fkwu: form_error: native source-check output shape changed`. I then launched
the response helper before noticing preparation had failed. It exited 1 with
`fkwu: str_concat: only strings join -- ask value_kind first` because no question
file existed; no model was admitted. Supported `rg -c` checks and a fresh
evidence directory repaired preparation before the successful baseline began.
The failed evidence directory remains `.hearth/ordinary-response-2026-09-23`.
A later source lookup also named a nonexistent memory file and exited 2;
`rg --files` resolved the actual `form-cli-code-memory.bml` path.

The repaired response run observed its stale BML cache, selected rebuild through
the compiler's health flow and continued from the rebuilt image. Glass's first
frame was 27 ms; I stopped the viewer with Ctrl-C. Its overlap with the coding
run means that coding time is not an isolated performance measurement. Counsel
reports **orphans 0**, with 11/12 lanes unobserved because no hearth stood.
The native guide reports Python implementations 0, execution candidates 2,
unread files 0.

The [preceding completed coordinator turn](artifacts/2026-09-23-response-prompt/preceding-coordinator-cost.json)
used **10,669,973 rented tokens**, including 10,360,448 cached input,
262,249 uncached input and 47,276 output; unattributed 0. Its 72 model calls and
70 tool calls reconcile. This open turn is excluded. The output-only meter
reads 5,173,066 cumulative output tokens and cannot substitute for the full
input-inclusive cost. These costs leave minimal-rented whole-session parity
open even when the native implementation completes useful work.

Drift gates pass **8191/8191**, with no kernel source changes. The comparison's
wall times include overlapping guide/share reads and landing checks; they are
execution receipts, not isolated speed measurements. Share remains declared
and its percentage withheld while the append range is still being checked.

The verified implementation and adverse comparison were retained for native
session learning as event `2026-09-23-ordinary-response-contract-v1`, session
`native-arrival-bootstrap`, row
`1e081fe23d355a6eb16d0d21367bbe1c5427f619bf68894dac20c961a0d42049`.
The learner launched; its completion is pending. Neither failed generated
answer was presented as a correct learning target.
