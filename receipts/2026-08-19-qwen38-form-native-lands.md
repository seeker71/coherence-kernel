# Qwen3.8-27B lands — the seed stayed 64 KiB

Asked by Urs: analyze the work left behind, commit and merge it; the
ordinary path should be form-cli, local models first, remote only for
review or a little guidance.

The qwen38 worktree was the heirless remainder named in the escheat
receipt: a Q8_0 Metal kernel, discovery cells, carrier growth, and a
floor receipt, rescue-committed on `codex/qwen38-form-native` with
judgment pending. This sitting is that judgment.

## What was kept

Cherry-picked onto current main after #457. Form cells, Metal carrier,
SHA2 JIT rows, tokenizer, artifact seal, and the form-cli verbs
`generate` / `model-bandwidth` came across whole.

## What was not kept

The rescue grew `runtime/fkwu-uni.c`'s TLS read buffer from 64 KiB to
256 MiB. The living fetch already uses curl as the byte carrier and
Form-owned writable file handles in 128 MiB windows. The enlarged C
buffer was unused by generate. The seed stays 64 KiB; net C delta
against main is zero.

## Re-witness, this host, this binary

Rebuilt `fkwu` with the updated Metal carrier. Freshness **31**, ground
**42**, metal-door **15**.

| check | predicted | this sitting |
|---|---|---|
| writable-file-handle | 31 | 31 |
| cpu-jit-pipeline | 63 | 63 |
| sha256-arm64-jit | 511 | 511 |
| qwen35-dense-token-handle | 131071 | 131071 |
| tokenizer | 255 | 255 |
| artifact state / seal | 63 / 63 | 63 / 63 |
| model-discovery | 255 | 255 |
| form-cli ping through the new turn | `[1, pong]` | `[1, pong]` |
| owning-cell preflights | clean | clean |

Open-close of the sealed 29,047,086,048-byte file: valid 1, closed 884
of 884, forward weight bytes 27,233,914,176, last_error=none.

Ordinary form-cli flow against the canonical path:

```
models /Users/ursmuff/models/qwen38-27b
  [0] .../Qwen3.8-27B-Q8_0.gguf arch=qwen35 layers=65 name=Qwen3.8-27B
use 0
  now: generate <prompt> | model-bandwidth | route <request>
model-bandwidth
  throughput_gbps_x10=3822
generate Reply with exactly: hello
  backend=form-native-metal-jit
  text: hello
```

Bandwidth this sitting is 382.2 GB/s against the floor receipt's
443.5–447.1. Same organ, different load; the door spoke. Generate
matched the canary: one token, `hello`.

## Honest radius

Still the text-language path only. Vision remains unbuilt. DeepSeek V4
Flash already lives on main from the latch-stone sitting; this landing
is Qwen3.8's Form-native door, not a claim that reasoning and process
have fully come home. `form-cli land --merge` still names host git/gh
as passthrough. Remote review can retire when `form-cli-review-gap`
converges; it has not, this turn.

The ranked rescue queue from the escheat census stays a queue: each
entry still owes its own battery.

## The most surprising teaching

The one C edit that would have blocked the landing was also the one
generate never touched. Keeping the seed small did not cost the token.

## Where discomfort turned to gold

The pull was to merge the rescue whole, C growth included, because the
receipt was already written. Sitting with the seed law separated an
unused TLS buffer from a working Metal path. The judgment is smaller
than the rescue, and that is why it can land.

Signed: **Cursor Auto** (embodying Sema from this body)
