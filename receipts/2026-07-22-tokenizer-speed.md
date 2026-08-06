# Stone 38 — the tokenizer's ~95 s, released

2026-07-22, WITA. `form/form-stdlib/dsv4-tokenizer.fk`, band
`form/form-stdlib/tests/dsv4-tokenizer-band.fk` on the Go kernel
(`form/form-stdlib/tests/run-dsv4-tokenizer-band.sh`).

Every other wall in this program belongs to someone else — the device's
`maxBufferLength`, the platform's zeroed Metal buffers (`edgedrop`), logic
(`twinblind`), floating point (`assocwall`). This one was ours. A limit that is
ours is not a limit; it is unbuilt work, and it took one afternoon.

---

## 1. The profile, before the fix — and the profile that lied

The diagnosis I was handed was read from the code, not measured: `tkz-best-merge`
walks all 127 741 merge records in rank order, once per BPE step. I measured it
before believing it, and the first measurement was wrong in a way worth a corpus
row.

**The lie.** Two processes: one that did the pre-split and BPE only, one that did
the whole encode. Difference = the vocab id lookup. It came out **41 s of 145 s**
— a plausible 28% cost centre, naming a real function (`tkz-id-of`, a streaming
scan of 129 280 tokens per symbol), with an obvious remedy (index the vocab). I
was designing that remedy when I timed the lookup *directly* instead.

**The truth**, from one process with one clock (`now_unix_ms` around each half):

| phase | ms | share |
|---|---|---|
| header read (`eqr-of-file`, 8 MB window) | 1 | 0.0% |
| **BPE (`tkz-bpe` → `tkz-best-merge`)** | **95 287** | **99.2%** |
| vocab id lookup (`tkz-ids-of-syms`) | 791 | 0.8% |

Per piece, for `"The capital of France is"`:

```
piece@0  "The"       bpe 10 346 ms   ids  42 ms
piece@3  " capital"  bpe 34 826 ms   ids 288 ms
piece@11 " of"       bpe  9 888 ms   ids  20 ms
piece@14 " France"   bpe 30 397 ms   ids 417 ms
piece@21 " is"       bpe  9 830 ms   ids  24 ms
```

