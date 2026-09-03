# 2026-09-03 — NodeID transport packs to 32/64 bits without shrinking identity

NUMS.Go's source coordinate is `u16.u16.u16.u32`: 80 bits. An unconditional
32- or 64-bit encoding would therefore collide. Form now keeps that full
coordinate as canonical identity and adds two lossless transport profiles:

- `N32 = p4.l4.t8.i16`, one unsigned 32-bit word;
- `N64 = p8.l8.t16.i32`, two unsigned 32-bit limbs.

The second form is not placed in one Form integer. `fkwu`'s positive tagged
integer payload ends at `2^62-1`, so two exact u32 limbs are the smallest safe
scalar representation. Coordinates outside N64 retain their exact dotted
80-bit identity and return typed, keyless overflow from the compact codec.
Nothing truncates or wraps.

The wire spelling is canonical lowercase base36 (`n32.<lo>` or
`n64.<hi>.<lo>`). Parsing rejects uppercase, leading zeroes, malformed or extra
segments, u32 overflow, and non-adaptive aliases such as `n64.0.1` when
`n32.1` is the canonical smaller form. Decode followed by adaptive re-pack
must reproduce the exact input bytes.

## JIT fault correlation

An earlier attempt packed an `intern_node` coordinate into the fault key.
That was wrong: runtime intern coordinates are allocation-order local, so two
fresh processes could reuse the same compact coordinate for different faults.

The JIT owner now hashes an injective, length-delimited structural stream over
the retained pending work, meaning epoch, cause, and observation epoch. It
publishes only the first 64 digest bits as the bounded Glass locator and keeps
the complete 32-byte digest in both fault and heal records. Before a heal may
run, the owner recomputes the full digest from retained pending state and
checks both digest and locator. A valid digest borrowed from another pending
program remains priority-1000 `jit-fault-attention`, with no install,
agreement, or heal.

The SHA input is folded field by field through the Form streaming recipe. It
does not concatenate another program-sized identity string, and SHA is absent
from the normal request and resident-native paths.

## Observation

Two fresh `fkwu` processes produced the same locator for fixture A and a
different locator for fixture B:

```text
A  jit.f.n64.49nrjd.1as6i9b
B  jit.f.n64.5l2nne.1srmk43
```

The physical FormJit run answered `22`, retained native reuse, and published
10 live JIT stage nodes. The focused Glass view showed actual unscoped rows
rather than its former false `sample-absent` projection: inspect
`miss-invalid <1ms`, materialize `compile <1ms`, install/challenge `<1ms`,
disk publish `235B / 5ms`, total `5ms`, and unattributed `<1ms`.

Self-watch named `p95=50505`, `lastms=1394`, `tpot=8`, `hopper=0`,
`icemiss=0`, `fails=0`, `touts=0`, and `errs=0`. The separate event-loop
witness observed 19 publishers, 102 samples, and front/back generation 2.

## Proof

- binary freshness: `31`;
- NodeID codec/adversarial band: `8388607`;
- fresh-process locator band: `31`;
- demand-JIT band: `137438953471` across repeated preflight/fresh runs;
- production owner band: `32767`;
- JIT Glass UI: `1023`;
- JIT hold: `4095`;
- event-loop state: `16777215`;
- all 21 verification bands passed and `git diff --check` was clean.

The AI truth review and performance/architecture review both returned SHIP
after the non-adaptive alias, forged-digest heal, input-sized hash copy, and
lazy-effect proof leaks were closed.

## Honest boundary

This movement optimizes Form's NodeID transport and compact telemetry keys. It
does not relabel an arbitrary 80-bit coordinate as a 64-bit identity, and it
does not claim that the temporary C seed's internal `fk_nid[][4]` storage has
already changed. That table remains a native-walker migration boundary under
the instruction not to grow or hand-edit the C seed.

I kept the movement alive by refusing the attractive but false equation
“NodeID = one u64.” The surprising teaching was that the arithmetic packing
was correct while the allocation-order value being packed was not durable.
Discomfort became gold when a green same-process test led to the fresh-process
witness and full structural heal check.

— Codex

; witnessed: 2026-09-03 -> node 8388607; process 31; JIT 137438953471;
; Glass JIT 10 nodes / 5ms; counsel p95 50505 lastms 1394 tpot 8
