# Linear-attention span entered the dense walker

**Date:** 2026-08-24
**Movement:** Codex/Sema with Urs and the local sibling line

The completed Claude candidate carried a valuable direction: move the token
axis into the eleven independent linear-attention operations while keeping the
conv and delta recurrences ordered. Reading its first generated addresses
exposed three defects before another model run could make them expensive:

- the shared `dt_bias[nh]` vector was indexed by packed activation identity;
- allocation capacity was used as QKV stride without a producer-side pitch;
- its first new pipeline collided with cooperative decode GQA at 39.

The selective movement now lives in
`form/native/metal/qwen35-dense-token-handle.fk`. Existing pipelines 0..39 are
unchanged. The five Form-generated MSL kernels append as l2norm 40, gates 41,
norm-gate 42, conv-offset 43, and delta-offset 44. Their dispatches read those
indices from the pure layout contract rather than renumbering an earlier lane.

During the rebase, newer ground on `origin/main` changed that second fact rather
than merely contradicting it: the batched producer now accepts `ystride` and
writes `y[t*ystride+r]`. Both full and linear projections into the shared bs2
buffer explicitly bind the selected 12288-float pitch; qsplit receives the same
pitch as `sstride`; and linear l2norm/conv/delta receive it too. The allocation
capacity and output pitch are numerically equal here only because the producer
is explicitly bound to that pitch. Capacity alone still does not imply layout.

Three independent token offsets are computed as Form data and used by the two
recurrence loops:

```text
bs2 qkv   t * output_pitch
alpha/beta t * nv
output    t * nv * dv
```

The gate emitter uses packed `o=t*nh+h` for activations and `dtb[h]` for the
shared parameter. The conv and delta loops retain token order; projections,
silu, l2norm, gates, norm-gate, output projection, and residual operate over the
span. Only the layer-major prefill lane selected by chunk `-3` reaches this new
linear recipe. Chunk `-2`, scalar/token-wise attention, full-attention span,
width-bounded RMS selection, and cooperative decode remain present unchanged.

The new pure emitter/address band observes both ends of the pitched crossing:
the producer's `ystride` write and qsplit's `sstride` read, alongside independent
linear/gate/output offsets, present 0 versus invalid `nothing`, `dtb[h]` and
absence of `dtb[o]`, five unique emitted names, and pipeline continuity 39..44.
The underlying layout contract independently exercises both sides of the
max-width choice. Their post-rebase verdicts are recorded below rather than
inherited from the earlier packed-layout witness.

The layout contract was then placed before the dense walker in every canonical
`form-cli` source/build/regeneration mirror. The bounded tokfast-v2 recorder and
lookup organ was also added after the first post-rebase build exposed that it
was witnessed but absent from the executable recipe. Final regeneration
answered `pong` and produced **3358 callable functions**, **112379 nodes**, and
**609704 table tokens**, stamp **4cdac5f7008cee23**. The generation attestation
gate passed. The rebuilt self-contained recipe is **4508728 bytes**, carries
**2303858 bytes** of its own current source, answers `pong`, and its `source`
response contains `q38-linear-attn-span`, `qlslc-bs2-output-pitch`, and
`qtf2-rec-begin`.

The physical dense-handle band now expects 45 handles, but it was neither
preflighted nor run. No Qwen model was opened, no MSL was compiled by Metal, and
no carrier function was invoked while another session owned that organ.
Scalar-to-span numerical parity, padding canaries, and a physical 40..44 handle
witness remain owed before performance or model-output correctness can be
claimed.

I kept the movement alive by letting the candidate's useful schedule survive
its incorrect addresses, then making each address independently observable.
The most surprising teaching was temporal: the widest buffer dimension was not
a layout fact in the first producer, then became a valid output pitch when a
later producer accepted it explicitly. Discomfort turned to gold when an
apparently settled defect diagnosis had to be re-witnessed against changed
ground; token identity, head identity, logical width, output pitch, and capacity
remain separate Form values even when two of their integers agree.

— Codex / Sema, in relation

; witnessed: 2026-08-24 -> layout 32767; emitter/address 262143; form-cli pong + source embodiment; physical Metal parity pending
