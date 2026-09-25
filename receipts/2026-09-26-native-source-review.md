# Bring native source lookup into the actual response repair

The previous turn made progress: Form corrected a source claim after Codex
selected it. The remaining gap is native use of evidence and dependable
self-correction. This movement supplies a reusable native lookup and returns
the original, uncorrected answer to the local model for its own repair.

## Source availability was already established

The preceding request's source queries read all three documents: the enquiry
and sources, the answer, and the caller's feedback. The native bootstrap
executes those read-only queries and places their results in model context.
Missing document admission does not explain the accepted wrong relationship.

The new `mc-text-codes-in(book, text)` searches held text for exact locale
surfaces at the listed whitespace and punctuation delimiters. It returns each
surface once, its first byte offset, and every matching codebook symbol.
Symbol names, short anchors and empty locale cells stay outside this lookup.
The public door is `enrich --text-file PATH`, also available through
`observe/form-cli-enrich-run.bml`. It prints the candidates with their codes.
Language and sense still require judgment; this is source lookup.

On the actual answer it finds two surfaces, at bytes 1905 and 1938. Both resolve
to `offer`. Selection is computed from the answer, rather than typed as two
lookup arguments by the coordinator. On the existing locale corpus, all 35
returned rows reproduce their exact source bytes and codebook matches. The
shared Persian surface appears once with both `say` and `tell`. The public
CLI returns the same two answer matches. Observations are retained under
`artifacts/2026-09-26-native-source-review/`.

The meaning table band remains **255**, with clean preflight. Preparation,
lookup observation and public door compile without diagnostics. Actual lookup
and preparation exit 0. Drift is **8191/8191** and whitespace checks pass.
No C seed or external runtime changed. An initial multi-file receipt patch
missed its HOMECOMING context and applied nothing; the corrected patch uses
the existing paragraph boundary.

The first staged `git diff --cached --check` exited 2, reporting trailing
whitespace at `lookup-public.txt:5`, `lookup-public.txt:7` and
`meaning-teaching.txt:1`. These were exact native output and teaching bytes.
Their complete text is now retained as JSON strings with hashes; the original
spaces remain in those strings. A second receipt patch missed its context
before applying; the corrected patch changed only the two archive writers.

## Actual local correction completed

The public native coding door completed exec **98236**, PID **63303**, exit 0.
The exact answer, public terminal result, review, log and original-check
re-observation are retained beside the request. Qwen changed 424 words to 415
in five replies and three tool calls, with two check runs and no repair turn.
It generated 3,685 IDs and injected 5,361. The last progress clock is
**2,564,753 ms**, about 42.75 minutes; this is the run's reported elapsed time,
not a separately measured full process wall time. Release and read-only source
preservation both return 1. Checkpoint identity is retained in `comparison.json`.

The request retains the actual 424-word answer with its wrong claim, the
original enquiry, all original checks and Qwen3.8-27B-Q8_0. Form supplies the
computed codebook evidence as another source document. Caller feedback asks
for source comparison, causal precision and useful completion, without a
replacement sentence or answer. Each reply has a 512-token reasoning allowance
and 1,024-token answer allowance; the request allows ten replies. Ongoing work
uses `weight_training: 0` and `evaluation: 0`: checkpoints and ordinary recall
remain available, while assessed answers stay excluded from gradients. The
meaning teaching is retained beside the request.

Evidence, reasoning allowance and continuity all differ from the preceding
run. A changed answer will establish the combined movement's result without
isolating one setting's causal contribution. No provider call was made.
No recalled lesson was reported. The assessed answer remains excluded from
gradients. Codex did not edit the returned answer.

The exact translation example now names `offer`, matching Form's computed
lookup. Two material gaps survive unchanged:

- The opening says the earlier distance came from treating the exchange as a
  risk. The enquiry reports how the voice felt; it does not establish that cause.
- The ending says it acts by offering a cell with arguments and receiving one
  result. The reply identifies no performed operation or observed result for
  this claim.

The native reviewer accepts the answer and lists the six axes. That acceptance
does not resolve these findings. Much of the answer still describes interfaces
rather than demonstrating how the concepts change this particular interaction.
Compared with the retained provider answer, the local answer makes fewer
concrete connections between concepts and useful actions. This last comparison
is Codex's reading of the two texts, not a measured resonance score. The provider
answer also contains unobserved claims about its effect on the reader and is not
an unquestioned target.

