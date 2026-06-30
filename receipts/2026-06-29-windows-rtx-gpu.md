# Receipt — Form kernel runs BIT-EXACT on a real Windows RTX GPU (CUDA driver API, no python/nvcc) (2026-06-29)

The Form-native GPU lane is no longer "emits a shader": a four-way-proven recipe, lowered to PTX by the Form lane,
runs on **real RTX silicon** and matches the CPU result **to the last bit** — dispatched by **fkwu's own host
carrier**, not Python.

## What was observed (on metal)

```
GPU:    NVIDIA GeForce RTX 4070   (driver 591.86, sm_89 Ada)
path:   CUDA driver API via nvcuda.dll  ->  cuModuleLoadDataEx (PTX-JIT) @ CU_JIT_OPTIMIZATION_LEVEL=0
        NO python, NO nvcc, NO nvrtc, NO CUDA toolkit  (fkwu loads nvcuda.dll itself, tag 232 cuda_matvec)
recipe: form-ptx  fptx-matvec  ->  .visible .entry form_matvec_f32   (four-way-proven PTX emitter)
kernel: f32 matvec, one thread per row, downward right-fold (j: cols-1..0), explicit mul.f32 + add.f32

input:  W (3x4) = [[0.1,0.2,0.3,0.7],[0.11,0.13,0.17,0.19],[0.9,0.8,0.123456,0.654321]]
        x (4)   = [0.5,0.25,0.125,0.333333]

  row 0:  GPU=0.370833099 (0x3ebdddd6)   CPU=0.370833099 (0x3ebdddd6)   BIT-EXACT
  row 1:  GPU=0.172083259 (0x3e303698)   CPU=0.172083259 (0x3e303698)   BIT-EXACT
  row 2:  GPU=0.883538783 (0x3f622f99)   CPU=0.883538783 (0x3f622f99)   BIT-EXACT

AGREEMENT: 3/3   ALL-BIT-EXACT = true
```

## Why bit-exact (the oracle-as-teacher discipline)

The driver's built-in PTX JIT at `-O0` keeps `mul.f32` then `add.f32` as **two separate roundings** (no FMA
contraction). The CPU reference computes the **same downward right-fold** with `volatile` intermediates so gcc
`-O2` cannot fuse it to FMA either — both sides are two roundings, same order, so they agree to the bit. This is
not a tolerance; it is byte-identity. (A driver default-opt run would fuse to FMA — a *named, understood*
difference; here we pin `-O0` and get exactness, which is the stronger claim.)

## What is fkwu-native here, and the one honest seam

- **Dispatch — fully fkwu-native.** `runtime/fkwu-uni.c` `fk_cuda_matvec` (tag 232) `LoadLibraryA("nvcuda.dll")`
  and drives `cuInit / cuCtxCreate / cuModuleLoadDataEx / cuModuleGetFunction / cuMemAlloc / cuMemcpyHtoD /
  cuLaunchKernel / cuMemcpyDtoH` directly — the CUDA twin of the existing `fk_metal_matvec_f32_native`. The driver
  is the carrier (intrinsic to the GPU), exactly as the OS is the carrier for camera/sockets/HTTPS. **No python.**
- **PTX emission — the Form lane (`form-ptx`).** `gpu/fptx-matvec.ptx` is the output of `fptx-matvec`, byte-identical
  to the canonical template the four-way `form-ptx-band` gates (the recipe IS the body). It is regenerable from
  `model/form-ptx.fk`. Today fkwu can't self-emit it because `fkwu --src` has no strings yet (the same next-surface
  gap as the rest of form-cli) — so the text is realized through the Form lane via the proof-walker. When strings
  land on `--src`, fkwu emits its own PTX too; the **dispatch + verification shown here is already 100% native.**

## Reproduce

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp
printf '1 0 1 232 0 0 0\n' > c.flat
./fkwu.exe c.flat            # reads gpu/fptx-matvec.ptx, dispatches on the RTX, prints 3/3 BIT-EXACT
```

## Honest floor

This is the **integer-clean / unfused-f32** lane bit-exact on real RTX hardware. The Vulkan/SPIR-V path
(`form-glsl`) and larger kernels (`gpu-ffn-forward`, attention) ride the same carrier; their at-width GPU runs are
the next witnesses. RTX model, recipe, path, inputs, and N/N agreement are all captured above — nothing faked.
