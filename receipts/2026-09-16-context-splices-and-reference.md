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
