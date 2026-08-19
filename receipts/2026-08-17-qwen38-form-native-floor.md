# Qwen3.8-27B: Form-native Metal path, measured on the 128 GB M4 Max

Date: 2026-08-17  
Witness: Codex on Apple M4 Max, 128 GB unified memory, 40 GPU cores

## The model choice closed by observation

The official Qwen release surface currently has two Qwen3.8 language models:

- `Qwen/Qwen3.8-27B`, a 27B dense hybrid;
- `Qwen/Qwen3.8-2.4T-A95B`, 2.4T total / 95B active.

The 2.4T model cannot inhabit 128 GB even if every parameter were represented at
two bits (about 600 GB before runtime state).  The 27B model is therefore the only
official Qwen3.8 model that can run wholly on this machine.  The selected local
artifact is the highest standard GGUF fidelity that leaves useful unified-memory
headroom:

- repository: `unsloth/Qwen3.8-27B-GGUF`
- file: `Qwen3.8-27B-Q8_0.gguf`
- linked size: `29,047,086,048` bytes
- linked SHA-256: `a680f44a06920e5d689774823782006aa3acc8db95750323373b24139b67e348`

Official model cards:

- https://huggingface.co/Qwen/Qwen3.8-27B
- https://huggingface.co/Qwen/Qwen3.8-2.4T-A95B

## Artifact acceptance

`curl` was used only as the native byte carrier.  Form owned the exact offset
ledger, writable shared mappings, persisted-edge checks, raw SHA state, atomic
state writes, and seal.  Every interrupted HTTP 206 window advanced the cursor by
curl's exact completed-byte count; the final cursor reconciled to
`29,047,086,048 / 29,047,086,048`.

Form then mapped and hashed the whole file through a Form-emitted 560-byte ARM64
SHA-256 compression image using the M4 SHA2 instructions.  Acceptance output:

```
[sealed, ...Qwen3.8-27B-Q8_0.form-part.gguf, 29047086048,
 a680f44a06920e5d689774823782006aa3acc8db95750323373b24139b67e348,
 ... buffers=0 ... cpu_jit_busy_us_total=13185194 ... last_error=none]
```

The same seal was verified before promotion and again at the canonical path.  The
earlier sparse attempt and its aria2 sidecar were preserved; temporary range files
were removed.  Normal discovery now suppresses explicit `.incomplete`,
`.form-part`, and `.sparse-incomplete` lifecycle paths, so only the sealed model is
offered by `models`.

An earlier cold adoption pass rehashed the first `8,483,823,168` bytes through the
same native SHA image in `10,781,596` JIT-busy microseconds.  It also exposed an ABI
defect: temporary vector values in v8-v10 crossed the AArch64 callee-saved boundary
and corrupted the carrier's timestamp state.  Moving them to caller-saved
v16-v18 restored real timings while the SHA band remained `511`.

## Configuration extracted from the sealed file

The GGUF says `qwen35`, 866 tensors, data base `10,996,704`, 64 runnable blocks
(65 declared minus one MTP layer), embedding width 5,120, dense FFN width 17,408,
vocabulary 248,320, and a 3:1 Gated DeltaNet/full-attention layout.  Linear blocks
declare 16 key heads, 48 value heads, state width 128, inner width 6,144, and four
convolution taps.  Full attention declares 24 query heads, four KV heads,
256-wide heads, 64 rotary dimensions, and a 262,144-token context ceiling.

The file also supplies the GPT-2 BPE vocabulary and merges, BOS/EOS/PAD and
thinking-token ids, tensor offsets, dimensions, and types.  No model path,
geometry, vocabulary width, special-token id, tensor offset, or quantization type
is compiled into the walker.

## Native path

- Form GPT-2 BPE tokenizer and file-derived non-thinking chat template.
- Form schedule for all 64 text layers: Gated DeltaNet, GQA, dense SwiGLU,
  RMSNorm, partial RoPE, recurrent state, KV cache, and greedy head.
