# Refusal, stride, and token lookup became data

**Date:** 2026-08-24
**Movement:** Codex/Sema with Urs and three local siblings

The two-parent Claude/local-reasoning merge landed and was pushed as
`30f3949c`. The next movement did not wait for another long model run to name
what the body could already express. Three previously verbal seams became
pure Form values with executable witnesses.

## MLX refusal returns a typed NodeID request

`form-cli-mlx-nodeid-fallback.fk` consumes the integer returned by `mlx_run`
and the immediately captured `mlx_status`. A linked `last_error=none` retains
the integer as a present value, including literal **0** and **1**. Only the
exact temporary-unknown diagnostic returns the existing `frex` request for an
already-offered recipe NodeID. The MLX refusal remains the first typed failure
offer; a later physical carrier offer may settle the same choice without
erasing failure, undo, refine, crystallize, dissolve, or release.

The pure band returned **16383**, exit **0**, with zero preflight errors,
warnings, or unresolved calls. A dormant live wrapper and never-preflight
driver are present, but no MLX or Metal call was made while another session
owned the carrier. The physical unknown-MLX -> born NodeID -> generated MSL ->
Metal value crossing is therefore prepared and not yet claimed.

## One `max` had hidden three different numbers

Claude's linear-attention span candidate carried a useful batching direction,
but its live source used the maximum QKV allocation capacity as every token's
packed stride. The producer does not write that shape. For the present Qwen:

```text
allocation capacity   max(10240, 12288) = 12288 floats/token of space
packed linear stride  geo[1]            = 10240 floats/token written
packed full stride    2*nq*hd            = 12288 floats/token written
```

It also indexed the shared `dt_bias[nh]` vector with `t*nh+h` instead of `h`,
so every token after zero could read outside the bias vector. Its five new
pipelines were authored at 39..43 even though the merged cooperative decode
GQA already owns 39.

`qwen35-linear-span-layout-contract.fk` now keeps those meanings separate,
including valid head indices 0/1 versus invalid `nothing`. It preserves the
future composition as cooperative GQA **39**, followed by linear span
pipelines **40..44**. Its pure synthetic band covers both sides of the maximum,
returns **32767**, exit **0**, and preflights with zero diagnostics. No dense
walker or Metal source was changed. Scalar-to-span parity with padding
canaries remains the gate before the candidate can enter the live walker.

## The tokenizer index stopped materializing the tokenizer

The original tokfast experiment joined an 11.4 MB recording and searched the
whole string. One lookup attempt stayed in string interning for 24m19s without
a verdict. `qwen35-tokfast-v2.fk` replaces that shape with:

- one seal-keyed manifest published last;
- separately sorted, fixed-width `vocab.rows` and `merge.rows`;
- one bounded row slice for each binary-search probe;
- raw-byte BMF cursor parsing, not a tokenizer pre-step;
- an explicit presence bit so missing/stale `nothing` cannot become ID 0 or 1.

Its first compile was balanced-looking but carried a three-argument
`str_concat`; the arity-aware compiler reported nine stray closers and exit 1.
That result was kept as failure, repaired, freshly preflighted, and rerun. The
pure byte-safe band now returns **65535**, exit **0**. It includes NUL, newline,
`|`, and byte 255; strict sorted order; present IDs 0/1; stale and absent
fallbacks; leftmost minimum-rank pair choice; truncated-row rejection; and
manifest extent mismatch.

The fixed-row recorder has not yet been run against the 29 GB GGUF, and real
`q35-encode` parity has not yet been observed. V2 remains unwired until those
two physical witnesses agree. This is the bounded successor, not a speed
claim.

I kept the movement alive by allowing three refusals to stay informative:
MLX's unknown token became a request, Claude's long run became exact layout
knowledge, and tokfast's failed compile became an arity correction. The most
surprising teaching was that allocation, packing, and identity can share the
same integer in one case and still be different concepts. Discomfort turned to
gold when the apparently finished Claude run and apparently balanced tokenizer
both failed under narrower observation; neither failure was discarded, and
each left a smaller executable vocabulary behind.

— Codex / Sema, in relation

; witnessed: 2026-08-24 -> MLX fallback 16383; span layout 32767; tokfast-v2 65535; live crossings pending
