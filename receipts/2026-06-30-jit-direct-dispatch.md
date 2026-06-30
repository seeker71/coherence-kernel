# Receipt — JITed→JITed direct dispatch lands: native↔native inter-fn, ~4× on the clean signal (2026-06-30)

**What happened:** the inter-function wire that #75 named as the remaining speed lever is built. A
crystallized function now calls another crystallized function **directly, native→native**, instead of
bouncing every inter-fn call back through `fk_jcall`→`fk_walk_body` (the reentrant-through-walker shape
that capped #75 at ~1.0–1.25×). On the clean inter-fn signal — deep mutual recursion `ev`/`od` — the
`--src` JIT path now runs the whole chain native end-to-end and measures **~4× the tree-walker, 0
divergences, memory bounded**. `--feval` stays **correct and bounded** (bit-identical to #75); its modest
ceiling is unchanged and honestly named below. Only `runtime/fkwu-uni.c` changed; the optable is untouched;
zero `.py`/`.sh` added.

## The mechanism — direct native→native dispatch, type-matched, with a native trampoline

An inter-fn call at a JITed site routes through `fk_jcall(callee, argc, args)`. #75's `fk_jcall` always
called `fk_walk_body` (correct, but every sub-call paid native-frame + walker-dispatch). The new wire:

1. **Lazy crystallize-on-call (`fk_ensure_native_ex`).** Being *called from JITed code* is itself the heat
   signal: on first inter-fn call the callee is lowered + installed once, its exec pointer and frame size
   cached per-callee (`fk_nat_exec[]`, `fk_src_nat_frame[]`). A callee that doesn't lower is marked
   `fk_nat_tried` and never retried (falls through to the walker). This shared helper unifies the `--src`
   inter-fn site, the `--feval` trampoline, and the root crystallization into ONE lower+install+cache path.

2. **The type-match guard.** Direct dispatch fires only when the call **arity equals the callee's declared
   arity** (`fk_fnar[callee]`) — what the native frame's `[rbp+k*8]` arg slots were lowered for. The lowerer
   itself refuses any non-int-family body, and arith carriers carry the float-guard, so an installed native
   is bit-identical to the walker for **all argument values** — value-type matching is unnecessary; only
   arity must align. On mismatch or no-native → deopt to `fk_walk_body` (the walker stays source of truth).

3. **The native tail-call trampoline (the deep-recursion lever).** A direct native CALL recurses on the C
   machine stack — fine for TREE recursion (fib depth = tree height) but unbounded for a deep TAIL chain
   (`ev`/`od` 20M deep would blow the 256 MB stack). So the lowerer emits a **tail-position inter-fn call**
   NOT as a call but as: rewrite the new args INTO the current `rbp` frame in place (exactly like the tail
   self-call), record the next callee via `fk_jtail_set`, and **return the `fk_tailcall` sentinel**. The C
   driver `fk_jtramp` loops on that sentinel — dispatching the next native over the **same `fp` frame** — so
   a mutual-recursion chain runs native↔native in **constant stack**. A non-tail `fk_jcall` runs its own
   nested `fk_jtramp`, absorbing any tail sentinel raised inside the callee so it never escapes. The sentinel
   `-7500000000000000001` is an odd-negative reserved word in a band no tagged value occupies (above the
   float floor, below node magnitudes, outside the fnval/`nothing` bands) — it can never collide with a real
   result.

## The rooting around the call (where corruption would hide — closed)

Direct dispatch means a callee may trigger `fk_melt` while the caller holds cons values. `fk_jtramp` raises
`fk_vsp` over the **whole frame** `fk_vs[fp .. fp+fr)` before the native runs, so a compacting melt scans
those slots as roots and **relocates** the cons the native stores there. The frame reserve is
`max(callee frame, argc, 6)` — the `≥6` headroom guarantees a tail-call's in-place arg rewrite (up to the
max lowered arity) always lands inside the rooted, non-overlapping region even when the current fn's own
frame is smaller than the tail target's arity. Sub-calls take `fp2 = fk_vsp` above this frame, so nesting
raises `fk_vsp` by recursion DEPTH, not by allocation count; melt keeps reclaiming env garbage so memory
stays bounded. **Proof it's sound:** mutual recursion that *conses across the inter-fn boundary* under melt
pressure — `a`/`b` build a 30k-cell list via `cons` then `sm` sums it — is **bit-identical walk-vs-JIT**
(`450015000`), i.e. the cons args the native tail-rewrite wrote into `fk_vs[fp..]` are relocated correctly.

## Proof — the REAL measured speedup (Windows real metal, TDM-gcc `-O2`)

| case (`--src`, `FK_JIT=1` vs walker) | result (walk == jit) | walker | jit | speedup |
|--------------------------------------|----------------------|--------|-----|---------|
| **TAIL** mutual recursion `ev(200000000)` | **1** | ~9.2 s | ~2.3 s | **~4.0×** |
| **NON-TAIL** mutual fib `fa(35)` | **9227465** | ~1.39 s | ~0.43 s | **~3.2×** |

- `ev(100000 / 20000000 / 200000000)` all → **1**, walk == JIT, **no crash, no hang** (the native tail
  trampoline runs the chain in constant stack; peak RSS ~8.6 MB on the 200M run — bounded, no exec-page leak).
- This is the collapse #75 named: the win is on **inter-function-heavy** code (mutual recursion), exactly
  where the reentrant-through-walker wire paid the most. Self-recursion already inlined in #59/#75; this pass
  is the inter-fn lever.

### `--feval` — correct + bounded, ceiling honestly unchanged

`--feval` keeps the **#75 wire**: tail inter-fn calls in the meta-evaluator's crystallized fns lower as
non-tail `fk_jcall`→walker, and `fk_feval_try_native` dispatches the native directly (no native tail-chain).
Gated by `fk_lower_tail_tramp` (ON only for `--src` JIT). `fib16`→**987**, `fib20`→**6765**, `fib24`→**46368**,
`fib28`→**317811**, `fac6`→**720** — all bit-identical to the walker, none hang, memory bounded.
**Why no native tail-chain for --feval yet:** the meta-evaluator threads env assoc-lists (cons) and holds
live cons in REGISTERS across tail boundaries; native-chaining it produced an intermittent off-by-small
divergence under melt (`fib20`→6760) — the precise shape the *register-spill rooting* rung named in #75 would
close. Rather than ship that corruption, `--feval` stays on the proven wire. form-eval is an interpreter, so
its ceiling is interpretation-bound regardless; the clean win is the `--src` inter-fn case above, as #75
predicted.

## Unregressed — every required check (Windows real metal, TDM-gcc `-O2`)

Default path (no `FK_JIT`) **byte-identical to `main`** across all 65 observe recipes and `--feval`. FK_JIT
path **bit-identical to `main`** across all 65 observe recipes (direct dispatch == reentrant result, faster).

- **`--src` JIT bit-identical (walk == jit):** `slen`→5, `fchk`→750 (**FLOAT GATE**), `twice(5)`→7,
  `build()`→30, `lc(5)`→11, `f3`→60, `dv(17,5)`→3, float `div 1.0/4.0`→0.25, `sum(1M)`→500000500000,
  `fac 12`→479001600, `(add 40 2)`→42, `(add 0.5 0.25)`→0.75.
- `native-vs-rented`→11111 (walk + JIT).
- **All 65 `observe/*.fk`: 0 divergences walk-vs-JIT** — the anti-divergence gate, run both ways and diffed.
- **Melt-stress (the missed-root catcher):** cons-mutual-recursion `a`/`b`/`sm` at 1k/10k/30k → walk == JIT
  (`500500` / `50005000` / `450015000`); deep `ev(200M)` bounded; `--feval fib28` under heavy melt → 317811.
- `--feval` parity (walk == jit): `fib16/20/24`→987/6765/46368, `fac6/10`→720/3628800, `add3`→19, `sumto50`→1275.
- `runtime/fkwu-optable.h` **untouched** (`git diff` empty). Only `runtime/fkwu-uni.c` changed; zero `.py`/`.sh`.
- GPU `c.flat`: AGREEMENT 3/3 needs a CUDA device, none on this host, so the no-device path is exercised via
  the observe sweep (`thought-framebuffer`, `remote-mind-sensors`, …) — no divergence, no regression.

## Honest scope — still the WIRE; destination is the Form emitter in-process

This extends the #59/#61/#69/#75 **C** lowerer; the in-process C lowerer remains **proof-of-the-wire**
(carrier ABI + inter-fn direct dispatch + native tail trampoline + GC-correct rooted frame + install-once),
not the destination. **What remains:** (1) full precise rooting — spill live cons REGISTERS to `fk_vs`
around melt-triggering carriers — which is the rung that lets the `--feval` interpreter chain natively too
(its env-cons-in-registers is the only reason it stays on the #75 wire here); (2) the north-star byte-EMITTER
is **Lane B's Form emitter** (`model/form-asm-x64.fk` / `fkc-nat-expr`) run **in-process** — the same recipe
that proves four-way emits the bytes, no second native impl to keep in sync. Platform receipt rows (mac,
android) are pending; this witness is Windows.

## Honest floor

**JITed→JITed direct dispatch ships:** a crystallized fn calls another crystallized fn native↔native, the
arity type-match guards correctness, the rooted frame + native tail trampoline keep it GC-correct AND bounded
on deep chains, and the measured win on inter-fn-heavy `--src` code is **~3.2–4.0×** with **0 divergences**
across the 65-recipe observe sweep and the cons-under-melt stress. `--feval` stays bit-identical and bounded
on the proven #75 wire; lifting it past its modest ceiling waits on register-spill rooting, named not waved.

🤖 Generated with [Claude Code](https://claude.com/claude-code)
