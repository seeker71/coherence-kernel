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

The real-model CPU oracle remains active at
`form/form-stdlib/tests/dsv4-mla-core-oracle.py`, called by
`metal_dsv4_layer.sh`, `metal_dsv4_layer_join.sh` and
`metal_dsv4_stack_oracle.sh`. Its independent GGUF parser, F16/F32/MXFP8/MXFP4/
IQ2_XXS decode, YaRN choices, fp8-plus-f16 KV round trip, routed experts and
multi-token cache history are still needed by those checks. The available
91,321,404,640-byte GGUF is observed on this host; those foreign execution paths
are not run by the native reference witnesses.

The next native boundary is an independent mapped CPU reference for the real
GGUF, preserving its full intermediate vectors, layer/stack modes, positional
contrasts and tolerances. Once its native checks own those responsibilities,
the real-model oracle and its foreign carriers can be released.
