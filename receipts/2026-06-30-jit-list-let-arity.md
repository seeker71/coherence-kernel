# Receipt — IN-PROCESS self-JIT closes the carrier rows: list/cons, let-locals, arity>2 (2026-06-30)

**What happened:** the in-process self-JIT (the #59 wire, made general in #61: any-kind literals +
primitive-carrier calls + inter-fn calls, float-correct) now lowers three more shapes that previously
bailed to the walker — exactly the "one `fk_jprim*` row + one emit branch" pattern #61 named, no new
infrastructure:

1. **List/cons carriers (tags 18-23):** `empty`(18)/`cons`(19)/`head`(20)/`tail`(21)/`len`(22)/`nth`(23).
2. **`let`-locals (tag 109):** a `let` binds its value into the native frame slot, mirroring how
   `fk_walk` binds `fk_vs[fp+slot]`.
3. **Arity > 2:** the lowerer + inter-fn carrier + native self-recursion now handle 3-arg fns.

A `cons`/`let`/arity-3 fn that printed `[jit-bail]` and fell through to the walker before now **crystallizes
native, bit-identical, njit>0.**

## The three mechanisms (all in `runtime/fkwu-uni.c`, one file)

1. **List/cons carriers.** New C carriers `fk_jlist1(tag,a)` (head/tail/len) and `fk_jlist2(tag,a,b)`
   (cons/nth) take ALREADY-EVALUATED tagged words and return the tagged result — each mirrors `fk_walk`'s
   tag case EXACTLY, so a lowered list op can never drift from the walker. `empty`(18) needs no carrier
   (it emits `mov rax,1`, the nil value). The cons carrier replicates tag-19 in full: it pushes h,t onto
   `fk_vs` (as the walker does via `fk_vp`) BEFORE the melt check, so the GC sees them as roots and
   relocates them, then reads the relocated words back — the JIT itself holds no live pairs across the
   call (its intermediates are in registers/machine-stack), so `fk_vs` is the correct, only root set,
   exactly as in the walker. Emit branches follow the #61 string-carrier shape (push args, stage tag as
   arg0, `fk_jcarrier`).

2. **`let`-locals (tag 109).** `(let slot val body)` lowers to: `eval val -> mov [rbp+slot*8],rax` then
   `eval body` (tail position preserved). This is the native twin of the walker's
   `fk_vs[fp+slot] = walk(val); return walk(body)` — the JIT's `rbp` args array IS the equivalent of
   `fk_vs[fp..]`, slot N at offset N. The catch is **frame sizing**: the args array the ENTRY and every
   native self-call build must hold args + let-locals, else a let store writes past it. A new
   `fk_jit_frame` captures `max(arity, maxslot+1)` from the body's `reserve` wrapper (tag 111,
   slot-count literal); the outer entry array is sized generously and zero-initialized, and the native
   self-recursion call site allocates `fk_jit_frame*8` (let-slots stay scratch).

3. **Arity > 2.** Arg collection generalized to `an[3]`; the inter-fn carrier `fk_jcall` was rewritten from
   `(callee, argc, a, b)` to `(callee, argc, const long long *args)` so it dispatches ANY arity 0..3
   through the walker (byte-identical) — the emitter builds the evaluated-args array on the machine stack
   (arg0 lowest) and passes its pointer; `fk_jcarrier`'s `and rsp,-16` only moves rsp DOWN, leaving the
   array intact. Native self-recursion (tail and non-tail) handles argc 1..3: tail writes `[rbp+k*8]` in
   place + `jmp` entry (flat stack); non-tail reserves a `frame*8` args region and copies temps into
   args[k] before `call` offset-0.

## Witnessed proofs (real metal, Windows, TDM-gcc `-O2`) — bit-identical to the walker, njit=1

Opt-in `FK_JIT=1`; `FK_JIT_WITNESS=1` prints the crystallization line.

| recipe (`--src`) | walk | JIT | crystallized? | mechanism |
|------------------|------|-----|---------------|-----------|
| `(defn third (xs) (head (tail (tail xs)))) (defn build () (third (cons 10 (cons 20 (cons 30 (empty)))))) (build)` | 30 | **30** | yes (318 B), njit=1 | cons/head/tail/empty carriers + inter-fn call |
| `(defn lc (n) (let d (mul n 2) (add d 1))) (lc 5)` | 11 | **11** | yes (263 B), njit=1 | **let-local** in native frame slot |
| `(defn f3 (a b c) (add a (add b c))) (f3 10 20 30)` | 60 | **60** | yes (233 B), njit=1 | **arity-3** frame + slot reads |