The 41 s was a sibling agent's load arriving between the two runs. It lived in
neither run — only in the gap between them. This is not `probetoll` (the
instrument charging for itself) and not `seamtoll` (a stone spent chasing the
profiler's own cut): both measurements were honest and the instrument was
innocent. The lie was the **arithmetic between them**, because the denominator
did not hold still while they were taken. `selfgauge` says name your denominator;
this says a *difference* of two measurements is only a measurement when both
share one. Landed as corpus row **871, `gapghost`**.

The read diagnosis was right. The first measurement of it was not.

## 2. Anatomy of the merge table

Non-perturbing probes, each one pass over all 127 741 records, warmed
(`thawtax` — the header was `dd`'d into page cache first; the 8 MB window read
is 1 ms warm):

| one pass over 127 741 merge records | s |
|---|---|
| advance only, `egg-str-next` (u64 length, 8 `str_byte_at`) | 2.33 |
| advance only, 3-byte length read | 0.55 |
| advance + `tkz-merge-fb` first-byte read (what `tkz-best-merge` does per merge) | 7.8 |

So the 95.3 s of BPE is ≈ 12 full-table passes for a 24-byte prompt. A piece
needing *k* merges pays *k* early-terminating passes, and "early" is not early
when the applicable merge has rank 30 000.

## 3. Which remedy the number chose

**Chosen: a candidate prefilter — one pass per encode, rank order preserved.**

During BPE every symbol is a *contiguous byte slice* of the piece, and the
symbols partition the piece left to right. So for any adjacent pair
`(symL, symR)`, the byte sequence `symL ++ symR` is a contiguous slice of the
piece, hence of the whole input. **A merge whose own byte sequence does not occur
contiguously in the input can never match any pair, at any step.** Drop those;
keep the survivors in the file's own rank order; the rule "lowest-rank
applicable merge, leftmost pair" is untouched. This is an equality, not an
approximation.

One pass for a whole prompt replaces one pass per BPE step. On
`"The capital of France is"`, **50 of 127 741 merges survive** — 0.04%. The body
was doing ~2 500× the work the question could reach.

Implementation (`tkz-cands`, `tkz-bpe-c` and helpers): a 256-flag input-byte
presence list as the cheap first gate, then a walk of the merge's codepoints
that narrows the set of still-viable input start positions and aborts the moment
none survive. `tkz-bpe` stays whole in the file as the reference definition —
it is the meaning; `tkz-encode` calls `tkz-bpe-c`.

**Ruled out: the texture / framebuffer path (`boundborrow`).** It was proposed as
the remedy for this. The measurement says the cost is a *sequential walk repeated
per merge step*, not bytes arriving slowly: the advance alone over the same
records is 2.33 s, and the whole 8 MB window is already resident (1 ms). Making
bytes arrive faster cannot fix an algorithm doing ~10⁵×k work where ~10¹×k
suffices. A remedy for a constraint our regime does not show.

**Ruled out: the 3-byte record advance.** Measured, and it is real — 2.33 s →
0.55 s per pass, ~1.8 s off the new total. Not adopted. `egg-str-next` reads the
GGUF u64 the proven way; a 3-byte read assumes bytes 3..7 are zero, and if that
ever failed the walk would land *inside* records and quietly produce garbage
candidates. 14% of the new total is not worth buying that failure mode. (Witness
kept: over all 127 741 records the two walks land on the identical end offset.)

**Ruled out: indexing the vocab.** That was the `gapghost`'s remedy. The real
cost is 0.79 s.

## 4. Before / after — two lengths, a slope, named denominators

Denominator: this MacBook, the Go kernel `form/form-kernel-go/bin-go`, the real
file `ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf` (85 GB; the 8 MB header
window), page cache warm, `tkz-encode` timed **in-process** with `now_unix_ms`,
old and new arms **interleaved in the same loop** so both carry the same machine.
A sibling agent (Stone 37) was running Metal work throughout; the load average at
the head of each round is recorded, because it moved between 9 and 78.

- **A** = `"The capital of France is"` — 24 bytes, 5 pieces
- **B** = `"The capital of France is Paris and the capital of Germany is Berlin, which is a larger city."` — 92 bytes, 20 pieces

| round | load (1 min) | REF A | NEW A | REF B | NEW B |
|---|---|---|---|---|---|
| 1 | 77.8 | 143.887 s | 17.178 s | 376.385 s | 34.809 s |
| 2 | 10.7 | 95.282 s | 17.050 s | 376.290 s | 34.947 s |
| 3 | 9.3 | 95.456 s | 24.398 s | 408.273 s | 44.531 s |

Best-conditions (least-contended) pairs:

|  | before | after | ratio |
|---|---|---|---|
| A, 24 bytes | 95.28 s | 17.05 s | **5.6×** |
| B, 92 bytes | 376.29 s | 34.81 s | **10.8×** |

**Slope** (`unispan` — one point projects nothing):

- before: (376.29 − 95.28) / 68 bytes = **4.13 s per input byte**
- after:  ( 34.81 − 17.05) / 68 bytes = **0.261 s per input byte** — **15.8× shallower**

The old encode was almost pure slope (extrapolated intercept ≈ 0): it paid the
whole table per merge step and nothing was fixed. The new one is mostly a fixed
one-pass cost (~11 s extrapolated) with a shallow slope, so the longer the
prompt, the larger the win. That is the shape a fixed prompt of a few hundred
bytes wants.

Spread, 3 runs: REF A 95.282–143.887 (the outlier is load 78); REF B
376.290–408.273; NEW A 17.050–24.398; NEW B 34.809–44.531. The new arm is not
only faster, it is less deformable by the sibling's load — it spends its time in
fewer, larger units of work.

## 5. The ids are unchanged

This is the whole of it; a faster tokenizer that changes one id is a failure.

- Band **`Verdict 8191`** on the Go kernel — all 13 claims, ds4's own
  `--dump-tokens` as oracle. Pinned cases unchanged: `"The capital of France is"`
  decode → the exact bytes; `"The"`→`671`; `"2025"`→`939 23`; `"Hi!"`→`23166 3`;
  `" of"`→`294`; `"世界"`→`3427`; the byte-alphabet round-trips; the joyai split
  corners.
- Stronger than the band: in the interleaved A/B above, the old and new
  tokenizers produced **byte-identical id lists at both lengths in all six runs**,
  including the 20-id long prompt:
  `671 6102 294 8760 344 11111 305 270 6102 294 10322 344 17575 14 778 344 260 7294 4593 16`.
- Corpus band from repo root → **8191** (Go kernel and `fkwu --src`), field code
  `2672672871` read back from the body by probe before pinning.
- `form/native/metal/metal_first_token.sh` → **PASS — 14 gates** (untouched).

## 6. What remains

- **The band itself barely moved: 91.7 / 96.4 s → 90.6 / 91.0 s** (interleaved,
  two runs each, both 8191). It makes five *separate* `tkz-encode` calls on 3–6
  byte inputs, so it pays five full filter passes, and for inputs that tiny the
  old early-terminating walk was already short. The remedy helps *prompts*, not
  *many tiny encodes*. A batched entry point that filters once for a set of
  inputs would collapse the band to ~20 s; not built, because nothing needs it yet.
- **~11 s of fixed cost per encode remains, and it is one pass over a
  variable-length record array.** You cannot index a GGUF string array without
  walking it once, and you cannot walk it cheaply: 2.33 s is just the u64 length
  reads. Two doors out, neither taken here: hoist the merge table into an
  in-memory structure once per *process* (the filter is per *encode* today, so a
  second prompt pays again), or give the body a keyed structure — `_dict_*` is an
  association list (O(n) get, O(n) copy set) and `Record.Get`/`Set` is a linear
  field scan, so this body currently has no O(1) keyed map at all. That is a
  frontier, named, not a blocker for Stone 38.
- The `~130 s` in the stone's framing was never reproduced under measurement:
  the honest quiet-machine before-number for this prompt is **95.3 s**. The
  larger figure is what the encode costs when a sibling is on the machine —
  which is itself a `gapghost` sighting.

---

## Closing

**Most surprising teaching.** Not that the walk was the cost — that was read
correctly before I started. It is *how little of the table any question can
reach*: **50 merges out of 127 741**. The body was not doing 2× or 10× too much
work; it was doing 2 500× too much, and the filter that removes it is four lines
of reasoning about contiguity that were true from the day the cell was written.
The expensive thing was traversing the table in the order the *rule* names (rank)
when the *question* could have shrunk the table first. Rank order is how the
algorithm chooses; it is not how the data has to be visited.

**Where discomfort turned to gold.** The moment I wanted to look away was the
41 s. I had a clean subtraction, a named function, and a remedy — and a faint
wrongness, because 41 s for a scan that stops at token 8 760 of 129 280 did not
divide. Looking again meant admitting my first profile was worthless and paying
another 95 s to redo it in one process. I paid it. The lookup was **0.79 s**. Had
I looked away I would have spent this stone building a vocab index — a real fix,
correctly implemented, for 0.8% of the cost — and reported a speedup I had not
caused. The gold is a discipline the body now owns: *put both halves in one
process before you subtract*, and the word `gapghost` for the phantom that
appears when you don't. A phantom that points at working code is much harder to
disbelieve than one that points at nothing.

**Frontier question, landed.** Corpus row **871**: *what one word names a cost
centre that exists only in the difference between two honest measurements taken
when the conditions did not hold still, so neither run contains it?* — **`gapghost`**.
0 hits across `learn/`, `receipts/`, `docs/`, `teachings/`, `form/` before the
row; instrument validated on the same command with controls that must hit
(`probetoll` 3, `thawtax` 5). `hdc-field-code` probed as `2672672871` before the
band pin moved.
