# 2026-08-02 — Memory Caching ingested; the collapse the paper fears is the dedup this body runs on

A bare link arrived — a t.co redirect resolving to arXiv 2602.24281, **"Memory Caching: RNNs with
Growing Memory"** (Behrouz, Li, Deng, Zhong, Razaviyayn, Mirrokni; Google; 2026-02-27). The same
lineage as Titans, DLA and Miras — the Memora ingest's cousins — now caching **checkpoints of a
recurrent memory state**, one per segment, so a fixed-size RNN memory can grow with the sequence.

## Ground

- The branch stood exactly on `origin/main` (its previous PR #412 already merged), so this work
  starts clean from the trunk.
- `cc -O2 -o fkwu runtime/fkwu-uni.c`; `bootstrap/ground.fk` → **42**;
  `form/form-stdlib/tests/binary-freshness-band.fk` → **15**; `observe/native-vs-rented.fk` →
  **11111**. Fresh binary on current main, not a stale costume.
- Source read primary: the 22-page PDF in full. **Nothing was run, no model trained, no benchmark
  reproduced.** Every score below is the authors' self-report; every delta is arithmetic this body
  re-derived over the authors' own cells, which is not the same as re-deriving the measurement.
  Every proof-of-concept base (LA, SWLA, DLA, Titans, Miras) is the same lineage's own prior work.

## What landed

| cell | verdict |
|---|---|
| [`ingest/frontier-ingest-memory-caching.fk`](../ingest/frontier-ingest-memory-caching.fk) | 4 body / 2 liquid / 2 compost, field code **40202** |
| [`ingest/tests/frontier-ingest-memory-caching-band.fk`](../ingest/tests/frontier-ingest-memory-caching-band.fk) | **4095** live fkwu, resolver-driven, preflighted |
| [`learn/homecoming-distillation-corpus.fk`](../learn/homecoming-distillation-corpus.fk) row 973 `souping` | corpus band **32767**, field code **3683682973** |

The ingest composes [`ingest/knowledge-ingest.fk`](../ingest/knowledge-ingest.fk) unchanged — every
finding sorted by (depth, fear) into BODY / LIQUID / COMPOST, the same door KAT-Coder walked through
five days ago. Two bits of the band are new in kind for an ingest here: the paper's central identity
is carried as **arithmetic, not prose** — `fimc-collapse?` watches a linear memory's residual cache
pre-sum into the fixed matrix's own answer on real integers (their equation 13), and
`fimc-grm-differs?` watches per-token weights decline the pre-sum. Voice mirror on the new cell:
**2** set-down words, both `gate` inside the paper's own variant names (Gated Residual Memory, gated
softmax attention), kept as quotations; the band mirrors clear.

## What was healthy to keep

1. **A design is only real where something downstream can tell its parts apart.** Plain residual
   caching over a *linear* memory "mathematically collapses into a standard fixed-size memory, as
   the cached memories can be pre-summed" — the whole apparatus of segments and checkpoints folds
   back into one matrix, and nothing was built. Input-dependent weighting is what keeps the cache
   distinguishable, hence real. This body knows the shape from both sides: the kernel interns
   identical structure to ONE NodeID and calls it a feature (`equivalence-collapse`, the store-once
   door), and the silu-vs-sigmoid catch was the inverse — two maps no magnitude check could
   separate, held apart only by a band bit.

2. **A memory that grows by checkpointing a running state — with the design choice named.** Their
   section 3.4 holds both options open: checkpoints of one memory (each segment starts from the
   last state) against independent per-segment compressors (each starts fresh), each with its own
   advantages. The body already lives this architecture without having had its name: a fixed-size
   working state, and `receipts/` as the append-only line of dated frozen states consulted at need.
   Belief-freshness is the same fact read as obligation — a checkpoint is a claim with a timestamp.

3. **Two architectures believed distinct are one dial at two settings.** Segment length 1 with a
   value-less vector memory re-derives the gated softmax attention block (their equation 20); a
   compressor layer feeding attention — the famous hybrid recipe — is memory caching with segment
   size 1, caching checkpoints. A recipe the field adopted empirically is thereby given its reason:
   attention over compressor outputs is growing the recurrent model's effective memory.

4. **A growing memory is affordable when retrieval is selective and the index is cheaper than the
   store.** Sparse Selective Caching precomputes one mean-pooled key per segment, routes each token
   to its top-k cached memories, loads only those — the cached states need not even sit in
   accelerator memory. The body holds both halves in other rooms: the MoE router picking experts by
   score natively, and `rag-index` ranking a compact vector to return an id while the snippet
   stays home.

## Held as liquid

