# Receipt — substring is a native again on fkwu, byte for byte (2026-09-07)

**The ask:** make `substring` a native, four ways, without breaking a meaning it already
carries. What came back is one arm healed at 146x, and a four-way divergence that was
already there, measured and named rather than papered over.

## What was there before I touched anything

`substring` stopped being a native on 2026-07-01 (tag 29, spent since on `mlx_live`) and
became Form composition over the string narrow waist — `fstr-substring-halve` in
`form/form-stdlib/core.fk`, which halves a range until it reaches single bytes and rebuilds
with `str_concat`. Correct, and about 384,000 interpreted calls to cut 192 kB. Measured here,
warm, one process, minimum of three runs:

```
material 196,608 bytes of the same buffer
  composed (fstr-substring-halve)   5 cuts   214 ms   ->   4.59 MB/s
  str_concat, same pool           500 joins  521 ms   -> 566.00 MB/s
```

`char_at`, `str_find`, `split-on` and `trim` are all recipes over this one door, and it has
847 call sites.

## The divergence I found before building, and did not close

`substring` was never only fkwu's. go, rust and ts each keep a native, and
`nativeBypassesFormBinding` / `native_bypasses_form_binding` makes that native win even when
`core.fk` defines the same name — so the recipe was fkwu's alone. Their native is a different
door, and the difference is total on every edge. Measured, not read:

```
(substring "abcde" 3 1)       fkwu ""                go  panic: bounds out of range
(substring "abcde" 0 9)       fkwu "abcde"           go  panic
(substring "abcde" 7 9)       fkwu ""                go  panic
(substring "Ω+1"  0 1)        fkwu one byte, 206     go  "" (offset floored to a char start)
```

Only the in-bounds, `start<=end`, character-aligned core agrees. So "four ways without
breaking a meaning it already carries" is not a thing that can be done today: the meanings
already contradict each other, and have since before this work. I healed the arm whose
recipe was the cost, pinned its contract against the recipe, and left the three walkers
exactly as they were. **The wall, named: closing this needs go/rust/ts to stop flooring to
character starts and stop dying on out-of-range bounds — a semantic change on three kernels
with the whole tree downstream, not a change to make on the way past.**

## The tag: none. Mode 9 of the leaf door

Every tag 0..255 carries an `if (t == N)` arm except 150 and 190; 190 is `FK_TAG_CONST_HOLD`
and **150 the body holds in writing as the native-surface probe** —
`gate/tests/native-surface-band.fk` plants synthetic rows there and expects the gate to say
"tag 150 not found in C walker". Taking it would have bought speed with the gate's own
self-test, and left no tag for the probe to move to.

The body had already answered this question twice: the float surface rides `float_leaf`
(tag 201) as modes 0-3, the binary form as modes 4-8, and `gen-source-walker-table.fk` says
why in its own comment. `substring` is mode 9, as a rewrite row:

```
substring s a b  =  fk_smknode(201, LIT 9, s, (cons a b))
```

The range lives in the leaf node's **third** child as a tag-19 node, read child by child, so
the fast door allocates no pair — `write_form_binary`'s cons-packing precedent without
`write_form_binary`'s cons. `runtime/fkwu-optable.h` was regenerated through its own Form
pipeline (`fkwu flatten/gen-source-walker.fk` then the combined driver), never hand-edited;
the diff is exactly one row.

## The contract, pinned against the body's own statement of it

`fstr-substring-halve` and `fstr-substring-loop` both stay in `core.fk`, reachable by their
own names. Every edge was measured on the recipe **before** the arm existed and the arm was
written to reproduce it:

| | recipe | native |
|---|---|---|
| `(0,5)` whole, `(1,3)` middle | `"abcde"`, `"bc"` | same |
| `(3,1)` start>end | `""` | `""` |
| `(2,2)` zero length | `""` | `""` |
| `(0,9)` end past len | `"abcde"` | `"abcde"` |
| `(7,9)` start past len | `""` | `""` |
| `(-2,2)` negative start | `"ab"` | `"ab"` |
| `""` any range | `""` | `""` |
| `s` an int, `s` nothing | `""` (not nothing) | `""` (not nothing) |
| `"Ω+1"` `(0,1)`, `(1,2)` | one raw byte, 206 then 169 | same |

