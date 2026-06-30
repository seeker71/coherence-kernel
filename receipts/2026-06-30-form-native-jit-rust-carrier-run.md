# Form Native JIT Rust Replacement Carrier Run

Date: 2026-06-30

## What Landed

Added `carriers/rust-native-jit`, a dependency-free Rust replacement carrier.
It is not a proof walker and not C lowering. It installs a Form-emitted byte
payload into W^X executable memory and calls it as `fn(*const i64) -> i64`.

Current live payload:

- `form-add1`
- `aarch64`: `ldr x0, [x0]; add x0, x0, #2; ret`
- `x86_64`: SysV args-vector payload `mov rax, [rdi]; add rax, 2; ret`

## Witness

```sh
cargo test --manifest-path carriers/rust-native-jit/Cargo.toml
```

Observed:

```text
5 passed; 0 failed
```

Direct live call on this checkout:

```sh
cargo run --quiet --manifest-path carriers/rust-native-jit/Cargo.toml -- add1 82
# native 84
```

Runtime route samples:

```sh
cargo run --quiet --manifest-path carriers/rust-native-jit/Cargo.toml -- route runtime 82
# exception runtime-fault observe/jit-live-execution-evidence.fk:1:1:8 form-add1

cargo run --quiet --manifest-path carriers/rust-native-jit/Cargo.toml -- route stale 82
# melt
```

## Proved

- non-C replacement carrier code can install and call Form-emitted native bytes;
- current ARM64 host returns `native 84` from tagged input `82`;
- guard failure routes to deopt without calling native;
- runtime failure returns a source-attributed exception stack;
- invalidation routes to rewalk and stale cache routes to melt;
- foreign `c-lowering` payload ownership rejects.

## Remaining Gap

This is real implementation, but not yet full rung 20. The carrier is not yet
wired into `fkwu --src` hot-recipe dispatch. The next implementation step is to
feed bytes emitted by the Form backend into this replacement carrier from the
runtime integration path rather than invoking the carrier as a separate Cargo
binary.
