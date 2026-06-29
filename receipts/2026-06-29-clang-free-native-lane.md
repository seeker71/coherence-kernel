# Receipt — a Form recipe `(add 40 2)` runs as native bytes with NO clang in the path (2026-06-29)

**What happened:** the trivial pure recipe `(add 40 2)` was lowered **Form → arm64 asm bytes → a runnable
native Mach-O executable** through the existing form-asm lane, then **ran**, returning **42** as its exit code —
with **clang, gcc, rustc absent from the entire build+run path**. This is a concrete advance on the standard
receipt's `c-bootstrap` / clang-free row: the kernel emits its own machine code; clang's compiler and assembler
are gone.

The lane is the one CLAUDE.md names: `form-asm` (instruction byte encoder) → `form-lower` (tree → instruction
sequence) → `form-macho` (wrap the bytes in a valid Mach-O `.o`). `ld` links the object; the binary runs.

## The recipe

The program is an op-tagged tree — the shape `form-lower.fk` lowers. `(add 40 2)` is:

```
prog = ( (1 40 0 0)    ; row 0: LIT 40
         (1  2 0 0)    ; row 1: LIT 2
         (3  0 1 0) )  ; row 2: ADD(row0, row1)   <- root index 2
(lo-compile prog 2)    ; lower root, append ret -> arm64 instruction bytes
(mo-object  ...)       ; wrap in a Mach-O object exporting _main
```

Tags: `1`=LIT (value in slot 1), `3`=ADD (children in slots 1,2). This is a real arithmetic lowering, not a
fixed program — `form-lower`'s `lo-add` selects `add w0,w0,#imm` from the tree.

## The build + run commands (every tool named)

```sh
ROOT=<Coherence-Network>; FORM="$ROOT/form"; GO="$FORM/form-kernel-go/bin-go"

# STEP 1 — Form recipe -> Mach-O bytes. bin-go RUNS the .fk recipe (the bootstrap
#          flattener/emitter); it emits NO compiler and contributes ZERO code to the program.
hex=$( "$GO" add40_2.fk | sed 's/^MACHO //' )      # 243 bytes of Mach-O object, as hex

# STEP 2 — hex -> bytes (xxd is a hex<->binary filter, not a compiler)
echo "$hex" | xxd -r -p > add.o                    # add.o: Mach-O 64-bit object arm64

# STEP 3 — ld (the linker, ld-1230.1, a standalone Mach-O linker) links the object.
#          NOT clang. ld spawns no clang (verified by process watch during the link).
SDK="$(xcrun --show-sdk-path)"
ld -arch arm64 -platform_version macos 13.0 13.0 -L"$SDK/usr/lib" -lSystem -o add add.o
                                                   # add: Mach-O 64-bit executable arm64

# STEP 4 — RUN. exit code = the recipe's value.
./add ; echo $?                                    # -> 42
```

## What Form emitted (otool disassembly of the linked binary)

```
_main:
  100002d8  mov  w0, #0x28      ; 40   (LIT 40)
  100002dc  add  w0, w0, #0x2   ; +2   (ADD imm)
  100002e0  ret                 ; return w0  -> exit code 42
```

The two arm64 instructions are exactly `(add 40 2)`. The Mach-O header is a real `MH_EXECUTE` arm64
(`magic 0xfeedfacf`, `cputype 16777228 = ARM64`), ad-hoc signed by `ld` (`codesign -dvv`: `Signature=adhoc`),
linking only `libSystem.B.dylib` (the syscall/C-ABI carrier — a host carrier, not a compiler).

## What ran clang-free vs. what is a named carrier

**Clang-free — verified, the result of this receipt:**
- No `clang`, `gcc`, `rustc`, or `cc` in the build or run path. `form_macho_demo.sh` references none;
  a process watch during the `ld` link saw no `clang` spawned. The program binary contains zero compiler output.
- The machine code is **Form-emitted bytes** (`form-asm` / `form-lower` / `form-macho`), byte-verified against
  the assembler through the encoder-conviction gate. Per `lowering-conviction.fk`, clang is an **oracle** the
  byte-identity gate drops — and here it is dropped: it is not in the path at all.

**Named carriers (allowed, not compilers):**
- `ld` — the Mach-O linker (`ld-1230.1`). Links the `.o` + `libSystem`; ad-hoc signs to meet the macOS arm64
  signing requirement. A linker, not a compiler. (`codesign` would be the same kind of host step; here `ld`'s
  ad-hoc signature already satisfied the loader, so no separate `codesign` invocation was needed.)
- `xxd` — hex↔binary filter.
- `libSystem.B.dylib` — the syscall/C-ABI host carrier.

## The remaining wall (precise — this is the next clang-free/sovereignty work)

This receipt closes the **clang** half of the `c-bootstrap` row: native bytes built with **no clang**. It does
**not** yet close the **c-bootstrap fkwu** half of the standard receipt. The precise remaining wall:

- **The recipe is RUN by `bin-go` (Go), not by the c-bootstrapped `fkwu`.** Per CLAUDE.md, `bin-go` is the
  legitimate **bootstrap** flattener/emitter — it runs the `.fk` and emits the Mach-O bytes — and it puts zero
  Go into the program. But the standard receipt's full bar is the recipe running on **c-bootstrap `fkwu` via
  form-cli, toolchain-free, on real metal**. The named step toward that: have `fkwu` itself drive
  `form-asm`/`form-lower`/`form-macho` to emit this `.o` (replacing `bin-go` as the emit host), so the entire
  chain — emit AND program — is c-bootstrap with no Go.
- **`ld` is a host linker carrier**, like the C ABI. Dropping it is a separate, deeper step (`form-macho`
  already emits a complete `.o`; a future `form-link` recipe wrapping the `.o` into a self-contained
  `MH_EXECUTE` with no external linker would close it). Today `ld` is named honestly as a carrier, not hidden.

So the honest floor: **`(add 40 2)` runs as native bytes, clang/gcc/rust-free, witnessed by exit code 42 and
`otool` disassembly.** The clang-free row is advanced and real. The `fkwu`-emits (drop `bin-go`) and
`form-link` (drop `ld`) steps are the named, tracked next walls — neither faked, neither hidden.

## Reproduce

```sh
cd <Coherence-Network> && bash scripts/form_macho_demo.sh   # ships ((40+1)-1)=40
```
The 42 variant above swaps the `prog` tree to `((1 40 0 0)(1 2 0 0)(3 0 1 0))` with root index 2; same lane,
same tools, exit code 42.

— machine: arm64 macOS (Darwin), `ld-1230.1`. clang/gcc/rustc: present on the host, **absent from this path**.
