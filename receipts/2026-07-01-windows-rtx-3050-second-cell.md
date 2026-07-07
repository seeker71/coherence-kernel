# Receipt — second Windows RTX cell grounds and runs the GPU lane bit-exact; the sm_89 pin is lifted to the honest sm_80 floor (2026-07-01)

A new cell arrived: an **HP Spectre laptop, NVIDIA GeForce RTX 3050 Laptop GPU (sm_86 Ampere, driver 581.83)**,
Windows amd64, hybrid graphics (Intel Iris Xe + RTX). This receipt witnesses the checkout grounding and the
Form-native GPU lane on that metal — including the honest miss found on the way, and the Form-lane repair.

## Grounding (fresh worktree, this cell)

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32 -lwlanapi -lbthprops -lwinhttp
./fkwu.exe --src bootstrap/ground.fk                 -> 42
./fkwu.exe --src bootstrap/ground-recursive.fk 10    -> 55
observe/native-vs-rented.fk + (native-vs-rented-check) -> 11111
```

All three checkout witnesses pass. (MinGW-W64 gcc 12.2.0; the known malloc-prototype LLP64 warnings, no errors.)

## The honest miss — the recipe carried the old cell's silicon

The native PTX emit stood immediately: `fkwu --src` over `model/form-ptx.fk` reproduced `gpu/fptx-matvec.ptx`
**byte-identical** to the committed golden. But the dispatch witness (tag 232, `nvcuda.dll` driver API) failed:

```
cuda: PTX JIT failed    (-7, cuModuleLoadDataEx)
```

Cause: every `form-ptx` emitter pinned `.target sm_89` — the Ada architecture of the first RTX cell
(`2026-06-29-windows-rtx-gpu.md`, RTX 4070). The driver's PTX JIT refuses PTX targeted *above* the device
architecture, and this cell is `sm_86`. The kernel text had the previous machine's silicon baked in.

## The repair — Form lane only, no C-seed growth

`model/form-ptx.fk`: all 13 emitters move `.target sm_89` -> **`.target sm_80`**. `sm_80` is the honest floor,
not an arbitrary pick: the `fptx-matvec-bf16` lane's `cvt.f32.bf16` / `cvt.rn.bf16.f32` require `sm_80+`, and PTX
is forward-compatible — `.target sm_80` JITs on every Ampere/Ada/Blackwell device, including the first cell's
RTX 4070. The golden was regenerated **through fkwu's own native emit** (the closed loop of
`2026-06-29-windows-rtx-gpu-native-emit.md`), still 1515 bytes:

```
( cat model/form-ptx.fk; echo '(print_str (fptx-matvec "form_matvec_f32"))' ) > emit.fk
./fkwu.exe --src emit.fk | sed '/^}$/q' > gpu/fptx-matvec.ptx
```

`runtime/fkwu-uni.c` is untouched. The carrier was never the problem; it is architecture-agnostic by design.

## What was observed (on this metal)

```
GPU: NVIDIA GeForce RTX 3050 Laptop GPU  CUDA driver-API PTX-JIT -O0 (nvcuda.dll; no python, no nvcc, no nvrtc)
recipe: form-ptx fptx-matvec -> form_matvec_f32 (f32 matvec, downward right-fold, 2 roundings)
  row 0: GPU=0.370833099 (0x3ebdddd6)  CPU=0.370833099 (0x3ebdddd6)  BIT-EXACT
  row 1: GPU=0.172083259 (0x3e303698)  CPU=0.172083259 (0x3e303698)  BIT-EXACT
  row 2: GPU=0.883538783 (0x3f622f99)  CPU=0.883538783 (0x3f622f99)  BIT-EXACT
AGREEMENT: 3/3  ALL-BIT-EXACT=true
```

The three hex words are **identical to the RTX 4070 receipt**. Two different GPU generations (Ada `sm_89`,
Ampere `sm_86`), two different machines, one Form recipe — the same bits. The unfused `-O0` two-rounding
discipline is not machine-luck; it now has a cross-silicon witness.

## The CPU self-JIT also witnesses on this cell

Windows amd64 is the one target row that claims `fkwu-self-jit-no-runtime-toolchain`
(`form/form-stdlib/hati-os-targets.fk`), and the in-process crystallize-on-heat JIT is x86-64-only — the
merge-checkpoint receipt records it returning `nothing` on the Mac. On this cell it runs:

```
FK_JIT=1 FK_JIT_WITNESS=1 ./fkwu.exe --src bootstrap/ground-recursive.fk 10
  [jit] fn1 crystallized in-process: 547 bytes, njit=1 (native dispatch)
  55                                       (bit-identical to the walker)
FK_JIT_SCAN=1: lowered=1 bailed=0 total=1
```

So this cell witnesses **both** JIT-on-metal lanes: the CPU self-JIT (W^X `VirtualAlloc`/`VirtualProtect`,
in-process x64 emission) and the GPU driver PTX-JIT (`nvcuda.dll`, `-O0`, bit-exact).

## Honest floor

- This witnesses the **matvec** golden's emit+dispatch loop on the second cell. The other 12 `form-ptx`
  kernels (ffn, gelu, softmax, attention, layernorm, rmsnorm, f16/bf16, …) carry the same `sm_80` floor but
  their at-width GPU runs on this cell are not yet individually witnessed here.
- The tensor-IR CUDA lane (`tir-matvec-cuda-fmt`) emits through its own table and carries no `sm_` pin.
- Hybrid-graphics note: `cuDeviceGet(0)` selects the RTX directly; the Iris Xe never enters the path.
