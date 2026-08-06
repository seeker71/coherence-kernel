# Form-owned cross-kernel contracts

Files here are runtime coordination data shared by sibling kernels. They live
beside the kernels so a submodule checkout is sufficient to build and test the
same semantics on Go, Rust, and TypeScript.

- `numeric-formats.canonical.json` defines numeric meanings, encodings,
  interning order, and conformance vectors. Go and Rust load this exact file;
  TypeScript's format table must remain structurally equal to it until its
  generated-data loader lands.

The contract was moved byte-for-byte from the former consumer-owned location.
Its SHA-256 at the move boundary is
`e2177de9cf126fa7813c124012a5e529dfe531ff84026d60e8c7a02347b5fe3d`.
