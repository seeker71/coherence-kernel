# Exact changed reads and a measured reference answer

Signed: Codex, 2026-09-16.

The next actual native review reread the source after its edit. The admitted
document remained useful, but returning the whole changed file cost another
long observation pass. Form now offers an exact line splice against that
admission snapshot when it is smaller than the full result.

## The observed transport change

The original snapshot is the base for every splice, including repeated reads.
The result names the first changed line, removed line count, full current
replacement lines and current byte count. The controller reconstructs the
result and compares it byte-for-byte with the actual read before emission.
It falls back to full output when reconstruction differs, no snapshot exists,
the tool reports an error, or the splice saves no space. `cat` always returns
full current text. Caller-owned checks continue to inspect actual documents.

Replaying the actual retained native edit and review read established:

| Observation | Value |
| --- | ---: |
| Original source bytes | 12,189 |
| Current source bytes | 12,310 |
| First changed line | 58 |
| Replaced lines | 1 |
| Full observation bytes | 13,584 |
| Splice observation bytes | 1,927 |
| Exact reconstruction / documents preserved | 1 / 1 |

This is a transport observation, not generated-answer quality. The live replay
already in progress uses its previously loaded controller; it does not acquire
this source change mid-run.

Native policy checks cover insertion, deletion, repeated lines, Unicode, final
newlines and separated changes. The policy band returned 65535; request,
session and CLI validation completed with exit zero. One validation invocation
failed because I passed a repository-relative path and an unsupported extra
argument to the form-relative runner. Its exact diagnostic was
`validate.sh: declared Form dependency not found: form/form-stdlib/tests/form-cli-code-policy-band.fk`.
The corrected invocation, `form-run ./form/validate.sh form-stdlib/tests/form-cli-code-policy-band.fk`,
passed. The original failed log is retained.

## A source-supplied provider reference

Form invoked one Codex CLI request with the same question and seven source
documents prepared for the enriched native answer. Native source verification
ran first. The provider was asked for one final report, with no exploration,
extra model calls or file changes. It ran in a temporary directory without
the repository's additional arrival context. No native candidate answer or
expected report values entered that prompt.

This is an independent reference for one answer. It is not the full requested
comparison between a context-equipped agent session and a Form-owned session.
The provider retained its own base instructions, while the local model uses
the Form coding controller. Those differences remain part of the comparison.

The provider completed in **22,075 ms**, one completed turn, exit zero. Its
reported usage was **19,476 input tokens**, including **10,624 cached**, and
**777 output tokens**, including **62 reasoning tokens**. Input plus output
was **20,253**; subset quantities are not added again. The unchanged native
source/report checks passed. These checks establish the requested fields,
not the prose's quality.

The reference distinguishes identity from labels and missing translation from
invented meaning, using the added arrival table. Its actual answer is retained
for comparison with native generation from that same enriched snapshot.

Reference preparation first reached `eval-depth wall` while hashing the large
request through `sha256`. The native session's existing `fcam-hash` resolved
that preparation failure; no provider call occurred before preparation passed.

## Cost and learning stay visible

The previous completed coordinator turn, `01a0a832-20df-7483-bfce-e75740264df0`,
reconciled **45 provider calls**: input **6,481,913**, including cached input
**6,287,360** and uncached input **194,553**; output **59,676**, including
reasoning **45,436**. Reported total **6,569,178** retains an explicitly
unattributed **27,589**. This is substantial rented coordination cost, separate
from the reference call and the native executor.

Panel reading: native/local/remote boundary events **34/44/45**, normalized
**28/36/36**. This measures events, not semantic contribution. The hearth query
still answered `signal=nothing reason=no-standing-hearth`.

The preceding verified context teaching completed local learning round 10;
promotions remained 4 and serving generation remained 5. That is Llama 3B
learning, not a Qwen weight update. The verified line-splice teaching was
offered under `verified-resident-line-splice-v1`. Assessment answers remain
excluded from training.

The useful teaching is that a changed document need not discard the value of
its admitted context. Exact reconstruction makes that reuse examinable. The
difficulty yielded a smaller observation with all current source bytes still
recoverable; generated use must now show how that changes completion.

