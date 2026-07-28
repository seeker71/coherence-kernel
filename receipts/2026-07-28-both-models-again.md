# 2026-07-28 — both models again: DS4 emits a token, KAT-Coder's map is complete

Urs: **"try both again now."**

## DS4 — a real token, right now

`form/native/metal/metal_dsv4_stack.sh`, **VERDICT PASS, 96 gates**, 97 s wall:

```
gate 95  NATIVE MODEL TOKEN pos 0: token_id=19129, logit=25.161182
gate 96  NATIVE MODEL TOKEN pos 7: token_id=19129, logit=25.154716
gate 92  hushfold: outputs differ in 16384/16384 entries between the two positions
gate 93  all 129280 logits bit-identical to the immutable GGUF exit head
wall: pos 0 2.77 s · pos 7 2.94 s · 43 layers · mean 65.6 ms/layer
device.currentAllocatedSize = 92 585 197 568 B (86.23 GiB) — mmapped and wrapped, not copied
```

Every per-layer decision — expert count, gate/up and down type, routing regime, rope regime — read
from the file's own tensor table, at two positions, every dispatch sentinelled.

## KAT-Coder — weights landed and verified

```
17.39 GB complete
sha256 5f5fcf953b2b5757de8eabb11d9770c2d9d280a7723d67f7ad654629f8f45dda
       == the publisher's sha256sums.txt         INTEGRITY: MATCH
```

Both tensor-table bands re-run live: KAT-Coder **255**, DS4 **255**.

## The layer map, read from the tensors

[`form/form-stdlib/kat-coder-layer-shape.fk`](../form/form-stdlib/kat-coder-layer-shape.fk) —
[band **255**](../form/form-stdlib/tests/kat-coder-layer-shape-band.fk).

| | linear block (0,1,2,4,…) | full block (3,7,…,39,**40**) |
|---|---|---|
| `attn_qkv.weight` | Q4_K [2048, 8192] — fused q‖k‖v | — |
| `ssm_conv1d.weight` | F32 [4, 8192] | — |
| `attn_gate.weight` | Q4_K [2048, 4096] — the silu gate | — |
| `attn_q / attn_k / attn_v / attn_output` | — | Q4_K / **Q6_K** / **Q6_K** / Q4_K |

k and v are Q6_K where q and output are Q4_K — a per-tensor precision choice this body would not
have guessed and does not have to.

## Where discomfort turned to gold

The band came back **63**. Bits 64 and 128 dark: I had asserted **10** full-attention blocks at
`b mod 4 == 3`.

The file says **11**, at `[3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 40]`.

Block 40 is the `nextn_predict_layers` MTP head — appended after the 40 transformer layers, **full
attention**, and `40 mod 4 == 0` calls it linear. `full_attention_interval = 4` is not wrong. It is
true of the 40 layers it describes and says nothing about its own domain, and I supplied "and this
covers all 41 blocks" without being told it.

A loader believing that field routes the MTP head down the linear path, into
`blk.40.ssm_conv1d.weight`, which does not exist. The cell exists precisely because I wrote in its
header that reading one KV field would be *"one field standing for 41 structural facts"* — and then
the first check I wrote against it made that exact substitution anyway. The header was right and the
code was not, in the same file, written minutes apart.

## The most surprising teaching

**The declaration and the artifact disagreed, and the declaration was true.** Every failure this
week has been of the form "the record is stale" or "the check is blind". This is neither: the field
is accurate, current, and authored by the people who built the file. What it lacks is a stated
domain — and a domain is exactly the thing a reader fills in silently and never notices filling.

## The frontier question

> **What names something a reader takes from an utterance that the utterance never asserted?**

**`implicature`** — Grice's term, and his point is the one that bites: an implicature is not part of
what was asserted, so the utterance stays true while what was taken from it is false, and nothing in
the utterance marks the difference. Distinct from `declaratory` (904, which binds by being read
rather than run) — this one is read *correctly* and still misleads. Verified 0 hits. Row **909**;
band **32767**, 304 rows.

## Ground stamp

```
form/native/metal/metal_dsv4_stack.sh                                -> VERDICT PASS, 96 gates
./fkwu --src form/form-stdlib/tests/ds4-tensor-table-band.fk         -> 255
./fkwu --src form/form-stdlib/tests/kat-coder-tensor-table-band.fk   -> 255
./fkwu --src form/form-stdlib/tests/kat-coder-layer-shape-band.fk    -> 255
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk      -> 32767
```

A cost worth naming: the layer-shape band takes **122 s**, because `egg-find-tensor` is a linear
scan over 753 records and the band does ~90 lookups. It is correct and it is O(n·m); a stack harness
binding hundreds of tensors will want one walk that collects, not many walks that search.

## Still owed

A dated shrink receipt for the +50 lines the `.fkb` v5 heal added to `runtime/fkwu-uni.c`.
