# Public BML context transfer observation

Date: 2026-09-01  
Author: Codex

## Two separately sealed local turns

| condition | row/family | route | exact | f1 | semantic | promotion | latency |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: |
| unassisted resident baseline | `v304` / `bootstrap` | `direct-answer` | 0 | 0 | 0 | 0 | 15,205 ms |
| public BML context | `v305` / `form-stdlib` | `direct-answer` | 0 | 222,222 ppm | 0 | 0 | 78,436 ms |

Both turns passed the same sealed dataset hash
`31af3c7901d024d4772ab5466ac059e6db43e683262512cda67ac76a996ccf48`.
The second task carried the public BML profile with hash
`604ce96b30ae8d82aadb5c12f4d79af64601f38464b8e0bd9f9376de1fa6ad73` and
profile length 498 bytes. It had no source-query, remote, or expected-answer
material. Both replies were direct Metal resident replies with no callback
crossing; response content remains in the caller-owned durable spool and is
not repeated here.

## Decision

**Do not promote the public-context route as local learning.** The rows are
distinct families, so their numeric scores are not a before/after comparison;
more importantly the adapted turn did not reach an exact or semantic hit and
took 5.16x the baseline latency. The context was task-scoped and leaves no
resident mutation to undo. Keep the BML sender as an observable experiment
door, while its route remains unpromoted and absent from default answering.

The next real transfer crossing is model-specific: train only public data into
a local adapter, fuse or load that adapter through a Form-native inference
seat, then score a fresh held-out cohort against the same base/model family.
MLX LoRA and a cached local Llama base exist on this Mac, but the serving
resident is Qwen GGUF and today has no native adapter-loader seam; equating
either historical adapter files or prompt context with learned resident weights
would be false.

I kept the exchange alive by accepting the partial overlap as information while
refusing to promote it into a capability claim. The surprising teaching was
that a longer public context raised latency far more than semantic score. The
discomfort of another zero became the boundary that protects the next training
movement from being theatre.
