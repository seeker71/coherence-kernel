# Receipt — form-eval lowers WHOLE (Piece A); --feval JIT wire root-caused to a GC-root gap, reverted (Piece B) (2026-06-30)

**What happened:** the in-process self-JIT now lowers **86 of 86** form-eval functions — **form-eval
lowers WHOLE, 0 bails** (was 66/86 in #69). The two precisely-named #69 gaps are closed: arity > 3
(extended to 6) and the `div`/`mod`/`lt` primitive carriers. Wiring the heat gate into `--feval`
(Piece B) was implemented and proven to fire (`[jit]`, njit>0, result unchanged on shallow inputs) —
but it **diverges from the walker under GC**: a deeply-recursive form-eval function dispatched natively
holds cons pointers in a private args frame the collector cannot see, so a melt mid-call leaves them
stale and the meta-eval loops. That is a correctness/termination bug, not "modest speedup", so Piece B
is **reverted** per the divergence gate; the precise fix is named below. This PR ships Piece A.

## PIECE A — the last bails close, form-eval lowers whole (all in `runtime/fkwu-uni.c`)

Two follow-on rows in the exact #61/#69 pattern — one `fk_jprim*` row + one emit branch each, no new
infrastructure:

1. **`div`/`mod`/`lt` carriers (tags 10/11/103).** Added to `fk_jprim2`, each MIRRORING `fk_walk`'s
   tag case EXACTLY so it can't drift. `div`/`mod` are **float-aware via the same `fk_isf`/`fk_num`/
   `fk_fbox` runtime tag-check #61 used for add/sub/mul** — boxed-float operands take the float carrier
   (`fk_fbox(fk_num(a)/fk_num(b))`, and mod's `x - y*(double)((long long)(x/y))`), else the int path
   (`((a>>1)/(b>>1))<<1`). So float div is CORRECT, never an int-div wrong answer. The emit branch is the
   2-arg `fk_jprim2(tag,a,b)` carrier shape (identical staging to str_concat). These three go always
   through the carrier (no int-inline asm `idiv`), which is both simplest and uniformly bit-identical.

2. **Arity > 3 → 6.** The variadic call path (`an[3]` + `cnt>3` bail) lifted to `an[6]` + `cnt>6`; the
   root-call collection in `fk_run_src` lifted `ac>3` → `ac>6`. The `fk_jcall` args-pointer ABI already
   generalized in #69 (it takes `const long long *args`), and both native self-recursion paths (tail
   `mov [rbp+k*8]`, non-tail temp→args copy) already loop over `argc` — only the emit-loop caps were at
   3. `k*8` for k≤5 fits disp8; Win64 alignment is handled by `fk_jcarrier`'s `and rsp,-16` (the args
   array is captured as `rsp` and survives, as for argc 1-3).

### Piece A proofs (real metal, Windows, TDM-gcc `-O2`) — bit-identical to the walker, njit=1

Opt-in `FK_JIT=1`; `FK_JIT_WITNESS=1` prints the crystallization line. walk = JIT for every row:

| recipe (`--src`) | exp | walk | JIT | bytes |
|------------------|-----|------|-----|-------|
| `(defn f4 (a b c d) (add a (add b (add c d)))) (f4 1 2 3 4)` | 10 | 10 | **10** | 343 |
| `(defn dv (a b) (div a b)) (dv 17 5)` | 3 | 3 | **3** | 86 |
| `(defn md (a b) (mod a b)) (md 17 5)` | 2 | 2 | **2** | 86 |
| `(defn ltf (a b) (lt a b)) (ltf 3 7)` | 1 | 1 | **1** | 86 |

Float-correctness (the gate that makes div/mod safe to lower) and higher arity:
- `(div 1.0 4.0)` → **0.25** (walk + JIT) — NOT int-div 0; `(mod 5.5 2.0)` → **1.5**; `(lt 2.5 2.5)` → **0**.
- arity 5 `(f5 1 2 3 4 5)` → **15**; arity 6 `(f6 1 2 3 4 5 6)` → **21** (walk + JIT).

### THE PAYOFF — form-eval lowers whole

`FK_JIT_SCAN=1` (measurement-only — installs nothing, changes no result) over the form-eval bundle
(helpers + `grammars/form-eval.fk`, 86 defns):

**`[scan] lowered=86 bailed=0 total=86`** — **every form-eval function now lowers** (verbose `FK_JIT_SCAN_V`
confirms **0 `BAILS`**). Up from 66/86 in #69; the closed 20 were the 19×4-arg/2×5-arg/1×6-arg dispatch
helpers plus the one `div`, one `mod`, one `lt` fn.

## PIECE B — wired, witnessed firing, then root-caused to a GC-root divergence and REVERTED

The wire was built and **works on shallow recipes**: a heat-gated dispatcher (`fk_jit_feval_dispatch`,
hot ≥ 5 matching `observe/jit-decision.fk`) consulted at `fk_walk`'s call sites (tags 12/240/241/244,
guarded by an `FK_JIT`-only flag so the default path stays byte-identical). It crystallizes each hot
callee once (install-exec cached, no per-call syscall) and dispatches native. Witnessed BEFORE revert:
- `FK_JIT=1 --feval (do (defn fac …) (fac 6))` → **720** with **`[jit]` lines, njit>0** (and **720**
  without FK_JIT) — the gate fires on `--feval`, which #69 could not do.
