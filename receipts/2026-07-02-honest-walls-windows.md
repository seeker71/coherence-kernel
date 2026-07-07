# Receipt — three honest walls on the Windows cell: the stack, the camera, the byte doors (2026-07-02)

Same cell (HP Spectre, Windows amd64, RTX 3050). Yesterday's depth-wall repairs patched recipes one at a
time around a silent killer; today the killer's root was found and the walls now speak.

## 1. The depth wall, healed at its root

The POSIX `main` has always run the walker on a big explicit thread stack (`FORM_KERNEL_STACK_MB`,
default **256MB**). The Windows `main` was a bare `return fk_run(...)` on the OS default **1MB** — that
asymmetry *was* the ~120-frame eval ceiling and the silent exit-127 deaths (three recipes in one day:
`tk-join`, `op-rows`, `re-zeros`). The Windows `main` now mirrors the POSIX law: `CreateThread` with the
same `FORM_KERNEL_STACK_MB` reservation. The documented one-`cc` build line is untouched.

Witnessed: non-tail recursion to depth **5000 now returns 5000** (died silently at ~120 before);
depth 4,000,000 dies with a **named wall** (`fk_vp: value stack overflow`), not silence. A second honest
wall rides in `fk_walk`: real stack use is measured against the reserve and the walker dies *saying so*
("the wall is honest, the silent crash was not") before the host stack can die mute. The recipe-side
discipline stands: deep recursion still belongs tail or balanced; yesterday's repairs remain right.

## 2. The camera, refusing honestly

`sense_cam_health` ran the legacy VfW connect inline and hung forever behind this cell's modern camera
stack (`2026-07-01-windows-camera-carrier-probe.md`). The probe now runs on a worker thread with a 3s
bound: witnessed `3.1s -> "sense: camera 0 connect timed out (legacy VfW shim) — health 0, honestly"`,
and `sense_report` completes (mic health 1, camera health 0). On timeout the probe struct and stuck
thread are deliberately abandoned — the named cost of a hung driver. The Media Foundation eye remains
the deliberate, separate movement.

## 3. The byte doors, binary-safe

`read_file` (63) and `read_file_slice` (62) opened text-mode on Windows: CRLF translation plus a `0x1A`
byte truncating the read — a binary checkpoint could not pass the Form body's own read door. Both now
open `_O_BINARY` (`O_RDBIN`, 0 on POSIX), matching the PTX read path and yesterday's write-door repair.
This door is load-bearing for the native-weights work that follows.

## Post-shrink re-witness

Upstream landed the **first C-seed shrink** (`7aa5ab5`: `substring`/`str_find`/`int_to_str`/`str_to_int`
retired into Form — `form/form-stdlib/core.fk` on the `str_byte_at` floor). With `core.fk` as prelude,
the whole witness set holds on this build: `42 / 55 / 11111`, CUDA fixture 3/3, `cuda_matvec_f32` 3/3,
self-JIT 55, and the form-cli ask — Paris, bit-exact 8/8, margin 7, refusal unchanged.
