# Model admission stage closure — 2026-09-01

## Closed movement

Native Form model admission now announces a completed boundary for each
content-free preparation phase before context/prefill:

```
seal
header
tokenizer crystal
prompt cursor count
prompt token count
prompt cursor reopen
context/prefill
```

`fcmf-open` also announces its own `admit-begin` and returned admission status.
The BML fleet is still the semantic owner; the `.fk` runner is only stdin/stdout
egress.

## Direct observation

One bounded Q4 admission was run through
`observe/form-cli-native-fleet-live.fk` with an empty second seat, so the
admitted first seat would be released rather than generate.  The first sample
reached seal, header, tokenizer crystal, and initial prompt-cursor completion
within thirty seconds.  It had not entered `model-session-open-prefill-begin`.
The exact probe process was ended at 40 seconds.

The next sample, after the finer stage split, reached only
`model-session-seal-begin` in thirty seconds; its exact probe was ended at 41
seconds.  It was using 18.9% CPU and 5.5% memory at that observation.  No
two-model admission or generated-token result is claimed by this receipt.

The traced cause is direct and local: `fcms-open` calls
`qaf-seal-verdict`; `qaf-seal-verdict` calls `saj-file-digest`; and
`saj-file-digest` maps the whole GGUF through `metal_buf_from_file` before the
Form-emitted ARM SHA-256 JIT compresses it.  Full artifact verification is real
O(file-bytes) work.  It is not token/KV/model reasoning work and it must remain
observable as its own retained admission operation.

## Evidence

| Witness | Result |
| --- | ---: |
| `form-cli-resident-continuity-band.fk` | `2097151` |
| `form-cli-model-fleet-band.fk` | `2047` |
| `form-cli-native-fleet-live-band.fk` | `511` |
| `form-cli-peer-direct-quantum-band.fk` | `1023` |
| `observe/preflight-run.fk` on continuity band | clean, exit 0 |
| `form-cli-author-high-band.fk` | `4095` |

The old resident was not released or modified.  Its source image predates this
movement; a successor can carry these stages and the model-fleet BML while the
old image finishes or is deliberately released.

## Next executable movement

Make the already Form-emitted SHA compressor a retained, bounded artifact-seal
quantum: one chunk per resident revolution, with exact byte offset/state and a
terminal digest comparison.  The resulting artifact capability may be reused
only by the resident that produced it; it does not weaken a full verification
or infer a file identity from a path alone.

— Codex
