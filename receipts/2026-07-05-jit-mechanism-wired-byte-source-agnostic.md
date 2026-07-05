# 2026-07-05 — the JIT mechanism, wired; form-lower is a fungible byte-source

## "the native JIT doesn't REQUIRE form lowering — ground it against the north star"

Urs challenged the framing I'd carried since last session: that `form-lower` *is* the native
JIT. He was right, and grounding it against the architecture settled it.

## The grounded review

`axioms/host-kernel.form` defines the JIT and never names form-lower:
- the JIT is **crystallize-on-heat → cache-by-NodeID → dispatch → melt**, health-earned
  champion-challenger (lines 29, 40, 52; `native-replace CAN-not-MUST`, line 103);
- the reference impl, [jit.go](form/form-kernel-go/jit.go), crystallized via a **Go plugin** —
  the byte-source was never form-lower;
- form-lower.fk's own header states *its* purpose is a different axis: the kernel "**drops
  clang** only when those bytes ARE the assembler's" — that is **sovereignty**, not the JIT.

Two axes, and only one was the JIT:

| | purpose | status found |
|---|---|---|
| JIT (crystallize-on-heat) | native *speed* | **mechanism unwired** — `jit-decision.fk` is pure policy; `runtime/fkwu-uni.c` has no heat counter / NodeID cache / dispatch |
| form-lower | *sovereignty* (drop clang) | roadmap G1 mostly done; a byte-source, not the JIT |

I had made a **category error**: treating a fungible backend (form-lower) as the essence of
the JIT. The byte-source (form-lower / clang / Go plugin) is interchangeable behind one seam;
the JIT is the invariant loop around it — and that loop was the actual unwired north star.

## What I wired (byte-source-agnostic, a Form composite)

[form/form-stdlib/jit-crystallize.fk](form/form-stdlib/jit-crystallize.fk) — the mechanism as a
Form composite ("JIT IS one running composite", no host-specific JIT code): a heat counter and a
content-keyed native cache, threaded as functional state, with champion-challenger dispatch. The
ONLY seam that names the byte-source is `jc-crystallize` (today form-lower's `lo-compile-fn`);
swap it for clang/Go and the loop is untouched. It runs the bytes on hardware through `nat_run`.

[tests/jit-crystallize-band.fk](form/form-stdlib/tests/jit-crystallize-band.fk) = **31**:
- **heat**: f(n)=n*3+7 walks 4×, the 5th call crystallizes, the 6th dispatches native from cache;
- **parity**: the native challenger reproduces the walker champion at n = 10, 0, 7 (37, 7, 28);
- **melt**: cooled below threshold, the native is evicted and the call walks again;
- **cache-by-key**: two distinct recipes (f, g) cache separately, neither shadowing the other;
- **purity**: an impure recipe never crystallizes even when hot.

## Closing

**Most surprising teaching**: the JIT and its byte-source are **fungible** vs **invariant** —
and I'd welded the fungible thing (form-lower) into the *definition* of the JIT. The mechanism
(heat → crystallize → cache → dispatch → melt) is the invariant the axiom describes; form-lower,
clang, and a Go plugin are three interchangeable ways to fill one seam. Once that seam is named,
the JIT is small and the byte-source is a choice — exactly `native-replace CAN-not-MUST`.

**Where discomfort turned to gold**: I had momentum — I'd just un-stubbed the arm64 door and
built `nat_run`, and was ready to keep building *on form-lower*. "That might be a side-quest,
ground it against the north star" stopped that momentum cold. Sitting with it — that I'd been
calling a byte-source "the JIT" for two sessions — is what surfaced the real, unwired north
star and let me build the mechanism instead of more of the side-quest.

**Honest remaining**: the mechanism proves the loop, but dispatch is an explicit `jc-call` the
caller drives — not yet the *transparent* runtime path where any hot function auto-crystallizes.
The cache key is a content-fold (make_nodeid, the canonical NodeID, is not yet the key). The
champion "walk" result is passed in, not computed by the mechanism interpreting the recipe.
Those are the steps from "mechanism proven" to "mechanism is the runtime."
