# 2026-07-29 — Q2_K carved and proven before the weights arrived

Urs: *"so, pick the best model, download it and verify it can be used and then enable it form-native
please."*

## The pick, and why

**`DeepSeek-V4-Flash-IQ2XXS-w2Q2K-AProjQ8-SExpQ8-OutQ8-chat-v2-imatrix.gguf`, 86.7 GB.**

Not the biggest that fits, and not the best coder. It is the only candidate that is *both* runnable by
the ds4 already on this disk (Metal, mainline, ds4's own words: *"recommended model for 96 and 128 GB
RAM machines"*) **and** the exact architecture this body's Form lane already implements — 43 layers,
MLA, hyper-connections, the MoE router, all of it. gpt-oss-120b at 62.6 GB would run and would
validate nothing we have built. **An oracle has to speak the same architecture or it is just another
program.** Downloading; ~2.5 hours at the observed rate.

## What it needs from us, and what was missing

| type | ds4 | this body |
|---|---|---|
| IQ2_XXS — routed experts | ✓ | ✓ `iq2xxs-dequant.fk` |
| Q8_0 — attn proj, shared, output | ✓ | ✓ `q8-0-msl.fk` |
| **Q2_K — the w2/down projections** | ✓ | **missing** |

One reach. It is now written, and proven, and the weights are still 4% downloaded.

## `form/form-stdlib/q2k-dequant.fk`

The struct is ds4's own (`ds4.c:759`, size asserted 84 there): `scales[16] @0`, `qs[64] @16`,
`d @80`, `dmin @82`. The index decomposition is `ds4.c:3466 q2_k_value_f32`, transcribed with the same
op order rather than re-derived — the point of a carver is that its arithmetic is *someone else's*
recipe held exactly, so a disagreement is a real disagreement and not two guesses:

```
group  = idx / 16                        l = idx mod 16
q_base = 32*(group/8) + 16*(group mod 2)
shift  = ((group/2) mod 4) * 2
value  = d * (sc mod 16) * q  -  dmin * (sc / 16)
```

Two things in there are traps, and the band is built around both. `q_base` is *not* `group*16` — the
two bits for a group live interleaved across a 32-byte half. And the min stands **outside** the
product: it is a per-group offset, not per-element, so a group's 16 values share one subtraction.
Getting that wrong yields a tensor that is sign-symmetric and wrong only in the mean — the quietest
possible error.

## Verified against an independent reference, on identical bytes

`askalike` (row 931) applied a second time, and this time *before* the data — `q2koracle.c`, ds4's
`q2_k_value_f32` verbatim, over a seeded 168-byte fixture whose second block carries an awkward f16
`d` (0x3555) and a **negative** `dmin` (0xB400) so the sign path is exercised rather than assumed:

```
512 of 512 elements — IDENTICAL
```

Band `q2k-dequant-band.fk` → **511**, with ten values pinned on the seams (0/1 share a qs byte;
15/16/17 straddle a group boundary; 127/128 straddle the 32-byte half the fold exists for).

**Mutation-proven**, caches cleared between every run per `tickblind` (929):

```
M1 kill the 32-byte half fold           511 -> 475   (-32 pinned, -4 half-boundary)
M2 contiguous stride q_base = group*16  511 -> 475
M3 monotone shift instead of the cycle  511 -> 407   (-64 falsifier, -32, -8 cycle)
M4 scale the MIN by q (per-element)     511 -> 351   (-128 per-group, -32)
M5 swap the scale and min nibbles       511 -> 479   (-32)
```

Each kills exactly the bits that should notice it, and bit 64 is a live falsifier: it *requires* the
naive contiguous-stride reading to disagree, so the band cannot pass while proving nothing about the
fold.

## The most surprising teaching

The fixture could not be inline. Form's string primitives are `str_byte_at`, `str_concat`, `str_len`,
`str_find`, `str_eq`, `str_to_int`, `str_to_float`, `str_ascii_prefix` — **`str_byte_at` reads a byte
and nothing writes one.** There is no way to build a string from byte values, so a carver can never be
handed a block; it must be handed a file. That is not a gap I worked around, it is a shape the whole
tree already has: every carver in this body takes `src` from `read_file_slice`, and I had read four of
them without noticing they *had no choice*. `onewaybyte` — 0 hits before this row, as are `noinverse`
and `halfcodec`.

The asymmetry is load-bearing rather than incidental: a language that can decompose bytes but not
compose them cannot fabricate its own test data, so every byte a carver is judged against has to come
from somewhere real. The constraint enforces the discipline.

## Where discomfort turned to gold

Reaching for `chr` and getting `[unresolved-call]` — and the band returning **399**, which is 511 minus
exactly the three bits that needed the byte source. The failure was legible: I did not have to guess
what broke, the verdict named it. That is what a bit-folded verdict buys and I have spent two days
watching it pay, but this is the first time it caught *me* mid-construction rather than catching a
kernel. The discomfort worth keeping is that my first instinct was to work around the missing
primitive; the right move was to ask why four other carvers had never needed it.

## Ground stamp

```
form/form-stdlib/q2k-dequant.fk — new; struct + decomposition from ds4.c:759 / ds4.c:3466
form/form-stdlib/tests/q2k-dequant-band.fk -> 511
  vs the independent reference over 2 blocks: 512 of 512 identical
  mutations: 475, 475, 407, 351, 479 — each darkens exactly its own bits
fixture: form/form-stdlib/tests/fixtures/q2k-two-blocks.bin, 168 bytes, seeded 20260729
download: DeepSeek-V4-Flash IQ2XXS 86.7 GB, in flight
```
