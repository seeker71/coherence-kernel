# Receipt - Form-owned JIT backend emission bundle (2026-06-30)

## What landed

Added:

- `observe/jit-backend-emission.fk`
- `observe/tests/jit-backend-emission-band.fk`

This cell consumes the witnessed host-dispatch packet as a compact packet
summary and emits backend bundles for CPU and GPU targets. It proves the backend
metadata contract without importing the whole runtime/emitter/IR/dispatch source
into one oversized `--src` run.

Backend bundles carry:

```text
("backend" owner packet target code-size abi reloc safepoint exception deopt root source)
```

The bundle is executable only when it is Form-owned, target-compatible, has
non-zero code size, and includes ABI, relocation, safepoint, exception, deopt,
root, and source maps.

## Witness

Run:

```sh
( cat observe/jit-backend-emission.fk observe/tests/jit-backend-emission-band.fk ) > /tmp/jbe.fk
./fkwu --src /tmp/jbe.fk
```

Observed:

```text
8191
```

Meaning:

- `1`: x64 CPU backend bundle is safe.
- `2`: arm64 CPU backend bundle is safe.
- `4`: PTX GPU backend bundle is safe.
- `8`: incompatible CPU->PTX emission is rejected.
- `16`: foreign `c-lowering` packet is rejected.
- `32`: invalid packet summary is rejected.
- `64`: missing deopt table is rejected.
- `128`: foreign backend owner is rejected.
- `256`: emitted code size matches the lowered IR op count.
- `512`: guard/runtime passing execution selects native.
- `1024`: native execution returns the native value.
- `2048`: guard failure selects deopt.
- `4096`: runtime failure selects exception.

## Honest boundary

This is backend emission metadata and execution simulation in Form, not loaded
host machine code. It deliberately consumes a compact packet summary because the
full policy/emitter/IR/dispatch source exceeds current `fkwu --src` composition
envelopes. Follow-up receipt `2026-06-30-form-native-jit-backend-bytes.md`
adds deterministic x64/arm64/PTX-like payload emission as Form data. The
remaining lift is to load and execute host-native code generated from these
Form-owned backend bundles while preserving the witnessed maps and
guard/deopt/exception behavior.
