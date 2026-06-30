# Receipt - Form native dispatch sweep (2026-06-30)

## What landed

Added:

- `observe/jit-native-dispatch-sweep.fk`
- `observe/tests/jit-native-dispatch-sweep-band.fk`

The full source-byte, host-handoff, and cache-lifecycle cells exceed this
checkout's source-runner envelope when concatenated together. This compact sweep
therefore consumes their repository-native band totals as receipt rows and
verifies the joint post-admission dispatch contract they cover.

Detailed authority remains in:

- `observe/jit-source-byte-pipeline.fk` -> `2097151`
- `observe/jit-host-handoff.fk` -> `524287`
- `observe/jit-source-cache-lifecycle.fk` -> `65535`

## Witness

Run:

```sh
( cat observe/jit-native-dispatch-sweep.fk \
      observe/tests/jit-native-dispatch-sweep-band.fk ) > /tmp/jnds.fk
./fkwu --src /tmp/jnds.fk
```

Observed:

```text
16383
```

Meaning:

- `1`: source-byte pipeline receipt total is present and exact.
- `2`: host-handoff receipt total is present and exact.
- `4`: source-cache lifecycle receipt total is present and exact.
- `8`: all three post-admission receipts compose.
- `16`: passing guard/runtime/parity/carrier/non-stale path selects native.
- `32`: guard failure selects deopt.
- `64`: runtime failure selects exception.
- `128`: invalidation selects rewalk.
- `256`: parity failure selects deopt.
- `512`: unavailable/failed carrier selects deopt.
- `1024`: stale entry selects melt.
- `2048`: wrong receipt total rejects.
- `4096`: missing source metadata rejects.
- `8192`: missing map metadata rejects.

## Honest boundary

This is a compact receipt sweep, not a replacement for the detailed witnesses.
It still does not install arbitrary Form byte lists with `fk_nat_install` or call
them through `fk_native_call`. It proves the post-admission dispatch outcomes
covered by the latest source-byte, host-handoff, and cache lifecycle receipts can
be tracked together without C-lowering.
