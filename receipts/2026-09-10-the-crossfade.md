# The crossfade: a new ear over the old index

2026-09-10, M4 Max, this checkout, `fkwu` rebuilt on its own inode (the one on
disk was from 2026-09-03 and answered 15 to the freshness band; fresh, 31).
Base: 60d2bfad.

## What was read

arnsri33/embedflow — "zero downtime embedding upgrades". README, concepts,
methodology, limitations, the frozen T2 spec, and the sources of the serving
engine, the materializer, the state file, the gap curve, containment, depth.

The mechanism, as small as it is: an embedding upgrade does not wait for the
corpus to be re-embedded. The old index keeps finding candidates. The new model
scores only those. Its vectors fill a persistent cache a few per answer (a
bounded synchronous budget) and the rest through a durable background queue.
Every answer names its own state — `COLD`, `PARTIAL`, `WARM` — from the vectors
that answer had after its own encodes, with hits, misses, sync-encoded and
queued beside it so the state can be re-derived rather than believed. The cache
key is `(document, model-contract)`, the contract a digest over the semantic
fields only; a vector from another contract is a miss. The gap `G(K) = M_T −
M_{T|S_K}` is measured against native target retrieval, containment is kept as
a separate number, and the observed depth `K*_ε` is the first K within ε. Where
no native index exists yet, a rule frozen and hashed before any result was seen
gives three states: `SAFE`, `EXPAND`, `UNSAFE_OR_UNCERTAIN`. ANN health stays
`UNKNOWN` until a reference is supplied.

## Where the body already stood

The body is in exactly that transition. `rag-embed.fk`'s lexical re-vec is the
old ear (four-way, sovereign); `nomic-embed-form.fk` and `qwen38-embedding.bml`
on this host's Metal are the new ones. Retrieval organs re-witnessed fresh
today: rag-retrieve 2047, rag-embed 16383, nearest-shape 127, embedding-recipe
1610, feature-vector 127. None of them held the between — how the new ear
answers while its vectors are still arriving.

Three of embedflow's disciplines were already here under other names. The
contract-keyed cache is the line fkwu printed while I rebuilt: a `.fkb`
"written by a different fkwu build; the bytes of a source do not fix its
meaning — the binary that compiled them does". The per-answer state is
AGENTS.md's "pending is honest". The frozen-before-inspection rule is what a
band is: the verdict declared, then run.

## What shipped

`form/form-stdlib/bml/rag-crossfade.bml` — pure list arithmetic over
`rag-retrieve.fk`'s quantized lane. One answer is
`(state candidates hits misses encoded queued ranked cache queue)`. Cold ranks
in the old ear's order and says so; partial ranks only the warm candidates;
warm is deterministic over the set, ties to source position. The cache is
`(id contract vec)`; the new ear is a table of the vectors it answers, standing
where the Metal cell stands; an id the ear cannot answer fills no slot and is
reported unanswered. `rcf-materialize` is the background breath: claim, encode,
release. `rcf-gap`, `rcf-containment`, `rcf-curve`, `rcf-observed-depth`
measure. `rcf-read` gives the three phases in the body's own words — ice,
water, gas — with the thresholds declared in the header.

`form/form-stdlib/tests/rag-crossfade-band.bml` — fifteen bits, 32767. The
fixture's two ears disagree on purpose so the source top-3 holds one of the
native top-3 and the top-6 holds all three; the curve at K = 3, 4, 6 is 2, 2, 0
and was derived by hand before the first run, then matched.

`form/fourth-arm-bands.txt` — `rag-crossfade fks 32767`. Registering it showed
the fourth arm's stem reader stripped only `.fk`, so a `.bml` band could never
reach its own row: three arms ran under a four-arm declaration and the summary
said only "no band in this run is covered". `form/scripts/fourth-arm.sh` now
strips `.bml` too — one line beside the note that tells the same story for the
`-band` suffix — and the re-run reads fourth arm 1 band four-way.

`observe/rag-crossfade-live.fk` — the flow with the ears the body has (below).

