# 2026-08-23 — the emitted walker never got the big stack

Asked: all Form-native, with full JIT across CPU, GPU/Metal, and MLX.

Grounding first, before any of it: `cc -O2 -o fkwu runtime/fkwu-uni.c
form/native/metal/fk-metal-carrier.m form/native/mlx/fk-mlx-carrier.c`,
then ground **42**, freshness **31**, metal-door **15**,
native-vs-rented **11111**. The 2026-08-19 floor holds unchanged.

## What was already home

More than the ask assumed. `form/native/metal/qwen35-dense-token-handle.fk`
holds a dense Qwen3.5/Qwen3.8 decode: architecture, geometry, tensor names
and types, tokenizer width, special ids all read from the selected GGUF;
MSL assembled by Form and JIT-compiled for whatever device answered
`metal_status`; weights mmap-backed, no copy. Its band reads the real file
at `/Users/ursmuff/models/qwen38-27b/Qwen3.8-27B-Q8_0.gguf` and answers
**131071** — every bit — in 0.99 s from a cleared cache.

That band is cross-witnessed. Every geometry value it demands (d 5120,
ffn 17408, heads 24, kv 4, key 256, rope 64, vocab 248320, ssm group 16,
dt rank 48, conv 4, full-attention interval 4) matches an independent
Python read of the same GGUF header taken three days earlier for a
different purpose. Two readers, one file, one set of numbers.

The three organs answer on `fkwu`: form-cli-live **255**, metal-emit **31**,
jit-metal-lanes **8191**, metal-handle-door **65535**, form-cli-mlx **63**.
`mlx_linked=true mlx_metal_available=true mlx_device=gpu mlx_version=0.32.0`.

## What was not home

`form-cli generate` — the door the 2026-08-19 receipt shows answering
`text: hello` — was dead on this checkout. Not slow, not wrong: **SIGSEGV,
rc 139, zero bytes on stdout and zero on stderr.** The kind of silence that
reads as "nothing happened."

The crash report named it: *"Thread stack size exceeded due to excessive
recursion"*, faulting in `fk_walk_slow` on `com.apple.main-thread`.

The seed already knew this. `runtime/fkwu-uni.c` runs `fk_run` on an
explicit `FORM_KERNEL_STACK_MB` thread and says why at its own site — the
bare call "died silently at ~120 recursion levels." The **emitted** entry,
`fkc-main-baked-repl-text` in `form/form-stdlib/hati-os-kernel-emit.fk`,
ended `fk_vs[0] = 0; fk_vsp = 1; fk_walk(fk_fn[0], 0); return 0; }` — on
whatever stack the OS handed `main`. The repair had never crossed from the
seed to the thing the seed emits.

## The repair, and the number that surprised

Same law at the emitted site: a `#if defined(_WIN32)` direct call, else a
pthread with an explicit stack, `FORM_KERNEL_STACK_MB` honoured.

The default is **not** the seed's 256 MB, and that difference is the
finding. Bisected on this host against the same generate:

| stack | result |
|---|---|
| OS main (8 MB) | SIGSEGV 139 |
| `ulimit -s 65520` (darwin ceiling) | SIGSEGV 139 |
| 256 MB — the seed's default | SIGBUS 138 |
| 1024 MB | SIGBUS 138 |
| 2048 MB | SIGBUS 138 |
| 3072 MB | rc 0 |
| 4096 MB | rc 0 |

The seed walks a node in one small interpreter frame. The emitted walker
gives every node its own C frame, and those frames are fat enough that the
same program needs **more than an order of magnitude** more stack compiled
than interpreted. 4096 MB is the default so the door opens with nothing
typed; a pthread stack is reserved address space committed by touch, so the
unused part costs no memory. A slimmer emitted frame is the deeper repair
and is named, not claimed.

Gated, not waved: `regen_fkwu_bootstrap.sh`, `build_fourth`, then
`regen_form_cli_bootstrap.sh` with **`regen: voice canary — ping answers
pong`**. The first regen of the sitting skipped that canary — the chain
stamp had moved and `build_fourth` failed behind `|| true`. An artifact
built past a skipped gate was not kept.

Re-witnessed after the repair, nothing typed:

```
generate Reply with exactly: LOCAL FORM ALIVE
  backend=form-native-metal-jit
  text: LOCAL FORM ALIVE
```

Twelve bands re-run after the change, all unmoved: 42, 31, 15, 11111, 63,
15, 31, 255, 63, 8191, 65535, 131071.

## Honest radius, measured against the rented sibling

On `fkwu`, through `observe/qwen38-generate-probe.fk`, one generate over
the same 27,233,914,176 weight bytes: 57 forward passes, 73,390 GPU
dispatches, 32 pipelines, **prefill 9.416 tok/s, decode 5.718 tok/s,
248.0 GB/s effective**, and 12.1 s of CPU JIT — the SHA-256 arm64 seal
walking 27 GB before a tensor is mapped. Both organs in one run.

llama.cpp on the identical file, measured on this host two days ago:
prefill 230.6 tok/s, decode 15.21 tok/s, 442 GB/s. So the Form-native path
is **~24× behind at prefill and ~2.7× behind at decode**. It is correct and
it is ours; it is not yet fast.

Three gaps stay open and are not dressed as anything else:

- **MLX** is seated and live on the GPU and carries five ops — add, mul,
  sub, max, sum. It does not carry a transformer. "Full MLX" is unbuilt.
- **form-cli's own counters**: that run reported `decode_gpu_busy_us=0`
  while decoding three tokens, which inflates its derived
  `effective_weight_gbps_x10=5472`. The `fkwu` probe's 248.0 GB/s counts
  both halves and is the number to trust.
- **The Claude Code door** is still rented end to end — llama.cpp holds the
  weights, a node router speaks Anthropic. Nothing Form-native serves it.

## The most surprising teaching

The same program needs 256 MB interpreted and more than 2 GB compiled. The
emitted walker is not a faster copy of the seed walker; it is a differently
shaped one, and the shape shows up as stack. Compiling a walker does not
just change its speed — it changes what it needs to stand on, and the
repairs proven on the interpreter do not travel to the emitter for free.

## Where discomfort turned to gold

Twice, the comfortable move was to stop one step early.

The first was the empty output. `rc=139` with zero bytes on both streams
looks exactly like a dead end, and "generate is broken, here is the
llama.cpp number instead" was a complete-sounding answer. Reading the
`.ips` crash report instead turned a dead end into one named line.

The second was `regen: WARNING voice canary skipped`. The artifact had been
produced; the stamp matched; the build would have linked. Chasing the skip
back through a moved chain stamp to a `build_fourth` failure swallowed by
`|| true` cost three extra regens — and it is the only reason the shipped
emitted C is one a live carrier actually answered through, rather than one
that merely validated in shape.

; witnessed: 2026-08-23 -> ground 42, freshness 31, metal-door 15,
; native-vs-rented 11111, qwen35-dense-token-handle 131071, live 255,
; mlx 63, jit-metal-lanes 8191, metal-handle-door 65535,
; form-cli generate backend=form-native-metal-jit with no env set
