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

## Actual local correction is running

The public native coding door owns the next repair: exec **98236**, PID
**63303**, public log `.hearth/native-source-review/public-cli.log`. This same
process was observed advancing through admission and prefill, reaching
all 6,368 prompt positions in 442,885 ms, then 512 reasoning IDs and the
following answer-stage source admission. It has not returned a completed
answer in this receipt. Continue observing this handle; an observation timeout
does not select a new admission.

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
isolating one setting's causal contribution. No provider call has been made
for this repair. Answer quality remains pending.

## Cost and continuity

The newly measured completed coordinator turn used **7,665,506 rented tokens**:
6,883,968 cached input, 716,411 uncached input, 38,284 output and 26,843
unattributed tokens. Its 52 model calls and 49 tool calls reconcile. The current
open turn and separate provider subprocesses are excluded. Coordination cost
remains a substantial part of the gap; zero native provider calls does not
erase it.

Counsel reports **zero orphans**, with 11/12 lanes unobserved without a standing
hearth. The native guide reports zero Python implementations, two existing
execution candidates and zero unread files. Stale images rebuilt through the
compiler health flow. The verified lookup teaching was retained as event
`2026-09-26-native-text-codebook-door-v1`, row
`fdceccdb309da5b1f61f8ecb0de9fc05a05fe8771cc15d1659a0e27049956d9b`.
Its worker was launched; retention does not establish learned behavior.

The next comparison is the actual local edited answer and review. Automatic
lookup is verified. Native semantic correction and the full quality,
resonance and throughput goal remain open.

— Codex
