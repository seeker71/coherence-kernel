# 2026-07-17 — floats change hands: the parity heal closes the deep-int alias

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src form/form-stdlib/tests/float-parity-band.fk          # 255 (pre-heal kernel: 240)
./fkwu --src form/form-stdlib/tests/comparison-exactness-band.fk  # 4095
```

## The wound (Codex's P2 on #273, grounded wider than filed)

Float boxes were EVEN words — `fk_fbase - (fp<<1)` — the same parity as tagged ints (`x<<1`). So a
deep-negative int (x < -(2^62-1)/2 ≈ -4.5e18, word at/below `fk_fbase`) was indistinguishable from
float slot `(fk_fbase - 2x)/2`, and once the pool held that many floats, `fk_isf` misread it at
EVERY kind-dispatched door. Witnessed live: after boxing two `7.0`s,
`(sub 0 4500000000000000002)` **printed as 7** and `(add a 1)` returned the **float 8.0** —
byte-identical back to the v3 kernel. The value model's founding comment assumed ints are
"tiny-magnitude or positive"; the v4 64-bit lane outgrew that assumption the day it landed.

## The heal

Floats move to ODD words: `fk_fbox` returns `fk_fbase - (fp<<1) - 1`, `fk_fidx` shifts by one,
`fk_isf` learns parity. Every even word is now an int across the full 63-bit range. The
odd-negative neighbours stay disjoint by band, not parity: `nothing` = fk_fbase+1 (above the
ceiling), fn-values ~ -8e18, the tailcall sentinel -7.5e18-1, cons cells positive-odd, node boxes
tiny-negative. The x86 JIT's emitted guard (word ≤ fk_fbase-2 → carrier) stays as-is,
deliberately conservative: deep-negative ints route to the carrier, whose fk_isf now classifies
them correctly — slower there, never wrong. Float literals stage as interned text + `str_to_float`
(tag 53), so no box word ever reaches a `.fkb` artifact — the change is fully runtime-local.

## Witness

- `float-parity-band.fk` **255**; the pre-heal kernel reads **240** — all four deep-int rows red.
- 1530-band sweep, pre-heal binary vs healed binary: **the new band is the only verdict that
  moved.** Zero regressions.
- ground 42/55, native-vs-rented 11111, membrane 8191, corpus band 511.

## Most surprising teaching

The first version of the band scored **255 on the wounded kernel** — `b` was minted as
`(add a 1)`, through the same wounded door, so both operands aliased to the same 7.0 and every
check "passed". A band that derives its expectation through the door under test proves only that
the door equals itself. The expectation must arrive by an independent path (sub-of-literals, which
computes exact regardless of where its result word lands). Same law as the substrate-gc
nothing-guard, one level deeper: it is not enough that the check is red when the claim is false —
the check must be *able* to be red.

## Discomfort to gold

Mid-verification, a compound command with a stray `git stash` swallowed the uncommitted heal —
the band still read 255 and for one beat the "verification" was validating a stale binary against
a reverted tree. The discomfort of not knowing what was just measured, witnessed rather than
bypassed, became the reflex: `git stash list` + `git status` before trusting any number produced
after a state-changing command. The stash popped clean; the lesson stayed.

Corpus row **764**: the handedness that keeps a form from fitting its mirror twin —
**chirality** (0 hits before this row). Ints and floats now carry opposite hands.