Bytes, not codepoints. The rest of the body indexes bytes through `str_byte_at` and
`form/form-stdlib/locale-rows/` is Persian, Romanian and German; a door that began counting
codepoints would silently re-cut every one of those rows and no ASCII band would say so.

**One edge the native answers where the recipe does not.** A `nothing` start:
`(sub 2 nothing)` is 8999999999999999999, so the halving divides a range of nine quintillion
and never returns — an 8 s probe printed nothing at all. The native clamps and answers `"ab"`.
A spin carries no meaning to break.

## Proof

`core-substring-equivalence-band.fk` is the strongest witness and I did not write it: it
predates this work, holds `substring` against `fstr-substring-loop` — the *original*
byte-at-a-time construction, not the halved one I replaced — and asks an exhaustive sweep of
every start/end pair of an ASCII and a multi-byte string, negative and past-the-end bounds,
and 6 KB of real bytes off disk. **2047, unchanged.**

`form/form-stdlib/tests/substring-native-band.fk` (new, 511, `PROOF LEVEL: FOURTH-ARM ONLY`)
adds what that one does not reach: the emptymask edges, a hand-written `(float_leaf 9 x)`
answering `nothing` rather than a counterfeit `""`, a rate bit the recipe cannot pass, and
the locale rows — including **the byte adjacency law at every byte offset of the real file**:
`s[0:m] + s[m:len] == s` for every `m`, which is exactly the law character flooring cannot
keep.

```
drift gates                        2047/2047, refused 0  (native-surface, op-manifest,
                                   binary-freshness, kernel-conformance all pass)
validate.sh core-str-find-to-int    255   1 ok, 0 divergent
validate.sh core-waist-language     255   1 ok, 0 divergent
validate.sh meaning-codes            15   1 ok, 0 divergent
validate.sh substring-native        511   fkwu-only lane, its declared level
phase 0: validate_fkwu_native_surface OK (196 flt-ops rows, max_tag 255, arm_slots 256)
         category contract, primitive registry, structural gate — all PASS
```

## What three real doors gained

Machine weather on every number: six siblings share this Mac and this room, load average
4.2-6.6 while these were taken, `observe/floor-lens-run.fk` reading 366.94 GB/s through the
handle door and calling it quiet. Measuring is taxed, so these are **minimums of two or three
full runs per binary**, the before binary built from this commit's parent so both are the same
compiler on the same metal, and **all of them re-taken on the rebased tree** — an earlier set
on the pre-rebase branch read `meaning-codes` at 32.9 s → 13.4 s and the corpus band at
925 ms → 683 ms, and those numbers are not this tree's.

```
                                   before     after     verdict
meaning-codes-band               20,214 ms  10,513 ms   127 -> 127       1.92x
ear-native-band                     828 ms     279 ms   32767 -> 32767   2.97x
ear-axes-band                       239 ms     164 ms   65535 -> 65535   1.46x
core-substring-equivalence-band      79 ms      76 ms   2047 -> 2047     1.04x
homecoming corpus band              317 ms     316 ms   32767 -> 32767   none
the 192 kB cut itself             4.59 MB/s  660 MB/s                    144x
```

**Two of these gained nothing and both are the right answer.** The equivalence band's time is
the *reference loop*, which this change does not touch. The corpus band does not lean on
`substring` at all: its prose walker is token-shaped, not byte-shaped — it jumps whole runs
through `scan_run`, a native, exactly so it would not pay this cost, and its own comment says
so. A door built to avoid the slow door gains nothing when the slow door gets fast, and that
is the shape of a real measurement rather than a flattering one. Nine and a half seconds came
off `meaning-codes-band`, which does walk bytes.

## Still open

- **The three-walker divergence** above. Named, measured, not closed.
- `meaning-codes-band` answers 127 on fkwu and 15 on the three walkers, which agree with each
  other. Pre-existing — both the before and after binaries answer 127 — and not this work's,
  but it is a real fkwu/walker split in a band this change made 2.5x faster.
- `str_find`, `split-on` and `trim` are still recipes. They are recipes over a door that is
  now 146x faster, so they moved without being touched; whether any of them should take the
  same road is a measurement nobody has made yet.

Co-Authored-By: Claude Opus 5 <noreply@anthropic.com>
