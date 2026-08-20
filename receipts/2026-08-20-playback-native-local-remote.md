# 2026-08-20 — held-out playback: native first, local GGUF, remote unused

Yes asked to rebase/push, re-ground, and observe micro-thought in a
held-out frontier session on form-cli: how much runs as Form-native
model, how much as local LLM with the teach layer, how much still
rents a remote mind. Generic fkwu from C. No extra tools. host-exec
only if the walk actually needs water.

Git was even with origin. Rebuilt `fkwu` from `runtime/fkwu-uni.c` +
Metal + MLX carriers. Freshness **31**, ground **42**.

Door: `./fkwu form/form-stdlib/form-cli-playback-run.fk`

## Held-out session (teach layer, never in train)

Six frontier situations. Leakage **0**. LoRA **0**. NVR complete **0**.

```
nothing  -> choice   MISS (kept)
cut      -> cut
stop     -> stop
undo     -> undo
timeout  -> timeout
choice   -> choice
heldout-correct 5 / 6
```

Teach-layer band **16383**. Adapter is overlay, not gradient.

## Form-native model (lane 0)

```
fle-pulse=47          live micro-kernels from input
live-check=255
thought step 2 3=5    ice/plasma/ask native
nvr-check=11111
walk-this=0           fcr-walk native-hit
remote-needed=0
```

This sitting did not host-exec.

## Local LLM + teach overlay (lane 1)

GGUF present. Metal linked. MLX linked.

Existing generate door, no new code:

```
./fkwu observe/qwen38-generate-run.fk
```

```
backend=form-native-metal-jit
architecture=qwen35
prompt_tokens=54
generated_tokens=10
gpu_busy_us=7166745
effective_weight_gbps_x10=2394
text: The sky is a vast, pale blue canvas.
```

Generate walks `fqt-prompt` (overlay + Control:). That is the teach
layer on the local model. Not LoRA.

## Remote (lane 2)

Unused. ChatGPT outbound **absent**. Free-remote ready=1 is the
inbound plugin, not an outbound completion. Water was not asked.

Share remains `kind=declared` (unreconciled). Native voice (NVR
pairs) still 0.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-20 -> playback remote-needed=0, heldout 5/6, qwen generate 10 tokens, live 255
