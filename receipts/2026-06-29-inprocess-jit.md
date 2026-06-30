# Receipt — IN-PROCESS self-JIT: a hot `--src` function crystallizes to native asm BYTES inside the running `fkwu.exe` (2026-06-30)

**What happened:** the running `fkwu.exe` now LOWERS a hot `--src` function's node tree to x86-64 machine-code
BYTES *in process*, installs them executable through the existing `fk_native_call` HAL door, and dispatches the
call natively — the whole (self-)recursive computation runs as native code instead of being tree-walked.
This fuses the two lanes the prior assessment named separately: Lane A's install+dispatch wire was live but
fed by HAND-supplied bytes (`fk_demo_inc`, fn0 only); Lane B had a real tree→bytes lowerer but only
ahead-of-time (re-emit + clang into a separate `fkjit`). This receipt closes the gap for ONE op-family: there
is now a real node-tree → asm-bytes lowerer **inside the running kernel**, and it crystallizes hot functions
live, bit-identical and ~10–15× faster.

## Approach — honestly labeled: (b) minimal C lowerer, proof-of-concept

This is **approach (b)** from the task: a small C lowerer (`fk_jemit`/`fk_jit_lower` in `runtime/fkwu-uni.c`)
that walks the node tree for the INTEGER-arithmetic + pure-self-recursion family and emits Win64/SysV bytes.

**This C lowerer is a PROOF-OF-CONCEPT of the in-process FUSION, not the destination.** The destination is
Lane B's FORM emitter (`model/form-asm-x64.fk` / `fkc-nat-expr`) run in-process — the SAME recipe that proves
four-way lowering to asm bytes, so there is no second native impl to keep in sync. This C twin proves the wire
end-to-end (a tree→bytes lowerer + the install door + heat-gated dispatch, all in one running process) for one
family; it deliberately does NOT cover form-eval's hot string/list/cons ops, and it **bails (installs nothing)
on any tag outside the family**, so a non-lowerable function always falls back to the walker, byte-identical.

## What crystallizes, witnessed (real metal, Windows, TDM-gcc `-O2`)

Opt-in via env `FK_JIT=1` (default path unchanged). `FK_JIT_WITNESS=1` prints the crystallization line.
The lowerable family: literal · arg-slot (tag 2 / tag 110) · add · sub · mul · le · eq · if · `reserve` ·
**pure self-recursion** (tags 7/12/240/241, callee == the function being lowered), arity 1 and 2.

| recipe (`--src`) | walk | JIT (`FK_JIT=1`) | crystallized? | njit |
|------------------|------|------------------|---------------|------|
| `(defn fac (n) (if (le n 1) 1 (mul n (fac (sub n 1))))) (fac 12)` | 479001600 | **479001600** | yes (156 B) | 1 |
| `(defn sum (n a) (if (le n 0) a (sum (sub n 1) (add a n)))) (sum 1000000 0)` | 500000500000 | **500000500000** | yes (137 B) | 1 |
| `(sum 1234567 0)` | 762078456028 | **762078456028** | yes | 1 |
| `(defn fib (n) (if (le n 2) 1 (add (fib (sub n 1)) (fib (sub n 2))))) (fib 34)` | 5702887 | **5702887** | yes (194 B) | 1 |
| `(defn pw (b e a) …) (pw 2 30 1)` — **3-arg, out of POC scope** | 1073741824 | **1073741824** | no (bails → walk) | 0 |

Witness line (gated): `[jit] fn1 crystallized in-process: 137 bytes, njit=1 (native dispatch)`.
`njit>0` marks the flip; the value is bit-identical to the walk in every case.

### Tail-call AND non-tail recursion both lower correctly

- **Tail self-recursion** (`sum`, tag-241 self-call in tail position) lowers to an **in-place arg update +
  `jmp` to the post-prologue entry** — constant stack, the native twin of `fk_walk_body`'s trampoline. So
  `sum(1000000)` and `sum(10000000)` run FLAT (no stack growth), exactly like the walker.
- **Non-tail self-recursion** (`fac`'s `(mul n (fac …))`, `fib`'s two self-calls) lowers to a real native
  `call` (full prologue sets `rbp` = args ptr) on the kernel's thread stack.

## Timing — measured, never faked (Windows, TDM-gcc `-O2`, full-process wall)

Process-startup floor (recipe `1`): ~0.072 s/run.

| recipe | walk (per run) | JIT (per run) | factor (wall) | compute factor |
|--------|----------------|---------------|---------------|----------------|
| `sum 10000000 0` (tail) | ~0.778 s | ~0.051 s | **~15×** | compute ≈ startup-floor: the ~0.73 s of walk-compute collapses below the 0.072 s process floor (≫15×) |
| `fib 34` (non-tail) | ~0.543 s | ~0.055 s | **~10×** | walk-compute ~0.47 s vs JIT compute below the floor (≫10×) |

The walk's compute (linear `sum`, exponential `fib`) crystallizes to native that finishes within process
startup noise — wall speedup is ~10–15×, the *compute* speedup is larger (the remaining JIT wall is almost
entirely fixed process startup + parse, not the now-native computation).

