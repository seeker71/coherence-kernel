# Native Llama and LoRA on Metal

Form reads the local Llama 3.2 3B affine 4-bit safetensors checkpoint and its
existing LoRA adapter. Base weights stay mapped. Form supplies Metal source,
dispatch bindings, tensor ownership, rotary frequencies, tokenizer rules,
reverse-mode gradients, Adam updates and checkpoint metadata.

The supported architecture has tied embeddings, grouped-query causal attention,
RMS normalization, SwiGLU, affine nibble quantization and optional Llama 3 rotary
scaling. Configuration and tensor dimensions are checked before execution.
Every admitted low-rank A/B pair participates in forward and backward execution.
The existing voice adapter has 56 pairs: all seven projections in layers 20–27.
Arithmetic uses FP32 activations; bitwise agreement with an FP16 implementation
is not claimed.

Safetensors payloads can be unaligned. Byte-addressed device reads preserve their
meaning without expanding or copying the base weights on the host. Mutable
adapter factors have their own GPU buffers. A checkpoint copies those buffers
directly into a shared file mapping, verifies every byte, requests OS writeback
and publishes the file by rename. Optimizer moments are independently serializable.

Entry points:

- `nll-open`, `nll-context`, `nll-forward`, `nll-forward-close`, `nll-close` own
  inference buffers and the training tape.
- `nlb-state`, `nlb-backward-masked`, `nlb-safe-step`, `nlb-state-close` implement
  full LoRA training with explicit target masks and finite-gradient checks.
- `nlc-save` and `nlc-load-optimizer` persist actual parameters and moments.
- `nlg-open`, `nlg-ask`, `nlg-close` retain model/tokenizer ownership for callers.
  Generation emits tokens, grows KV capacity as needed and reports EOS,
  cancellation, caller token limits, architectural limits and memory pressure.
  A zero caller token limit means streaming until another stopping condition.
- `observe/native-llama-homecoming-run.fk` exercises the real local checkpoint,
  generation, a full LoRA update and byte-verified saving using a public fixture.

The tokenizer reads the selected model's Hugging Face JSON vocabulary and merge
table. The Llama alternation follows the model's tokenizer pattern, including
case-insensitive contractions and three-number groups. Unicode categories come
from the versioned data under `form/form-stdlib/data`. The chat scaffold currently
uses the tokenizer template's deterministic date branch, 26 Jul 2024. This is
template parity data, not a claim about today's date.

Focused witnesses are `native-affine-metal-band.fk` (127),
`native-affine-file-band.fk` (15), `native-llama-backward-band.fk` (7),
`native-lora-checkpoint-band.fk` (3) and `native-llama-tokenizer-band.fk` (31).
They live under `form/form-stdlib/tests`. The backward witness checks 28 finite
differences, covering both factors of every projection in a two-layer GQA model.

Current integration work remains in the existing voice/corpus training callers,
model fusion and the large Whisper speech path. These organs establish the
native execution and learning path; their presence alone does not remove those
remaining external calls. Tokenizer construction and repeated inference buffer
allocation also remain measured performance work.
