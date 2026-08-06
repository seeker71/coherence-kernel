# Receipt - Form-owned JIT host dispatch packet and cache (2026-06-30)

## What landed

Added:

- `observe/jit-host-dispatch.fk`
- `observe/tests/jit-host-dispatch-band.fk`

This layer proves the runtime integration shape above the Form IR without C
lowering. It introduces a Form-owned `host-packet` receipt carrying:

```text
("host-packet" owner target program signature installed valid source-map guard-map fault-map deopt-map)
```

and a dispatch-cache row:

```text
("dispatch-cache" signature packet)
```

The packet is only installable when it is owned by `"form-ir"`, targets CPU or
GPU, carries a safe Form IR program, and includes source, guard, fault, and
deopt maps.

## Witness

Run:

```sh
( cat observe/jit-runtime-fault.fk observe/jit-emitter-bundle.fk observe/jit-native-ir.fk observe/jit-host-dispatch.fk observe/tests/jit-host-dispatch-band.fk ) > /tmp/jhd.fk
./fkwu --src /tmp/jhd.fk
```

Observed:

```text
8191
```

Meaning:

- `1`: CPU Form-IR packet installs.
- `2`: GPU Form-IR packet installs.
- `4`: foreign `"c-lowering"` packet is rejected.
- `8`: packet missing a fault map is rejected.
- `16`: matching installed valid packet is a cache hit.
- `32`: signature mismatch is a cache miss.
- `64`: invalidated packet is a cache miss.
- `128`: cache hit executes the native action through the Form IR interpreter.
- `256`: cache-hit execution returns the native value.
- `512`: guard failure deopts.
- `1024`: runtime failure returns a source-attributed exception.
- `2048`: invalidated cache entry rewalks.
- `4096`: rewalk returns the walker fallback value.

## Honest boundary

This is Form-owned host-dispatch packet/cache integration, not host machine
code execution. It proves the install/lookup/invalidate/execute contract that a
runtime backend must preserve and refuses non-Form-owned packets. Follow-up
receipt `2026-06-30-form-native-jit-backend-emission.md` adds the Form-owned
backend bundle metadata contract. The remaining lift is to load and execute
real host-native code from these Form-owned bundles without moving optimizer
meaning into C.
