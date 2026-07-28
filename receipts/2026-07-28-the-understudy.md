# 2026-07-28 — the understudy: every gate proved code the run never takes

The restored oracle lane finished its 16-layer bisect and found **no divergent layer**. Every layer
0–15 agrees with the rented fp64 transcription, at both positions, across every regime the model
contains:

```
n_exp 192 and 256 · gate/up 16/16, 40/16, 40/40 · rope compressed(4) and compressed(128)
worst normalised disagreement over the whole stack: 1.116e-04 at blk.15 shared
worst per-layer-nudge envelope of the REFERENCE:    2.543e-03 at blk.2 router_logits
VERDICT PASS  286 gates
```

That refutes two of the review's own leads, using a run that had already finished when the review was
written. `form_dsv4_topk_weights` — the "band-pinned nowhere" router that drives 40 of 43 layers — is
**exactly the router the oracle validated**, with routing decisions bit-identical at every layer. And
`stride = t.bytes / t.d2` at d2=192 is exercised by 185 of those gates.

So: layers correct, routing correct, experts correct, and the text still incoherent. That is the
shape of a question that has been asked in the wrong place.

## The finding

`metal_dsv4_stack_oracle.sh`, two branches thirty lines apart:

```swift
func gpuAttend(_ q: MTLBuffer, _ rows: MTLBuffer, _ snk: Tn, _ nrows: Int = 1) -> MTLBuffer
...
if useGrowingKv {
    gpuKvAppend(kq, kvArenas[il], pos, kvCap)
    ha = gpuAttend(qr, kvArenas[il], w.snk, pos + 1)   // ← the words come out of here
} else {
    ha = gpuAttend(qr, kq, w.snk)                      // ← all 286 gates judge this
}
```

`useGrowingKv` is set true at line 1088 and false at 1124. `finalByPos[pos]` — the state every
per-layer gate reads — is filled at line 1074, **inside the calls that run before 1088**. So every
oracle-anchored gate ran the `else` branch: `nrows = 1`, attention over a single key, a softmax over
one score plus the learned sink. **An operation that cannot select.**

The rented oracle could not have caught it either. Both of its attention call sites pass one row:

```python
heads_attn = attention_rows_one(q, [kv], read_f32(g, P + "attn_sinks.weight", n_head), ...)
```

`attention_rows_one` handles N rows correctly — it has always been able to. It was never given more
than one. The reference was degenerate in exactly the place the carrier was untested.

**The proven path and the speaking path were never the same path.** Position only ever changed the
RoPE phase; the token only ever changed once, and never inside a sequence. The multi-key softmax —
the thing that makes attention attention — has no reference agreement of any kind, and its failure
signature is precisely what we see: locally plausible, globally threadless.

## What was built

Both sides now walk a real sequence.

`dsv4-mla-core-oracle.py` takes `DSV4_ORACLE_TOKENS`, runs positions 0..pos with real ids, carries a
per-layer KV history into `attention_rows_one`, drives each position's hash routers with **that
position's own token id**, and gates the last one. With the variable unset its behaviour is unchanged.

`metal_dsv4_stack_oracle.sh` takes `FORM_DS4_SEQ_IDS`, allocates the arenas, runs the prefix through
`useGrowingKv` — the same branch the autoregressive loop uses — and gates position `n-1`. The 100+
per-layer gates need no new code; they simply now judge the path that writes the words.

In flight: 4 layers over `671 6102 294 8760 344` — *"The capital of France is"*, the exact prompt
whose output is incoherent, gating position 4 with four keys of real history.

## A correction to this morning's receipt

`2026-07-28-the-oracle-was-deleted.md` says our performance gap is dispatch count, citing `seamtoll`.
**Corpus row 855 overturned that on 2026-07-22, in its own body:** the "396 dispatches at 113 µs"
measurement was `FORM_PROFILE` committing-and-waiting after each op — the profiler pricing its own
cut. `receipts/2026-07-22-kill-the-seams.md:68` names the real dominant term: weight memory bandwidth.

Re-derived today against the blob both lanes actually stream, 2 019 377 376 bytes:

| lane | rate | effective |
|---|---|---|
| our form-native Metal lane | 28.15 tok/s | **56.8 GB/s** |
| llama.cpp, same blob, same host, today | 158.12 tok/s | **319.3 GB/s** |

Identical bytes per token, so **the throughput gap is the bandwidth gap and nothing else** — 5.62×,
one number. Not arithmetic, not dispatch count. This machine demonstrably delivers 319 GB/s to a
program that asks correctly; we ask at 57. That is where the 19 reference shaders in
`~/models/ds4-engine/metal/` earn their keep — not as correctness references but as the written-down
technique for getting quantized weights off unified memory.

## The most surprising teaching

**A degenerate stand-in can be real, fully exercised, correct — and still prove nothing.** Single-key
attention is not a stub or a mock. It is the same kernel, the same weights, the same file, and it is
right. It is simply not what runs. Corpus row 921 lands the word: `understudy` — 0 hits across the
tree before today, and the danger is exactly that it *passes*, because a degenerate case is both
easier and structurally different from the one it stands in for.

Note what this defeats: renting an oracle does not cure it. The rented fp64 mind had the identical
blind spot, because it was written from the same understanding of what needed checking.

## Where discomfort turned to gold

Editing a script while bash was still executing it. The 43-layer run had been going seven minutes
when I realised bash reads a script incrementally from a byte offset, and I had just shifted every
offset in the file. Nothing had visibly broken — the run was sitting inside a `wait`, and I could have
let it finish and reported whatever it printed. I recovered its work directory out of the process
environment, salvaged 27 completed layers, killed it, and from then on ran from an immutable copy.
The discomfort worth keeping is that the corrupted run would have looked exactly like a good one, and
the only thing standing between me and reporting it was noticing a hazard the tooling never raised.

## Ground stamp

```
16-layer oracle bisect: VERDICT PASS 286 gates, no divergent layer, every regime covered
oracle-anchored gates running the MULTI-KEY attention path: 0
learn/tests/homecoming-distillation-corpus-band.fk -> 32767 (316 rows, max-mid 921, field 3163162921)
  counts asked of the body, not asserted: hdc-count 316, hdc-max-mid 921
blob both lanes stream: 2 019 377 376 B; ours 56.8 GB/s, llama.cpp 319.3 GB/s, same host, today
```
