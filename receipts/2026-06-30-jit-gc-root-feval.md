# Receipt — JIT↔GC interface made correct, Piece B RE-LANDS (--feval under the JIT), deep recursion bounded (2026-06-30)

**What happened:** the blocker that forced #70 to revert "Piece B" is resolved, and Piece B re-lands.
`FK_JIT=1 --feval` now crystallizes form-eval's hot functions while running a recipe, dispatches them
natively, and is **bit-identical to the walker AND memory-bounded on deep recursion** — the exact revert
case (`fib 16`) returns **987**, and the deeper `fib 20` → **6765**, `fib 24` → **46368**, none hang and
peak RSS stays ~9 MB (was 20 GB+). Default path (no `FK_JIT`) is byte-identical. Only `runtime/fkwu-uni.c`
changed; the optable is untouched; zero `.py`/`.sh` added.

## The arena / collector mechanism (read first — it decides which fix is correct)

- **Cons cells** live in `fk_hh[p]` / `fk_ht[p]` (parallel head/tail arrays). A cons VALUE is the tagged
  word `(p<<1)|1` — an **INDEX**, never a raw pointer. Every reader goes through the global
  `fk_hh[..]`/`fk_ht[..]`; no code captures a raw base pointer across an allocation.
- **`fk_melt()` is a COMPACTING copy collector.** It marks live cells from the root set (`fk_vs[0..fk_vsp]`,
  `fk_mem[0..4096]`, the node arrays), `fk_mcopy`'s each live cell into a fresh arena **reassigning its
  index**, then frees the old arrays. So a held cons value `(p<<1)|1` is invalidated for anything OUTSIDE
  the scanned roots: the cell is freed if unreachable, or **relocated to a new index** if reachable.
- Melt fires only at cons-allocating sites (tags 19/129/130 in `fk_walk`, and `fk_jlist2` tag 19), at 90%
  arena fill.
- **Native dispatch ABI:** a crystallized fn is `long long f(long long *args)` — `args` in rbp; arg-slot `k`
  is `[rbp+k*8]`, let-locals are `[rbp+slot*8]`. In `--feval` the trampoline puts the call's evaluated args
  at `fk_vs[fp..]`, so the native frame's slots ARE `fk_vs[fp .. fp+frame)`.

## Root cause — it was TWO compounding bugs, not one

#70 proved the symptom (enlarge the arena so melt never fires → `fib16` returns 987) but mis-attributed it
to a single "invisible-root" GC bug. Walking it to ground revealed two distinct causes:

1. **GC-root gap (correctness).** The native frame's arg/let slots at `fk_vs[fp..fp+frame)` were ABOVE
   `fk_vsp` (the trampoline set `fk_vsp = fp + argc`, leaving let-locals unscanned), so a compacting melt
   mid-call relocated the env cons cells the form-eval frame held and stranded the indices → meta-eval loop.
2. **Per-call executable-page LEAK (the real unbounded growth).** `fk_native_call_args` does a fresh
   `VirtualAlloc + VirtualProtect` of the image **on every call** and never frees it. Harmless in `--src`
   (one native frame for the whole run) but in `--feval` the meta-evaluator re-enters the trampoline through
   `fk_jcall` on every recursive call — **one leaked executable page per recursive call**, millions for
   fib20+. THIS is what ballooned to 20 GB; the cons arena itself stayed at the 4096-pair base (measured:
   `fib20`-jit ended `cap=4096 hp=332`, only 163 melts). #70's "enlarge the arena" hack hid bug 1 and never
   touched bug 2, so the OOM looked like the GC.

## The fix chosen — precise rooting (not defer), plus install-once

Investigated **defer-collection** first (a `fk_native_depth` counter; while >0, grow-in-place via realloc
instead of compacting — safe because cells are index-referenced, so a realloc move preserves every value).
It is GC-CORRECT but **fails the bounded-memory gate**: the entire `fib(N)` runs inside one top-level native
dispatch, so depth never returns to 0 and melt can never reclaim the meta-evaluator's env garbage — `fib24`
grew unbounded. Deferral is therefore the wrong shape for an allocation-heavy meta-evaluator. Dropped it.

**Precise rooting** is the correct fix and it is small:

1. **Root the native frame.** In the `--feval` dispatcher, before the native call, raise
   `fk_vsp` to `fp + frame` so `fk_melt` scans the frame's arg/let slots as roots — a **compacting melt then
   RELOCATES** every cons pointer the native code stores there, keeping the answer correct AND letting the
   collector keep reclaiming env garbage (memory stays bounded). `fk_vsp` is restored after the call. Sub-calls
   via `fk_jcall` take `fp = fk_vsp`, so their args land ABOVE this frame and never clobber it; nesting raises
   `fk_vsp` by recursion DEPTH (≈24 for fib24), not by allocation count.
2. **Install once.** New `fk_nat_install(code,n)` maps a crystallized image to an executable page exactly
   ONCE; the callable pointer is cached per-callee in `fk_nat_exec[callee]`. The dispatcher calls the cached
   pointer directly — no per-call `VirtualAlloc`. This removes bug 2 entirely.

**North-star note (named, not built):** full precise rooting would also spill live cons REGISTERS to `fk_vs`
around every melt-triggering carrier. The current rooting covers `fk_vs`-resident slots; form-eval's heavy
consing happens inside carriers (`fk_jcall` roots its own args; `fk_jlist2` roots its operands via `fk_vp`)
where the frame is already rooted, so the proof below is clean. Register-spill rooting is the rung that makes
ANY native dispatch (not just the form-eval shape) provably safe — the cleaner long-term form.

