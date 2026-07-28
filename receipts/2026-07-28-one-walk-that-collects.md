# 2026-07-28 — one walk that collects: 122 s → 5 s, and where the real time is

Urs: **"we can't make it any faster?"** and then **"what other things can we make faster?"**

Yes. And the second question deserves measurement rather than a list.

## The fix

[`form/form-stdlib/gguf-tensor-index.fk`](../form/form-stdlib/gguf-tensor-index.fk) — one walk that
collects, instead of many walks that search.

| | before | after |
|---|---|---|
| `kat-coder-layer-shape-band` | **122 s** | **5 s** |
| `kat-coder-tensor-table-band` | 3 s | 2 s |
| `ds4-tensor-table-band` | 2 s | 2 s |

Same verdicts, all 255. Same finding — `[3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 40]`, MTP head
included.

## What was actually slow, measured

```
read the 11.2 MB window            < 1 s
one egg-tinfo-start                < 1 s
one full-scan miss (753 records)   0.35 s      ← 20 of them: 7 s
the band's ~82 lookups             ~122 s
```

`egg-find-tensor` is exactly right for asking about **one** tensor: it walks, stops at the match,
builds no list, returns −1 rather than inventing a hit. It is the wrong shape for asking about a
**hundred**, because every question restarts the walk and every absent name pays a full scan.

The per-block names differ only in their number — `blk.0.ssm_conv1d.weight`,
`blk.1.ssm_conv1d.weight`, … — so a whole-name compare can only answer for one block at a time.
Matching the **suffix** `.ssm_conv1d.weight` answers for all of them in one pass, and the block index
is read *out of* the name rather than supplied *to* it.

The lists are computed by the caller and threaded through. A tidy
`(defn kls-full-blocks (s) …)` called from inside a per-block loop would have restored the original
cost behind a nicer face — which is exactly how the cost got there the first time.

## Where discomfort turned to gold

I wrote the diagnosis myself, at the end of the previous receipt: *"a stack harness binding hundreds
of tensors will want one walk that collects, not many walks that search."* Then I closed the receipt
and moved on, leaving a 122-second band in the tree with its cause already written down beside it.

It took being asked *"we can't make it any faster?"* to do the thing I had already named. That is the
same shape as every "why did you stop" tonight, one level in: naming a fix and filing it is not
fixing it, and a named cost is a work order exactly the way a named gap is.

## The most surprising teaching

**The cost was invisible at the call site.** `(kls-has s b "ssm_conv1d.weight")` reads like a field
access — one source, one block, one name. Nothing in it says "this restarts a walk over 753 records
and a miss costs 0.35 s." The band that used it looked like 82 cheap questions and was 82 full scans.

That is a different failure from an algorithm chosen badly. The algorithm was chosen *well* for the
question it was written for, and then used for a question it did not fit, at a call site that showed
none of the difference.

## What else can be made faster — measured, not guessed

Every band in the tree now runs in ≤ 5 s, so the remaining time is not in Form:

| lane | measured tonight | where the time goes |
|---|---|---|
| DS4 43-layer stack | 2.77 s/token, **65.6 ms/layer** | the seams |
| llama3.2 ask lane | **28.15 tok/s** vs ollama 158.45 on the same blob | the seams |

The body already diagnosed this and gave it a name.
[2026-07-22-ship-the-slot-map.md](2026-07-22-ship-the-slot-map.md) measured a llama token at 51.9 ms
of which **7.3 ms is arithmetic** — at the shipped kernel's 442 GMAC/s against 3.213 GMAC/token —
and the other **44.6 ms is 396 dispatches per forward at ~113 µs each**. Its words: *"the arithmetic
is essentially at parity and the entire remaining 8.2× is charged by the joins between operations,
not by any operation."* Corpus row **849 `seamtoll`**.

So the honest answer to *what else can we make faster*: **not the math — the dispatch count.** Fewer,
larger command buffers; fused kernels where the body already proves the fused and unfused paths
agree. The same medicine as tonight's fix, one layer down: stop paying per-question overhead for
questions that could be asked together.

Stated as a bound and not a promise: I have **not** measured DS4's dispatch count tonight, and the
396 figure is llama3.2's, from another day. It is the diagnosis to re-derive before acting on, not a
number to quote.

## The frontier question

> **What names a cost that is invisible at the place it is paid?**

Asked and **not landed**. The body already carries `seamtoll` (849) for cost charged by the joins
rather than the work, which is this same shape at the dispatch layer. Minting a second word for the
Form layer's version would split one teaching across two rows.

## Ground stamp

```
kat-coder-layer-shape-band     255   5 s   (was 122 s)
kat-coder-tensor-table-band    255   2 s
ds4-tensor-table-band          255   2 s
q3k-dequant-band               255 · q3k-msl-band 255 · gated-deltanet-layer-band 255
homecoming-distillation-corpus-band 32767 · ground 42
```