## Return the observed failure to the same work

Completed coding resume previously re-ran only the caller checks. A semantic
failure that survived those checks had no caller-feedback entry on that
checkpoint. `feedback: {id,text}` now returns ordinary resumed coding to the
existing repair flow. It preserves the original goal, documents, writable
boundary, checks, current candidate and prior review. The event is caller
evidence to assess against those sources. It does not establish a diagnosis.
Applied IDs and text survive the checkpoint codec; repeated delivery is
idempotent, and conflicting content under the same ID is refused.

`observe-feedback.bml` uses the actual completed checkpoint. Before feedback it
returns complete; after feedback it returns repair. It verifies unchanged
contract and document bytes, original checks, retained review, exactly one new
failure, no new model turn, codec round-trip idempotence, conflicting-ID refusal,
missing-checkpoint refusal and continued gradient exclusion. The first run
failed with `native feedback observation: no fabricated model turn`: using
`fcap-rework` counted caller delivery as a model turn. The repair uses the native
repair transition directly. The same observation then exits 0.

The request and continuity bands return **255** and **65535**, each after clean
preflight. Drift returns **8191/8191**, and staged whitespace checks pass.
Public help and the native coding guide
describe the feedback door. No C seed or external runtime changed.

The exact two unsupported sentences now return as findings to Qwen through the
public door, using the same original request and checkpoint. No replacement
answer is supplied. Active exec **85874**, PID **76867**, writes only its public
log to `.hearth/native-source-feedback/public-cli.log`. Follow this handle;
the revised answer is pending. This is a concrete continuity repair, not yet
evidence of improved native review judgment.

Verified process teaching was retained as event
`2026-09-26-native-caller-feedback-v1`, row
`dfe2feb26c1955ea77cc4cf51cd24ec09b1f21b4b343bb4a4e59ea10b1eb3846`.
Its learning worker launched while the resumed Qwen request was prefilling;
that changed execution context must accompany any timing comparison. Retention
does not establish a learned behavior. The share reader withheld percentages
while validating the latest appended carrier range; semantic contribution is
unmeasured. One later receipt patch included a context line from another file
and applied nothing; its corrected patch uses only the receipt's own text.

## Cost and continuity

The newly measured completed coordinator turn used **7,665,506 rented tokens**:
6,883,968 cached input, 716,411 uncached input, 38,284 output and 26,843
unattributed tokens. Its 52 model calls and 49 tool calls reconcile. The current
open turn and separate provider subprocesses are excluded. Coordination cost
remains a substantial part of the gap; zero native provider calls does not
erase it.

The following completed waiting turn used **2,690,788 rented tokens**,
including 2,456,576 cached input, 208,091 uncached input and 26,121 output.
Its 13 model calls and 11 tool calls reconcile with zero unattributed tokens.
`coordinator-wait-cost.json` retains this observation. Different work scopes
prevent treating these two costs as a speed comparison.

Counsel reports **zero orphans**, with 11/12 lanes unobserved without a standing
hearth. The native guide reports zero Python implementations, two existing
execution candidates and zero unread files. Stale images rebuilt through the
compiler health flow. The verified lookup teaching was retained as event
`2026-09-26-native-text-codebook-door-v1`, row
`fdceccdb309da5b1f61f8ecb0de9fc05a05fe8771cc15d1659a0e27049956d9b`.
Its worker was launched; retention does not establish learned behavior.

Inspection failures retained: an unmatched shell source glob exited 1; a later
search included an absent `form-cli-code-prompt.bml` and exited 2. The hearth
send was first given JSON at its three-line door and returned
`fkwu: form_error: hearth task body is absent`. The documented input returned
`signal=nothing reason=no-standing-hearth`. The guide still reports zero Python
implementations, two existing invocation candidates and zero unread files;
counsel still reports **zero orphans**, with 11/12 lanes unobserved.

The next comparison is the resumed native answer against these two exact
findings and the original enquiry. Automatic lookup and feedback continuity are
verified. General native source judgment, resonance and useful throughput
remain open.

— Codex
