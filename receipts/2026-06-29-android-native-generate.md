# Receipt — Rung 3: native greedy generation on the S23 matches the reference

**Status: WITNESSED on device.** The Galaxy S23 / Adreno 740 ran the real llama3.2:1b generation path through
`fkwu --src`, with KV-cache decode attention and GPU matmuls, and matched the reference token vector for the
first 12 generated tokens.

## Gate

Prompt:

```text
The capital of France is
```

Prompt token ids:

```text
[128000, 791, 6864, 315, 9822, 374]
```

Reference generated ids, from `native/vulkan/gen-llama-generate-reference.py` against the local Ollama GGUF:

```text
[12366, 13, 578, 469, 3168, 301, 22703, 374, 7559, 304, 12366, 13]
```

Decoded text:

```text
 Paris. The Eiffel Tower is located in Paris.
```

## Device run

Device: `R5CW20DK17A` (`/data/local/tmp/formvk`).

Runtime:

```sh
/Users/ursmuff/Library/Android/ndk/android-ndk-r27c/toolchains/llvm/prebuilt/darwin-x86_64/bin/aarch64-linux-android34-clang -O2 -o /tmp/fkwu-android runtime/fkwu-uni.c -ldl
adb -s R5CW20DK17A push /tmp/fkwu-android /data/local/tmp/formvk/fkwu
```

Recipe:

```sh
adb -s R5CW20DK17A push model/form-llama-generate-rung3-WIP.fk /data/local/tmp/formvk/rung3.fk
adb -s R5CW20DK17A shell 'cd /data/local/tmp/formvk && FORM_KERNEL_STACK_MB=2048 timeout 1200s ./fkwu --src rung3.fk'
```

Observed output:

```text
12
```

The recipe returns `12` only after checking all 12 generated token ids against the embedded reference vector.
If any token diverges, it returns `800000000 + position*1000000 + actual_token`; if a checked Vulkan call fails,
it returns `900000000 + site`.

After adding checked model-artifact reads and `rung3.err` sidecar logging, the same device command was re-run and
again returned:

```text
12
```

No `rung3.err` file was left after the clean run. On failure, that sidecar records either
`kind=infra site=<site> value=<return>` or
`kind=token-mismatch position=<i> expected=<token> actual=<token>`.

## What changed

- The source runner now supports registered multi-arg calls beyond two operands; this fixed the four-arg-call
  collapse to 0.
- Rung 3 now latches Vulkan return codes instead of silently continuing after failed setup or dispatch calls.
- Rung 3 now checks every `c_fread` model-artifact load. Per-layer read sites are `5000 + layer*16 + slot`,
  shared tensor read sites are `5400..5401`, and embedding tile read sites are `5500..5507`.
- Rung 3 writes `rung3.err` on infrastructure or token-vector failures and removes stale `rung3.err` at the
  start of each run.
- The 34-entry Rung 3 buffer metadata layout no longer overlaps buffer handles, memory handles, and mapped
  pointers.
- `run-stage` resets the command pool before reusing the command buffer.
- The generation block calls real `decode-attn` with `Pn = step + 1`, `invs = 0.125`, and
  `koff = layer * 32768` floats. The previous `gqacp` path was only the pos-0 V-copy shortcut and is not valid
  for multi-token attention.

## Honest floor

This closes Rung 3's token-agreement gate for the named prompt and 12-token greedy stream. It does **not** claim
Rungs 4-9: confidence calibration, RAG-grounded answering, `form-cli` local routing, build/test loop, proxy
fallback, and the capstone remain pending.
