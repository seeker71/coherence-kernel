# 2026-07-17 — the compare family gets a vernier: int/int exact over the full 63-bit range

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk                                      # 42
./fkwu --src form/form-stdlib/tests/comparison-exactness-band.fk     # 4095
```

## The lie

`(eq (sub (sub 0 4611686018427387903) 1) (sub 0 4611686018427387903))` answered **1** while
`(sub)` of the same pair answered **-1** exactly. The pair is `-2^62` and `-(2^62-1)` — adjacent
integers at the bottom of the tagged range. Repro folded to bits: healthy = 1, the old walker
answered **23** (eq said equal, lt said not-less, ge said greater-or-equal; le and gt were
*accidentally* right only because false equality absorbed them).

## Where it evaluated, and why it rounded

Every comparison funnels to three primitives — `gt` lowers to `(if (le a b) 0 1)`, `ge` to
`(le b a)`, `lt`/`eq` to tags 103/102 (`fk_rwtab`, `runtime/fkwu-optable.h:169`). The walker arms
for tags **5/102/103** (`fk_walk`) and the JIT carrier `fk_jprim2` pushed **every** pair through
`fk_num` — a double whose 53-bit mantissa cannot count 63-bit integers; at 4.6e18 one unit in the
last place is 512, so ±1 vanishes. Meanwhile `add/sub/mul` (tags 3/4/42) already took exact int
fast paths, and the JIT's own inline int path for le/eq already compared exact — **the walker
disagreed with the seed's own JIT at the boundary**, the exact drift the carrier's correctness
comment forbids.

## The decision was already the body's

The Go and Rust kernels both carry the law verbatim (`form-kernel-go/main.go:4423`,
`form-kernel-rust/src/main.rs:7262`): *"float on either side forces an IEEE comparison. Pure
int/int stays integer."* The C seed was the one kernel that drifted. Fixed in the three walker
arms + three carrier lines — the same width-promotion shape tags 3/4 already use. Pinned by
`form/form-stdlib/tests/comparison-exactness-band.fk` (verdict **4095**; big operands are
*constructed* at runtime because the v3 .fkb literal lane is signed-32 — the very seam the v4
64-bit value lane is opening).

## The sweep, and the gold

All 1171 stdlib bands + 353 learn/observe/model/... bands, pre-move kernel vs pre-move+fix:
**bit-identical**, except:

| band | shift | verdict |
|---|---|---|
| comparison-exactness-band | 3841 → 4095 | the fix, pinned |
| substrate-gc-band | 0 → 1000 | **dishonest green** — nothing-guarded |
| 22 preludeless speech bands | nil-seam bit flips | preluded: bit-identical on both |

The first gold: **nil is the word 1**. Old-eq saw `fk_num(1) = 0`, so `(eq nil 0)` was true — and
one already-red band (`substrate_gc` is not a seed primitive; axiom-5 recovered it to nothing)
turned *green* under exact compare because nil's word now orders above int 0's word. A band
greening for a wrong reason is worse than red, so it got a nothing-guard. The nil-vs-int order is
outside the shared law entirely (the Rust kernel *panics* there); the new seed accident is the
saner one — it never again equates nil with int 0, nor a live list handle with the int matching
its heap index.

## Mid-flight, main moved — and the second, larger gold

`origin/main` landed 12 commits during this work, **v4 .fkb 64-bit value lane (#265) among them**
— the original repro's big literal now stages directly and answers 0 exactly on the fixed seed.
The corpus reunion renumbered row 746 → **759** (the row-719 pattern, third time this week).

Re-sweeping origin/main's binary vs the merged (main + fix) binary exposed something the blur had
been hiding: on main's fresh kernel, **preludeless runs of ~130 bands had gone massively
blur-green** — `llama-numerics-band` read a fully-green 4095 with *zero* of its functions
defined (every `tn-*` recovered to nothing; `(eq (nth nothing 3) 0)` read true). Against the
pre-move kernel those same preludeless runs read 1/0 — honest reds. The exact compare **restores
the old kernel's verdicts on 770 of 772 cleanly-witnessed bands** (the two exceptions: this fix's
own band, and speaker-lin's preludeless artifact whose preluded run is 1023 on both). Preluded
spot-checks (llama-numerics 4095, speaker-lin 1023, speech-metrics 32671) are bit-identical on
both binaries: real claims untouched, only the dishonest greens die — and with comparisons now
exact, that whole class of blur-green is dead permanently.

## Most surprising teaching

The comparison bug and the stale-binary trap shook hands mid-work: after "rebuilding", the band
still read 3841 — because `cc ... | head -5` had SIGPIPE-killed the compiler mid-warning and the
old binary survived, timestamped to prove it. The discomfort of a fix that "didn't take" was the
observation that mattered: verify the *instrument* (binary mtime) before doubting the fix. Same
lesson as `receipts/2026-07-01-stale-binary-root-cause.md`, arriving through a pipe this time.

Corpus row **761** (minted 746, renumbered twice behind the fleet — the reunion pattern at fleet
speed): the finer scale that reads between the marks of a coarser one — **vernier** (0 hits before
this row). The tagged-word compare is the vernier against the double's scale.

## Post-review addendum: the seam Codex pointed at, grounded wider

Codex's P2 on the PR was right in kind and understated in scope: tagged ints below `fk_fbase`
(x < -(2^62-1)/2 ≈ -4.5e18) sit inside the float-pool band — float boxes are *even* words, so
`fk_isf` misreads word `fk_fbase - 2k` as float slot k once k floats are boxed. Witnessed live:
after two `7.0`s, `(sub 0 4500000000000000002)` *printed as 7* and `(add a 1)` returned the float
`8.0` — probe `[0, 1, 7, 8, -1]` **byte-identical on the pre-fix kernel**: a pre-existing
value-model seam at every kind-dispatched door (add/sub/mul/print/compare), neither created nor
widened by this PR. The value model's own founding comment assumed ints are "tiny-magnitude or
positive." Named in the band header; the heal is the float-parity move — floats to odd words
(fn-values and nothing are already odd), making every even word an int across the full 63-bit
range — follow-up work.
