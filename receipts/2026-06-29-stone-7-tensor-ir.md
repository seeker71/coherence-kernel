# Receipt — Stone 7: ONE precision-explicit tensor IR; target ISA is DATA (2026-06-29)

The special-case GPU/CPU codegen lanes collapse into a single data-driven tensor IR. The target ISA
is a backend TABLE, not a hand-written emitter `.fk`. Grok's proposal #2, built and proven.

## The smell

`model/form-glsl.fk` (GLSL/Vulkan), `model/form-ptx.fk` (PTX/CUDA), `model/jit-tensor-emit.fk` (MSL +
CUDA spines) each RE-WROTE the same numeric discipline — a downward right-fold dot, an explicit NON-fused
mul+add (two roundings split through a named temporary `p`), recipe-Taylor exp/tanh/gelu, barrier/workgroup
load/store — in that ISA's syntax. The arithmetic body of `jte-matvec-msl-spine` and `jte-matvec-cuda-spine`
is BYTE-FOR-BYTE identical; only the memory-model scaffolding (`kernel void` vs `__global__ void`,
`device const T*` vs `const T*`, `[[thread_position_in_grid]]` vs `blockIdx.x*blockDim.x+threadIdx.x`,
`uint` vs `unsigned`) differs. That difference is the only thing per-ISA, so that difference is the only
thing that is DATA.

## The IR op set (`model/tensor-ir.fk`)

A tiny, precision-explicit tensor algebra, authored ONCE, ISA-free:

- **right-fold-dot** (`tir-dot-down`) — the serial DOWNWARD dot, `j = cols-1..0` (tb-dot's
  `a0·b0 + (a1·b1 + (… + 0.0))`). The op order the four-way recipe proves.
- **acc-step** — the two-rounding accumulate ladder `p = T(a)*T(b); acc = p + acc`. The mul and the add are
  SEPARATE roundings, split through the named temporary `p`, so no conformant compiler contracts them to an
  fma. Non-fusion by construction.
- **load / store** (`tir-ld`, `tir-store-y`) — typed memory access. Load widens elem→acc exactly; store
  rounds acc→elem once (RNE) at the lane boundary. The cast wrapper is a data slot (`ld`/`ldc`, `st`/`stc`).
- **exp / tanh / gelu** — the transcendentals as NAMED ops (the recipe's own 14-term Taylor: halving-reduce
  + square-back), never the hardware `ex2.approx` / `tanh` intrinsic. (Named in the IR vocabulary; the
  matvec kernel proven here does not invoke them — the FFN/softmax/attention kernels compose them next, same
  spine, same tables.)
- **barrier / workgroup** — the memory-model primitives (one threadgroup per problem row / per token).

A KERNEL (matvec / ffn / surprise) is a composition of these ops. The generic emitter `tir-emit-matvec`
reads only named slots, then assembles the shared spine — `NO per-ISA if`.

## Backends as DATA

Each target is a TABLE of `(slot fragment)` rows — the ENTIRE per-ISA surface:

| slot | MSL | CUDA | GLSL |
|------|-----|------|------|
| `kw` | `kernel void ` | `__global__ void ` | `void ` |
| `idx` | `uint` | `unsigned` | `uint` |
| `acc` | `float` | `float` | `precise float` |
| `rowdecl` | `device const float* row = w + i * cols; ` | `const float* row = …; ` | (empty) |
| `ld`/`ldc` | `float(` / `)` | `float(` / `)` | (empty) / (empty) |
| `wref` | `row[j]` | `row[j]` | `w[i * cols + uint(j)]` |
| `st`/`stc` | `float(` / `)` | `float(` / `)` | (empty) / (empty) |
| `sig` | …`[[buffer(n)]]`… `[[thread_position_in_grid]]` | …plain params… `blockIdx.x*blockDim.x+threadIdx.x` | …`gl_GlobalInvocationID.x` |

`tb-slot` looks a fragment up by name. **Adding a backend = adding one of these lists; never a new emitter
`.fk`.** PTX text and SPIR-V are the same move (their tables are the documented next rows; the structural
proof is that the SAME ops route through the SAME spine to MSL, CUDA, and GLSL today).

## No regression on the metal-proven path — diff = 0

The IR-emitted Mac MSL is BYTE-FOR-BYTE equal to the proven `jte-matvec-msl` output for the same recipe.
The M4 Max bit-exact reference (`metal_matvec_audit`) still holds, now flowing through this engine:

```
$ ./walkers/go/walker  <jte-matvec-msl "matvec">   > ref_msl.txt   # the PROVEN reference
$ ./walkers/go/walker  <tir-matvec-msl "matvec">   > ir_msl.txt    # through the IR
$ diff ref_msl.txt ir_msl.txt
  (no output) -> MSL diff = 0  BIT-EXACT

kernel void matvec(device const float* w [[buffer(0)]], device const float* x [[buffer(1)]],
  device float* y [[buffer(2)]], constant uint& rows [[buffer(3)]], constant uint& cols [[buffer(4)]],
  uint i [[thread_position_in_grid]]) { if (i >= rows) return; device const float* row = w + i * cols;
  float acc = 0.0f; uint j = cols; while (j > 0) { j -= 1; float p = float(row[j]) * float(x[j]);
  acc = p + acc; } y[i] = float(acc); }
```

