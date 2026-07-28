# DS4 prefill membrane fold

## Ground

The real query was:

> Identify the largest remaining bottleneck in this local native generation
> path and name the next code change.

It tokenized to 22 prompt positions and walked all 43 layers through the
Form-emitted ARM64 controller and linked Metal transaction dispatcher.
Network, remote generation, shell generation, Swift, and temporary model
copies remained zero.

## Observed change

The trusted layer-major schedule previously crossed the native membrane three
times per layer-position:

1. prompt-state load;
2. layer transaction;
3. prompt-state save.

The folded route keeps the same order and all state operations, but carries
the load and save as the first and last recipe lines of the existing JIT
transaction.

For 22 positions times 43 layers:

- before: 2,838 membrane calls;
- after: 946 membrane calls;
- removed: 1,892 membrane calls (66.7%);
- arithmetic transactions: unchanged at 946;
- state operations: unchanged at 1,892.

The final framebuffer now closes the prefill surface explicitly rather than
asking a caller to infer completion from the last periodic heartbeat.

## Live result

The folded real run completed in 394.87 seconds and closed both the model and
the token stream. Its exact generated ids were:

`201,223,680,223,18,14,223,14490`

Its text was:

```text

  { 0,  React
```

Those ids are byte-identical to the trusted layer-major run. The sampled
first-token boundary moved from approximately 5:24 to 5:18, which is too
small and variable to claim as a throughput gain. This patch is therefore a
measured membrane reduction and trace simplification, not a claimed speedup.

The complete stdout and timing stderr are retained beside this receipt:

- `2026-07-28-dsv4-prefill-cadence-after-live.txt`
- `2026-07-28-dsv4-prefill-cadence-after-live.stderr.txt`

## Rejected faster route

A position-major schedule reduced the full run to 348.49 seconds, but changed
the ids to `201,438,438,438,582,3104,5712,7`. Its 11.7% speed gain was rejected
because its semantics diverged. PASS-shaped prefill receipts did not override
the token witness.

; witnessed: 2026-07-28 -> PASS structural membrane reduction, semantic identity
