# Receipt — self-JIT crystallize-on-heat measured: a hot pure cell goes native, ~13× (2026-06-29)

**What happened:** the `fib` recipe — a pure recursive cell (tags 1–7,12, the pure-compute family) — was
emitted through fkwu's self-JIT (`fkc-emit-jit` → `fkc-emit-jit2` / `fkc-nat-expr`, in
`form/form-stdlib/hati-os-kernel-emit.fk`) and run past its heat threshold so dispatch flips from the
tree-walker to the **crystallized native form lowered FROM the same cell**. Cold (tree-walked) and hot
(crystallized) times were measured. Native fired; the speedup is real.

**The cell (one recipe, two forms).** The fib recipe as data:

    (fk-if (fk-le (fk-arg) (fk-lit 1)) (fk-arg)
           (fk-add (fk-call 0 (fk-sub (fk-arg) (fk-lit 1)))
                   (fk-call 0 (fk-sub (fk-arg) (fk-lit 2)))))

`fkc-nat-expr` walks that tree tag-by-tag and lowers it to a straight native C body — NOT a hand-written
fast-path, the recipe itself become native:

    static long long fk_nat0(long long a) {
        return (fk_jle(a, 2) == 0
                ? fk_jadd(fk_nat0(fk_jsub(a, 2)), fk_nat0(fk_jsub(a, 4)))
                : a);
    }

**What triggers the JIT.** The emitted walker counts heat per function (`fk_heat[f]`, incremented on every
CALL/SELF dispatch — tags 7 and 12). When `fk_heat[f] > fk_hot` the next dispatch flips to
`fk_nat_tab[f](v)` — the crystallized native — and `fk_njit` ticks. `fk_hot` is the threshold (argv[2];
absent ⇒ 10^12 ⇒ pure walk forever). Crystallize-threshold sits above the melt-threshold (hysteresis,
`jit-decision.fk`) so a boundary cell never thrashes. This is the live decision named in
`jit-decision.fk` and roadmap Gap 4, observed firing.

**Genuine native, not a walker callback.** Disassembly (`otool -tV`) confirms `_fk_nat0` calls only
`_fk_jle`/`_fk_jsub`/`_fk_jadd` and **recurses into `_fk_nat0` itself** — it never re-enters `_fk_walk`.
Once the root of a recursion crystallizes, the whole subtree runs native in one call (that is why `njit`,
which counts only top-of-subtree entries, is small while the speedup is large).

**Parity across the flip.** Cold walk, hot native, and the Go walker all agree:
fib 30 → 832040 (cold) = 832040 (hot, njit=26) = 832040 (Go walker). The crystallized form is the recipe,
bit-for-bit; only the speed changes.

**Measured cold-vs-native** (Apple M-series, clang -O2, median of N full-process invocations; "net" subtracts
the ~1.9 ms process-startup floor measured at fib 1):

| n  | cold (walk) | hot (native) | cold-net | hot-net | speedup (net) |
|----|-------------|--------------|----------|---------|---------------|
| 28 | 26.40 ms    | 3.87 ms      | 24.48 ms | 1.96 ms | **12.5×**     |
| 30 | 64.77 ms    | 6.47 ms      | 62.86 ms | 4.56 ms | **13.8×**     |
| 32 | 165.80 ms   | 13.87 ms     | 163.89 ms| 11.95 ms| **13.7×**     |
| 34 | 429.40 ms   | 34.01 ms     | 427.48 ms| 32.09 ms| **13.3×**     |

**Threshold sweep (fib 32) — the crystallization boundary, watched directly:**

    walk-only (∞):  165.98 ms   njit=0     ← never crystallizes; pure tree-walk
    thr=1,000,000:   36.12 ms   njit=12    ← crystallizes once heat earns it
    thr=200:         14.01 ms   njit=24
    thr=50:          14.26 ms   njit=26
    thr=10:          14.03 ms   njit=12
    thr=2:           13.88 ms   njit=4     ← crystallizes almost immediately

`njit=0 → finite` is the flip itself: any reachable threshold takes the recipe from 166 ms (walked) to ~14 ms
(native). The decision is what drives the speed — not a separate compiler pass we invoke, the same recipe the
walker walks becomes the native it dispatches.

**Honest classification:** this is a **native measurement on the C-bootstrapped self-JIT lane**, NOT a
four-way band. The emitter (`fkc-nat-expr` and the walker text) is proven three-/four-way elsewhere; this
receipt witnesses the *live crystallize decision and its measured speedup*, which is a timing fact, not a
value-parity fact. Value parity across the flip is shown (832040 cold=hot=Go-walker). The native lane here
is the audit's clang-compiled self-JIT C (`fkc-emit-jit`), the speed-reference rung; the sovereign target is
the same recipe lowered Form→asm bytes (`form-asm`), which the byte-identity gate already proves four-way.

**What it means for wiring live cognition.** The gap named in the roadmap was never "can a recipe go native"
— `fkc-nat-expr` already lowers the pure-compute family, and `form-asm` already lowers Form→asm bytes
four-way. The gap is *wiring hot cognition cells to crystallize live*, and this receipt shows that wire
carrying current: a pure cell, run hot, flips to native at a measured threshold and runs **~13× faster with
bit-identical output**. The pieces that matter for cognition — matvec/dot/layernorm/softmax, whose
transcendentals are Taylor/Newton recipes over exactly these `fk_jadd`/`fk_jsub`/`fk_jmul` ops — crystallize
by the same mechanism (`fkc-jhelpers-text` is built so the native is bit-identical to the walk for both int
and float, the call boundary blocking any FMA contraction). The honest floor: this is the integer
pure-recursion family observed live; the float tensor lane crystallizes by the same door but its live-wiring
measurement at real model width is the next witness, not this one.

**Honest blocker note (named, not waved):** the full `scripts/hati_os_kernel_audit.sh` currently FAILs at its
`fkw-emit` probe section (a stale-cache / probe-carry check) *before* reaching its own §8–10 self-JIT row, so
the in-repo audit does not presently print this measurement end-to-end. This witness was produced by driving
the same emit cells directly over the `FOURTH_EMIT_CHAIN` (minimal-surface → hati-os-kernel →
host-io-fs-fkwu-emit → fkc-table-serialize → hati-os-kernel-emit), which emits, compiles, and runs cleanly.
The audit's earlier rows DID print and are corroborating: Form-emitted native fib28 1.7 ms vs Go-walker
9.0 ms / Rust 207 ms / TS 1286 ms.

**Reproduce:** emit `(fkc-emit-jit (list fibc))` over the emit chain above with `bin-go`, `clang -O2` the
emitted C, then `./fkjit <n>` (cold, walk-only) vs `./fkjit <n> <threshold>` (hot; line 1 = value, last line
= njit native-dispatch count).
