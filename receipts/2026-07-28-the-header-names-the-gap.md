# 2026-07-28 — the header names the gap I had been estimating around

Urs: **"next? why did you not already do it?"** Because I had written the next step down instead of
taking it. Fourth time tonight, and this one cost something specific.

## What was read

An HTTP range request over the first **25 165 824 bytes** of
`Kwaipilot_KAT-Coder-V2.5-Dev-APEX-MTP-I-Compact-v2D-lite.gguf` (17.39 GB total). That window
carries the entire header: GGUF v3, 50 KV pairs, and all **753 tensor infos**, which begin at byte
10 943 173. **No weights were downloaded, nothing was run.** Every number below is a header field.

| cell | verdict |
|---|---|
| [`form/form-stdlib/kat-coder-tensor-table.fk`](../form/form-stdlib/kat-coder-tensor-table.fk) | the census and the shapes |
| [`form/form-stdlib/tests/kat-coder-tensor-table-band.fk`](../form/form-stdlib/tests/kat-coder-tensor-table-band.fk) | **127** |
| [`learn/homecoming-distillation-corpus.fk`](../learn/homecoming-distillation-corpus.fk) row 905 `imputation` | corpus **32767**, 300 rows, 3003002905 |

## The gap

```
F32   310   no decode needed
Q4_K  197   q4k-msl.fk          COVERED
Q6_K  140   q6k-msl.fk          COVERED
Q3_K   94   nothing.            THE GAP
Q8_0   12   q8-0-msl.fk         COVERED
753 total
```

**This body has no Q3_K decoder** — not in Metal, not in Form. Ninety-four tensors. `grep -rIil
"q3_k\|q3k" --include="*.fk"` returns nothing.

## What the header confirms

Three sources now agree on the same architecture through completely different field names — the
model card, `config.json` + `modeling_qwen3_5.py`, and now the loader's own tensor table:

- `ssm_conv1d.weight` is **[4, 8192]** and `attn_qkv.weight` is **[2048, 8192]**. Four taps over
  eight thousand channels, and the channel count *is* the qkv width — so the conv runs over the
  **concatenation**, exactly as `gated-deltanet-layer.fk` built it from the reference. Per-projection
  convs would have been three smaller tensors. Band bit 4 pins that identity.
- `ssm.group_count 16`, `ssm.state_size 128`, `ssm.inner_size 4096` → 4096/128 = **32 value heads
  over 16 key groups**, the same 2:1 grouping `gated-deltanet-conv.fk` read from `config.json`.
- `ffn_gate_inp.weight` is **[2048, 256]** and **F32**, with `expert_used_count 8` — the shape
  `moe-route-wide-msl.fk` routes. And F32, not F16, so `mm-f16mv-msl`'s F16 path (written for
  mixtral's router gate) is not needed on this model.
- `block_count 41` = 40 layers + one `nextn_predict_layers` MTP head; `full_attention_interval 4`.

## The most surprising teaching

**Three times tonight I wrote what remained for this model, and all three lists were imputed.**

*"What's left isn't new tissue: the GGUF tensor table, the projections, and the full-attention layer
every fourth position."* Reasonable, architecturally sound, and derived entirely from a model of the
model. None of the three could name a missing **quantization**, because none of them had opened the
file. Q3_K sat there through all of them.

The lists were not wrong about what they covered. They were wrong about being complete, and nothing
in them could have signalled that — an estimate of an unopened container reports the model I hold,
never the cargo. What made this one different is a 25 MB read that cost less than any of the three
paragraphs did.

## Where discomfort turned to gold

The band bit I nearly wrote backwards. My first instinct was a bit asserting **full coverage** —
every quantization used has a decoder. That bit would have been **dark**, and to land the cell I
would have had to either build a Q3_K decoder first or edit the test to admit the gap.

An inconvenient truth that requires editing a test in order to be stated is a truth that gets
deferred. So bit 8 asserts the gap is **reported**: exactly one uncovered kind, exactly 94 tensors.
The band stays green while naming precisely what is missing, and goes dark if someone quietly marks
Q3_K covered without a decoder behind it. The discomfort was noticing I had been about to design a
test that punished honesty.

## The frontier question

> **What names a value filled in from a model rather than observed?**

**`imputation`** — the statistical term. Honest when labelled, a lie the moment it is read as a
count. Distinct from `autoepistemic` (897, not-found read as not-there): nothing here was searched
for and missed — the container was never opened. Verified 0 hits. Row **905**.

## Ground stamp

```
./fkwu --src bootstrap/ground.fk                                        -> 42
./fkwu --src form/form-stdlib/tests/kat-coder-tensor-table-band.fk      -> 127
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk         -> 32767
```

## The path, restated from the file rather than from a model of it

A Q3_K decoder — Form recipe, Metal emitter, device gate — is now the named next stone, and it is
named by the artifact instead of by me.
