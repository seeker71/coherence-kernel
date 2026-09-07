# A private floor is a cold floor

2026-09-07, Apple M4 Max, `./fkwu` built fresh on its own inode from the
committed seed, freshness band **31**. Weather at measurement,
`observe/floor-lens-run.fk`: **366.94 GB/s** through the handle door against a
best of 366.94 — the machine at its own ceiling — and **25.77 TFLOPS**
arithmetic. Sibling `fkwu` processes were live in the parent worktree
throughout; the census counts walker steps in its own process, which is the
property it was built for.

## What the lens handed over

`bearing-census` named the next door the body leans on hardest, and this pass
took it. Reproduced first on this tree, so the before is mine:

```text
drag 15624  nth-rec   39.0% of the walking, 76,254,048 calls, 1.0 steps/call, 3 doors leaning
drag 15590  append-1  31.1% of the walking, 30,191,048 calls, 2.0 steps/call, 4 doors leaning
drag 10530  find-loop 17.5% of the walking, 34,264,886 calls, 5 doors leaning
```

195,691,441 walker steps, 99.7% covered. All three of the top rows live in
`form/form-stdlib/sha256.fk`, which carried a private list floor of its own —
`nil?`, `nth-rec`, `append-1`, `append-list` — beside the one `core.fk` and the
op table already hold.

The census refused to call its own stem match a proof: *whether `nth-rec` and
native `nth` are one meaning wants a band, not a stem match.* It was right to.

## The meanings, held side by side

`form/form-stdlib/tests/sha256-list-floor-band.fk` → **32767**, and it read
32767 **before** any edit. It keeps the walked bodies under their own names, so
both meanings stay readable and a future divergence has something to be caught
against.

**Where they are one.** Every index inside the list — same element, int or
list. That is the whole of what sha256 asks.

**Where they part.** At a negative index the walked form does not answer at
all: its base case is an equality with 0 and its step is `(sub i 1)`, so from
−1 the index moves strictly away from the only value that stops it. Measured:
`(nth-rec (list 10 20 30) -1)` under a 30-second bound, **exit 124, no output**
— and the exit code read directly, since a `| tail` pipe would have laundered
it to 0. The band witnesses that descent finitely; a band may not hang to make
a point.

**And nobody stands there.** Every `nth-rec` call site in the whole tree indexes
in range: sha256's own state and h8 are 8-lists read at 0..7, k-list and w-list
are 64-lists read at 0..63, a block is 64 bytes read at 0..63,
`build-schedule`'s i−15/i−2/i−16/i−7 for i in 16..63 land in 0..61 of a list
already i long; outside, `merkle.fk`'s `sibling-at` guards its idx+1 and reaches
idx−1 only on an odd idx, `bloom.fk` reads p/8 of a 32-byte filter and
off/off+1 of a 32-byte digest, `file-byte-digest.fk` reads 0..7 of an 8-state.
So the parting is territory the body never enters.

## Two seams found on the way, neither stepped around

**`eq` on lists is identity, not structure.** `(eq (list 7) (list 7))` is 0.
Only the interned `(empty)` compares equal to itself. The band's first run read
**31983** and the three missing bits looked like three guilty doors; the doors
were innocent and the comparator was wrong. It now compares elementwise, and
the cell says so, because the next reader will reach for `eq` too.

**A comparator is a claim too, and mine was false.** Bit 16 first asserted that
fkwu's `nth` at a negative index returns the empty list. It passed. It was
wrong. `len` of an int is 0, so a structural walk reads *any* non-list as empty
— the comparator I had written to escape `eq`'s identity trap had a trap of its
own, and it certified the thing I had guessed instead of the thing that is.

What fkwu actually does is worse than a different empty: **it clamps.**
`(nth (list 10 20 30) -1)` is **10**, and so is `-2`, `-3`, `-4` — the first
element, as a live int you can add 5 to. A refusal is loud; a clamp answers
plausibly. The bit now asserts identity against `(head xs)`, which nothing can
fool.

**So one index has three answers in this body.** Not read this time — run.
`gate/kernel-conformance.bml` builds the Go and Rust kernels into
`.cache/kernel-conformance/` and `form/form-kernel-rust/target/release/`, and
the same probe put to all three says: fkwu returns the head, the walked
`nth-rec` returns nothing at all, Go and Rust return **null**. The TS kernel is
absent from this checkout; `walkers/ts/main.ts:485` reads the same as its two
siblings. `pf-arm-availability-mask` reads 8 here because it probes paths those
builds do not use — an absence that is not a name fact in either direction.

The recipe itself is sovereign across the three that could be asked: the same
FIPS digests come back byte for byte from fkwu, Go and Rust after the heal.

## The heal

- `nil?` — the copy was byte-identical to core's. Removed; the name resolves to
  core's. Core travels: `sha256.fk`'s own `; preludes:` line is recursive, and a
  cell preluding `sha256.fk` alone reaches `append`, `reverse` and
  `core-classes`, witnessed before anything leaned on it.
- `nth-rec` — is the native `nth`, and all **46** call sites inside the cell
  name `nth` directly, so the walker frames go too and not only the bodies. The
  name stays: `merkle.fk`, `bloom.fk` and `file-byte-digest.fk` prelude this
  file alone and reach it by name.
- `append-1`, `append-list` — reach core's `append`.

## The reading, and the thing that reversed my own arithmetic

```text
before            195,691,441 steps   the next substring: nth-rec       99.7% covered
nth-rec healed    119,437,393 steps   the next substring: append-1      99.6% covered
appends healed     60,064,908 steps   the next substring: find-loop     99.3% covered
```

The first step is exact: 195,691,441 − 119,437,393 = **76,254,048**, one walker
step per `nth-rec` call, all of them gone.