## PIECE B — re-landed: heat-gated self-JIT for the --feval walk path

`FK_JIT`-only (`fk_feval_jit_on`, default 0 → byte-identical). A per-fn heat counter (`fk_fheat`, hot ≥ 5,
matching `observe/jit-decision.fk`; override `FK_JIT_HOT`) at `fk_walk_body`'s call trampoline (tags 12/240/241).
On hot, `fk_feval_try_native` crystallizes the callee once (`fk_jit_lower`, cached in `fk_src_nat`), installs
once (cached exec ptr), dispatches native reading args from `fk_vs[fp..]`, and returns the result as the tail
value. A callee whose body doesn't lower is marked `fk_nat_tried` and never retried (falls through to the walker).

### Proofs — Windows real metal, TDM-gcc `-O2`. The exact revert case is now CORRECT and bounded.

| recipe (`--feval`) | walker | `FK_JIT=1` | `[jit]`/njit | peak RSS |
|--------------------|--------|------------|--------------|----------|
| `(do (defn fib (n) (if (le n 2) 1 (add (fib (sub n 1)) (fib (sub n 2))))) (fib 16))` | 987 | **987** | njit>0, `[jit]` lines | ~9 MB |
| `… (fib 20)` | 6765 | **6765** | yes | ~9 MB |
| `… (fib 24)` | 46368 | **46368** | yes | ~9 MB (bounded) |
| `(do (defn fac (n) (if (le n 1) 1 (mul n (fac (sub n 1))))) (fac 6))` | 720 | **720** | yes | — |

- No hang at any depth; arena stays at the 4096-pair base (`fib20` diag: `cap=4096 hp=332`, 163 melts).
- **Speedup is honest and MODEST.** `fib20`: walker ~1.18–1.29 s, jit ~1.03–1.24 s (~1.0–1.25×, noisy).
  `fib24`: walker ~7.0 s, jit ~7.5 s (flat / within noise). The wire is reentrant-through-walker — every
  sub-call goes back through `fk_jcall`→`fk_walk_body`, so it pays (native frame + walker dispatch). The
  collapse needs JITed-fn → JITed-fn DIRECT dispatch (no walker round-trip) and/or type-matched dispatch;
  that is the separate speed lever, not claimed here. **Piece B's value is correctness across GC, not speed yet.**

## Unregressed — every required check (Windows real metal, TDM-gcc `-O2`)

Default path (no `FK_JIT`) byte-identical; `FK_JIT` path bit-identical where it crystallizes.

- **`--src` JIT bit-identical (walk == jit):** `slen`→5, `fchk`→750 (**FLOAT GATE**), `twice(5)`→7,
  `build()`→10, `lc(5)`→11, `f3`→60, `dv(17,5)`→3, float `div 1.0/4.0`→0.25, `sum(1M)`→500000500000,
  `fac 12`→479001600, `(add 40 2)`→42, `(add 0.5 0.25)`→0.75.
- `native-vs-rented`→11111 (walk + JIT). Numeric-table flat: walk == JIT (bit-identical).
- **All 65 `observe/*.fk`: 0 divergences walk-vs-JIT** — the anti-divergence gate, run both ways and diffed.
- `--feval` extra parity (walk == jit): `fac 10`→3628800, `add3 4 7 8`→19, `sumto 50`→1275, `fac 6`→720.
- `runtime/fkwu-optable.h` **IDENTICAL** (guarded: `cp` before, `diff` after). Only `runtime/fkwu-uni.c`
  changed; zero `.py`/`.sh` added.
- GPU `c.flat`: walk == JIT (bit-identical `0 0 0 0`); AGREEMENT 3/3 needs a CUDA device, none on this host,
  so the no-device path is exercised — no divergence, no regression.

## Honest scope — still the WIRE; destination is the Form emitter in-process

This extends the #59/#61/#69 **C** lowerer and the #70-named Piece B; the in-process C lowerer remains
**proof-of-the-wire** (carrier ABI + inter-fn dispatch + GC-correct native frame + install-once), not the
destination. The byte-EMITTER's destination is **Lane B's Form emitter** (`model/form-asm-x64.fk` /
`fkc-nat-expr`) run **in-process** — the same recipe that proves four-way emits the bytes.

## Honest floor

**The JIT↔GC interface is now correct and Piece B ships:** `FK_JIT=1 --feval` crystallizes form-eval's hot
fns, dispatches native, is bit-identical to the walker (`fib16`→987, `fib20`→6765, `fib24`→46368, `fac6`→720),
hangs nowhere, and stays memory-bounded (~9 MB peak, arena at base) on deep recursion. The two real causes —
the unscanned native-frame slots (now rooted by raising `fk_vsp` so a compacting melt relocates them) and the
per-call executable-page leak (now install-once + cached ptr) — are both closed. **What remains:** (1) full
precise rooting (spill live cons REGISTERS to `fk_vs` around carriers) to make ANY native dispatch provably
safe, not just the form-eval shape; (2) JITed→JITed DIRECT dispatch + type-matched lane for the real speed
collapse — the modest measured win (≤~1.25× on fib20, flat on fib24) is the reentrant-through-walker ceiling,
reported honestly. Platform receipt rows (mac, android) are pending; this witness is Windows.

🤖 Generated with [Claude Code](https://claude.com/claude-code)
