# form-kernel-ts — the walker and the compiled path

Companion to [`kernel-comparison.md`](./kernel-comparison.md) (Go/Rust).
The TS kernel has two paths:

1. **Walker** — same shape as the Go and Rust kernels: tree-walking
   recipe interpreter, dispatching on RBasic category at every step.
   Lives in `form/form-kernel-ts/src/kernel.ts`.
2. **Compiled** — recipe → JS source → `new Function(...)` → JIT.
   The structural unlock that brings pure-arithmetic overhead from
   walker range down to native range. Lives in `form/form-kernel-ts/src/compiler.ts`.

The compiled path answers *"can a tree-walking interpreter reach native
parity?"* For the canonical bench cases (fib, fact, sum, ackermann) it does —
because the host already has V8's optimizing JIT, and compiling recipes to
direct JS lets V8 see the actual algorithm instead of a generic dispatch loop.

## The compiler's architecture

The walker pays per recipe step: a map lookup keyed by the string-serialized
NodeID, a category fetch, a switch dispatch on `cat.type`, and per-arm work
including allocating a new `Value` on every arithmetic operation.

The compiler emits structurally different JS that V8 JITs through its
optimization tiers (ignition → sparkplug → turbofan). For `fib`:

```js
// Recipe tree:                          // Generated JS (after compiler):
// (defn fib (n)                         function fn_fib_1(p_n_2) {
//   (if (le n 1)                          return ((p_n_2) <= (1))
//     n                                     ? (p_n_2)
//     (add                                  : ((fn_fib_1(((p_n_2) - (1)) | 0))
//       (fib (sub n 1))                          + (fn_fib_1(((p_n_2) - (2)) | 0)) | 0);
//       (fib (sub n 2)))))                }
//                                        // ... then: fn_fib_1(28) at root
```

Native ints are kept in `i32` via `| 0` and `Math.imul` to match Go's and Rust's
int32 semantics. After turbofan, this is the same machine code as the native
reference; the kernel overhead approaches one boxed-Value return at the outermost
call site plus parameter extraction at entry.

## What the compiled path does not yet cover

The fallback (calling the walker for unsupported constructs) is wired in:

- **`LIST` literals** fall through to the walker; compiling list-producing
  expressions to JS arrays is one breath.
- **Substrate-write natives** (`intern_node`, `make_nodeid`, ...) compile to
  `callNative` calls — direct, but each crosses the boxed-Value boundary.
- **Closures over outer-scope variables** beyond top-level FNDEFs fall back to
  `frame.lookup` per access.
- **`LET` bindings** emit as IIFEs; direct JS `let` inside a block would be cheaper
  but needs statement-context tracking.
- **Non-int return types** at the boundary use a small typeof check.

## Where each path earns its existence

- **Substrate processing** (a few thousand recipes per second): the walker is fine.
- **Interactive playground** (user types, kernel evaluates, render): the walker is fine.
- **Tight inner loops** (Form-side parsers, million-call recursion): the compiled
  path is required; that is what makes the TS kernel a candidate for a
  keystroke-by-keystroke surface with no API round trip.

## Cross-kernel agreement

Content-addressing is geometric — the same `.fk` source produces the same NodeIDs
in Go, Rust, TS, and fkwu; `validate.sh` is the witness. The bench values
(`fib(28)` 317811, `fact(12)` 479001600, `sum(1000)` 500500, `ackermann(3,6)` 509)
are checked by that run, not by this page.

## Repro

```sh
cd form/form-kernel-ts
npm install
npx tsx src/main.ts --bench     # walker and compiled beside the native TS reference
```
