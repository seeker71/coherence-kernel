# 2026-07-05 -- Source compiler BMF stall

## Context

While closing the stdlib grammar gap, the first implementation tried to make the
new BMF grammar witnesses depend directly on `source-compiler.fk` lowering real
`.bmf` section files.

## Observed Failure

`stdlib-uplift-bmf-use-band.fk` and `stdlib-section-pressure-band.fk` first
failed when `source-compiler.fk` reached `FORM-CATEGORY-TABLE` without the
ontology loader prelude.

After adding the loader prelude, Go and Rust failed through
`form-ontology-loader.fk:bp` on an unbound `nothing` path. TypeScript either
segfaulted on the uplift band or returned a different result on the pressure
band, so the kernels disagreed.

A comparison run of the existing source-compiler multibyte band:

```text
cd form && ./validate.sh form-stdlib/tests/source-compile-multibyte-band.fk
```

was still silent after roughly 90 seconds and was interrupted. This was treated
as a stall signal, not ignored.

## Decision

The new stdlib grammar witnesses no longer depend on the broad source-compiler
sidecar path. They now prove the narrower live claim:

- real `.bmf` files use the repo's `section [name.bmf] { rule ::= ... }` source
  shape;
- code observes those BMF files directly;
- reversible BMF object rules use examples from those grammars;
- focused bands prove source -> object -> source -> object roundtrips.

The generic `.bmf` text-to-runtime-rule compiler/sidecar path remains a named
repair, not a claimed completion.

## 2026-07-05 Bounded Reprobe

After lifting the reviewed bootstrap `bp` rows for the engine/source-compiler
load contract, the dedicated health band passed, and the focused source scanner
band passed. The multibyte source-compile band still did not complete within a
bounded window:

```text
cd form && perl -MPOSIX=:sys_wait_h -e '...' 45 ./validate.sh form-stdlib/tests/source-compile-multibyte-band.fk
```

Result: exit `124` after 45 seconds. The watchdog terminated the validation
process group; this was a bounded stall, not an OOM kill. The repair is still
open and now belongs to the source-compiler health surface rather than to an
unbounded manual run.

## 2026-07-05 Root Cause

The stall reduced to a one-row multibyte source-compiler probe.

With `core.fk` loaded before `source-compiler.fk`, the sibling kernels diverged:

```text
fsc-compile-section "form.bml" one multibyte def:
  go/rust -> 61494
  ts      -> 61

form-source-compile-text one multibyte section:
  go      -> fatal error: stack overflow
  rust    -> 122934
  ts      -> 61
```

Without `core.fk`, the same Go/Rust probes returned the sane `69`, and
`fkwu --src` with `core.fk` also returned `69`. That named the actual seam:
Go/Rust allowed same-named Form bindings from `core.fk` to shadow regular
substrate natives. The source compiler then routed `substring`/`char_at`
through the portable fallback string wrappers instead of through the native
byte-indexed waist. On multibyte text, that fallback path widened non-ASCII
bytes through `byte_to_str`, inflating one row into tens of thousands of bytes.
The file-level source compiler then fed that inflated output into right-recursive
string builders and Go hit host stack overflow.

TypeScript did not have the same native-shadowing bug, but its string natives
still counted UTF-16 code units. That let the high-level band pass while the
direct length/offset probes disagreed with Go/Rust/fkwu.

This was not an OOM. It was not caused by `head` alone. `head`/line-style code
made the source compiler fragile, but the immediate stall was primitive dispatch
and string-index drift underneath that code.

## 2026-07-05 Repair

The first repair made regular native dispatch win over same-named Form fallback
bindings in the Go and Rust proof kernels. That fixed the stall but was too
broad: it let obsolete host natives change unrelated Form-level calls such as
`intern_node`. The repair is now narrowed to the byte-string/cursor waist only:

- `str_len`
- `str_byte_at`
- `byte_to_str`
- `substring`
- `char_at`
- `str_find`
- `scan_run`

Those names stay native when same-named portable fallback definitions are
loaded, because source compilers and BMF cursors require byte-indexed behavior.
Other same-named Form bindings remain ordinary user definitions.

TS string natives now use UTF-8 byte-index semantics for:

- `str_len`
- `substring`
- `char_at`
- `str_find`
- `scan_run`

`core.fk` now describes its string definitions as fallback shape, not as the
active path when a substrate native exists.

Direct reprobes after the repair:

