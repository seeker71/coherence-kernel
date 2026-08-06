# `form/python_bmf` — thin Python wire boundary

This package carries data produced by the Form kernels. It does not implement
Form language semantics.

The surviving surface is intentionally small:

- strict four-`u32` `NodeID` values, treated only as kernel-issued data;
- canonical `FORMBIN2` recursive DTOs and byte codec;
- `.fkl` symbol/source lens DTOs.

Composite identity is absent from the wire by design. A Go, Rust, or TypeScript
kernel reconstructs artifact-scoped composite NodeIDs when it loads the recipe
tree. Python does not mint, hash, or pretend to preserve those identities.

There is no Python parser, evaluator, rule engine, compiler, decompiler, or
emitter in this package. The real sibling-kernel CLIs execute and emit Form.
The Form-native emitter at `form-stdlib/emits/python-native.fk` may produce
ordinary Python source; that output has no private Python-kernel dependency.

Proof from the repository root:

```sh
python3 -m unittest discover -s form/python_bmf/tests -v
python3 form/scripts/verify_kernel_conformance.py
```

The second command proves both directions: Python-coded FORMBIN2 is executed by
all real kernels, and each kernel's FORMBIN2 output byte-roundtrips through the
Python codec before every sibling executes it.
