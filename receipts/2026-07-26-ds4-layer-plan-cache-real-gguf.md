# DS4 layer-plan cache — real GGUF observation

; witnessed: 2026-07-26 -> partial success, dominant-cost hypothesis disproved

## Inquiry

Can the active 91 GB DS4 source and framebuffer be used to reduce the
multi-hour prefill toward seconds?

## Change

`dsv4-layer-walk.fk` now resolves its 39-field immutable tensor/state plan once
per layer and reuses that Form data across every prompt position.  Metal call
order, arithmetic parameters, state names, and receipts are unchanged.

## Real comparison

Both witnesses read the configured
`DeepSeek-V4-Flash-REAP25-DSpark-ds4-GGUF` descriptor graph.  Neither uses a
mock model or opens a competing Metal session.

```text
REAL before-checksum=0
real 1.13
user 0.57
sys 0.50

REAL after-checksum=0
real 0.70
user 0.44
sys 0.24
```

Commands:

```text
./form-run /usr/bin/time -p ./fkwu-metal-opt observe/dsv4-layer-plan-lookup-before.fk
./form-run /usr/bin/time -p ./fkwu-metal-opt observe/dsv4-layer-plan-lookup-after.fk
```

The equal checksums witness equal selected descriptor identity.  Eight repeated
lookups became one lookup plus eight reads: `1.61x` faster, saving `0.43 s`, or
about `54 ms` per prompt position in this bounded observation.

## Adjudication

The improvement is real, but the hypothesis that descriptor lookup explains
the multi-second position time is disproved.  Millisecond Metal receipts plus
this bounded result leave repeated interpreted membrane orchestration and
model-memory movement as the next inquiry surface.  This receipt does not claim
seconds-to-first-token.

## Re-witness offer

Run the two commands above on the same configured GGUF, then compare a complete
one-layer/two-position walk after the active model lock is released.
