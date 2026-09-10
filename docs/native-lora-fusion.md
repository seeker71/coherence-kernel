# Native LoRA fusion

`./fkwu observe/native-lora-fuse-run.fk` reads a base directory, adapter directory,
and stable output root on three stdin lines. Form loads the actual adapter,
applies each `scale × A × B` delta on Metal, and requantizes each changed
projection to affine four-bit weights. Unchanged tensors retain their original
bytes. Changed projections use F32 scales and biases. This is lossy
requantization: each projection records its measured maximum weight error and
rounding bound in `fusion-errors.jsonl`.

`tle-fuse` uses this door through `host_spawn_at` and `host_wait`. Each call
retains its request, stdout, stderr, and actual child exit in a unique run
directory. There is no external model runner. A failed child returns failure;
an existing model is not evidence that the child succeeded.

Fusion writes a candidate model directly into shared file pages, checks every
tensor byte against the resident source buffer, requests OS writeback, then
publishes a complete directory. `native-fused-current` selects the generation
with one atomic rename. Existing generations remain available. Inference
callers resolve this pointer; the FIFO peer reloads when its value changes.

Reuse requires current content seals for the base weights, configuration,
tokenizer, adapter weights, adapter configuration, fusion implementation, and
all six output files. Input seals are checked again before publication. Form
computes these on Metal using 65,536-byte SHA-256 leaves and a domain-separated
SHA-256 root over the file length and ordered leaf digests. The identity is
named `sha256-tree-65536-v1`; it is **not** the ordinary SHA-256 of the file.
Paths, mtimes, or file sizes alone cannot establish freshness. Padding edges
and a multi-leaf root are checked against the independent Form SHA-256 recipe.

Observed on 2026-09-10: the existing 16-layer authoring adapter contributed
112 pairs to a 648-tensor, 1,908,160,008-byte standalone model. Merge and input
sealing took 1,799 ms; save and verification took 14,951 ms. A subsequent
content-checked worker call reused the same generation in 851 ms. The public
unknown-word probe answered `nothing` and reached EOS before and after fusion.
That one probe establishes a real execution crossing, not general quality
parity with the former runtime. Both processes released all model buffers.

Witnesses:

- `form/form-stdlib/tests/native-lora-fuse-band.fk`: independent scalar arithmetic
  for all 4,096 fixture weights, mixed U32/F32 reload, zero retained buffers: 7.
- `form/form-stdlib/tests/native-content-seal-band.fk`: FIPS vector, padding
  edges, independent tree root, zero retained buffers: 15.
- `form/form-stdlib/tests/native-lora-fusion-current-band.fk`: legacy artifact
  refusal, unchanged content admission, same-length input/output mutation,
  and actual child exit 1: 63.
- `observe/native-lora-fusion-homecoming-run.fk`: full local authoring adapter,
  training-loop caller, content reuse, and native before/after generation.

The current file writer uses 32-bit payload offsets; its supported published
file size is below 4 GiB. Larger publication is explicitly refused. Generation
retention currently keeps all outputs; automatic pruning is still separate work.