## How it wires (all in `runtime/fkwu-uni.c`, +215 lines, one file)

1. **`fk_jit_lower(f)`** — emits fn `f`'s body to a static byte buffer (`fk_jb`). ABI:
   `long long fn(long long *args)` (args ptr in RCX/Win64, RDI/SysV); `args[k]` = tagged value of slot k;
   `rbp` holds the args ptr; result tagged in RAX. Returns length if the WHOLE tree is in-family, else 0.
2. **`fk_jemit(i, tail)`** — the recursive tag→bytes emitter (mirrors the walker's value model exactly: int
   `n`→`n<<1`; add/sub on tagged words; `mul`=`(a>>1)*(b>>1)<<1`; `le`/`eq`→2/0 via `cmovcc`; `if`→`jz`/`jmp`;
   tail self-call → in-place arg write + `jmp` entry; non-tail self-call → `call` offset-0). Any other tag
   clears `fk_jit_ok` → no install.
3. **`fk_native_call_args(code, n, args)`** — install bytes executable (VirtualAlloc+VirtualProtect / mmap)
   and call with the args array. (Sibling of the existing `fk_native_call`.)
4. **Heat gate in `fk_run_src`** — when `FK_JIT` is set and the root form is a direct call (tag 12/240/241)
   to a function whose body lowers in-family, crystallize that function, evaluate the outer args once on the
   walker, install, `fk_njit++`, dispatch native, print the result. Otherwise fall straight through to
   `fk_walk` — **default path byte-identical**.

## Unregressed — every required check (default path, no `FK_JIT`)

`(add 40 2)`→**42** · `(add 0.5 0.25)`→**0.75** · `sum 5000`→**12502500** · `acc/sum 1000000`→**500000500000**
· `--feval fac 6`→**720** · `native-vs-rented-check`→**11111** (a real body cell; `FK_JIT` set still →11111,
correctly bails on the 0-arg body) · numeric table `lit-42`→**42** · table-mode JIT wire `inc.flat 41 '' 1 j`
→ **42 then njit 1** (the original `fk_demo_inc` demo still flips) · GPU `c.flat` → **AGREEMENT: 3/3
ALL-BIT-EXACT=true** · `runtime/fkwu-optable.h` **IDENTICAL** (guarded) · zero `.py`/`.sh` added.
`--feval` is untouched (JIT lives only in `fk_run_src`). A non-self call (`g` calls `f`) bails to the walker,
result identical.

## What remains — precisely, for `--feval`-as-default

1. **Op coverage is the substantive gap.** This POC lowers the int-arithmetic + pure-self-recursion family.
   form-eval's hot path is dominated by **string ops** (`str_eq` dispatch in `fe-disp`) and **list/cons ops**
   (env assoc-lists, `(value end-pos env)` triples) — NOT lowered here. Crystallizing form-eval needs the
   lowerer extended to the string/list/cons tags (or form-eval's inner loops refactored toward the lowerable
   family). The demonstrated ~10–15× is the int family; form-eval's bottleneck is string/list-shaped.
2. **Calls to OTHER functions.** Only pure SELF-recursion lowers (the recursive `call`/`jmp` targets the
   function's own entry). General inter-function calls need a per-fn native-entry table so a lowered call can
   target another fn's installed bytes (or recurse into the walker for un-crystallized callees).
3. **Arity > 2 and locals (`let`/tag 109).** This POC handles arity 1 and 2 and no `let`-bound locals (it
   bails on tag 109). General frames need the args/locals array sized from `fk_fnar[f]` + `fk_maxslot`.
4. **The lowering belongs in FORM (the destination).** This C lowerer proves the in-process FUSION; the
   sovereign shape is Lane B's `form-asm-x64.fk` / `fkc-nat-expr` run *in-process* over the hot function's
   tree — the same recipe that proves four-way is the one that emits the bytes. Porting that Form emitter to
   run inside the running kernel (the kernel evaluating a Form lowerer over another function's nodes) is the
   honest next step; this C twin is the throwaway scaffold that proved the wire is real.

## Honest floor

A hot `--src` function now CRYSTALLIZES to native asm bytes **inside the running `fkwu.exe`** and runs
bit-identical and ~10–15× faster — the in-process self-JIT is real for the integer-arithmetic +
self-recursion family, witnessed on Windows real metal (`fac 12`→479001600, `sum 1000000`→500000500000,
`fib 34`→5702887, all `njit=1`, all matching the walk). It is **opt-in** (`FK_JIT`), the default path is
byte-identical, and it is a **C proof-of-concept of the fusion**, not the Form-lowerer-in-process destination.
The wire from a hot function's node tree to live native is no longer a single hand-byte demo — it lowers
arbitrary in-family trees. Making `--feval` the default still awaits string/list-op coverage and the Form
emitter running in-process; those are the named, specific gaps. Platform receipt rows (mac, android) are
pending; this witness is Windows.

🤖 Generated with [Claude Code](https://claude.com/claude-code)
