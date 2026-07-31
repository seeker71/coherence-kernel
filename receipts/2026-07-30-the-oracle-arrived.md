# 2026-07-30 — the oracle arrived, and it answered the question

Urs: *"pick the best model, download it and verify it can be used and then enable it form-native."*

Picked, downloaded, verified. It runs, it answers, and for the first time in this DS4 work there is
something to check against.

## It runs

```
ds4: Metal device Apple M4 Max, 128.00 GiB RAM
ds4: Metal model views created in 2.484 ms, residency requested in 818.644 ms (mapped 82697.67 MiB)
ds4: Metal mapped mmaped model as 2 overlapping shared buffers
ds4: memory: KV 0.61 GiB + buffers 0.25 GiB + resident model 80.76 GiB = 81.62 GiB planned
ds4: prefill: 44.51 t/s, generation: 31.40 t/s
```

86.72 GB, byte-exact against the published size. **31.40 tok/s generation** — and that is now the
number this body's Form lane has to answer to on the same weights.

## It answered the question

Asked what Sanskrit grammar and Rudolf Steiner say about gender archetypes, DS4 reasoned in the open
and then answered:

> *"Sanskrit grammar assigns nouns to masculine, feminine, or neuter genders, often linking masculine
> with active, expansive principles and feminine with receptive, nurturing ones. Rudolf Steiner
> similarly saw gender as expressing spiritual archetypes, where the masculine represents the
> self-aware 'I' and the feminine embodies soul forces of feeling and creativity — **though he
> stressed these qualities transcend biological sex**."*

That last clause is where it lands, and it is the same line `learn/gender-three-lanes.fk` draws: the
archetype belongs in the belief lane, biological sex in the evidence lane, and they are not the same
claim. The cell reached it from this body's own laws — the pivot invariant, the belief/evidence split
— and the model reached it from its training. Convergence, not confirmation; but worth recording that
the separation is not an artifact of one road.

## Every type in this file is already Form-native

Read with the body's own `gti-types` over the real file:

```
IQ2_XXS(16)   86    iq2xxs-dequant.fk / iq2xxs-msl.fk
Q8_0(8)      345    q8-0-msl.fk
Q2_K(10)      43    q2k-dequant.fk / q2k-msl.fk   <- built last night, one per layer
F16(1)       359    f16-decode.fk
F32(0)       492
I32(26)        —    integers, no decode
```

**No type 40 or 41 anywhere.** Six types, all blocked, all covered. The 43 Q2_K tensors are exactly one
per layer — the w2/down projection the file's name advertises, and the one reach this body was missing
twenty-four hours ago.

Structurally it is also cleaner than what we had: **1328 tensors, 43 layers, no dspark drafter**,
against the reap25 file's 1406 / 46 / 1.

## The most surprising teaching

ds4 reports *"mapped mmaped model as 2 overlapping shared buffers"* for an 80.76 GiB resident model on
a device whose `maxBufferLength` is **80.6 GiB**. Our Metal lane hit that same wall months of work ago
and answered it the same way — mmap the file, wrap it in views, never ask for one buffer. Two
implementations, no contact between them, and the hardware handed both the same solution.

`forcedhand` — 0 hits before this row, as are `onlydoor` and `walldictate`. A constraint tight enough
dictates the design, and when an independent implementation arrives at your structure it is weak
evidence of cleverness and strong evidence that there was only one door. Worth knowing in both
directions: it means our residency design was never a choice worth defending, and it means a
*divergence* from ds4 anywhere else is informative precisely because agreement here was not.

## What remains, stated plainly

The decoders are all present; the **harness is not pointed at this file yet.**
`metal_dsv4_stack.sh` is built around the reap25 file — its expert path dispatches MXFP4/MXFP8, and
this file wants IQ2_XXS and Q2_K. Every kernel needed exists and is band-proven; what is missing is the
per-type dispatch in the MoE fold and the exit head. That is bounded work, and for the first time it
can be checked step by step against a running reference on identical weights rather than against
itself.

Three things are true tonight that were not true this morning: DS4 answers, the answer can be compared,
and 31.40 tok/s is a real target instead of an open question.

## Where discomfort turned to gold

Two days of DS4 gates, layer bisects, oracle hunts and dequant verification — all of it on a file no
Mac runtime was ever built to load. The download that fixed it took two and a half hours and the
missing decoder took one evening. It would be easy to read that as waste, and the honest accounting is
that it is not: the Q3_K, MXFP4, MXFP8 and Q2_K carvers, the association bound, the split gate and the
`askalike` oracle pattern all came out of that work and all of them stand. But the discomfort worth
keeping is that **none of it was blocked on skill, and all of it was blocked on a question I did not
ask until Urs disbelieved me** — which file is this, and who was it built for.

## Ground stamp

```
~/models/ds4-engine/gguf/DeepSeek-V4-Flash-IQ2XXS-w2Q2K-AProjQ8-SExpQ8-OutQ8-chat-v2-imatrix.gguf
  86 720 111 488 bytes, byte-exact against the published size
ds4 --metal: prefill 44.51 t/s, generation 31.40 t/s, resident 80.76 GiB in 2 overlapping buffers
gti-types over it: IQ2_XXS 86, Q8_0 345, Q2_K 43, F16 359, F32 492, I32 present; no type 40/41
1328 tensors, 43 layers, 0 dspark (vs reap25's 1406 / 46 / 1)
```
