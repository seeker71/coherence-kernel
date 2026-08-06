# Receipt — IN-PROCESS self-JIT goes GENERAL: any-kind literals, primitive-carrier calls, inter-fn calls (2026-06-30)

**What happened:** the in-process self-JIT (the #59 wire that crystallizes a hot `--src` function to native
asm BYTES inside the running `fkwu.exe`) is no longer limited to the integer-arithmetic + pure-self-recursion
family. It now lowers recipes that (1) carry **literals of any value kind** (string literals, and floats — which
parse as `str_to_float` over a string literal), (2) **call kernel primitives** the JIT doesn't inline (string
ops, float-correct arithmetic), and (3) **call OTHER functions**, not only themselves. The reframe from the repo
owner landed exactly as stated: *"string ops" are not a special JIT feature — they are RECIPES over primitives.*
Once the JIT can emit an any-kind literal and a CALL to a kind-correct carrier, ALL recipes lower as recipes;
strings/floats come for free with **zero per-op coverage work** beyond one carrier line apiece.

## The three GENERAL mechanisms that landed (all in `runtime/fkwu-uni.c`, +217 lines, one file)

1. **Any-kind literals.** A string literal (node tag 24) emits `mov rax, idx<<1` — the SAME shape as an int
   literal (tag 1), because the interned tagged word is known at lower-time (the string pool index is fixed at
   parse). A float literal needs no special case: the parser lowers `0.5` to `(str_to_float "0.5")`, so it
   composes from a string literal + a `str_to_float` carrier call.

2. **Primitive-carrier calls.** New C carriers `fk_jprim1/2/3(tag, args…)` take ALREADY-EVALUATED tagged words
   and return the tagged result, mirroring `fk_walk`'s tag case EXACTLY — so a lowered op can never drift from
   the walker (the computation lives in one place). The JIT emits: eval args → push → realign stack → call the
   carrier → result in rax. Covered carriers: `add/sub/mul/le/eq` (float-aware), `str_len` (25), `str_eq` (26),
   `str_concat` (27), `str_byte_at` (28), `substring` (29), `str_to_float` (53), `float_to_int` (54). Adding the
   next op is one row in `fk_jprim*` + one emit branch — not new infrastructure. The carrier call uses a robust
   stack-realign trampoline (`fk_jcarrier`) that works at any incoming alignment (save rsp → `and rsp,-16` →
   pad+shadow → call → restore), Win64 (rcx,rdx,r8,r9 + 32B shadow) and SysV (rdi,rsi,rdx,rcx).

3. **Inter-function calls.** A call to a function OTHER than the one being lowered emits a call to the carrier
   `fk_jcall(callee, argc, a, b)`, which dispatches that callee through the walker (`fk_walk_body`) — always
   byte-identical, and reentrant (it saves/restores `fk_vsp`). So a JITed function can call ANY other function
   correctly. Pure self-recursion still lowers to the native `call`/tail-`jmp` from #59 (the fast path).

### Float correctness — the gate, handled by a runtime tag-check (NOT a wrong int answer)

The #59 path INLINED `add/sub/mul/le/eq` as integer ops — **wrong** for float operands (`fchk` would have
returned an integer-add answer). The fix keeps the int fast path AND is float-correct: each binary op emits a
runtime guard — `cmp` both operands against the float-band ceiling (`fk_fbase-2`, a huge negative); if EITHER is
a boxed float, branch to the float-correct carrier `fk_jprim2`, else run the int inline. Ints (`n<<1`, including
negatives like `-500500`) never trip the guard; floats always do. So `fac/sum/fib` stay native-fast (int inline)
and `0.5+0.25` is correct. **This is a real fix, not a feature dressed over a divergence.**

## Witnessed proofs (real metal, Windows, TDM-gcc `-O2`) — bit-identical to the walker, njit>0

Opt-in `FK_JIT=1`; `FK_JIT_WITNESS=1` prints the crystallization line. Each proof's RESULT is an int (easy to
compare) but its COMPUTATION exercises the new mechanism.

| recipe (`--src`) | walk | JIT (`FK_JIT=1`) | crystallized? | mechanism exercised |
|------------------|------|------------------|---------------|---------------------|
| `(defn slen () (str_len (str_concat "ab" "cde"))) (slen)` | 5 | **5** | yes (148 B), njit=1 | string literals + str_concat + str_len carriers |
| `(defn fchk () (float_to_int (mul (add 0.5 0.25) 1000))) (fchk)` | 750 | **750** | yes (420 B), njit=1 | float literals + **float-correct** add/mul + float_to_int |
| `(defn inc (n) (add n 1)) (defn twice (n) (inc (inc n))) (twice 5)` | 7 | **7** | yes (177 B), njit=1 | **inter-function call** to `inc` (not self) |

`fchk` is the float-correctness gate: **750, not an integer-add answer.** Extra correctness checks (walk = JIT):
`(sub 10.0 0.5)*100 → 950`, `(le 0.5 0.25) → false-branch (200)`, negative-int accumulation `-500500`,
`str_byte_at "ABC" 1 → 66`, `substring "hello" 1 4` len `→ 3`, `str_eq s "add" → 1`.