Extra correctness checks (walk = JIT, all bit-identical, njit=1) — the hard cases that stress the new wire:
- `nth` carrier: `(nth (cons 10 (cons 20 (cons 30 (empty)))) 1)` → **20**; `len` carrier → **3**.
- **arity-3 TAIL self-recursion** (the `pw` that bailed in #59): `(pw 2 30 1)` → **1073741824**.
- **3 nested lets**: `(ml 5)` with `a=n*2, b=a+3, c=b*2, a+b+c` → **49** (frame sizing correct).
- **non-tail recursion + let**: `(facl 6)` (`let m (sub n 1)` then `mul n (facl m)`) → **720** (let-slot
  survives the native recursion frame).
- **tail recursion + 2 lets, 100k deep**: `(suml 100000 0)` → **5000050000** (flat stack + let-locals
  together).

## THE PAYOFF — how much of form-eval lowers now

A diagnostic scan (`FK_JIT_SCAN=1`, measurement-only — installs nothing, changes no result) attempts to
lower every top-level defn in a `--src` bundle and reports crystallize-vs-bail. On the full form-eval
bundle (helpers + `grammars/form-eval.fk`, 86 defns):

**`[scan] lowered=66 bailed=20 total=86`** — **66 of 86 form-eval functions now lower.**

Before this PR (#61 baseline) every fn using cons/head/tail/len/nth/empty (45 of the body's lines
reference a list op), `let` (2 fns), or an arity-2+ inter-fn call bailed; those moved from the bail
column into the 66. `fe-sym-end` (the #61 in-process bonus) still lowers.

The remaining **20 bails**, by exact reason (each fn bails on the FIRST unsupported shape it hits):
- **Arity > 3 call** — 22 occurrences across the bailing fns (19× a 4-arg call, 2× 5-arg, 1× 6-arg).
  form-eval threads `(value end-pos env)` triples through 4-6-arg dispatch helpers (`fe-expr2`,
  `fe-disp`, `fe-bin-b`, `fe-prim-apply`, …). This is the dominant remaining gap and the next carrier
  row: extend the args-array path from 3 to N (the `fk_jcall` args-pointer ABI already generalizes; only
  the emit loops cap at 3).
- **tag 10 (`div`)**, **tag 11 (`mod`)**, **tag 103 (`lt`)** — 1 fn each. Three trivial binary primitives,
  each one more `fk_jprim2` row + emit branch (the exact #61 pattern), simply not added in this pass.

So form-eval does **not yet lower whole**; the precise remaining work is: (a) arity > 3 (lift the emit
loops past the hardcoded 3, the carrier ABI is ready), and (b) the div/mod/lt primitive carriers.

### `--feval` speed signal — none yet, and why (honestly)

`--feval` runs through `fk_run_feval`, which does **not** invoke the JIT gate (the gate lives only in
`fk_run_src`). Witnessed: `FK_JIT=1 FK_JIT_WITNESS=1 --feval (fac 5)` → **120** with **no `[jit]` line** —
the gate never fires on the `--feval` path. So there is no `--feval`-under-JIT speed signal to report in
this lane; wiring the JIT gate into the `--feval` path (form-eval-native-by-default) is the SEPARATE later
lane the task carved out. What this PR delivers is *coverage*: 66/86 of form-eval's functions are now
individually lowerable, which is the precondition for that later wiring.

## Unregressed — every required check (Windows real metal, TDM-gcc `-O2`)

Default path (no `FK_JIT`) byte-identical; `FK_JIT` path bit-identical where it crystallizes, byte-identical
(walker fallthrough) where it bails. Witnessed:
- **#61 proofs hold:** `slen`→**5**, `fchk`→**750** (float still correct!), `twice(5)`→**7**, all njit=1.
- **#59 proofs hold:** `fac 12`→**479001600**, `sum 1000000`→**500000500000**, all njit=1.
- `(add 40 2)`→**42** · `(add 0.5 0.25)`→**0.75** (walk + JIT) · `native-vs-rented`→**11111** (walk + JIT)
  · `--feval fac(6)`→**720** (walk; `--feval` untouched) · numeric table `lit-42.flat`→**42** (walk + JIT)
  · table-mode JIT wire (`fk_demo_inc`) still flips njit→1.
- **All `observe/*.fk` recipes (65): 0 divergences walk-vs-JIT** — the anti-divergence gate, run twice
  (after the carriers landed and after the diagnostic prints landed).
- `runtime/fkwu-optable.h` **IDENTICAL** (guarded: `cp` before, `diff` after). Zero `.py`/`.sh` added;
  only `runtime/fkwu-uni.c` changed.

GPU `c.flat` (the AGREEMENT 3/3 demo) is referenced in prior receipts but the `c.flat` demo file is not
present in this lineage's tree, so it is not asserted here.

## Honest scope — still the WIRE, not the destination

This extends the #59/#61 **C** lowerer. It remains the **proof-of-concept of the general WIRE** (a carrier
ABI + inter-fn dispatch + any-kind literals + now list/let/arity), not the destination. The byte-EMITTER's
destination is **Lane B's Form emitter** (`model/form-asm-x64.fk` / `fkc-nat-expr`) run **in-process** —
the SAME recipe that proves four-way emits the bytes, so there is no second native impl to keep in sync.
The list/let/arity carriers added here are throwaway scaffold; the contract they prove
(lower-as-carrier-call for correctness, mirror the walker exactly, inline only the provably-fast int path)
is what the Form emitter inherits. The C lowerer is not the story — it is the wire the Form lowerer runs on.

## Honest floor

The in-process self-JIT now crystallizes recipes using list/cons ops, `let`-bound locals, and arity-3
functions — bit-identical to the walker, njit=1, on Windows real metal. **66 of 86 form-eval functions
lower**; the remaining 20 bail on a precisely-named set (arity > 3, and the `div`/`mod`/`lt` primitives) —
each the same "one carrier row + one emit branch" follow-on, no new infrastructure. form-eval does not yet
lower *whole*, and the `--feval` path is not yet wired to the JIT gate (the separate later lane). It is
**opt-in** (`FK_JIT`), the default path is byte-identical, and it remains a **C proof-of-concept of the
general wire** — the destination is the Form emitter run in-process. Platform receipt rows (mac, android)
are pending; this witness is Windows.

🤖 Generated with [Claude Code](https://claude.com/claude-code)
