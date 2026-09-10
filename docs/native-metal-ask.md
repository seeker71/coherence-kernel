# Native Metal ask

`form/form-stdlib/bml/metal-ask.bml` owns the GGUF request in one `fkwu`
process: model and prompt admission, longest-match vocabulary tokenization,
RAM program generation, mapped quantized weights, the KV state, decoding,
costs, content identity, release and answer publication. The Metal framework
enters through the dynamically admitted host adapter.

Run `./fkwu observe/metal-ask-run.bml` with one JSON line on stdin:

```json
{"prompt":"The capital of France is","max_tokens":12,"artifact":".hearth/native-metal-ask/answer.txt"}
```

The optional `model` is `llama3.2:3b`. The optional `blob` may locate the same
registered content elsewhere. Full-file SHA-256 is measured before admission
and after generation; a different model or changed content withholds the answer.
The default positive token cap is 96. The existing `form-metal` and
`llama32.form-metal` routes retain their 12-token default and environment
overrides through the private file-input door. Prompt bytes remain exact.
The JSON door requires complete object syntax and typed request fields before
model admission. Malformed syntax, duplicate fields and wrong field types are
refused. Answer files are created privately at mode 0600 before any content is
written; newly owned parent directories use 0700. Existing directories retain
their permissions.

The answer artifact retains the `ASK-NATIVE v1` question binding read by
`ask-native-lane.fk`. It names the actual EOS or cap stop, the model digest and
runtime observation. A generation does not claim that it ran the benchmark
suite. Tensor traffic and MAC totals are labeled derived; timing, dispatches
and synchronization counts are measured. Joules and an external denominator
remain absent until measured.

Every ask owns and releases its tensor buffers and program admissions.
`sha256-owned.bml` gives each content hash an independent CPU admission while
sharing the compiled image. A failed release retains its handle on the owner.
Hash admission requires a complete final read and unchanged file extent;
settlement checks the extent again after releasing resources.
Admission failures retain correlated organ observations, responses and applied
abstentions. All three rows travel through the process diagnostic channel;
prompt and answer bytes do not enter the framebuffer.

The physical witness `observe/metal-ask-live-run.bml` reads the registered
2,019,377,376-byte GGUF, produces all twelve expected token IDs and text,
checks the 8,177-dispatch/17-sync execution, verifies question binding and
returns live buffers and pipelines to their incoming counts: **255**.
The admission/refusal witness is **511**. Independent Form SHA agrees at all
tested padding boundaries, with a separate shared-image owner surviving and
actual missing/short reads and changed extents refused: **16383**.
These checks establish their stated cases, not arbitrary model support
or peak bandwidth.

The remaining foreign Metal scripts have distinct active responsibilities:

- `metal_first_token.sh` still carries the broad numerical and alternative
  kernel comparison suite. `metal_uncertainty.sh` splices its per-forward
  whole-logit statistics into that suite. Native ask parity does not replace
  these measurement cases.
- `metal_batched_prefill.sh` owns batched-versus-sequential prompt comparisons.
  The GGUF ask currently uses one forward per prompt token.
- `metal_isa_diff.sh` and the tensor residency audit scripts measure compiler
  and mapping comparisons. `ollama_oracle.sh` obtains an explicit external
  denominator; the native ask does not invoke it.
- The DeepSeek, MoE, KAT, GDN and quantization scripts carry separate model or
  operator witnesses. They require their own native replacements and checks.
- `native_model_route.sh` still carries its explicit Ollama comparator and
  challenger package route. Its direct Metal branch only carries environment
  and stdin bytes to Form. Those comparator branches remain unresolved native
  migration work.

The north star is one resident Form owner with reusable model, tokenizer and
content observations; reusable independently retired CPU/GPU programs; native
batched prefill; streaming and cancellation; and the same measured reasoning,
choice and resource contracts across all admitted architectures.
