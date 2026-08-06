# form-kernel-ts — vertical-slice host for Form-on-top, in TypeScript

Third sibling kernel beside [`form-kernel-go`](../form-kernel-go/) and
[`form-kernel-rust`](../form-kernel-rust/). Same content-addressed identity and
conformance contract; it is a proof sibling, never the sovereign `fkwu` runtime.

The TS kernel earns its place in exactly one role the Go and Rust kernels can't reach
without paying multi-megabyte WASM tax: **the browser**. For interactive client-side Form
evaluation (the FormPlayground, in-page editors, live inspectors), TS is the only
kernel that ships at a reasonable bundle size with full type integration.

Secondary surfaces: Node.js server-side (Next.js server components, route handlers),
React Native (mobile), edge runtimes (Cloudflare Workers, Vercel Edge). All native
targets for TS without cross-compile.

## What ships

**Full vertical-slice surface** — same 9 RBasic dispatch arms Go and Rust
implement (the Python kernel's 22 arms are its territory; vertical-slice
kernels carry the load-bearing ones):

- `NodeID` 4-tuple identity (pkg, level, type, inst)
- Content-addressed intern table (same shape ⇒ same NodeID)
- String table with NameID for fast identifier lookup
- Tagged `Value` union (null, int, str, bool, list, closure, nodeid)
- Walker with all 9 RBasic arms — `MATH`, `COMPARE`, `LOGIC`, `COND`
  (IF_THEN and IF_THEN_ELSE), `BLOCK` (DO/SEQUENCE/LET), `IDENT`, `FNDEF`,
  `FNCALL`, `LIST`
- Native primitives — `print`, `str_len`, `substring`, `char_at`,
  `str_concat`, `str_eq`, `int_to_str`, `str_to_int`, `ord`, `list`,
  `cons`, `head`, `tail`, `len`, `nth`, `empty`, `read_file`, substrate-
  write surface (`make_nodeid`, `intern_trivial_int`, `intern_trivial_string`,
  `intern_node`, `node_category`, `node_children`, `node_value`,
  `walk_recipe`), `trace`
- S-expression bootstrap reader with `buildVerb`-style name dispatch
  (matches Go/Rust exactly — `add`, `sub`, `mul`, `eq`, `le`, ... all
  intern to the same NodeIDs across kernels)
- **Recipe → JS compiler** — emits JS source from recipe trees, compiles
  via `new Function`, V8 JITs the result. Brings overhead from 100–500×
  native down to 1× for arithmetic recursion.

## Performance — bench numbers

Three of four canonical workloads at **1× native overhead** with the
compiled path. See
[`kernel-ts-comparison.md`](../kernel-ts-comparison.md) for the
full analysis.

| Workload | Native TS | Walker | walk-over | Compiled | comp-over |
|---|---|---|---|---|---|
| `fib(28)` | 1.97 ms | 590 ms | 300× | **2.25 ms** | **1×** |
| `fact(12)` | 31 ns | 14 µs | 454× | 377 ns | 12× |
| `sum(1000)` | 7.46 µs | 780 µs | 105× | **7.83 µs** | **1×** |
| `ackermann(3,6)` | 773 µs | 135 ms | 174× | **832 µs** | **1×** |

```sh
npx tsx src/main.ts --bench       # walker + compiled side-by-side
npx tsx src/main.ts --compiled "(do (defn fib (n) (if (le n 1) n (add (fib (sub n 1)) (fib (sub n 2))))) (fib 28))"
```

## Browser entry

`src/browser.ts` is the canonical browser entry. It re-exports the same
`kernel.ts`, `reader.ts`, binary codec, and `field.ts` used by the Node proof
sibling, plus the stable `runLocalFormBinary` result contract. Host effects are
explicit `KernelHost` capabilities. The Node-only implementation lives in
`src/node-host.ts` and is imported only by `main.ts`; it cannot enter the browser
graph accidentally.

```ts
import {
  runFieldRuntimeProof,
  runLocalFormBinary,
} from "form-kernel-ts/browser";

const run = runLocalFormBinary("(add 1 (mul 2 3))");
// run.result === "7"; run.root/rootCategory/trace are canonical kernel data.
runFieldRuntimeProof(); // canonical field.ts, no consumer-side field runtime
```

The proof command performs a browser-only typecheck, bundles for the browser,
rejects Node carriers/globals in the emitted graph, imports that bundle, and
executes arithmetic, recursion, output capture, binary round-trip, and the FMF
proof:

```sh
npm run proof:browser
npm run proof              # browser proof plus Node host-effect parity
```

## Active Work Queue

- Expand the canonical real-kernel vector set in
  [`../conformance/canonical-s-expression-vectors.json`](../conformance/canonical-s-expression-vectors.json).
- Form-on-top stack from `form/form-stdlib/` running on TS
- Compiler LIST literal emission through direct Form list recipes
- Compiler closure-over-outer-frame optimization (currently
  `frame.lookup` slow path)

## Category and NodeID contract

[`../category-contract.json`](../category-contract.json) is the machine-readable
authority for NodeID package/level semantics, `RBasic`, trivial slots, and core
instances. This kernel imports it directly. Other projections (including Python)
can load the JSON instead of maintaining an enum copy. It explicitly separates
`CHOICE=20` from `CHOICE_MATCH=35` and `RESOLVE=5` from
`FIELD_RESOLVE=97`, preventing the two historical name collisions from returning.

## Usage

```sh
cd form/form-kernel-ts
npm install                                    # zero runtime deps; tsx for dev
npx tsx src/main.ts --expr "(+ 1 2)"           # → 3
npx tsx src/main.ts --expr "(if (< 1 2) 'yes' 'no')"   # → "yes"
npx tsx src/main.ts ../form-samples/fact.fk    # when stdlib loads, runs samples
```

Cross-kernel conformance is driven by the public kernel CLIs, not by a
TypeScript-side language imitation:

```sh
python3 form/scripts/verify_kernel_conformance.py
python3 form/scripts/verify_kernel_conformance.py --kernel ts
```

The vectors use canonical S-expressions, binary arithmetic/logic, `mod`, and
recursive Form functions. Truth renders as the kernels actually represent it:
`1` or `0`.

## Lineage

This kernel is the browser sibling in the conformance circle. Go, Rust, and
TypeScript prove portability against the same contracts—same NodeIDs regardless
of host language, because content-addressing is geometric. Persistence adapters
consume that kernel surface; they do not carry another language implementation.

See [`../kernel-roadmap.md`](../kernel-roadmap.md) for the language/runtime
direction and [`../kernel-comparison.md`](../kernel-comparison.md) for the
benchmark + optimization arc on Go and Rust.