Whole-session quality, frequency, cost and throughput parity remain open.

## Completed re-observations

The coding replay completed with status `complete`, 11 replies, zero repairs,
832 generated IDs and 4,802 injected IDs, in **1,930,610 ms**. Its source checks
and document contract passed; its actual returned BML also passed the compiler.
Release was observed. This run used the earlier context-reference controller,
so its changed-file reread still took the full path. It does not measure the
new line-splice behavior in generation.

After that completed case, I deliberately stopped the batch's repeated
old-context dialogue and branched to the enriched assessment. The owned handle
75837 returned exit **143** after TERM to its observed process 80892.
`interruption.json` and `interruption-terminal.json` retain the decision and
terminal observation. The two-case batch remains incomplete; its completed
coding result remains usable on its own.

The enriched native dialogue completed in **326,142 ms**, one reply, zero
repairs, **470 generated IDs**, zero injected IDs and zero provider calls.
Source/report checks and release passed. Its request digest matches the
provider reference's frozen request:
`fa4614346e3b1d265140929ea0f776a252ba0ceeb5b77284ca00dd3c58a46096`.
The models retained their different controller/base instructions.

The exact answers are side by side in
`.hearth/response-parity/grounded-response-comparison.md`. My reading: the added
context reached the native answer's distinction between identity and labels.
The provider reference developed missing translations and alternate meanings
more fully. This reading is an attributed assessment, not an executable quality
verdict. Urs was invited to assess meaning, warmth and initiative; no answer
has yet been received. Neither response establishes whole-session parity.

The line-splice teaching completed local learning round **11**. Promotions
remain **4**, serving generation **5**; no Qwen weight update is claimed.

## A faster route that failed strict equality

A private experiment compared the existing observation route with the sliced
matrix route already used for initial prompts. It used the existing strict
real-state probe, one weight admission, two fresh stream states, a nine-token
prefix and the same 65 observation IDs starting at position nine. No production
route or original check changed.

The first run measured **11,964 ms** and **1,590 ms** respectively. Both returned
`[198, 74]`, finite logits and clean release. The check vector was
`[1,1,0,1,1,0,1,1]`: logits and state were not byte-identical. The command
`form-run ./fkwu .hearth/response-parity/sliced-observation-probe.bml` exited 1
with `fkwu: form_error: real Qwen bounded observation parity failed`.

A second run added numerical diagnostics while preserving that strict refusal.
It measured **11,071 ms** and **1,090 ms**. Of 248,320 logit floats, 238,637
differed; maximum absolute difference was **2.0742416381835938e-05**, maximum
relative difference **0.1476510067114094**. The first differing state buffer had
40,960 floats, 39,749 different, maximum absolute difference
**0.000148773193359375**, maximum relative difference **0.03528418639159039**.
All examined floats were finite. The strict state traversal stops at its first
differing layer; these figures do not describe every state buffer. The same
check vector failed and release returned zero live buffers.

This identifies a speed opportunity and a numerical difference. It does not
establish that the difference is harmless over later generation, other inputs
or longer contexts. The strict check remains unchanged and production still
uses its existing route. The next numerical and generated-behavior evidence
must address that scope before any runtime switch.

## Local evidence

- `.hearth/response-parity/context-splice-witness.json`
- `.hearth/code-memory/replies/80892-1789530317941.txt`
- `.hearth/code-memory/replies/80892-1789530446943.txt`
- `.hearth/response-parity/context-splice-policy-validation.log`
- `.hearth/response-parity/context-splice-policy-final.log`
- `.hearth/response-parity/grounded-provider-receipt.json`
- `.hearth/response-parity/grounded-provider-answer.json`
- `.hearth/response-parity/grounded-provider-process.log`
- `.hearth/response-parity/context-splice-share-final.log`
- `.hearth/response-parity/context-splice-embody.log`
- `.hearth/response-parity/grounded-response-comparison.md`
- `.hearth/response-parity/grounded-enriched-process.log`
- `.hearth/response-parity/native-help-replay-candidate.bml`
- `.hearth/response-parity/sliced-observation-probe.log`
- `.hearth/response-parity/sliced-observation-distance.log`