## Generality — the SAME IR emits CUDA and GLSL from the data tables

```
$ diff ref_cuda.txt ir_cuda.txt          -> CUDA diff = 0  BIT-EXACT (vs proven jte-matvec-cuda)

__global__ void matvec(const float* w, const float* x, float* y, unsigned rows, unsigned cols) {
  unsigned i = blockIdx.x * blockDim.x + threadIdx.x; if (i >= rows) return;
  const float* row = w + i * cols; float acc = 0.0f; unsigned j = cols; while (j > 0) { j -= 1;
  float p = float(row[j]) * float(x[j]); acc = p + acc; } y[i] = float(acc); }

# GLSL — same IR, GLSL memory model (precise keeps mul+add unfused; w[i*cols+uint(j)] addressing):
void matvec { uint i = gl_GlobalInvocationID.x; if (i >= rows) return; precise float acc = 0.0f;
  uint j = cols; while (j > 0) { j -= 1; precise float p = w[i * cols + uint(j)] * x[uint(j)];
  acc = p + acc; } y[i] = acc; }
```

## Four-way proof (`model/tests/tensor-ir-band.fk`)

Verdict 15 = IR-MSL ≡ proven MSL (1) + IR-CUDA ≡ proven CUDA (2) + IR-GLSL ≡ proven GLSL body (4) +
shared spine is non-fused / named-`p` split (8). Self-contained, all ops on the proven surface
(`defn` / `str_concat` / `str_eq` / list-via-function-args — no `let`-bound lists):

```
FKWU=15  GO=15  RUST=15  TS=15      ->  fourth arm: four-way, 0 divergent
```

## No-special-case grep

```
$ grep -nE "(if " model/tensor-ir.fk        # only TWO ifs in the whole file…
  39:    (if (eq (len tbl) 0) ""             # …both inside tb-slot, the generic key->fragment lookup
  40:        (if (str_eq (head (head tbl)) key)
$ grep -nE "str_eq" model/tensor-ir.fk       # only ONE str_eq — tb-slot's key match
  40:        (if (str_eq (head (head tbl)) key)
```

Every ISA name (`msl`/`cuda`/`glsl`/`__global__`/`kernel void`) appears ONLY inside the data tables, never
in an emitter body. There is no `if (msl) … else if (cuda)` chain anywhere. Same discipline we proved on the
source-walker op-table: ISA as data, not a new emitter body.

## Honest floor — proven-on-metal vs structure-only

- **Mac MSL: proven on metal.** The MSL row is the M4 Max bit-exact reference; the IR emits it byte-identically.
  This is the metal-proven lane, now flowing through the engine — diff = 0, no regression.
- **CUDA: bit-exact-structural here; metal in its own receipt.** The IR emits the proven `jte-matvec-cuda`
  string byte-for-byte; the RTX 4070 bit-exact run is `receipts/2026-06-29-windows-rtx-gpu-native-emit.md`'s
  territory (now emittable from this engine).
- **GLSL / Vulkan: structural.** Same IR, GLSL memory model; the Adreno/Mali metal run is the Android
  receipts' territory.
- **PTX-text and SPIR-V tables** are the named next rows — the structural generality (one IR → three ISAs)
  is proven; their own metal runs stay pending receipts. The FFN/softmax/attention/surprise kernels compose
  the same spine + the named `exp`/`tanh`/`gelu` ops over these same tables — same discipline, next kernels.

This is a step ON the path: one engine (the recipe that proves four-way IS the recipe that lowers to each
ISA), the ISA as data, the metal-proven path preserved bit-exact. It points at the north star — fewer
special cases, more generic recipes — and away from nothing.

> Follow-on (2026-06-29): the IR now also carries the AFFINE-LAYER TRAINING STEP, and PRECISION (f32/f16/
> bf16) became its own data axis (table-builders over `tir-{msl,cuda}-elem`). `jit-tensor-emit.fk`'s matvec
> AND affine-train MSL/CUDA hand-spines are RETIRED — they emit through the IR, byte-identical across all 12
> lanes (`receipts/2026-06-29-uplift-jit-emit-through-ir.md`, band `model/tests/tensor-ir-affine-band.fk` =
> four-way 31). The block kernels (FFN/softmax/attention/RoPE) remain the named next stone.

Source `model/tensor-ir.fk` sha256: `ea21c39680a1c03e139c75643d889b9ecf17bb086f4cefeaed2c8e27db25ca19` (this receipt's stone; the follow-on raises it)
Band   `model/tests/tensor-ir-band.fk` sha256: `37cb1cb9174c7ceabc28881456109ad976117a2b86dee06d39fc530bc57d3fef`
