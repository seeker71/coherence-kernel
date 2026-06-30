# Rust Native JIT Carrier

This is a replacement-carrier implementation, not a proof walker and not C
lowering. It installs a Form-emitted byte payload into W^X executable memory and
calls it as `fn(*const i64) -> i64`.

Current live payload:

- `form-add1`: load tagged integer arg `args[0]`, add `2`, return it.
- On `aarch64`: `ldr x0, [x0]; add x0, x0, #2; ret`.
- On `x86_64`: SysV arg-vector payload, `mov rax, [rdi]; add rax, 2; ret`.

Run:

```sh
cargo test --manifest-path carriers/rust-native-jit/Cargo.toml
cargo run --quiet --manifest-path carriers/rust-native-jit/Cargo.toml -- add1 82
```

Expected CLI result on supported hosts:

```text
native 84
```