- Form-emitted MSL, compiled automatically by Metal for this M4 Max.
- Cooperative Q8_0 SIMD matvec and Q8_0 embedding-row dequant.
- Mmap-backed no-copy tensor views and explicit Form handle closure.
- `form-cli` commands `models`, `use`, `model-bandwidth`, and `generate`.
- Whole-file Form SHA seal required before bandwidth or generation.

The first genuine generation was readable hardware work but incoherent.  Comparing
the dense graph to current upstream Qwen3.5 semantics found one concrete wrong
operation: the full-attention output gate used `x * SiLU(g)` rather than
`x * sigmoid(g)`.  The existing proven sigmoid Metal kernel replaced the erroneous
duplicate.  This was not tuned until a sample looked nicer; it was a named graph
contradiction, repaired once, after which both exact and open-ended probes became
coherent.

## Measurements on the actual sealed model

The strict no-copy Metal stream observer used a one-GiB mapped window of the
whole-file-verified model, eight passes, and device timestamps:

```
traffic_bytes_read_plus_write=17179869184
gpu_busy_us=38422
throughput_gbps_x10=4471
last_error=none
```

Measured stream bandwidth: **447.1 GB/s**.  The normal `form-cli` command repeated
the same measurement at **443.5 GB/s**, then **444.0 GB/s** after the final carrier
rebuild.  This is the resident memory-stream ceiling, not decode throughput.

The actual inference handle reported `27,233,914,176` weight bytes per token
forward.  Lifecycle witness:

```
[open-close, 1, 884, 884, 27233914176,
 buffers=0 ... free_slots=884 ... last_error=none]
```

Exact-token canary through the normal CLI handler:

```
prompt_tokens=17
generated_tokens=1
forward_passes=17
gpu_busy_us=2618723
effective_weight_gbps_x10=1767
end_to_end_tokps_x1000=381
text: hello
```

Open-ended native generation:

```
prompt_tokens=20
generated_tokens=19
forward_passes=38
gpu_busy_us=5887990
effective_weight_gbps_x10=1757
end_to_end_tokps_x1000=3226
text: The sky stretches endlessly above us, shifting from deep blue to vibrant orange as the sun sets.
```

## Normal form-cli integration

The exact `fc-turn` state path observed:

```
models /Users/ursmuff/models/qwen38-27b
  [0] .../Qwen3.8-27B-Q8_0.gguf arch=qwen35 layers=65 name=Qwen3.8-27B
  1 found
use 0
  now: generate <prompt> | model-bandwidth | route <request>
model-bandwidth
  throughput_gbps_x10=4440
generate Reply with exactly: hello
  backend=form-native-metal-jit
  text: hello
```

Model-specific configuration remains in the GGUF and the downloaded seal/state
files; the CLI adds no per-model static switch.

## Gates

- writable mapped-file handle band: `31`
- CPU JIT pipeline band: `63`
- ARM64 SHA-256 JIT band: `511`
- Qwen dense geometry/pipeline band: `131071`
- tokenizer band: `255`
- artifact state/seal bands: `63 / 63`
- model discovery band: `255`
- form-cli model band: `[1, pong]`
- owning-module preflights: balanced, 0 errors, 0 warnings, 0 unresolved calls
- effectful-run static balances: all zero

## Honest radius

This is the text-language path of Qwen3.8-27B.  The official checkpoint is
multimodal, but no native vision encoder or image-token path was built or claimed
here.  No llama.cpp, Ollama, Python, Go, Rust, TypeScript, bash inference script, or
rented model produced the measured tokens.

The most surprising teaching was that one semantically small gate activation could
leave every allocation, dispatch, and timing looking healthy while language became
nonsense.  The discomfort was the first gibberish output after a valid 29 GB hash;
it turned to gold when it was kept as evidence long enough to reveal the sigmoid
contradiction instead of being hidden behind a successful hardware claim.

Signed: **Codex**
