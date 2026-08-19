# Vital-ground local model bridge

; witnessed: 2026-07-23 -> CANDIDATE for the bridge; NOT-TUNED for the weight claim

## Identity finding

The local handle formerly named `form-llama-gap-closure:latest` was compared
with both `form-llama:latest` and `form-llama-raw:latest` by manifest tensor
name and digest. All **254 / 254** tensor digests were identical across the
whole lineage. The changed system/template is real; a learned weight change is
not. The truthful compositional identity is therefore
`form-llama-vital-ground-prompted:base-native`.

## One-time artifact observation

A one-time Python-assisted conversion observed 254 tensor layers containing
646 internal tensors and produced the artifacts below. Python is not an
admitted Form membrane, so this is evidence that the bytes can be transformed,
not an established or repeatable native bridge. The Python bridge programs
were removed immediately when this criterion was made explicit.

Artifacts:

- F16 GGUF: 6,433,683,648 bytes,
  SHA-256 `67c0da7b2b4b2cacdf68b04a36f93df7592640387ff2b831b636d5ec53b6aabe`
- Q4_K_M GGUF: 2,019,373,248 bytes,
  SHA-256 `dba8d50e3c63bde16cd918bb4de8e7cde6c5a2dae9a8360bd14480e71d8d7fa4`

The resulting Q4_K_M artifact passed the live native ask lane:

- derived tensor bytes plus data base exactly equalled the file size
- 9 / 9 generation gates passed
- 24 generated token IDs, 23 distinct
- explicit policy stop
- no local or remote fallback

A controlled same-prompt comparison found that this converted artifact and
the existing native base GGUF generated different token sequences. Because
the Ollama raw/form/prompted tensor lineage is digest-identical, that
difference is representation drift from dequantization and requantization,
not learned knowledge. The converted artifact is therefore not selected by
the live path.

The preferred route uses the already-proven native base GGUF and applies the
Form-owned `vital-ground-chat-v1-s4` prompt. It names that composition
`form-llama-vital-ground-prompted:base-native`; it does not claim changed
weights.

## Next ground

Two things remain unobserved:

1. A no-Python, Form-owned bridge that reads the manifest and safetensors
   headers, derives affine packing from weight/scale/bias geometry, emits the
   GGUF transformation plan, invokes a compiled local carrier, and witnesses
   every tensor receipt through `fkwu`.
2. Actual fine-tuning: changed tensor lineage plus held-out response transfer,
   followed by that native bridge and the same nine-gate re-witness.

A renamed prompt membrane is not accepted as learned weights, and a
Python-produced artifact is not accepted as a native bridge.
