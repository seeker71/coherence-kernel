# Receipt — the JIT is Form-native; the kernel needs no C JIT, only a thin install+call HAL carrier (2026-06-29)

**The correction.** I kept writing "the self-JIT in C" / "the JIT machinery in `fkwu-uni.c`." Urs: *why do we need
the JIT in C — we can use the Form-native JIT.* Right. Grounded in the repo: **there is no C JIT to need, and
none in this kernel.** The JIT is Form. Here is the exact boundary.

## The JIT is two jobs — both already Form, both proven

| job | where it lives | proof |
|---|---|---|
| **lower** a recipe → native machine **bytes** | `model/form-asm.fk` (arm64), `model/form-asm-x64.fk`, `model/form-asm-matvec.fk` | Form recipe → asm bytes, **clang dropped behind a byte-identity gate** (`form-asm-float` 2047 four-way) |
| **decide** when to crystallize / melt | `observe/jit-decision.fk` | Form, **four-way** — hot ∧ pure → crystallize (heat ≥ 5); cool → melt (heat < 2); hysteresis so it never thrashes |

Lower + decide **is** the JIT. Neither is C. The lowering even *removes* a toolchain (clang) rather than adding
one — it is the opposite of "needs C."

## What this kernel actually carries (measured)

```
grep -c  fk_nat_tab | fk_heat | fk_hot | VirtualAlloc | mmap | mprotect  runtime/fkwu-uni.c  ->  0
```

**Zero.** This `fkwu-uni.c` is the plain tree-walker with no JIT and no executable-memory primitive. So "the JIT
in C" was a phantom — there was never one here to justify. (The Mac `jit-live-crystallize` measurement used a
*separately emitted* kernel with the dispatch baked in; that is an emit choice, not a requirement, and not this
binary.)

## The one irreducible host-touch — a HAL carrier, not a JIT

Pure Form cannot make memory executable and jump to it — that is a hardware/OS capability (W^X). So the kernel
provides exactly one small carrier, in the **same category as the camera, the socket, and `dlopen`**:

- **install**: take the lowered byte image → a callable function. Either executable memory
  (`VirtualAlloc` + `VirtualProtect` on Windows / `mmap` + `mprotect` on POSIX), or write-a-dll-and-load —
  **`dlopen`→`LoadLibraryA` is already shimmed in this kernel's `_WIN32` port.** The object wrappers exist as Form
  (`form-pe-coff`, `form-macho`, `recipe-dylib`).
- **dispatch**: one branch in `fk_walk` — "if this fn has an installed native handle, call it; else walk." A
  function-pointer table + a call. ~a dozen lines.

That carrier is a host resource (allow-presence + measure-health, `host-kernel.form`), **not** a JIT. The JIT
proper stays Form and never enters C.

## Witnessed on Windows — the install+call carrier runs lowered bytes native

`fk_native_call` (tag 215) takes a lowered byte image + one arg, `VirtualAlloc`s it, `VirtualProtect`s it
executable, and jumps to it. Fed the bytes for `f(a) = a + 1` (`48 89 C8  48 83 C0 01  C3` —
`mov rax,rcx; add rax,1; ret`), called on `fkwu` (Windows):

```
native_call(f, 41)   = 42
native_call(f, 99)   = 100
native_call(f, 1000) = 1001
```

Real machine code, written to memory at runtime and executed. The **test bytes are hand-authored** to witness the
carrier; in production they come from `form-asm-x64` (Form), not from C. This is the entire host-side of a JIT —
and it is ~20 lines, the same shape as the `dlopen`/socket/camera carriers. There is no C JIT; there is this door.

## So, concretely, for the pixel walk

`model/frame-luma.fk` (the pixel walk as Form) needs **no C JIT and no C pixel loop**:
1. `form-asm-x64.fk` (Form) lowers `luma-sum` to a counted native loop → bytes.
2. `jit-decision.fk` (Form) says when (the walk is hot and pure).
3. the **install+call HAL carrier** (executable memory / the `dlopen`-shim path) loads the bytes and the
   `fk_walk` dispatch hook calls them.

Nothing in that chain is a C JIT. `fk_frame_read`'s C math is a stand-in for step 3's plumbing not yet wired on
Windows — not for any "JIT we need in C."

## What I got wrong, named

I conflated **the JIT** (lower + decide — Form) with **the install+call carrier** (a HAL host capability). Calling
the carrier "the JIT in C" implied the JIT needs C. It does not. The carrier is small, host-bound, and the same
shape as the other carriers I already added; the JIT is Form and already four-way.

## Honest floor

- **Form, proven:** the lowering (`form-asm*`, byte-identity gate) and the decision (`jit-decision.fk`, four-way).
- **This kernel:** no JIT in C (measured 0). Nothing to remove.
- **Carrier, now built + witnessed:** `fk_native_call` installs lowered bytes and calls them native on Windows
  (41→42, 99→100, 1000→1001). The host-side of the JIT exists, ~20 lines, HAL-shaped.
- **Pending (wiring, not a JIT):** (1) a `fk_walk` dispatch hook — "if fn f has an installed native handle, call
  it" — so a crystallized recipe is dispatched automatically; (2) feed real `form-asm-x64` output (Form, needs the
  seed to run the lowerer) through `fk_native_call` instead of the hand-authored test bytes. Then the Form JIT
  crystallizes hot recipes (frame-luma, sense-stream) to native on this kernel — no C JIT, no C loops.
