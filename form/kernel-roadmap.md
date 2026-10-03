# form-kernel — all of Form in Form, on top of fkwu

The body runs on one kernel, the native `fkwu`, the smallest substrate-walker host.
From there, **everything else lives in Form itself**.

## The validation discipline

**Every band runs on fkwu.** The band sweep ([`../gate/band-sweep-run.bml`](../gate/band-sweep-run.bml)) is the pre-merge check, the
rapid feedback loop, the safety net: it fails when a band exits nonzero, speaks a
diagnostic, or answers other than the verdict its head pins. It runs the structural
gate first, walks `form-stdlib/tests/*.fk` with `form-stdlib/core.fk` as prelude, and
honors a band's `; PROOF LEVEL: FKWU-STAGED` line (a band that needs a host carrier
reports pending until the carrier stands).

fkwu resolves a band's closure — `; preludes:` (recursive, deduplicated, honoring the
`none` sentinel), `import` in every spelling, and the names `form-stdlib/home-index.txt`
lists — and lowers every `.bml` and section-bearing `.fk` in it through its own memo'd
floor. So `./fkwu band.fk` is the whole invocation; no hand-typed closure
([`../docs/kernel-interface.md`](../docs/kernel-interface.md)).

```bash
./fkwu gate/band-sweep-run.bml </dev/null                                  # every band the body keeps
printf '{"files":["path.fk"]}' | ./fkwu gate/band-sweep-run.bml            # one
./fkwu observe/native-jit-witness-run.fk                                   # native Form emission and execution witness
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
  `node_value`; a recipe is identity, observed and never walked
  ([`form-stdlib/tests/nodeid-interning-band.fk`](form-stdlib/tests/nodeid-interning-band.fk),
  [`nodeid-one-cell-band.fk`](form-stdlib/tests/nodeid-one-cell-band.fk)).
- **Source-located errors and `trace`** — 1-based line/col on every bootstrap
  token, bounds-checked recipe reads pointing at the opening `(`, `(trace v)` /
  `(trace "label" v)` to stderr.
- **The Form-side parser** —
  [`form-stdlib/form-parse.fk`](form-stdlib/form-parse.fk): a cursor over the
  source text, no pre-tokenizer, producing a runnable kernel program. Content-addressing
  means two surface syntaxes for one program parse to the same NodeID — one
  substrate.
- **Grammar as data, parsing as engine** — the character-stream pattern engine
  [`form-stdlib/grammar-chars.fk`](form-stdlib/grammar-chars.fk) (primitives are
  data: char, char-range, string, any/eof/eol, not/peek, sequence/choice/star/opt,
  capture, cut/stop, rule), the BMF object engine
  [`form-stdlib/engine.fk`](form-stdlib/engine.fk) (rules match BMF source objects,
  reduce through template closures, carry the inverse back out), and the named
  grammar registry [`form-stdlib/grammar-loader.fk`](form-stdlib/grammar-loader.fk)
  (a new grammar is one registry row). Production grammars live in
  [`form-stdlib/grammars/`](form-stdlib/grammars/) — Python via BMF objects
  ([`python-bmf.fk`](form-stdlib/grammars/python-bmf.fk)), and grammars that read Go, Rust,
  TypeScript, Prolog, BML, Form-in-BML, Form-lift, the living equation and Sanskrit
  roots as input.
- **The persistence bridge** — [`form-stdlib/persistence.fk`](form-stdlib/persistence.fk):
  `cell-put` / `lookup-cell` / `store-cells` over `write_form_binary` /
  `read_form_binary`; a CELL recipe carries `(name, domain, blueprint, ctor)` with
  identity `(domain, name)`. The store is the contract; the backend is swappable
  beneath it ([`form-stdlib/tests/persistence-band.fk`](form-stdlib/tests/persistence-band.fk)
  declares its own verdict).
- **The current compiler path** — BMF cursor → layer grammar → semantic/data
  lowering → source compiler artifact lane, bridged by
  `form-stdlib/source-compiler-grammar-bridge.fk`
  ([`form-stdlib/tests/source-compiler-grammar-bridge-band.fk`](form-stdlib/tests/source-compiler-grammar-bridge-band.fk)).

## The breaths still ahead

- **Cross-validation of engines.** Same source × same registry × two engines
  (character + BMF) in one validation pass, any disagreement a single bug locus.
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

## Run the kernel

```bash
../fkwu ../gate/band-sweep-run.bml </dev/null
printf '{"files":["form-samples/fact.fk"]}' | ../fkwu ../gate/band-sweep-run.bml
../fkwu ../observe/native-jit-witness-run.fk

../fkwu form-samples/fact.fk                                                  # → 3628800
```

The runtime is built from the C seed (`cc -O2 -o fkwu runtime/fkwu-uni.c`);
the band sweep rebuilds it when the seed is newer.

When in doubt about whether to grow a kernel, the test is *"can this be expressed
using kernel primitives Form already has?"* If yes, it is a Form breath. If no, the
kernel grows by exactly one primitive and the rest is Form.
