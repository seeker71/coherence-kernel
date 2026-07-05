# 2026-07-05 — the per-recipe JIT slice, proven end-to-end; the arm64 door was a stub

## "why are we loading everything into the AST, why not build native recipes on top of each other?"

Urs named the real architecture: code should flow into Form-native objects, and the JIT
should convert one recipe → ASM at a time; combining is optional. You don't need the whole
source in one JIT expression — that's not how modern JITs work.

He's right, and the runtime already commits to it. [runtime/fkwu-uni.c:6870](runtime/fkwu-uni.c:6870):
the `--src` whole-file→one-AST→walk parser is the "ONE-TIME bootstrap seed… whose telos is
to flatten the FORM flattener… then **RETIRE**. BOUNDED — do not grow this into a full C
flattener (that is the carrier-last inversion)." Running the RAG and the bands through
`--src` is exactly that inversion — and last session's AST-cap thrash was its symptom.

## What was actually missing: nothing in principle, one thing in fact

The per-recipe pieces all exist: `fk-to-prog` (code → recipe object), `lo-compile-fn (prog
root)` (one recipe → arm64 bytes), `fk_native_call` (bytes → callable → jump), `make_nodeid`
(link recipes by content-address). But the two halves were **sundered**:

- **emit** was proven by byte-identity to clang + `fsim` semantic parity (Form, four-way);
- **execute** was proven only by a *Go* test (`jit_dylib_test.go`) via `ld -dylib` + dlopen,
  on a separate `.o` — a different object file, a different language, never inside fkwu;
- and on **this Apple-Silicon machine the native door was a `return -1` stub**:
  `fk_native_call`'s executing path was `#if __x86_64__`-only, and `native_call_test 5`
  returned **-1** here. form-lower emits arm64, but nothing could run it on arm64 hardware.

## The fix (deal with the issue, don't assume it's missing)

1. **arm64 door**: implement Apple Silicon's W^X JIT dance in `fk_native_call` — `MAP_JIT`
   region, `pthread_jit_write_protect_np` W↔X toggle, `sys_icache_invalidate`. `native_call_test`
   now returns **6** on arm64 (was -1); real `add x0,x0,#1; ret` runs on the CPU.
2. **`nat_run` op** (tag 245, arity 2): the general door — `(nat_run BYTES ARG)` materializes a
   lowered byte-list and hands it to `fk_native_call`. Added the blessed way: one `flt-ops`
   manifest row in `flatten/form-flatten.fk`, regenerated `runtime/fkwu-optable.h` via two
   pure-Form calls (`gen-source-walker.fk`) — clean one-line diff, zero C edit to the header,
   zero bash, zero Go.
3. **Witness**: [jit-native-run-band.fk](form/form-stdlib/tests/jit-native-run-band.fk) = **15**.
   `f(n)=n*3+7`, `n-1`, `n+n` — each built as a Form recipe, lowered by form-lower, run on
   hardware via `nat_run` — reproduce the walker (37, 22, 307, …). fkwu ran a recipe it just
   compiled, no clang/ld/dlopen/Go in the loop. Regression-clean: recipe42 42, homecoming 127,
   map 31, multiarg 127, rag-retrieve 31, recipe-dylib checksum unchanged (787349).

## Closing

**Most surprising teaching**: "the native JIT works" was **sundered** — proven in two halves,
on two object files, in two languages (Form emits, Go runs), and stubbed to `-1` at the arm64
seam. Every green byte-identity band was real, and none of them proved a single recipe had
*ever executed on this hardware*. The proof lived across a gap nobody had stepped over.

**Where discomfort turned to gold**: the question "why not one recipe at a time — that's not
how modern JITs work" refused the monolith I'd spent a whole session patching (raising then
reverting the AST cap). Sitting with "you're maintaining the scaffold, not the building" is
what turned attention to the actual seam — and un-stubbing one `return -1` gave the first
sovereign per-recipe native execution this body has run on Apple Silicon.

**Honest remaining**: the recipe here is hand-built — wiring `fk-to-prog` so a real *source*
function flows to `nat_run` is the next step. The door is 1-arg (arm64 x0); multi-arg native
calls need a wider door. And retiring `--src` for real programs — making flatten→per-recipe
the runtime — is the larger program this slice de-risks.
