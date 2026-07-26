# Receipt — Form-native model healing, carrier logic removed from authority

Date: 2026-07-26

## What moved into Form

Three executable decisions now run directly as Form programs on `fkwu`:

- `form/form-stdlib/native-model-native-route-cli.fk` owns native-first model
  selection;
- `form/form-stdlib/nanbeige-native-metal-readiness-cli.fk` owns staged
  Nanbeige admission;
- `form/form-stdlib/ds4-quant-layout-cli.fk` walks the full DS4 GGUF and owns
  layout versus arithmetic readiness.

Bash and Python do not decide any field in these receipts. A stdin carrier may
offer an artifact path, but Form opens the file, parses the GGUF header, walks
all tensor rows, computes byte geometry, and renders the verdict.

## Nanbeige live Metal foundation

The admitted 2,574,807,936-byte Q4_K_M artifact was mapped once into one
zero-copy Metal buffer. Form emitted all 201 tensor rows and exact offsets.
The existing Form-authored Q4_K/Q6_K kernels then proved:

- head and tail dequantization for both formats;
- whole-tensor dispatch over 33,030,144 weights per sampled tensor;
- fused matvec parity against Form at full tensor width;
- all 22 stored `ffn_down` layers dispatched from the one resident artifact;
- a 44-slot cache geometry (`22 stored layers * 2 loops`) with 8 KV heads and
  head dimension 128, allocated once and replay-verified.

The audit carrier's old hard-coded 28-layer Llama cache statement and 32-bit
file-size rendering were repaired during this witness. The rerun reported the
actual 2,574,807,936-byte artifact and 44 cache slots.

This establishes the direct-Metal weight foundation. It is not yet a full
Nanbeige token. Form's staged receipt currently reports:

```text
stage_count=13
observed_stages=7
native_port_stages=2
ready_stages=2
generation_ready=0
status=pending:native-quantized-metal-execution-port
```

On a fresh live re-witness, the admitted released model evaluated 25 prompt
tokens and emitted three real token ids before EOS:

```text
152343 -> " "
152395 -> "4"
152375 -> "2"
decoded -> "42"
```

Those ids now live as Form data in
`nanbeige-native-metal-readiness.fk`; they are the pinned parity target, not a
synthetic fixture and not evidence that the direct-Metal forward already
matches. Consequently the reference stage is observed while its native-port
and parity fields remain zero.

The absent graph stages are the 48x128 query state, factor-free RoPE,
between-loop final normalization, untied output head, composed two-loop
forward, and pinned token parity.

## DS4 live native coverage

The full 91,321,404,640-byte artifact was scanned directly by the Form cell:

```text
tensor_count=1406
layout_known=1406
arithmetic_known=1406
unknown_layout=0
type40_tensors=45
type41_tensors=370
encoded_tensor_bytes=91316062968
layout_complete=1
execution_ready=1
```

Form now owns byte geometry for every format present:

- F32: 4 bytes / 1 element;
- F16: 2 / 1;
- IQ2_XXS: 66 / 256;
- I32: 4 / 1;
- plane-split MXFP4 (GGUF 40): 34 / 64;
- plane-split MXFP8 (GGUF 41): 132 / 128.

Reunion with current `main` falsified the pinned DS4 engine's unused 36/64 and
18/128 rows. The artifact's offsets establish plane-split MXFP4/MXFP8, and the
merged body already carries their native carvers/MSL plus IQ2_XXS arithmetic.
All 1,406 tensors therefore have native layout and arithmetic. A complete
decoded-token witness remains a separate pending claim.

The encoded tensor sum is 1,928 bytes below `file_size - data_base`; those are
GGUF inter-tensor alignment bytes, not unknown tensor payload.

## Proofs

Focused sibling validation:

```text
nanbeige-native-metal-readiness-band.fk -> 63
1 ok, 0 divergent

ds4-quant-layout-band.fk -> 63
1 ok, 0 divergent

native-model-native-hierarchy-band.fk -> 127
1 ok, 0 divergent
```

## Remaining primitive boundary

The checkout's native manifest contains `metal_matvec_f32`, but the
self-contained `form-cli` reports its Metal carrier as unlinked, and that
primitive only covers a small dense f32 model. Calling an old shell harness
from Form would not remove the membrane.

The next irreducible build is a linked, quantized Metal execution port whose
request is Form data and whose receipt returns graph-stage observations. The
native router refuses promotion until that port executes the remaining graph
and token-parity stages.

; witnessed: 2026-07-26 -> carrier decisions moved to Form; Nanbeige weight
; foundation live on Metal; DS4 1406/1406 layouts native; generation still
; refused where the native execution/arithmetic ports do not exist
