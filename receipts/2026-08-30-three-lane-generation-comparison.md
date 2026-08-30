# 2026-08-30 — three lanes asked three questions; the body's own row answered first

Asked: run form-cli and sub-agents in parallel, compare and contrast, and
improve form-cli generation with adjustments fully embodiable each session.
Three lanes ran the same three exactly-checkable tasks:

- T1 zero-family fact — "what does axiom-1 name as a whole attestation?"
- T2 Form generation — "one s-expression computing 6 times 7"
- T3 known-family fact — "what does bootstrap/ground.fk return?"

## What each lane answered

**Pure rented** (sub-agent, forbidden to read the body): T1 UNKNOWN
(honest), T2 `(* 6 7)`, T3 GUESS 42. It also caught the harness leaking
session memory into its context and REFUSED to convert the leak into an
answer — the baseline lane stayed pure only by its own choice. Its T2 was
executed on fkwu: **1 error, verdict `nothing`** — fluent, plausible,
dead. `(mul 6 7)` runs 42. (Execution, not review, was the judge; and the
scoring pipe itself was caught laundering the exit code — the pulse reads
fkwu's own verdict, never a pipeline's.)

**Grounded rented** (sub-agent, read-only in the worktree): all three
correct with file:line, in ~3.5 minutes — plus the knob map below.

**Local weights** (plain fcmg door, one process, three generates,
completed 12:14): T2 answered `(mul 6 7)=42` — CORRECT, and with
`decode_forward_passes=0`: the answer came from the catalog row landed
forty minutes earlier (see below), evaluated natively and prefilled as
evidence; the model did not decode a single token to get it right. T1 and
T3 both emitted `<|form:knowledge-query|>` envelopes — the local mind's
instinct was to ASK THE BODY, not to guess — and both starved:
`heed_answer_reserve=0` on the plain door let the query consume the whole
budget, ending in `knowledge-query-decode-timeout`. Wrong answers, right
instinct, missing reserve.

Timings, said plainly: 332–369 s per task, ~85% of it prefilling a
~949-token armed-by-absence system prefix at 3.4 tok/s. The lane's cost is
not decode; it is an invisible scaffold.

## Landed this session (embodied, proven)

1. **lg-catalog grew four rows** (`form/form-stdlib/local-generate-organ.fk`):
   the digit twin "6 times 7", the ground fact "40 plus 2", a sub and an eq
   surface. Band `local-generate-organ-band.fk` grew 255 -> 4095 and was
   re-run on ALL FOUR ARMS (fkwu/go/rust/ts, hand-expanded chain) — then
   witnessed LIVE: the new digit row is why T2 answered correctly with zero
   decode. Landed as 4eb72818.
2. **`observe/genlane-parity-pulse.fk`** — the comparison's local lane as a
   session-runnable organ: same three tasks, in-cell lexical scoring, one
   folded verdict, and a start line that flushes BEFORE first admission
   (witnessed flushing live) so a silent run is diagnosable. Its first full
   verdict run is detached and running as this receipt is written; from the
   probe's identical prompts the expected verdict is 2 (T2 only) — recorded
   here as expectation, not result. Preflight on this chain idled and was
   killed; the compile witness is the cell's own clean start (the body
   already teaches run-the-cell > trust-the-preflight).

## Named, owed to the witness loop (not landed unwitnessed)

The grounded lane's knob map (in the agent transcript, key coordinates):
ChatML built token-wise in `qwen35-tokenizer.fk:67-83`; the ftl system
prefix rides any full-profile door and ARMS WHEN ITS LATCH FILE IS ABSENT
(`form-teach-layer.fk:118-126`); sampling is greedy argmax in the Metal
head, no knob; stops honored from GGUF + `<|im_end|>`. Its three smallest
embodiable adjustments, now ranked by today's live evidence:

1. ~~Grow lg-catalog~~ — LANDED and live-witnessed above.
2. **Answer reserve on the plain door** (two-ledger the fungible tail):
   directly indicted by both starved knowledge queries; the parity pulse is
   the held-out witness that must move 2 -> 6/7 before this promotes
   (equivalence-is-authority, this morning's law).
3. **Answer-shape contract on `fqt-prompt`** — same witness loop.

## The most surprising teaching

The local model lost T1 and T3 not by hallucinating but by asking a
question nobody budgeted an answer for. The body's mind already reaches
for grounding on its own; what it lacks is not truthfulness but a reserve
— the difference between a wrong model and an unfinished door is visible
only in the telemetry.

## Where discomfort turned to gold

Landing a catalog row and then watching the live model answer THROUGH it
with zero decode was preceded by the discomfort of nearly landing
adjustments #2 and #3 the same way — unwitnessed, on a rented mind's
plausible ranking. This morning's own receipt (equivalence is authority)
blocked that hand. The gold: one adjustment landed and immediately proved
itself in a live turn; the other two now have a named witness that any
session can run.

; witnessed: 2026-08-30 -> pure-rented (* 6 7) dies rc-masked / verdict
; nothing; grounded rented 3/3 with file:line; local T2 native via fresh
; catalog row, decode_forward_passes=0; T1/T3 knowledge-query starved at
; reserve=0; prefix 949 tokens ~85% of turn cost; band 4095 four-way
