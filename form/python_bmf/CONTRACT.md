# Python boundary contract

The singular language authority is the Form kernel family. Python is only a
wire/metadata carrier at this boundary.

## Working contract

1. `NodeID` is exactly four validated unsigned 32-bit fields. The Python
   package reads or carries identities; it never allocates one.
2. `FORMBIN2` uses the exact sibling-kernel layout: eight-byte magic,
   big-endian `u32` tags/counts/NodeID fields, little-endian `f64` and `i64`
   payloads, a UTF-8 string pool, and recursively embedded categories/children.
3. Composite identity is reconstructed by the loading kernel and is never
   claimed as stable across artifacts or allocation histories.
4. Human symbols and source spans remain in the separate `.fkl` lens.
5. Form-to-Python source emission, where used, runs as Form recipes through a
   real kernel and produces ordinary Python with no SDK or private-kernel import.

## Forbidden shapes

- Python interning, content-hash identities, or sequential pseudo-NodeIDs.
- A Python Form parser, evaluator, compiler, decompiler, rules engine, or
  handwritten emitter.
- `FKB1` or any private format presented as `.fkb`.
- Consumer-repository imports or consumer-owned semantic source files.
- Claims that a self-roundtrip proves kernel compatibility.

## Executable proof

```sh
python3 -m unittest discover -s form/python_bmf/tests -v
python3 form/scripts/verify_kernel_conformance.py
python3 form/scripts/verify_category_contract.py
```

The conformance command invokes the real Go, Rust, and TypeScript public CLIs.
No Python code in this package parses or evaluates a Form expression.
