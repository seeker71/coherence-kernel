# Receipt — a Form recipe computed bit-exact on a real Adreno GPU, no clang in the path (2026-06-29 20:10 MDT)

**What ran:** the matvec `y = W·x` — our four-way-proven recipe `form-glsl/fglsl-matvec` (the GLSL emitted from
Form, minted to SPIR-V, `precise`→`NoContraction`) — **dispatched on a real Adreno 740 entirely from Form**.
Not through a hand-written C program: the Form recipe [`model/form-vulkan.fk`](../model/form-vulkan.fk) makes
the ~20 `libvulkan` calls itself, through the kernel's FFI carriers. The compute body is Form-emitted SPIR-V;
the orchestration is Form; **`clang` / `gcc` / `rustc` are absent from the entire build-and-run path** of the
program that computed the result. `libvulkan.so` is a named host carrier, like `libSystem`.

**Where:** **Samsung Galaxy S23 Ultra (SM-S918U1)**, SoC SM8550 "kalama", Android 16
(`samsung/dm3quew/dm3q:16/BP4A.251205.006/S918U1UES7FZE3`), **Adreno 740**. The GPU identified itself *to the
Form recipe* — every field below was read by `fkwu` from the driver's own structs:

```
vendorID      = 0x5143  (20803, Qualcomm)
deviceID      = 0x43050a01
apiVersion    = 1.3.128   (0x402080)
driverVersion = 0x802a4001
```
(`driverVersion` is bit-identical to what the now-deleted rented C oracle once reported — the sovereign path
sees exactly what clang saw.)

## Witnessed result

