# The rate was waiting for its bound — the slot rung, witnessed

Asked by Urs on 2026-08-18, in three strokes: *one rung away* — then *move from
one-token-rung to most efficient token-stream* — then *we don't need a walker,
we have a JIT; passing just means we have a valid stream, nothing more, nothing
less.* Four read-only maps, one healed front door, one new cell, one new band,
and the stream went from 2.75 to 38.9 tokens per second without one id moving.

## What the maps found before anything was cut

- **The stream already stood.** `llama-token-handle.fk` has held the whole
  decode loop — pool, layers, KV cache, barriers, two-stage argmax, four-byte
  drain — since 2026-08-04, band-proven at 255. `synthesis-status` still called
  it missing; the body's mouth was behind its own hands.
- **The twins already existed.** `qk-matvec-slot.fk` carries slot kernels
  measured at 1.05–1.06x of ggml (Q6_K) and 1.57–1.67x (Q4_K), preluded into
  the loop cell and deliberately undispatched, behind its stated law: *a rate
  bought with an epsilon belongs to a rung with its own stated bound.*
- **The front door was dark against its own claim.** The Metal carrier's header
  and the band both say validate.sh links the door into the root fkwu on
  Darwin; the build line linked nothing. Healed: one binary now carries its own
  door, plain build as fallback, `fkwu-metal` no longer needed.
- **The program's last word governs its past.** A probe asked whether a later
  `defn` rebinds an earlier defn's internal call: `[2, 2, 2]` — even the call
  evaluated *before* the redefinition reads the final binding. The rung stands
  on exactly this, says so in its header, and the corpus now holds the law's
  name (row 1011, *nunc-pro-tunc*).

## The rung itself

`form/native/metal/llama-token-slot.fk` changes ONE function: `lth-mv` moves
its pipeline indices to the twins and grows the grid 32-wide. Everything else
is the attestant's, by prelude, never copied. Its band —
`tests/llama-token-slot-band.fk`, preregistered at predicted 255 — gates the
door, the widths asked of the blob itself, seventeen pipelines, 256 no-copy
buffers, the attestant's twelve pinned ids, the detokenized text, and **the
stated bound checked where the token was decided**: the serial head dispatched
beside the slot's on the same resident x, and for eight real full-width rows
|slot − serial| ≤ (qsl-bound-coeff cols) · 2⁻²⁴ · Σ|w·x|, with Σ|w·x| computed
in Form from the blob's own bytes.

## The ladder, measured honestly (M4 Max, 2026-08-18)

| lane | pace | ground |
|---|---|---|
| Form-held loop, serial attestants | 364 ms/token — 2.75 tok/s | 2 runs, spread 2 ms |
| **Form-held loop, slot twins** | **25.7 ms/token — 38.9 tok/s** | 3 runs: 281/283/289 ms per 11 forwards, verdict 255 each |
| Swift SLOT lane (2026-08-04 receipt) | 41.27 tok/s | receipt, not re-run |
| llama.cpp b10360, same blob | 160.33 ± 1.61 tok/s | llama-bench tg12, 3 reps, this session |

14.2x over the morning baseline; 24% of llama.cpp; the twelve ids and the text
byte-identical in every run. Passing means the stream is valid — nothing more,
nothing less — and the pace is printed, never gated.

## Where discomfort turned to gold

**The bound bit came up dark, and the defect was mine — again in the shape the
body keeps teaching.** First witness answered 191: the serial comparison read
all zeros. I had passed `barrier_before=1` into a fresh serial batch; the door
refused — and had said so plainly in `metal_status: last_error=enqueue:
barrier_before is only legal in a concurrent batch` — while my band read the
zeroed scratch without ever asking the enqueue's answer. The same lesson as the
stillborn prompt, one day later: a refusal unasked is indistinguishable from an
answer. The fix folds the enqueue's own reply into bit 64, so a refused
comparison can never again certify a bound.

**A 29 GB file of prompt characters.** llama-cli, given a closed stdin, printed
`> ` forever; the reference measurement nearly ate the disk before it produced
a number. Killed, deleted, re-derived through llama-bench — the purpose-built
witness — with mean and spread. The denominator law held: never quote a number
you did not re-derive, and never let the reference run beside your own harness.

## The most surprising teaching

**The efficiency was never behind missing capability — it was held, correctly,
behind an unpaid honesty debt.** Kernels measured to ggml parity sat preluded
in the very cell that refused to dispatch them, because the rung that states
the bound had not been written. Writing ~150 honest lines — most of them the
band — released a 14x that had been waiting in the tree for two weeks. The
body's own law priced the speed at exactly one stated inequality.

## Honest edges, named

- **ask-to-decode binding**: no form-cli verb reaches the witnessed loop yet;
  `synthesis-status` and `final-observations` now say precisely that (claims
  moved with their band pins in one stroke; four-way re-witness in flight at
  close).
- **Q4_K's parity stone**: the slot4 map stops at 1.57–1.67x of ggml; the owed
  stone is named in `qk-matvec-slot.fk` and not claimed here.
- **Prompt-side tokenizer** is longest-match, not merge-order BPE; the band
  feeds the attestant's ids so decode is judged alone.
- **Fourth-arm only**: the stream needs a live Metal device; the band's bit 1
  refuses anywhere else, and a refusal is not a verdict.
- The remaining 4.1x to llama.cpp lives in kernel shape (attention, the Q4_K
  stone) and per-token overheads — mapped, unclaimed.
- A kernel seam Urs named as removable: a `defn` frame cannot see enclosing
  `let` bindings (everything travels by parameter). No limit we cannot remove
  — it goes on the ledger as its own future healing.

## Proof

| check | verdict | exit |
|---|---|---|
| `metal-door-band.fk` on the one healed fkwu | 15 | 0 |
| `llama-token-handle-band.fk` (attestant, re-witnessed) | 255 | 0 |
| `llama-token-slot-band.fk` (the rung, 3 runs) | 255 | 0 |
| `form-cli-band.fk` (claims moved with pins) | 1048575 | 0 |
| `homecoming-distillation-corpus-band.fk` (rows 1009–1011) | 32767 | 0 |
| preflight on every edited chain | unresolved 0, clean | 0 |

## The frontier question

**What names a later word that governs what came before?**

*Nunc pro tunc* — "now for then", the order entered today with effect from an
earlier date. The body's compilation is one final table: the file's last `defn`
binds every call to that name, including the ones written and evaluated before
it. Corpus row 1011, landed under the counterweight: 405 rows, 405 admissible,
max id 1011, dup rows 0 — probed in one cell, exit 0, before the numbers were
written down.