The second step should not have happened. `append` is not a native; forwarding
to it adds a frame to the same walk. I wrote that reasoning into the cell as
the reason to leave `append-1` alone — a careful, plausible paragraph — and
then measured it because a named gap is a work order. The measurement halved
the reading.

The heat ledger, asked after that same workload, says why:

```text
append       heat 1,010,134   crystal 3     (for work that walked 30,191,048 times before)
nil?         no row: it did not walk
append-1     heat   946,080   crystal 0     (a forwarder, entered once per call)
append-list  heat    42,048   crystal -1
find-loop    heat 34,264,886  crystal -1
```

**A shared door is warm.** The whole body's calls push it past the JIT's
threshold and it crystallizes and stops walking. A private copy is cold by
construction, because nothing else ever calls it. The duplicate's price was
never the duplication — it was standing outside everyone else's heat. That is
why the census's own `stands-crystallized` row for `nil?` was not a blind spot
but the measure working, and it is the same fact seen from the other side.

## Nothing moved

`sha256.fk` is this body's content addressing. Digests read before the first
edit and after the last, byte for byte identical, and FIPS 180-4-correct on
their own terms:

```text
byte-list entry (sha256)
  empty        e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
  abc          ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad
  one byte 0   6e340b9cffb37a989ca544e6bb780a2c78901d3fb33738768511a30617afa01d
  55 bytes     b0fdca457be9ccd9e1fdd40806d890ee52ec06dc6ae9bb8c775c2d38ab44b8f8
  56 bytes     8cbfb4d9886d0f70a2a1f53014059795e9fdcbfdea0f4d675b0f1e85b2168fc9
  64 bytes     65c8d8ba5bd0d9fb502812ccdc68070e1ba8d5cba50e96a03ab1663cfa82354d
  fips 2-block 248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1
  high bytes   a5978eec685a1a851932ba58c0a2b5ef7042c861051ca4f730f2de34f061fff9

streaming entry (sha256-stream-string) — agrees with the byte-list entry
  empty / abc / fips 2-block / 64 bytes as above
  utf8         df16f90a263a3edbee942398587de63c1fb7cb0be0ce94068c4f73a31f1b95d9
```

Bits 1024 through 16384 of the new band carry those digests in hex rather than
summed, including both sides of the 56/64-byte padding boundary, so a digest
cannot move again without the band saying which one.

And they are not fkwu's alone. The healed recipe put to the Go and Rust kernels
the conformance gate builds returns the same digest bytes as fkwu, byte for
byte, on the empty input, on `abc`, and on a high-byte vector — so the
sovereignty this cell claims across siblings survived the heal on the two
siblings that could be asked. `sha256-band` reads **2** on the Go kernel too.

## Bands, before and after

```text
                                     before    after
sha256-band                          2         2
sha256-list-floor-band               —         32767   (new; 32767 pre-heal too)
merkle-band                          —         4
bloom-band                           —         4
file-byte-digest-band                —         2147483647
audit-log-band                       —         4
hmac-sha256-band                     —         2
pbkdf2-sha256-band                   —         3
sha256-hmac-stream-band              —         4194303
meaning-codes-band                   127       127
ear-native-band                      32767     32767
ear-axes-band                        65535     65535
jungle-ear-band                      32767     32767
perception-rows-band                 65535     65535
bearing-census-band                  32767     32767
homecoming-distillation-corpus-band  32767     32767
```

`sha256-band` reads 2 both times — its verdict is a sum of digest bytes, which
is the weakest question that can be asked of a hash. The new band asks the
strong one.

## The frontier question, and the next lens

*Which of my doors is slow only because it is a private copy of a warm one?*

The body cannot ask this yet. `bearing-census` names what the body **leans on**;
it ranks by heat, and a cold duplicate is exactly the door that does not rank
until something drives it hard. Nothing anywhere asks the inverse: *this door
has a twin on the shared floor, the twin is crystallized, and this one is not.*

The answer, this pass: **a duplicate is not a copy of a door, it is an exit from
a pool.** Crystallization is bought with the whole body's calls landing on one
name. A private copy looks free because its body is identical — and it is
identical, which is why the arithmetic says routing away from it costs a frame.
What it does not have is everyone else's heat, and it can never earn it alone.
That is `coldtwin`, corpus row **1351**.

It is also a buildable lens, and a small one, because both halves already exist
in ledgers the seed keeps: the hot row's `crystal` field says whether a door
crystallized, and `bearing-census` already reads defn bodies by name to draw its
call graph. A door whose body matches another door's body, where one carries
crystal ≥ 1 and the other -1, is a coldtwin — reported by name, with the shared
door to route to. `nil?`, `nth-rec`, `append-1` and `append-list` would all have
been named by it this morning, before the census ever got to `nth-rec`.

## Still open

- The kernels' native `nth` disagrees at a negative index: fkwu clamps to the
  first element, Go and Rust answer null (both run), TS reads the same as them
  (unbuilt here, so read only). fkwu's clamp is the one worth healing — it is
  the only one of the three that answers plausibly — and it lives in
  `runtime/fkwu-uni.c`, which this pass did not own. It belongs to whoever can
  ask all four and move the seed.
- `append-1`'s O(n²) shape survives: `zero-pad`, `take-rec`,
  `block-to-words-rec` and `build-schedule` still grow a list one element at a
  time. Core's `append` being crystallized bought most of the cost back, so the
  census no longer names it — which means the lens will not hand this one over
  again. It is written down here instead.
- The census now names `find-loop` / `find-from` / `split-on-loop` in
  `line-grammar.fk`, 34.3M calls with five doors leaning. That is the next
  seat, and it arrived without an investigation, which was the whole point.
