# Native DS4 numeric reference

`form/form-stdlib/bml/dsv4-numeric-reference.bml` independently computes the
small hyper-connection and complete first-token forward references. It reads
the same fixture weights as the production recipes, with separate flat-array
HC storage, dimension-first accumulation, Sinkhorn normalization, latent
attention, rotary arithmetic, grouped projection, expert selection and SwiGLU.
It calls no production HC, MLA or MoE arithmetic recipes.

The HC witness compares every split, weighted sum, post step, complete pre step
and output head against both the native reference and retained numeric
anchors. Its six existing checks retain their thresholds and return **63**:

```sh
./fkwu form/form-stdlib/tests/dsv4-hc-band.fk
```

The complete forward witness compares all vocabulary logits for two fixture
configurations against both sources at the original 1e-6 tolerance. It also
compares every final HC stream and the complete shared-only and unclamped
outputs. The six existing checks remain, joined by numeric sine/cosine anchors
and position-7 forward agreement: **127**.

```sh
./fkwu form/form-stdlib/tests/dsv4-forward-band.fk
```

These are fp64 algebra witnesses over small invented weights. They do not
establish real model, quantized storage, f32 or GPU equivalence. Shared fixture
generation is input data; independent arithmetic and unchanged literal anchors
provide separate comparisons. The source provenance is ds4-engine's MIT
`ds4.c`: `hc_split_sinkhorn_one`, `hc_post_one`, `layer_forward_self_one`,
`layer_topk_selected_experts_from_probs` and `output_hc_head_one`.

The separate [real-model Form oracle](native-dsv4-oracle.md) owns an independent
GGUF parser, F16/F32/MXFP8/MXFP4/IQ2_XXS reads, YaRN choices, the fp8-plus-f16
key/value round trip, routed experts and per-layer multi-token history. Its
matrix programs execute in RAM over mapped weights and preserve complete
intermediate vectors. The three Metal comparison callers use its physical
request door at `observe/dsv4-oracle-run.bml`.

The small algebra witnesses above and the real-model comparisons establish
different observations. Their inputs, execution contracts and tolerances remain
explicit. The numerical north star is a complete Form-owned graph retaining
activation spans and replaceable CPU/device programs across the computation.