```text
byte waist:
  go/rust/ts -> [2, 195, 164, 2, 2, 0, 2, 0]

fsc-q multibyte:
  go/rust/ts -> 46

fsc-compile-section one multibyte def:
  go/rust/ts -> 69

form-source-compile-text one multibyte section:
  go/rust/ts -> 69
```

Validation:

```text
cd form && ./validate.sh form-stdlib/tests/source-compile-multibyte-band.fk
  -> 15

cd form && ./validate.sh form-stdlib/tests/string-boundary-band.fk
  -> 8

cd form && ./validate.sh form-stdlib/tests/core-str-narrow-waist-band.fk
  -> 255

cd form && ./validate.sh form-stdlib/tests/core-str-shim-band.fk
  -> 15

cd form && ./validate.sh form-stdlib/tests/source-compiler-health-band.fk
  -> 65535

cd form && ./validate.sh form-stdlib/tests/bmf-source-scanner-rule-band.fk
  -> 4500

cd form && ./validate.sh form-stdlib/tests/bmf-byte-cursor-source-band.fk
  -> 4194303
```

## 2026-07-05 Obsolete Kernel Surface Removed

The old Go-only in-memory compile/run door was removed instead of promoted:

- `form_compile`
- `form_walk`
- `compile_form_source` / `compile-form-source`
- `compile_source_section` / `compile-source-section`
- `compile_source_text` / `compile-source-text`
- `source_compile_last_error` / `source-compile-last-error`

These were not common fkwu/kernel floor primitives. They were sibling-host
convenience paths around the real source-compiler work and made the Go proof
kernel larger than the bootstrap kernel. The primitive registry and affected
tests now track the smaller surface.

## 2026-07-05 Boolean Template Repair

After narrowing native precedence, `TestFkwuFormCli` exposed a real grammar
emission bug while source-compiling `http-client.fk`: `t-const-bool` in
`bmf-grammar.fk` built raw runtime `true`/`false` values. That was fine only
when a caller wanted a runtime answer. It is wrong when the grammar is emitting a
recipe object: `intern_node` children must be NodeIDs.

Repair:

- `t-const-bool` now emits `make_nodeid 1 1 3 n`, the boolean literal node.
- `tests/bmf-grammar-sugar-band.fk` now asserts that node shape directly.
- `core.fk`'s Form `ord` fallback now matches the native empty-string contract:
  `ord("") -> -1`.
- `intern_node` in the Go proof kernel now reports child index, bad value, and
  live Form stack when a non-NodeID child crosses that boundary.

Focused validation after this repair:

```text
cd form && ./validate.sh form-stdlib/tests/bmf-grammar-sugar-band.fk
  -> 63

cd form/form-kernel-go && go test -run TestFkwuFormCli -count=1
  -> PASS

cd form && ./validate.sh form-stdlib/tests/primitive-registry-band.fk
  -> 63
```

## 2026-07-05 Fourth-Arm Table Build Stall

`form-bml-cursor-lower-band.fk` did not fail semantically, but `validate.sh`
stalled while building its fourth-arm table. The live process was:

```text
form-kernel-go/bin-go /.../form-fourth-t.DOaZfA/driver.fk
elapsed ~= 4 minutes
RSS ~= 1.49 GB
CPU ~= 209%
driver.fk ~= 264 KB
```

That process was terminated deliberately. This is recorded as a fourth-arm
flatten/table-build pressure issue, not ignored and not confused with the
source-compiler multibyte stall.

The wrong-arm fallback is now disabled in `form/scripts/fourth-arm.sh`: if the
committed T_flat/fkwu self-host table path cannot produce a table, validation
skips the fourth leg and continues with the three proof siblings. It no longer
constructs a monolithic Go driver by concatenating the whole fourth chain plus
the workload.

The same prepared workload, with the source-compiled `compiler.fk` cache and
without the fourth-arm table build, agreed across the three proof siblings:

```text
form-bml-cursor-lower-band.fk:
  go/rust/ts -> 113

form-bml-cursor-full-band.fk:
  go/rust/ts -> 67
```

Post-fallback validation:

```text
cd form && ./validate.sh form-stdlib/tests/form-bml-cursor-lower-band.fk
  -> 113

cd form && ./validate.sh form-stdlib/tests/form-bml-cursor-full-band.fk
  -> 67
```

## Follow-Up

The source compiler still carries too much right-recursive string and line-style
code for the desired BMF cursor direction. The stall is repaired, but the next
health lift should move source-section scanning and emission toward the cursor
surface so this class is structurally harder to recreate. The fourth-arm
flatten/table builder now also needs its own bounded observation gate: large
driver recipes must not be allowed to run until OOM or manual interruption.
