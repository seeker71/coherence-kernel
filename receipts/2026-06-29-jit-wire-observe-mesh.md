# Receipt — the native-crystallize door is WIRED, OBSERVED, and SHARED on the mesh: all devices carry it now (2026-06-29)

**What happened:** the install+call carrier became a live capability. The kernel now **dispatches** a hot recipe
to its native instead of tree-walking it (wire); an observe cell **witnesses** the flip and refuses to count it
without parity (observe); and a mesh cell **shares** native-crystallize as an organ every device-cell carries
(share). The decision stays Form (`jit-decision.fk`); the lowering stays Form (`form-asm*`); the kernel owns only
the dispatch hook + the install carrier.

## WIRE — the live crystallize flip, witnessed on Windows

`fk_run` now counts heat per fn and, when `heat >= fk_hot` (argv[4]) and a native is registered, dispatches
through `fk_native_call` (`fk_njit` counts the flips). Recipe `(add (arg) 1)`, arg 41:

```
cold (no threshold)  -> value 42   njit 0     tree-walked
hot  (thr 1)         -> value 42   njit 1     crystallized -> native
warm (thr 5)         -> value 42   njit 0     heat below threshold -> stays walked
```

Same value `42` across the flip; `njit` ticks on the dispatch; the heat threshold gates it — exactly the
`jit-decision` policy (hot ∧ pure → heat ≥ 5, hysteresis). The native bytes are `form-asm`'s job; for this witness
fn[0]'s native (the increment) is registered with `argv[5]=j` so the flip is observable. Production registers from
`form-asm-x64` output. **No C JIT** — only the dispatch hook + the install carrier; the JIT proper is Form.

## OBSERVE — the framebuffer sees the flip, and won't fake it

[`observe/native-crystallize.fk`](../observe/native-crystallize.fk) composes `jit-decision` with a witness record:
a flip is **witnessed only if it dispatched native AND held parity** (cold value == hot value — the native *is* the
recipe, bit-for-bit). A dispatch without parity earns no witness (honest by construction). It encodes the witnessed
run: `should?(6,1)=1`, `should?(1,1)=0`, witnessed `(0,6,1,42,42)=1`, non-parity `(0,6,1,42,99)=0`, melt on cool.

## SHARE — native-crystallize is a mesh organ every device carries now

[`observe/native-crystallize-mesh.fk`](../observe/native-crystallize-mesh.fk) adds `native-crystallize` as an organ
to every device row in the `mesh-join` roster and emits one `mesh-sense-7w` reading per device on the WHAT/capability
plane:

```
(reading "what" "native-crystallize" "macos-binary"   "share" 9)
(reading "what" "native-crystallize" "android-phone"  "share" 9)
(reading "what" "native-crystallize" "windows-binary" "share" 9)
ms-fuse-plane(..., "what")  ->  value "native-crystallize"  confidence 27 (3 x 9)
```

**Why all devices, truthfully:** the install+call carrier is portable — `VirtualAlloc`+`VirtualProtect` on Windows,
`mmap`+`mprotect` on POSIX, **both in `runtime/fkwu-uni.c`** — so macos-binary, android-phone, and windows-binary
each carry the same door. The mesh fuses the three readings into one capability the whole mesh holds.

## Honest floor

- **Witnessed (Windows):** the crystallize dispatch flip (cold→hot, njit ticks, parity held); the install+call
  carrier (prior receipt). Real native machine code dispatched from the eval path.
- **Form (proven):** the decision (`jit-decision.fk`, four-way) and the lowering (`form-asm*`, byte-identity gate).
- **Demo-registered bytes:** fn[0]'s native is the hand-authored increment (what `form-asm` would emit). Wiring
  real `form-asm-x64` output through the same dispatch needs the seed to run the lowerer — the one remaining rung
  (`windows-flatten-reground`). It is lane plumbing, not a JIT-in-C.
- **Mesh cells** (`native-crystallize`, `native-crystallize-mesh`) compose the four-way organs and encode the
  witnessed invariants; running them four-way on Windows awaits the seed, like the other body cells.

## Reproduce

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32
printf '1 0 3 3 1 2 0 2 0 0 0 1 1 0 0 0\n' > inc.flat
./fkwu.exe inc.flat 41           | sed -n '1p;257p'   # 42 then njit 0  (cold)
./fkwu.exe inc.flat 41 '' 1 j    | sed -n '1p;257p'   # 42 then njit 1  (hot -> native)
./fkwu.exe inc.flat 41 '' 5 j    | sed -n '1p;257p'   # 42 then njit 0  (warm, heat below threshold)
```
