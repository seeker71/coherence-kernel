# Native Python specimen compiler

`observe/python-specimen-compile-run.bml` reads one JSON line containing `source` and `output`. The source is an existing `.py` input specimen; the output is a `.fk` path whose parent directory exists. The current `fkwu` process parses, lifts and emits the specimen, then publishes the complete file. A failed compilation preserves an existing output.

```sh
form-run ./fkwu observe/python-specimen-compile-run.bml <<'EOF'
{"source":"form/form-kernel-ts/seedbank/python-adapter/examples/python_demo.py","output":"/tmp/python-demo.fk"}
EOF
form-run ./fkwu /tmp/python-demo.fk
```

The example returns `40949`. `compilation.json` in the reported evidence directory records the current compiler PID, source, output and byte count. Compilation creates no child compiler. The independent trace utility can subsequently ask its proof sibling to execute the emitted specimen.

`form-ontology-loader.fk` binds category names as plain Form globals before native source resolution and proof bootstrap. Coordinates come from `fol-bp`; engine getters remain checked against their Form-owned rows. The lifter holds complete source bytes, resolves arithmetic categories through that registry and refuses unsupported token or statement shapes. Complete expression owners require all tokens to be consumed; class headers admit a single base and parameter readers preserve annotations while refusing default, starred and keyword-only parameter shapes. The Python interpreter owns the separate source-to-value aliases, so compilation does not import an unused interpreter.

`python-native-dispatch.bml` derives method selection and single-inheritance traversal from each specimen's actual class definitions. `python-class-runtime.bml` is the sole source of flat instance field lookup, child-first inherited field merging and local list append; the compiler lowers and embeds this authority. Calls carry the actual receiver, lifted argument definitions and argument list. A user-defined `append` selects its class method. List append in statement position carries its local rebinding into subsequent statements. Unknown fields, unavailable methods and wrong arity refuse explicitly.

`python-compiler-health.bml` carries executing-organ observations through the shared diagnostic channel. A refused compilation or publication emits correlated observation, abstention and applied rows without source bytes. Publication verifies its owned staging write and removes that stage on refusal; retained metadata reports the actual cleanup result. A pre-existing staging path is refused without changing it.

```sh
form-run ./fkwu observe/python-native-compiler-witness.bml
```

The witness checks exact results for literals, arithmetic, UTF-8, functions, classes, inherited construction, `super`, dynamic receiver dispatch, method lambdas, class and list append, annotations and the existing demo. It checks complete expression and header refusals, retained output, three real execution refusals, same-process compiler identity and publication failures against a directory and an absent parent. Each compilation refusal requires the correlated health rows. Python inputs remain fixture data. Finished child launch carriers release while process records and output remain available.

The native compiler covers the grammar and emission paths it currently implements. It does not claim a complete Python runtime: collection alias mutation, list append within value expressions, broader class mutation, multiple inheritance and remaining proof-only collection primitives are unfinished. Its existing string length convention counts bytes. The north star is one adaptable Form compiler, reusable native images, exact byte identity and additional language behavior learned into the same authority, with independent proof execution preserving observable results.
