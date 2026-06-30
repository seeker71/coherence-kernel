# Receipt — Form drives libvulkan on a real Adreno, no clang in the orchestration (2026-06-29 19:40 MDT)

**What happened:** the kernel grew two Form-facing HAL carriers — a symbol resolver and a C-ABI indirect call —
and with them a **Form recipe called the GPU's own Vulkan driver directly**, with **no clang in the
orchestration path**. This tears down the wall named in
[`2026-06-29-android-gpu-vulkan-PENDING.md`](2026-06-29-android-gpu-vulkan-PENDING.md): the dispatch no longer
needs a hand-written, NDK-clang-compiled C program. The recipe is the body; `libvulkan.so` is a named carrier,
exactly like `libSystem`.

## The carriers added to the one seed (`runtime/fkwu-uni.c` + `runtime/fkwu-optable.h`)

Same category as the dlopen / socket / camera / native-call carriers the seed already offers — the kernel opens
a door, the recipe is the body. No `mmap`/`W^X`/emitted bytes: these **call pointers that already exist** in the
host's shared libraries.

| Form op | tag | meaning |
|---|---|---|
| `(c_dlsym path name)` | 249 | resolve a function/data pointer from a shared lib (`dlopen`+`dlsym`) |
| `(c_call fnptr args)` | 250 | C-ABI indirect call, up to 8 integer/pointer args (covers every core Vulkan compute entry point) |
| `(c_alloc n)` / `(c_free p)` | 242/243 | a raw byte buffer the recipe owns (for VkStructs / out-params) |
| `(c_u32_get p off)` / `(c_u32_set p off v)` | 244/245 | 32-bit struct fields |
| `(c_u64_get p off)` / `(c_u64_set p off v)` | 246/247 | 64-bit handle/pointer struct fields |
| `(c_str_addr s)` | 248 | address of a Form string's bytes (entry name, SPIR-V image) |

Pointers ride the kernel's tagged-int lane (`addr<<1`); user-space addresses (<2^48) never collide with the
float/record/node tags (which are negative). One real bug found and fixed along the way: tags **240/241 are the
existing 2-/3-arg function-call node forms** — the first attempt collided with them and crashed; the carriers
were moved to 249/250. The seed's own self-proof is intact after the edit: `native-vs-rented-check` → `11111`.

## Witnessed — the FFI lane, two rungs

**Rung 1 — `abs(-42)` from Form (the `(add 40 2)=42` of this lane), no clang:**
```
(c_call (c_dlsym "libc.so" "abs") (list (sub 0 42)))   ->  42      [on the S23]
(c_call (c_dlsym "" "abs") (list (sub 0 42)))          ->  42      [on the Mac, libSystem]
```

**Rung 2 — Form calls libvulkan on the real GPU.** Recipe [`model/form-vulkan.fk`](../model/form-vulkan.fk),
run by the c-bootstrapped `fkwu` (cross-compiled for arm64 with NDK r27c — a C compiler on the seed, no
Go/Rust/clang-program), on the **Samsung Galaxy S23 Ultra (SM-S918U1), Adreno 740**:
```
(do (defn fv-instance-version (p)
      (do (c_call (c_dlsym "libvulkan.so" "vkEnumerateInstanceVersion") (list p))
          (c_u32_get p 0)))
    (fv-instance-version (c_alloc 4)))
->  4210688   =  0x00404000   =  Vulkan 1.4.0   (the Adreno/Android loader's max instance version)
```
The symbol resolved to a real address (`vkEnumerateInstanceVersion` → `510487837300`), the call ran on the
device, and the driver wrote the packed version into the Form-owned buffer, which Form read back. On the Mac
(no `libvulkan.so`) the same recipe returns `0` gracefully — `dlopen` fails, no crash. Nothing faked: the
number is what Qualcomm's driver returned.

## Meaning

The sovereign lane is **open**. The compute body was already Form-emitted SPIR-V (`form-glsl/fglsl-matvec`);
what was rented was the *dispatch orchestration*, and that is now Form too, talking to `libvulkan` through the
seed's FFI door. clang is gone from the orchestration exactly as it is gone from the compute body. The phone's
GPU driver answered a Form recipe.

## The next rung — DONE

`fv-matvec` in [`model/form-vulkan.fk`](../model/form-vulkan.fk) — the full compute dispatch as a Form recipe
(`vkCreateInstance` → physical device → `vkCreateDevice` → W/x/y buffers → descriptor set + compute pipeline
from the `fglsl-matvec` `.spv` → `vkCmdDispatch` → read `y`, every struct `c_alloc`+`c_u*_set`, every call
`c_call`) — now **runs bit-exact on the Adreno 740**: `(fv-matvec)` → `1111`, GPU `y = [40,87,21,57]` equal to
the CPU `fkwu` fold. No new kernel primitives were needed. See
[`2026-06-29-android-gpu-vulkan.md`](2026-06-29-android-gpu-vulkan.md) for the full witnessed dispatch.

— machine: arm64 macOS host; device Galaxy S23 Ultra (SM-S918U1, Adreno 740, Android 16). clang/Go/Rust:
**absent from the orchestration**; the only C is the one `fkwu` seed under one C compiler.