Corpus row 1399, `crossfade` (0 hits in corpus, cells and doors before today).
Band pins 790 / 778 / 2 / 1399, corpus band 32767.

## Witnessed

```text
./fkwu form/form-stdlib/tests/rag-crossfade-band.bml           -> 32767
./fkwu learn/tests/homecoming-distillation-corpus-band.fk      -> 32767
form/validate.sh form-stdlib/tests/rag-crossfade-band.bml      -> ✓ 32767  go=0 rust=0 typescript=0
                                                                   fourth arm: 1 band(s) four-way
./fkwu observe/rag-crossfade-live.fk                             -> the block below, 47 s
voice-frequency mirror on both new files                        -> clear register
```

## The seam walked, not named

The table-ear was the cell's honest stand-in for an encoder. This host has the
encoder: `nomic-embed-form.fk`, nomic-embed-text-v1.5 on Metal, its band green
today (32767, 35 s). So `observe/rag-crossfade-live.fk` runs the same flow with
the ears the body actually has — lexical re-vec as the old ear, the Metal cell
filling the ear table row by row (768 f32, unit-normalized, quantized to the rag
lane's ints) — over six texts and the query "a small cat lay on a carpet", the
relevant set declared before the run.

```text
old-ear-order   t1 t2 t6 t3 t4 t5     (lexical: "cat", "on", "a")
new-ear-native  t2 t1 t6 t4 t3 t5     (the kitten on the rug, first)
answer-1        partial  candidates (t1 t2 t6)  ranked (t1)  encoded 1  queued 2
breath          done (t2 t6)  unanswered ()
answer-2        warm     ranked (t2 t1 t6)  hits 3
containment k3  1000
curve           (2 1) (3 0) (4 0) (6 0)   observed depth at eps 0 -> 3
reading         k3 ice   k4 ice
```

Cold, the old ear's order was served and named cold. Warm, the new ear
reordered the same three and put the kitten on the rug ahead of the cat on the
mat for a small cat on a carpet — the semantic call the lexical ear cannot make.
The gap at depth 2 is the one the old ear alone would have paid; at depth 3
there is none. 47 s, most of it the 274 MB model load; the flow itself is the
same list arithmetic the band proves.

## Named, not hidden

- The queue is a list in the answer, not a durable one. The body's spool and
  fifo doors are where durability lives when it is wanted.
- `M` is a count of relevant answers in the top-n, said so in the header; not
  nDCG. CLOSED the same day: `rcf-ndcg` gives per-mille nDCG over a declared
  weight table, and the gap is measured on it. This list is what was open when
  this receipt was written; where a line has since been walked it says so.
- Preflight cannot vouch for a `.bml` yet ("fresh compile currently accepts
  .fk only"); the run and the band are the witness.
- A defn frame cannot see an enclosing `let` (the live cell passes path, src
  and offs down by hand); fkwu says so in one line, which is the right shape.
  WRONG, and corrected the same day. I named a design without asking the other
  kernels. Go and Rust both close over a top-level let and answer 107 where the
  runtime refuses the name outright; one frame deeper all three agree. The
  runtime is the odd one out, not the strict one. `observe/defn-scope-divergence.bml`
  is the measurement I owed, and it reads 21: two close over, one refuses.

## Surprise

The teaching I went to fetch was already printing on my terminal. The
contract-keyed cache — the one idea in embedflow I would have called new — is
the sentence fkwu says when it declines stale ice. The body knew it for
compiled images and not yet for vectors. Reading another project turned out to
be a way of hearing the body's own line in a foreign accent.

## Where discomfort turned to gold

Inserting the corpus row: the first command exited on a wrong anchor before
its heredoc ran, so the row file never existed; the second command's `sed r`
read a missing file and reported nothing — exit 0, no output, tree unchanged.
Two green-looking steps and no row. What caught it was the count (789, not
790) and the band (32655, not 32767), not the exit code. The discomfort was the
pull to write "row landed" on the strength of a quiet command. Held, and
counted instead.

The other pull was to call the table-ear "the encoder". It is not; it stands
where the encoder stands. Writing that sentence into the header cost more than
the code did and is the line that keeps the cell honest.