- A clean comparative where the JIT path completed (fib14 through form-eval): no-jit **159 ms** → jit
  **115 ms** (~1.4×), result 377 both ways; `--src` fib14 = 36 ms (the native lane). So the wire CAN help.

**But it diverges under GC** — the hard gate. fib16/20/22 through `--feval`-under-JIT **hung** (no result
in minutes) while the walker returned in ~3 s; the result was non-monotonic (fib14 fast, fib16 hung).
Root cause, proven by experiment: rebuild with the cons arena enlarged so `fk_melt()` never runs →
**fib16-under-JIT returns the correct 987**, fib20 likewise. With the normal 4096-pair arena (form-eval
mels constantly building env-lists) it hangs. The native dispatcher copies the callee's args into a
**private `fargs[]` frame** to feed the native ABI and protect `fk_vs` from the native frame's let-slots
— but that copy holds tagged **cons pointers the collector cannot see**. A melt during a long native call
relocates the pairs; `fargs[]` keeps the stale indices; form-eval's env-list walk then reads garbage and
loops. This is a **divergence** (fkwu computes a different/non-terminating answer than the three walkers),
not an unsupported op — so by the divergence rule it cannot ship. Piece B was reverted with surgical edits
(`git diff main` is now **only** Piece A: div/mod/lt + arity 3→6, 21 insertions / 5 deletions).

### The precise remaining work (named, not waved)

1. **GC-root the native frame (the blocker).** The native dispatcher's args/frame array must be a visible
   root set during the call — registered like `fk_vs` so `fk_melt` relocates the cons pointers inside it
   (or: forbid native dispatch for any fn that can allocate across a call, i.e. anything reaching a `cons`
   carrier). Until then form-eval — which is allocation-heavy — cannot be safely native-dispatched.
2. **Native-entry chaining (the speed lever, separate).** Even GC-correct, the current wire is
   reentrant-through-walker: a native form-eval fn calls every sub-fn back through `fk_walk`/`fk_jcall`,
   so a call-heavy meta-evaluator pays (native frame + walker dispatch) and sees at best a **modest** win
   (the measured ~1.4× on fib14, sometimes net-negative). The collapse needs JITed-fn → JITed-fn DIRECT
   dispatch (no walker round-trip). The honest measurement does **not** show a collapse, so none is claimed.

## Unregressed — every required check (Windows real metal, TDM-gcc `-O2`)

Default path (no `FK_JIT`) byte-identical; `FK_JIT` path bit-identical where it crystallizes. Witnessed on
the Piece-A-only binary:
- **Piece A proofs:** f4→10, dv→3, md→2, ltf→1 (FK_JIT bit-identical); float div/mod/lt correct (0.25/1.5/0).
- **#69/#61/#59 proofs hold:** `slen`→5, `fchk`→750 (**float still correct**), `twice`→7, `build`→30,
  `lc 5`→11, `f3`→60, `sum 1000000`→500000500000 (walk + JIT), `fac 12`→479001600 (walk + JIT).
- `(add 40 2)`→42 · `(add 0.5 0.25)`→0.75 (walk + JIT) · `native-vs-rented`→11111 (walk + JIT) ·
  `--feval fac(6)`→720 (FK_JIT **on AND off** — `--feval` is untouched, byte-identical to baseline) ·
  numeric table `lit-42.flat`→42 · table-mode JIT wire (`fk_demo_inc`) flips njit→1.
- **All `observe/*.fk` recipes (65): 0 divergences walk-vs-JIT** — the anti-divergence gate, run TWICE.
- `runtime/fkwu-optable.h` **IDENTICAL** (guarded: `cp` before, `diff` after, OPTABLE IDENTICAL). Zero
  `.py`/`.sh` added; only `runtime/fkwu-uni.c` changed.
- GPU `c.flat` (AGREEMENT 3/3) is not present in this lineage's tree, so it is not asserted here (as #69).

## Honest scope — still the WIRE, destination is the Form emitter in-process

This extends the #59/#61/#69 **C** lowerer; it remains the **proof-of-concept of the general WIRE** (a
carrier ABI + inter-fn dispatch + any-kind literals + list/let/arity + now div/mod/lt + arity-6), not the
destination. The byte-EMITTER's destination is **Lane B's Form emitter** (`model/form-asm-x64.fk` /
`fkc-nat-expr`) run **in-process** — the SAME recipe that proves four-way emits the bytes, so there is no
second native impl to keep in sync. The carriers here are throwaway scaffold; the contract they prove
(lower-as-carrier-call, mirror the walker exactly, inline only the provably-fast int path) is what the
Form emitter inherits.

## Honest floor

**form-eval lowers WHOLE: 86/86, 0 bails** (div/mod/lt float-correct, arity to 6) — bit-identical to the
walker, njit=1, on Windows real metal, opt-in `FK_JIT`, default byte-identical. **`--feval`-under-JIT does
NOT ship**: the wire fires and is correct on shallow recipes (`fac 6`→720 with `[jit]`) and even helps
where it completes (fib14: 159→115 ms), but it **diverges under GC** on deep recursion (the native frame
is an invisible root set; melt corrupts the cons copies) — a divergence, reverted, with the fix named
(GC-root the native frame; then native-entry chaining for the actual speed collapse). Platform receipt
rows (mac, android) are pending; this witness is Windows.

🤖 Generated with [Claude Code](https://claude.com/claude-code)