### BONUS — a real form-eval hot function now crystallizes in-process

`fe-sym-end` (form-eval's symbol-scanner, `grammars/form-eval.fk:68`) crystallizes **in-process, bit-identical
(3), njit=1** when given its deps as one `--src` bundle. It exercises ALL THREE mechanisms at once: string
literals, primitive carriers (`substring`/`str_byte_at`/`str_len`/`eq`), inter-fn calls (`fe-len`,
`fe-isdelim`, `fe-at`), AND self-recursion. This is the first form-eval inner-loop function to lower native.

## What still keeps form-eval from lowering whole — precisely

1. **List/cons ops are not carriers yet.** form-eval threads `(value end-pos env)` triples and an assoc-list env
   via `list`/`cons`/`head`/`tail`/`nth` (tags 19/20/21/22/23). These are pure value ops and will lower the same
   way (one `fk_jprim*` row + one emit branch each) — they are simply not added in this pass. A function using
   them bails to the walker, byte-identical (witnessed: a `cons`-using fn prints `[jit-bail] unsupported tag 19`
   and falls through with the identical result).
2. **`let`-bound locals (tag 109) and arity > 2** are still out of scope (the #59 frame is args[0..1] only).
3. **Inter-fn calls go through the WALKER carrier, not another fn's native entry.** Correct, but not yet a perf
   win for the callee — native-entry chaining (`fk_src_nat[callee]`) is the perf follow-on.

## Timing — measured, never faked

The int self-recursion fast path is **unchanged in speed** despite the new float-guard branches (for int
operands the guard is a not-taken `cmp`+`jle`): `sum 10000000` walk **~0.566 s** → JIT **~0.059 s** (~10×). The
string/inter-fn carriers route back into kernel C (and, for inter-fn, the walker), so those recipes are about
walker-speed — the win there is *coverage* (they crystallize at all, bit-identical), not raw speed yet. Raw
speed for string/list recipes comes when their carriers stop bouncing through the walker.

## Honest scope — this is the WIRE, not the destination

Extending the #59 **C** lowerer is the proof-of-concept of the general WIRE: it proves ANY recipe can crystallize
in-process, bit-identical, by composing any-kind literals + carrier-calls + inter-fn-calls. The byte-EMITTER's
destination is **Lane B's Form emitter** (`model/form-asm-x64.fk` / `fkc-nat-expr`, tensor-IR `jit-tensor-emit.fk`)
run in-process — the SAME recipe that proves four-way emits the bytes, so there is no second native impl to keep
in sync. The general wire built here (a carrier ABI + an inter-fn dispatch + any-kind literals) is reusable behind
either emitter. The carriers themselves are throwaway scaffold; the contract they prove (lower-as-carrier-call for
correctness, inline only the provably-fast path) is what the Form emitter inherits.

## Unregressed — every required check

Default path (no `FK_JIT`) byte-identical; `FK_JIT` path bit-identical where it crystallizes, byte-identical
(walker fallthrough) where it bails. Witnessed:
`sum 1000000`→**500000500000** (walk + JIT) · `fac 12`→**479001600** (JIT njit=1) · `fib 34`→**5702887** (JIT) ·
`(add 40 2)`→**42** · `(add 0.5 0.25)`→**0.75** (walk + JIT) · `--feval (do (defn fac…) (fac 6))`→**720**
(`--feval` untouched; JIT lives only in `fk_run_src`) · `native-vs-rented-check`→**11111** (walk + JIT) ·
numeric table `lit-42`→**42** · table-mode JIT wire `inc.flat 41 '' 1 j`→**42 then njit 1** (the original
`fk_demo_inc` demo still flips) · GPU `c.flat`→**AGREEMENT 3/3 ALL-BIT-EXACT=true** · all **63 `observe/*.fk`
recipes: 0 divergences** walk-vs-JIT · `runtime/fkwu-optable.h` **IDENTICAL** (guarded) · zero `.py`/`.sh` added.

## Honest floor

The in-process self-JIT is now GENERAL in shape: it crystallizes recipes using string literals, float literals,
float-correct arithmetic, string primitives, and calls to other functions — bit-identical to the walker, njit>0,
on Windows real metal. The float-correctness gate (`fchk → 750`) passes via a runtime tag-check, never a wrong
int answer. A real form-eval hot function (`fe-sym-end`) crystallizes in-process. What keeps form-eval from
lowering *whole* is named precisely: list/cons carriers (one row each, not infrastructure), `let`-locals, arity>2,
and native-entry chaining for callee speed. It is **opt-in** (`FK_JIT`), the default path is byte-identical, and
it remains a **C proof-of-concept of the general wire** — the destination is the Form emitter run in-process.
Platform receipt rows (mac, android) are pending; this witness is Windows.

🤖 Generated with [Claude Code](https://claude.com/claude-code)
