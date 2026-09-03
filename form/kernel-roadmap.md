# form-kernel — all of Form in Form, on top of any sibling kernel

The body holds three sibling kernels (Go, Rust, TypeScript) of the smallest
substrate-walker host, beside the native `fkwu`. From there, **everything else lives
in Form itself**. The siblings keep each other honest: every Form source file runs
through all of them, and a divergence is a bug in exactly one place — one kernel or
an undocumented spec corner — findable only because several implementations exist.

## The sibling-kernel discipline

**Every Form source file runs through the siblings.** [`validate.sh`](validate.sh)
diffs their outputs and fails on disagreement — the pre-merge check, the rapid
feedback loop, the safety net. It runs the structural gate first, walks
`form-stdlib/tests/*.fk` with `form-stdlib/core.fk` as prelude, and honors a band's
`; PROOF LEVEL:` line (a fourth-arm-only band runs on its home arm or reports pending).

```bash
./validate.sh             # all samples
./validate.sh path.fk     # one
./validate.sh --bench     # side-by-side bench output
```

## What "all of Form in Form" means

The kernel ships exactly:
- NodeID + content-addressed intern + recipe walker
- Frame/closure system
- A small set of native primitives (strings, lists, file I/O) — the leaves
- An S-expression bootstrap reader (parses `(add 2 3)` → recipe directly)

Everything else is `.fk` / `.bml` source loaded at startup: the surface-syntax
parser, the standard library, the query layer, the substrate persistence bridge,
the REPL, the diagnostics, the printer. When the body needs to change Form's
grammar, semantics, or operators, the change happens in a source file — *not* in a
kernel. A kernel grows only when something genuinely cannot be expressed in Form.

## What stands

- **The Form-side standard library** — [`form-stdlib/core.fk`](form-stdlib/core.fk):
  predicates, math wrappers, list traversal and shape, aggregators, quantifiers,
  and the narrow-waist string family (`substring` / `str_find` / `str_to_int`
  composed over `str_len` / `str_byte_at` / `byte_to_str` / `str_concat`).
- **The substrate write surface** — NodeIDs as first-class values and the natives
  that construct and read recipes: `make_nodeid`, `intern_trivial_int` /
  `intern_trivial_string`, `intern_node`, `node_category` / `node_children` /
  `node_value`, `walk_recipe`
  ([`form-stdlib/tests/substrate-write.fk`](form-stdlib/tests/substrate-write.fk)).
- **Source-located errors and `trace`** — 1-based line/col on every bootstrap
  token, bounds-checked recipe reads pointing at the opening `(`, `(trace v)` /
  `(trace "label" v)` to stderr.
- **The Form-side recursive-descent parser** —
  [`form-stdlib/seedbank/parser.fk`](form-stdlib/seedbank/parser.fk): arithmetic,
  parens, identifiers, function calls, comparisons, `if`/`then`/`else`, `defn`,
  recursion, `let`. Content-addressing means `defn fact(n) = ...` and
  `(defn fact (n) ...)` parse to the same NodeID — two surface syntaxes, one
  substrate ([`form-stdlib/seedbank/tests/parser.fk`](form-stdlib/seedbank/tests/parser.fk)).
- **Grammar as data, parsing as engine** — the character-stream pattern engine
  [`form-stdlib/grammar-chars.fk`](form-stdlib/grammar-chars.fk) (primitives are
  data: char, char-range, string, any/eof/eol, not/peek, sequence/choice/star/opt,
  capture, cut/stop, rule), the BMF object engine
  [`form-stdlib/engine.fk`](form-stdlib/engine.fk) (rules match BMF source objects,
  reduce through template closures, carry the inverse back out), and the dynamic
  grammar registry [`form-stdlib/runtime-grammar.fk`](form-stdlib/runtime-grammar.fk)
  (a new grammar is one registry row; both engines consult the same registry).
  Production grammars live in [`form-stdlib/grammars/`](form-stdlib/grammars/) —
  Python via BMF objects ([`python-bmf.fk`](form-stdlib/grammars/python-bmf.fk)
  with its band family), and siblings for Go, Rust, TypeScript, image, audio,
  video, document, natural language, and BML. The `lang-*.ts` host adapters under
  `form-kernel-ts/` are not load-bearing for parsing.
- **The persistence bridge** — [`form-stdlib/persistence.fk`](form-stdlib/persistence.fk):
  `cell-put` / `lookup-cell` / `store-cells` over `write_form_binary` /
  `read_form_binary`; a CELL recipe carries `(name, domain, blueprint, ctor)` with
  identity `(domain, name)`. The store is the contract; the backend is swappable
  beneath it ([`form-stdlib/tests/persistence-band.fk`](form-stdlib/tests/persistence-band.fk)
  declares its own verdict).
- **The current compiler path** — BMF cursor → layer grammar → semantic/data
  lowering → source compiler artifact lane, bridged by
  `form-stdlib/source-compiler-grammar-bridge.fk`
  ([`../docs/coherence-substrate/current-language-artifact-path.md`](../docs/coherence-substrate/current-language-artifact-path.md)).

## The breaths still ahead

- **Host-adapter compost.** Walk each `lang-*.ts` under `form-kernel-ts/` and name
  what `*-bmf.fk` does not already cover; most composts, some pieces (editor
  integration, IDE protocols, format detection) want different homes.
- **Six-way cross-validation.** Same source × same registry × two engines
  (character + BMF) × the sibling kernels in one validation pass, any disagreement
  a single bug locus.
- **Registry persistence.** The grammar registry materializes per session from
  source; its substrate cells could persist directly so a fresh kernel boot loads
  the registry from the lattice.
- **Bootstrap handoff.** Surface-syntax `.form` files become first-class through the
  Form-side parsers; the S-expression reader stays for bootstrap.
- **The query layer in Form.** `?equivalent`, `|>`, `?cells`, `?children`,
  `?annotate`, `?lattice` as pure Form over the substrate primitives.
- **One shared lattice.** The Form store and the parent's store are two backends of
  one interface, not yet a single lattice on disk; the parent consumes this kernel
  through the consumer submodule ([`README.md`](README.md)).

## Run the kernels

```bash
./validate.sh
./validate.sh form-samples/fact.fk
./validate.sh --bench

./form-kernel-go/bin-go      form-samples/fact.fk
./form-kernel-rust/target/release/form-kernel-rust  form-samples/fact.fk
npx tsx form-kernel-ts/src/main.ts form-samples/fact.fk
```

When in doubt about whether to grow a kernel, the test is *"can this be expressed
using kernel primitives Form already has?"* If yes, it is a Form breath. If no, the
kernel grows by exactly one primitive and the rest is Form.
