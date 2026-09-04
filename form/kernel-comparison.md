# form-kernel — Go, Rust, and TypeScript siblings

Three implementations keep the smallest Form host honest: Go
(`form-kernel-go/main.go`), Rust (`form-kernel-rust/src/main.rs`), and TypeScript
(`form-kernel-ts/src/kernel.ts`). Each reads real `.fk` source end-to-end through
NodeID/content-addressed intern, recipe walking, frame/closure state, native
primitives, and the S-expression bootstrap reader. The TypeScript kernel adds a
compiled path — tracked in [`kernel-ts-comparison.md`](kernel-ts-comparison.md) —
and the shared [`validate.sh`](validate.sh) checks all siblings.

## Why all siblings stay

The kernels are written in different languages with different optimization choices
and different bug profiles; their failure modes are unlikely to align. When Go,
Rust, and TypeScript agree on a Form program's output, that agreement is a far
stronger correctness signal than any one kernel alone. When they disagree, exactly
one of four things is true:

1. The Go kernel has a bug
2. The Rust kernel has a bug
3. The TypeScript kernel has a bug
4. The Form spec has an undocumented corner

All four are findable. None of them are findable if only one implementation exists.
This is **differential testing built into the architecture, not bolted on**. Every
new `.fk` source file joins the diff.

The runtime difference is a feature, not a verdict. Go is fast on native walker
workloads; Rust is small and safe and the natural candidate when the kernel ships
to edge cells via WASM; TypeScript has the browser-adjacent path and a compiled
recipe-to-JS mode for hot interactive work. None of them is the runtime of this
body — `fkwu` is (`../MANIFEST.md`); they are the arms that witness it.

## The shape of each kernel

- Each reads cleanly top-to-bottom: substrate → walker → frames → natives →
  bootstrap reader → main.
- Kernel overhead over native code is the cost of "recipes as runtime data" versus
  instructions the CPU recognizes directly — the normal range for tree-walking
  interpreters, and invisible for a substrate that processes a few thousand recipes
  a second. Hot inner loops fall through to native primitives or to the compiled
  path.
- The Rust kernel splits an immutable `Kernel` (substrate) from a mutable `Arena`
  (frames), closures holding a `FrameId`; the Go kernel uses linear-scan frames keyed
  by `NameID` and one lookup per walk step.

## Measure, don't remember

Bench numbers are re-taken, never quoted from a page:

```bash
form-kernel-go/bin-go --bench
form-kernel-rust/target/release/form-kernel-rust --bench
node --experimental-strip-types form-kernel-ts/src/main.ts --bench
./validate.sh --bench            # side by side
```

## Run

The binaries are build artifacts, not tree content: `go build -o bin-go .` in
`form-kernel-go/`, `cargo build --release` in `form-kernel-rust/`; the TS kernel
runs from source under Node's strip-types (`validate.sh` builds all three when
they are stale). Re-run 2026-09-04:

```bash
form-kernel-go/bin-go                              form-samples/fact.fk   # → 3628800
form-kernel-rust/target/release/form-kernel-rust   form-samples/fact.fk   # → 3628800
node --experimental-strip-types form-kernel-ts/src/main.ts form-samples/fact.fk   # → 3628800

form-kernel-go/bin-go                            --expr "(add 2 (mul 3 4))"   # → 14
form-kernel-rust/target/release/form-kernel-rust --expr "(add 2 (mul 3 4))"   # → 14
```

The path forward lives in [`kernel-roadmap.md`](kernel-roadmap.md).