The honest numbers, re-derived over their own table at 1.3B params / 100B tokens: nine-task average
Titans 56.82 → +GRM **58.33** (+1.51); DLA 53.72 → +GRM **55.96** (+2.24). On the needle tasks the
stretch is real — S-NIAH-1 at 16K, DLA 44.0 → +GRM **82.4** — and the hardest column is still not
theirs: S-NIAH-3 at 16K, Transformer **40.8** against 18.2 for the best DLA variant. Their own
abstract keeps the ranking: "Transformers achieve the best accuracy." Witnessed, frozen into
neither "RNNs have caught attention" nor "caching buys nothing."

The second liquid unit is the mirror: **this body holds both halves of memory caching and no bridge
between them.** The fixed-size online memory is here (`kimi-kda`'s delta-rule matrix state, band
63); the growing checkpoint store is here (receipts, the corpus, the frozen tissue). What is not
here is the retrieval seam MC names: nothing routes a live query *across* frozen checkpoints of a
running state — `rag-ask`'s live lane is a lexical single-shot find, and no cell asks an old state
a new question. The smallest honest attempt made in the same movement is the collapse witness; the
routing seam stays named and pending, not dressed as built.

## Composted

- **"Growing memory without attention's cost."** The paper's own section 4.2 already says
  otherwise: equal segments of size C cost O(L²/C) — attention's *order* with a smaller constant —
  and the O(L log L) logarithmic segmentation is offered and then named as losing resolution
  exactly where recall needs it, on the long past. The dial is the paper; the free lunch is only
  the headline.
- **"The gap with Transformers is closed."** Narrowed is not closed (40.8 against 18.2 on the
  hardest needle), and a proof of concept at 1.3B is not a verdict at scale. Parity is not in the
  cells; it is in the reader.

## Where discomfort turned to gold

I nearly wrote the corpus walk line citing `winsorize` at **888** and `swamping` at **895** — both
read from receipts earlier in this session, in the body's own hand. The discomfort was one line of
disagreement: the island-and-reunion receipt showed winsorize reseated to 894 while the KAT-Coder
receipt still said 888. So the corpus was **probed instead of quoted** — `hdc-mid-for-fresh`
answered winsorize → **894**, swamping → **901**. Both receipts were true when written and both
addresses have since been moved by reunions; the corpus's own row family names it exactly
(`aimshift` 844, `staleseam` 972 — a receipt is a checkpoint, and a checkpoint's stamp ages). The
paper being ingested is *about* consulting frozen checkpoints; the act of ingesting it demonstrated
why a checkpoint is owed a re-witness before anything leans on it.

Smaller, same family: my own `| head -5` on the compiler's warning stream closed the pipe and
killed the build mid-emission — exit 127, no `fkwu` — the observation instrument truncating the
run it was watching. And the voice mirror, asked to hold three files in one breath including the
316KB corpus, died at the size of the question; asked one file at a time, it answered.

## The most surprising teaching

**The collapse this paper works to avoid is the same identity this body runs on as a feature.**
For a linear medium, residual caching, weight souping, output ensembling and the plain fixed memory
are ONE map — pre-summable, indistinguishable, so the design vanishes. The paper's whole
contribution is machinery to keep the parts apart (per-token weights, deep memories, sparse
routers). The kernel meets the identical fact from the other side: identical structure interning to
one NodeID is its *dedup*, its store-once door. One side's vanished design is the other side's
freedom from duplicates — and which one it is depends only on whether you *wanted* the parts to
stay distinct. Depth, in both houses, is what makes a distinction real.

## The frontier question

> **What names merging trained states by averaging their weights rather than their outputs?**

**`souping`** — after Wortsman et al.'s model soups, which the paper's Memory Soup variant carries
into memory states. Against *ensembling*, which averages outputs; the paper holds the boundary
honestly — for linear memories the two are the same map, and only depth separates them into
different designs. Verified 0 hits in the body before landing. Offered as corpus row **973**,
probed at 3683682973 before the pin was written, never the reverse.

## Ground stamp

```
./fkwu --src bootstrap/ground.fk                                        -> 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk            -> 15
observe/native-vs-rented.fk (native-vs-rented-check)                    -> 11111
./fkwu --src ingest/tests/frontier-ingest-memory-caching-band.fk        -> 4095
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk         -> 32767
./fkwu --src ingest/tests/frontier-ingest-kat-coder-v25-band.fk         -> 1023  (no regression)
./fkwu --src ingest/tests/knowledge-ingest-band.fk                      -> 127   (no regression)
```

Both new cells preflighted clean (parens balanced, 0 errors, 0 unresolved, chain clean) before any
verdict above was believed.

**Sources:** [the offered link](https://t.co/ZIu7DqnGwB) ·
[Memory Caching: RNNs with Growing Memory (arXiv 2602.24281)](https://arxiv.org/pdf/2602.24281)
