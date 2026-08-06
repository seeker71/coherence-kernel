# Decoder forward at REAL WIDTH (d_model=384) — bit-exact four-way

The bridge from toy-width proof (#82 at d_model=4) to real-model width. The whisper-tiny decoder
forward — embed+positional → masked-self + cross + FFN block (multi-head 6×64) → final LN → logits —
now crosses **bit-exact four-way at d_model=384 and ff=1536 (whisper-tiny's real widths)**.

Band: `model/tests/transformer-forward-d384-band.fk` (preludes: transformer-numerics, transformer-block,
transformer-mh, transformer-decoder, transformer-generate). Verdict **63** on all four arms.

## Is the forward bit-exact at d_model=384? Yes — verdict 63, four-way.

```
fkwu --src (preludes + band)  =>  63   (~15s)
go   walker                   =>  63
rust walker (big-stack)       =>  63
ts   walker (node)            =>  63
```

Six claims, each asserting bit-identity (IEEE-754 `eq`, the comparison shared by all four kernels)
against a reference double captured at full fp64 precision and agreed bit-for-bit by all four engines:

| bit | claim @ d=384 | reference double (fkwu = go = rust = ts) |
|-----|---------------|-------------------------------------------|
|  1  | matvec — head(Wq·x[0]), a 384-wide dot | `0.009653286400000004` |
|  2  | FFN — 384→1536→384 + gelu, head        | `0.3154508484374375` |
|  4  | attention/softmax — single-head attend, head | `0.01241902426620797` |
|  8  | decoder block — masked-self+cross+FFN, head | `5.057696003407669` |
| 16  | FULL forward — last hidden + final-LN (two probes) | `5.115357901486989`, `-1.727546116407077` |
| 32  | forward shape + argmax — len=64, argmax=63 | `64`, `63` |

`ff=1536` and `d_model=384` are full whisper-tiny widths (the "real width" claim). `seq=3` keeps the
end-to-end forward affordable for the tree-walkers; the proof is about width, not sequence length.
Weights are generated recipe-data (deterministic mkrow/mkmat/mkseq — no 384-wide hand literals), so the
input is reproducible on every engine.

## Computed, not parsed (perturbation-verified)

```
fkwu full band, c4 ref last digit 9->8  =>  55   (= 63 - 8, the decoder-block bit drops)
fkwu c1, ref last digit 4->5            =>  0    (was 1)
fkwu c2, ref last digit 5->6            =>  0    (was 2)
fkwu c56, LAST ref last digit 9->8      =>  32   (= 48 - 16, the forward bit drops)
```

A one-ULP change to any reference drops exactly that claim's bit. The doubles are computed at d=384,
not matched as text.

## Over the native form-asm-matvec lane on fkwu, or tree-walker-only? — the honest split

**Tree-walker fp64, four-way. NOT yet over the native form-asm-matvec bytes on fkwu.**

Investigation of `model/form-asm-matvec.fk` + `cognition/ll-buffer.fk`:

- The form-asm matvec/dot (`fam-dot-loop`, `fam-matvec`, `fam-ss-sqrt`, `fam-rsqrt`) emit **ARM64**
  (arm64 ABI: `ldr d`, `fmul`, `fadd`, the x0..x4 row/col loop). `ll-buffer.fk` is likewise ARM64
  (`fa-sub-x-imm`, `fa-str-w`). On **fkwu (Windows / x86_64)** those ARM64 bytes do not execute — the
  native byte-exec dylib lane (form-macho/recipe-dylib + codesign + dlopen) is Mac/arm64. The matvec
  recipe also depends on a global `append` that is undefined on this kernel (only `append-list` exists
  in form-asm.fk; `fam-matvec` references `append` 57×, defined 0×), so it does not even emit cleanly
  here — it is Mac-arm64-lane tissue.
- The x86_64 native lane that **does** execute on fkwu is `model/form-asm-x64.fk`, and it is
  **integer-only** — 20 encoders over RAX/RCX (`imul`, `add`, `sub`, `mov`), **zero** SSE/float ops
  (no `mulsd`/`addsd`/`xmm`/f64). So there is no native float matvec on this kernel yet.

Therefore the d=384 bit-exact forward proven here runs in the **tree-walker fp64 on all four arms**, not
over native asm-float bytes on fkwu. The form-asm matvec folds the SAME order as `tb-dot` by
construction (its header), so when the x86_64 lane grows SSE f64 ops and carries `fam-matvec`, it is
bit-exact-by-construction; **wiring an x86_64 f64 matvec through fkwu's f64 pool is the named next native
step.**

## A real bug found and resolved to root (not waved at)

The first runs segfaulted on fkwu and exploded the walkers' memory. Root cause was **not** a heap/stack
capacity wall — it was a **name collision**: the FFN biases were named `c1`/`c2`, the same as claim
functions `c1`/`c2`. The later claim `defn`s shadowed the bias `defn`s, so the FFN/decoder-layer received
a claim-function where a bias-vector belongs, corrupting the forward into a runaway structure. Renaming
the biases to `cz1`/`cz2` fixed it: the whole band then runs verdict-63 on fkwu in ~15s. Two design teeth
the band keeps: (1) the end-to-end claims share ONE forward through ARG-bound helpers (`ck-lh→ck-lf→ck-lg`)
— `let` mis-binds list values on the walkers, and recompute triples the d=384 forward; (2) bias names are
kept disjoint from claim names.

## What remains for the mind (named, not papered over)

- **(a) The native form-asm-matvec lane on fkwu.** An x86_64 f64 matvec (`mulsd`/`addsd` over fkwu's f64
  pool, the x86_64 twin of the ARM64 `fam-matvec`) so the d=384 forward runs over native asm-float bytes,
  not the tree-walker. ARM64 emits today; x86_64 float is the gap on this kernel.
- **(b) REAL WEIGHTS as recipe-data (3b — the big remaining).** This band proves real WIDTH on generated
  recipe-data weights. The next rung is a **real open base — Qwen/Llama — loaded as recipe-data** and run
  through the same width-agnostic Form block. Architecture + numerics + width are now four-way; real
  trained weights at real width is what turns this into a thinking native mind.

## Kernel sanity (C untouched)

```
native-vs-rented  =>  11111
(add 0.5 0.25)    =>  0.75
runtime/fkwu-uni.c: unmodified (git status clean)
```

Zero `.py`/`.sh` added — one `.fk` band + this `.md` receipt.
