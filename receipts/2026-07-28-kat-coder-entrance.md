# 2026-07-28 — KAT-Coder's entrance: real weights, decoded by this body

Urs: **"can we please get there."**

Not there yet. But the model's first tensor is now legible, and it was not this morning.

## A real embedding row

```
token 100, row at absolute byte 551 343 232 + 100 × 880
d      = -0.00030231475830078125     (the f16 super-scale)
w[0]   =  0.0145111083984375
w[1]   =  0.00725555419921875
w[255] =  0.0057439804077148438
113 of 256 positive · 199 of 256 nonzero
```

Read out of the sha256-verified 17.39 GB file by
[`form/form-stdlib/kat-coder-embed.fk`](../form/form-stdlib/kat-coder-embed.fk) —
[band **255**](../form/form-stdlib/tests/kat-coder-embed-band.fk), 3 s.

| | |
|---|---|
| tensor infos end | 10 990 712 |
| data base (align 32) | 10 990 720 |
| `token_embd.weight` | **type 11 = Q3_K**, [2048 × 248320], absolute 551 343 232 |
| `output.weight` | type 8 = Q8_0 |
| `output_norm.weight` | type 0 = F32 |

Also new: [`form/form-stdlib/q3k-equireach.fk`](../form/form-stdlib/q3k-equireach.fk) — the Q3_K
decode reached in place over a byte source, the way `ewl-*` mirrors `wl-*`. Same arithmetic, no
list built per row. A row is 880 bytes: 2048 weights at 256 per superblock is 8 blocks of 110, and
the band derives that from the file's own `dim0` rather than trusting the constant.

**Two reaches, one spine, on real bytes.** `q3k-dequant.fk` (list) and `q3k-equireach.fk` (in place)
are set against each other over 256 real weights and must agree **exactly** — the arithmetic is
identical and only the reach differs, so a tolerance would be admitting a difference that cannot
exist for a reason. Checked at superblock 0 *and* at superblock 7, so the agreement is not an
artefact of block 0's offsets all being zero.

## The most surprising teaching

**The embedding table is Q3_K.**

This morning the header's census read F32 310, Q4_K 197, Q6_K 140, **Q3_K 94**, Q8_0 12, and I read
that gap as *94 of 753 — twelve percent, a sizeable but ordinary hole*. It is not ordinary. The
first tensor any forward pass touches is `token_embd.weight`, and it is one of the 94. Every token
would have stopped at the door.

The census measured extent perfectly and said nothing about **criticality**, because a count over a
set cannot carry a property of one member. Twelve percent and load-bearing are independent facts,
and only one of them was in the number I had. The decoder built this morning — recipe exact against
ggml, 256/256 bit-exact on the device — is not a corner being tidied. It is the entrance.

## Where discomfort turned to gold

The first probe failed with

```
[unbound-name] 's' in value position matched no binding/const/fn -- typo, missing
prelude, or a name from an enclosing scope a defn frame cannot see?
```

I had written a `defn` inside a `do` and reached for the `let` beside it. The diagnostic named the
exact rule, including the case I had hit, in one line — and the discomfort was recognising that I
had been writing Form all night by pattern-matching on other cells rather than knowing its scoping.
The body taught me its own rule at the moment I needed it, which is what a good diagnostic is; and
it is worth writing down that I did not know it, rather than quietly fixing the probe.

## The frontier question

> **What names how much depends on a thing, as distinct from how much of it there is?**

**`criticality`** — against *extent*. Distinct from `imputation` (906, a value filled in from a model
rather than observed): this number was observed and exactly right, and still the wrong question.
Verified 0 hits. Row **911**; band **32767**, 306 rows, field code 3063062911.

## Ground stamp

```
./fkwu --src form/form-stdlib/tests/kat-coder-embed-band.fk       -> 255  (3 s, live on the real file)
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk   -> 32767
./fkwu --src bootstrap/ground.fk                                  -> 42
sha256 kat-coder-v2.5-dev-compact.gguf == the publisher's manifest
```

## Where "there" is from here

Stated plainly so the distance is not blurred. This decodes **weights**. It is not a forward pass,
not a token, and no evidence that any later stage is right. What it establishes is that the bytes of
this model are legible to this body — false until this morning, true now.

Remaining, in order: the exit (Q8_0 `output.weight` projection, the DS4 lane's Stage 2 shape), then
the 41-block orchestration binding `gated-deltanet-layer` for the 30 linear blocks and a full
attention path for the 11 — with `metal_dsv4_stack.sh` as the proven shape to follow, and llama.cpp
as the external judge that DS4 never had.