Input (the four-way recipe's integers, written as f32 into GPU buffers by the recipe):
```
W = [[3,1,4,1,5],[9,2,6,5,3],[1,1,1,1,1],[0,3,1,4,1]]      x = [2,7,1,8,3]
```

| row | Adreno 740 GPU (f32 bits) | = | CPU `fkwu` fold |
|----:|--------------------------|---|-----------------|
| 0 | `1109393408` (0x42200000) | 40.0 | 40 |
| 1 | `1118699520` (0x42ae0000) | 87.0 | 87 |
| 2 | `1101529088` (0x41a80000) | 21.0 | 21 |
| 3 | `1113849856` (0x42640000) | 57.0 | 57 |

**`(fv-matvec)` → `1111`** — and the check is *independent*, not a comparison against baked answers. The recipe
holds the inputs as **integer lists** (`wlist`/`xlist`); it fills the GPU buffers from them via an in-Form
IEEE-754 encoder (`i2f`), and it verifies by **decoding each GPU output back to an integer** (`f2i`) and
comparing it to the matvec **recomputed fresh in Form** from the same integers (`dot` of the W-row with `x`).
Both sides are computed — GPU vs CPU on the same data — and `1111` means all four rows agreed. It is falsifiable:
corrupting only the GPU side (push-constant `cols` 5→4, so the shader folds a different matvec while the Form
reference stays whole) makes it return `0`. (Earlier I baked the expected f32 bits as constants — a weaker,
circular-looking check; this replaces it.)

## How — the sovereign dispatch

`model/form-vulkan.fk` `fv-matvec` runs the whole Vulkan compute sequence as Form:
`vkCreateInstance` → `vkEnumeratePhysicalDevices` → `vkCreateDevice`/`vkGetDeviceQueue` →
`vkGetPhysicalDeviceMemoryProperties` (the recipe scans memoryTypes for HOST_VISIBLE|HOST_COHERENT) →
create/alloc/bind/map W,x,y buffers → descriptor set layout/pool/set + `vkUpdateDescriptorSets` →
pipeline layout (push-constants rows,cols) → shader module from the `fglsl-matvec` `.spv`
([`native/vulkan/matvec.spv`](../native/vulkan/matvec.spv), sha256 `9113db32…`, `NoContraction` preserved) →
compute pipeline → command buffer (`vkCmdBindPipeline`/`BindDescriptorSets`/`PushConstants`/`Dispatch(1,1,1)`) →
`vkQueueSubmit` + `vkWaitForFences` → read `y`.

Each struct is built with `c_alloc`+`c_u32_set`/`c_u64_set` at fixed arena offsets; each call is `c_call` over a
`c_dlsym`'d `libvulkan` pointer. These FFI carriers were added to the one C seed for exactly this —
see [`2026-06-29-form-ffi-vulkan.md`](2026-06-29-form-ffi-vulkan.md).

## The honest seams (named, not hidden)

- **A rented clang carrier came first.** The earliest GPU run on this Adreno went through a hand-written,
  NDK-clang-compiled C program. That was the wrong lane for this body; it was **deleted** once `fv-matvec`
  worked. Its only lasting role was as an oracle for the target numbers — which `fv-matvec` now reproduces
  bit-for-bit, sovereignly.
- **Two real kernel constraints shaped the recipe, neither faked:** the source-runner binds user `defn`s of
  **≤2 args** (3+ silently yields 0) and **`let` does not bind** — so all state lives in one `c_alloc`'d arena
  threaded as the single arg, helpers are ≤2-arg, `do` sequences. Carriers take any arity (tag dispatch).
- **One real bug, and one of my own slips — kept distinct.** The *real* bug: the first full run gave row 1 = 89,
  not 87 — a wrong hard-coded f32 input constant (`9.0` written as `0x41200000`=10.0). That class of bug is now
  gone: inputs derive from integer lists via `i2f`, not hand-typed bit patterns. Separately, while debugging I
  *mis-read* y[0]=`0x42200000` as 38.0 in my head — it was always 40.0; my arithmetic slip, not a GPU error. The
  two are different and I conflated them earlier; this is the correction.

## What this is — and is NOT

- **General, reusable:** the FFI carrier kit in the seed (`c_dlsym`/`c_call`/`c_alloc`/`c_u32_*`/`c_u64_*`/
  `c_str_addr`). That is the actual capability — Form can now drive *any* host shared library by the C ABI.
- **NOT general Vulkan support:** `fv-matvec` is a **bespoke, single-shape (4×5) dispatch** — hand-laid arena
  offsets, fixed dimensions, one shader. It is a *witness* that the carrier kit can drive a real Vulkan compute
  pipeline end to end, not a reusable Vulkan binding. A general binding (arbitrary shapes, struct helpers, error
  checking on every `VkResult`) is unbuilt; this proves the lane is open, nothing more.

## Meaning

A matvec — the inner cell of inference — was emitted as data from `fkwu`, lowered to GPU bytes by the recipe's
own SPIR-V, and **executed on a real phone's GPU under the recipe's own orchestration, with no rented compiler
in the path**, its output decoded and checked against the same matvec recomputed in Form. One shape, one device —
but the sovereign compute lane is now demonstrably open to a third real GPU vendor (Apple → NVIDIA →
**Qualcomm/Adreno**), driven entirely from the body.

## Reproduce

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c                       # the one C seed
# emit GLSL from Form, mint SPIR-V (no spirv-opt — it re-fuses on Adreno)
( cat model/form-glsl.fk; printf '\n(fglsl-matvec "64")\n' ) | ./fkwu --src /dev/stdin > native/vulkan/matvec.comp
glslangValidator -V --target-env vulkan1.1 -S comp native/vulkan/matvec.comp -o native/vulkan/matvec.spv
# cross-compile fkwu for the phone (a C compiler on the seed — no Go/Rust/clang-program)
"$NDK/.../aarch64-linux-android34-clang" -O2 -o fkwu-android runtime/fkwu-uni.c -ldl
# push + run the SOVEREIGN dispatch on the device
adb push fkwu-android /data/local/tmp/formvk/fkwu
adb push model/form-vulkan.fk /data/local/tmp/formvk/form-vulkan.fk
adb push native/vulkan/matvec.spv /data/local/tmp/formvk/native/vulkan/matvec.spv
adb shell 'cd /data/local/tmp/formvk && ./fkwu --src form-vulkan.fk'    # -> 1111
```

— device Galaxy S23 Ultra (SM-S918U1, Adreno 740, Android 16). The only C is the one `fkwu` seed under one C
compiler; the GPU dispatch and the compute body are both Form. Nothing here is faked: `1111` was printed by
`fkwu` on the phone after the Adreno wrote `[40,87,21,57]` into a Form-owned buffer.
