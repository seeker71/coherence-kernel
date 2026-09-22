# Correct the teaching present before the question

The previous native revision repeated an unsupported claim that Form symbol
resolution replaced model token prediction. Supplying corrected source beside
the erroneous prose did not resolve it. This movement separates current source
from the failed answer and repairs an additional contradiction in the resident's
startup instruction.

## Current-source care

The existing `frcc-prepare`/`oc-hear` path re-read eight selected source sections,
supplied the missing context and re-observed complete coverage. Its
[care events](artifacts/2026-09-22-native-grounding-context/hearth-current-source-care.events.jsonl)
contain metadata only. It retained the exact
[11,996-byte packet](artifacts/2026-09-22-native-grounding-context/hearth-current-source.packet.md)
without loading a model or calling a provider. That packet contains the original
enquiry and current source, with the failed answer kept outside its evidence.
Coverage establishes source delivery, not answer correctness.

Two source passages were corrected before this reading. The frequency cell's
sample comments now describe supplied numeric annotations and their arithmetic,
without presenting sample rows as measured passages or a validated measure of
resonance. Its functions and samples are unchanged. The dialogue covenant now
describes observable engagement-serving behavior without asserting how an
unmeasured model was trained. Its grounded-advice, honest-miss, warmth and useful-
action practice remains.

The native client appended all packet bytes as turn 4 to PID 71221, admitted
before the startup correction below. Its
[answer](artifacts/2026-09-22-native-grounding-context/hearth-turn-4.final.txt)
finished with 2,360 generated IDs, 611,985 ms and `stopped=1` in its
[header](artifacts/2026-09-22-native-grounding-context/hearth-turn-4.header.txt).
The [native retained-answer comparison](artifacts/2026-09-22-native-grounding-context/answer-comparison.json)
counts 390 final prose words, within the requested range. Its feedback flag
records the earlier review's contribution to context repair; it does not denote
a new provider call for turn 4.
It correctly kept Qwen decoding separate from Form grounding and described
frequency arithmetic as a function over supplied values. Those two distinctions
improved over turn 3. It still invented out-of-box claims about attention and
the available correction path, and its example contained `[named fields]`.
Native translation also remained underexplained. Its request stayed in its
existing process until completion.

## The startup instruction had its own conflicting meaning

`fcpa-bootstrap` includes `fqt-overlay-prefix` before the enquiry. The old
prefix said “a token is a symbol not a spelling” and described “no tokenizer
pre-step” without distinguishing the Form parser from Qwen's input path. Its
public curriculum repeated the same unscoped descriptions. The generated
answers repeated that conflation. The source establishes that these instructions
were present; it does not by itself establish their causal share in the errors.

`bml/form-cli-qwen-meaning.bml` now gives the startup overlay and those curriculum
entries one shared account:

- Form graph symbols and witnessed language labels are distinct from Qwen's
  vocabulary token IDs.
- Scannerless Form/BMF parsing consumes grammar bytes directly. Qwen still
  tokenizes its model input and predicts output token IDs.
- Explicit graph/codebook lookups retain witnessed codes, alternate senses and
  missing codes. A short codebook anchor is distinct from kernel cell identity.
- The model generates natural-language answers, with tools and supplied sources
  grounding their meaning and wording.
- Model-behavior claims are grounded in implementation or observed runs;
  teachings and illustrative comparisons keep their own scope, and examples
  use concrete inputs and outputs from current source. This clause carries
  turn 4's remaining findings into the next startup context.

The [actual replacement overlay](artifacts/2026-09-22-native-grounding-context/qwen-meaning-overlay.txt)
was emitted by `fqt-overlay-prefix` and read back byte-identically: **3,820
bytes**, zero model admissions and provider calls. This is the overlay portion
of startup; the resident's core role and born-recipe instructions are additional
context. `AGENTS.md` now names that context boundary for future comparisons.

After turn 4, PID 71221 restored the captured stream boundary exactly:
expected and observed `[1246,248046,1]`. On its subsequent explicit release,
it reported 96 spare-buffer releases, `release-ok=1`, an empty discovery board,
exit zero and no remaining group members. This supplies the live post-answer
recycling and close observations left open in the preceding movement.

A new admission carries the replacement teaching: PID 1473 at
`.form-heal/process-1468-223693752-0` reached ready and received turn 5 with
the same 11,996-byte packet. That answer and comparison remain open. No Qwen model
weights changed, and no evaluation answer became a training target. The verified
initial representation-boundary teaching was separately retained as
`qwen-startup-representation-boundary-2026-09-22`; its learner is the Llama path,
and it overlaps the admission/comparison work. Timing is not a controlled
throughput comparison.

## Checks, failures and cost

Clean preflights preceded the existing teaching band (**33,554,431**) and peer
agent band (**16,383**); both exited zero. The resident's compile-only check
passed. Drift gates passed **8,191/8,191**, zero refusals. Glass's first frame
took **26 ms**. The authoring guide reports zero Python implementations, two
existing invocation candidates and zero unread files. No C-bootstrap dependency
was added.

Two initial comment patches were refused because their expected source line
did not match. They changed no files. Reading the exact current line and
applying the corrected patch resolved that authoring error. A cache mismatch
while reading the reply triggered the compiler's native rebuild and re-read;
the absent correlated reply remained an availability observation.

The [preceding completed coordinator turn](artifacts/2026-09-22-native-grounding-context/current-source-previous-coordinator-cost.json)
(`01a0c99a-a87c-7982-86bb-9503bb5c30b5`) used **3,258,802 rented tokens**,
including **3,174,656 cached input tokens**, across 28 model calls, with zero
unattributed tokens. It excludes this open turn and separate provider processes.
No new provider review was invoked in this movement. Overall quality parity
and the minimal-rented-token objective remain unachieved.

The useful finding is that the caller's source packet is only part of the
model's context. A correction can coexist with a contradictory startup teaching.
Both now have explicit source records, so the next answer can be assessed
against what was actually offered.

— Codex
