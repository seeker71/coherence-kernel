# Receipt — the GPU seam is closed: fkwu EMITS the PTX too (native strings), then dispatches it bit-exact (2026-06-29)

The prior RTX receipt (`2026-06-29-windows-rtx-gpu.md`) had one honest seam: the dispatch was fkwu-native, but the
PTX *text* was realized through the Go proof-walker because `fkwu --src` had no strings. **That seam is now closed.**

## fkwu emits its own PTX, natively

`fkwu --src` now runs the `form-ptx` recipe itself and emits the PTX — no Go walker, no python:

```
( cat model/form-ptx.fk; echo '(print_str (fptx-matvec "form_matvec_f32"))' ) > emit.fk
./fkwu.exe --src emit.fk | sed '/^}$/q' > gpu/fptx-matvec.ptx     # 1515 bytes, .visible .entry form_matvec_f32
```

This works because the `--src` source-runner is **data-driven** (`fkwu-optable.h`, generated from the same flt-ops
manifest the flattener reads) and carries the string ops (`str_concat`, `print_str`, …) plus the `"..."` string
literal — the sibling's "ops are data" refactor. `form-ptx`'s `fptx-matvec` is just `defn` + `str_concat` over
string literals, so the c-bootstrapped kernel evaluates it and prints the exact recipe bytes. (The earlier
Go-walker golden differed by one trailing newline the walker's print added; fkwu's emission is the exact recipe
string.)

## The full loop, all fkwu, all native (RTX 4070)

```
1) fkwu --src model/form-ptx.fk + (print_str ...)   ->  emits gpu/fptx-matvec.ptx        (NATIVE emit, no Go)
2) fkwu cuda_matvec (tag 232, nvcuda.dll driver API) ->  JITs it -O0, dispatches on RTX:
     row 0: GPU=0x3ebdddd6 == CPU=0x3ebdddd6   BIT-EXACT
     row 1: GPU=0x3e303698 == CPU=0x3e303698   BIT-EXACT
     row 2: GPU=0x3f622f99 == CPU=0x3f622f99   BIT-EXACT
     AGREEMENT: 3/3  ALL-BIT-EXACT=true
```

Emit AND dispatch are now the c-bootstrapped kernel's own work. **No Go, no Rust, no python, no nvcc, no nvrtc** —
the Form lane emits the PTX, fkwu's `nvcuda.dll` carrier runs it on the RTX, and it matches the CPU right-fold to
the bit. The standard-receipt rung: `c-bootstrap fkwu` + Form recipe + real RTX metal, toolchain-free.

## Honest floor

`gpu/fptx-matvec.ptx` is committed as the deterministic golden (regenerable by the command above). The same string
lane now opens the rest of `form-ptx` (affine-train, f16/bf16, gelu, ffn, softmax, attention) and the broader
form-cli string surface to `fkwu --src`; their at-width GPU runs ride the same closed loop.
