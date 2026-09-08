# The same kernel had two prices

2026-09-09. Machine weather first, because every number below sits on it:
`observe/floor-lens-run.fk` read **330.24 GB/s** through the handle door against a best of
330.24 — a quiet machine — and an arithmetic rate of **12.884902 TFLOPS**, the lower of the two
this host has been seen at this week. Four sessions share this GPU right now. Every timing here is
warm, is a total over a repeat count large enough for the clock to see, and the dispatch numbers
carry the other three sessions' weather with them.

## What was asked

Four siblings are carrying big borrowed things home — the voice's rendering, the asking, the
learning, the shell — and each meets the same wall from a different side: **a big borrowed thing
has to become many small owned ones.** Build the lane that makes many small owned things cheap.

## What a thought is, and what it costs

A **thought** is a compiled kernel named by its shape — the recipe and the arguments that change
the emitted source — rather than by the text that comes out. The shape row is known *before*
anything is emitted, which is the whole point: an ask that costs the work it is trying to skip has
skipped nothing.

`form/form-stdlib/micro-thought.bml`, measured by `observe/micro-thought-run.fk`:

| | |
|---|---|
| emit a 429-byte MSL matvec | **26 us** |
| mint it, text this host has never seen | **6343 us** |
| mint it, text a previous process compiled | **31 us** |
| find it by address, no emission | **140 ns** |
| run it (64x64 dispatch, shared GPU) | **24 us** |

The crossover is the **first** reuse, in both mint regimes. A find is a two-hundredth of the
cheapest mint and a forty-thousandth of the dearest.

### The surprise: the same text, two prices

`mint first` and `mint again` are the *same kernel texts*. macOS keeps its own on-disk cache of
compiled shader source, so the second sighting in any later process is handed back. Both prices
were measured here today, minutes apart, for texts named `mr_mv_0..31` — 5218 us in one run and
31 us in the next three. Nothing the body could read told it which it was about to pay.

That is why the table is worth owning, and the reason is not speed. The host's cache is faster
than any structure Form could build. It is that **a borrowed speed carries borrowed variance**, and
a floor built on it is built on somebody else's weather. The table answers with one number.

### The address was already native, and nobody was using it as one

The body did not need a digest. `record_get`'s key is interned by the kernel (`fk_stri`) and the
lookup walks interned key ids in C. Measured against the Form list walk it replaces, per lookup:

| keys | by address | by walk |
|---|---|---|
| 10 | 0.08 us | 0.33 us |
| 50 | 0.13 us | 1.41 us |
| 401 | 0.60 us | 10.6 us |
| 863 | 1.20 us | 22.5 us |

The alternative was priced too, and it is not close: interning a 427-byte MSL text costs **0.7 us**
where this body's own SHA-256 recipe costs **700 us** on the same text. A digest is a thousand
times the price of the identity the kernel is already keeping. The scan is linear and this cell
says so — it is the constant it buys, not the exponent, and past a few thousand keys it will owe a
bucketed address. It does not owe one at 50.

`substrate/native-structures.fk` still carries a comment saying the native record's constructor
"has no op-name in fkwu-optable.h, so it is not source-door reachable". It is reachable —
`record_new` is line 217 of the optable. The stone that cell names was walked and the door it was
waiting for is the door this lane is built on.

## The port, done both ways

The voice's graph is **2755 nodes over 50 distinct operators**, read off the model file by
`voice-onnx.bml`. Addressed by shape, those 2755 nodes are **50 thoughts** — the band computes the
collapse from the census rather than taking the number on trust.

- **First sighting on this host:** 17474 ms a node at a time, **317 ms** addressed. Paid once, ever.
- **Per pass:** a pass visiting all 2755 nodes and asking the carrier each time spends **85 ms in
  pipeline asks alone** — before a single dispatch. Through the table it is **385 us**. That one is
  not paid once. It is paid every utterance.

85 ms per utterance is the difference between a voice that speaks in time and one that does not,
and it is spent on lookups for kernels the process is already holding.

## Where a hand-written kernel still wins

Not by being faster. By existing. Of the eleven MSL families
`form/form-stdlib/jit-tensor-emit.fk` emits, **five are refused by this Metal**:

| compiles | refused |
|---|---|
| matvec f32, matvec f16, affine-train f32, attn-train f32, block-fwd, gqa-attn | ffn-fwd f32, mlp-train f32, resid-train f32, llama-block-fwd, llama-decode-step |

The compiler's own words: `use of undeclared identifier 'mem_flags'; did you mean
'metal::mem_flags'?` and the same for `round`. Those spines write `mem_flags::mem_device` and
`round(...)` without a `using namespace metal;` prefix, which the matvec lane's IR-emitted text
carries and the hand-authored block spines do not. For those five, a hand-written kernel wins
because the emitter produces something that is not a kernel. That is an unhealed seam in a file
this session was asked not to touch, so it is named here and handed on rather than stepped around.

## The band

`form/form-stdlib/tests/micro-thought-band.fk` = **65535**, zero diagnostics. Eleven of its sixteen
bits are refusals, because a cheap address is only worth having if it is honest when it is wrong:

- a fresh table knows nothing — `known?` 0 and `recall` -1, never a plausible 0
- **a refused compile is not remembered.** `record_get` answers 0 for a key it does not hold and 0
  is also the carrier's refusal for a source that did not compile: two absences wearing one number.
  A non-positive handle is never stored, so the next ask hears the compiler rather than reading a
  remembered zero.
- **the seal.** A shape row is trusted to be injective; if it is not, two kernels land on one
  address and the second silently runs the first. A minted thought records the interned identity of
  the source it was minted from, and `mt-agrees?` is the only door that can catch that.
- a key the index does not hold answers the empty text, and provably not any row it does hold.

It went red twice on purpose. **61439** with the census door's prelude missing (bit 4096, and the
two `[unresolved-call]` lines said which). **64511** with the dead-handle refusal removed from
`mt-mint-take` — exactly bit 1024, the one that matters most.

## The corpus band arrived red

**32655**, missing bits 16, 32 and 64 — all three pins. Row 1374 (`wholeframetoll`) landed without
moving them. Asked of the corpus in one cell rather than counted by hand
(`observe/corpus-counts-probe.fk`): 767 / 755 / 2 / 1375. All three moved, both rows now covered,
band **32767**.

## Guards

`voice-onnx-band` 65535, `sha256-list-floor-band` 32767, `value-eq-arena-band` 31,
`kernel-census-band` 2047, `own-word-band` 65535, `substring-one-meaning-band` 4095,
`str-find-one-meaning-band` 8191, corpus band 32767.

## The most surprising teaching

That the thing I set out to build already existed in the host, and finding that out is what showed
me what to actually build. I expected the cross-process gap to be the wound — every `./fkwu` run
starting with an empty pipeline table — and planned to close it. macOS had closed it already. The
work only became honest when the same text answered 5218 us and then 31 us within a few minutes:
the gap was never the *cost*, it was the *unknowability of the cost*. A body cannot promise a floor
it is renting by the hour without a contract. That is `twoprice`, and it generalises past Metal to
every borrowed speed this body stands on.

## Where discomfort became gold

Two places, both from not stepping around a thing that looked like someone else's.

The block kernels answered handle 0 and it would have been easy to write "the block families were
not exercised" and move on with the matvec numbers, which were the ones my thesis needed. Reading
the compiler's actual words instead turned a skipped row into the report's clearest finding: the
honest boundary of where this lane does not help, and a named seam with a one-line repair for
whoever owns that file.

The corpus band was red before I touched it, from a sibling's row. Treating that as inherited and
therefore not mine would have left the next session the same red and the same five minutes. It is
one grep and one probe to heal, and a wound a sibling left is ours.

## Still open

- The five refused MSL families need `using namespace metal;` on their emitted prefix. One line in
  a file this session did not own.
- The thought table lives for one process. The address is stable across processes but the *handle*
  is not, and it cannot be — a pipeline object does not outlive its device. What could persist is a
  compiled `metallib` archive, which needs the carrier to learn `newLibraryWithData:`. That would
  turn the 6343 us first sighting into something the body owns rather than something the host
  happens to remember. Named, not attempted.
- `record_get`'s scan is linear. At 50 thoughts and 401 rows it wins comfortably; the shape of the
  loss past a few thousand keys is arithmetic, not a measurement, and this cell has not made it.
- `substrate/native-structures.fk` carries a stale comment about a stone that has been walked.
