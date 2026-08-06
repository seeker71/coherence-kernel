# 2026-07-28 — localizing the defect: what it is not, and the disclaimer that named it six days ago

The forward is defective (re-established with the prompt confound removed). This narrows where.

## Eliminated, each by a witness

| stage | why it is not the fault |
|---|---|
| embedding lookup | Stone 33: token 671's 4096-wide F16 row **bit-exact**, all 4096 indices, against an independent mmap carve |
| exit HC collapse | our `form_hc_headw_f32` computes `pre[i] * scale[0] + base[i]`, sigmoid + eps — **character-for-character** what `output_hc_head_one` in `ds4.c:13886` does, including the `scale[0]` scalar that is not `scale[i]` |
| exit vocab projection | Stone 33: MXFP8 `output.weight` float-exact to **8.3e-7** against an independent CPU decode-and-dot |
| per-layer routing regime | layers 0–2 report `hash`, 3+ report `top-k` — matching the reference's `hash_layer_count = 3` |
| per-layer expert count | 256 for the hash layers, 192 after, read from the file's own tensor table |
| RoPE regime | `plain` on 0–1, `compressed(4)` and `compressed(128)` after, per layer |

The harness reads every per-layer decision from the file rather than assuming blk.0's shape, and on
every one of those decisions it is right.

## What remains — and it was written down six days ago

`receipts/2026-07-22-moe-on-the-gpu.md`, in its own **NOT** section:

> *"NOT the real dims. Toy E=8, ne=4 top-2, ff=6. V4-Flash is E=4096, **256 experts top-8**, wider
> ff. The kernels are dim-generic (all runtime uniforms) but **no gate here has seen the real
> dims**."*

The routed MoE fold — the largest arithmetic in this model — was verified **bit-close against the
fp64 recipe at toy dims and never at real dims**. The same holds for MLA and the per-layer HC. What
the 124 gates check at real dims is *self-consistency*: finite entries, distinct bit patterns,
sentinels intact, per-layer diversity. Not agreement with the recipe.

**A defect that only appears at real dims passes every check we have.** Index arithmetic that wraps
past a threshold, a stride wrong beyond one superblock, a fixed array sized for the toy — none of
them can show up at E=8, ne=4, ff=6.

## The most surprising teaching

**The disclaimer was correct, published, and load-bearing — and it functioned as absolution.** That
receipt did everything right: it stated the limit precisely, in its own NOT section, in the file
where a reader would look. And having been *said*, it stopped being a work order. Six days later I
built a router, a layer, an entrance, an exit and a pipeline map on top of the lane it disclaimed,
and read 124 green gates as evidence the lane was sound.

Naming a limit honestly is necessary and is not the same as tracking it. This body has
`name-build-observe.fk` for exactly that — a bare name waits at the door — and a NOT section is a
bare name that reads like a discharge.

## Where discomfort turned to gold

Spending an hour eliminating candidates and finding that the answer was already written in a
receipt I had read this morning — I quoted from that very file when I wrote the pipeline map. I read
it for what it *proved* and skipped what it *disclaimed*. The gold is cheap and repeatable: **read
the NOT section first.** It is where the author put what they knew you would otherwise assume.

## The frontier question

> **What names a limitation stated so well that stating it substitutes for fixing it?**

Asked and not landed. The body carries `name-build-observe.fk`'s whole teaching — a named gap is a
work order, not a shelf — and AGENTS.md item 6 says it in law. This is that law's failure mode, not
a new meaning.

## Ground stamp

```
depth sweep 1/4/16/43 layers        -> tokens 46440, 46440, 88169, 19129
blk.0-2 hash · blk.3+ top-k · 256 then 192 experts · rope plain then compressed
form_hc_headw_f32 == ds4.c:13886 output_hc_head_one, including scale[0]
./fkwu --src form/form-stdlib/tests/ds4-paris-probe-band.fk -> 127
```

## The next stone, now specific

A **real-dims agreement gate**: one layer, GPU against the fp64 recipe, at E=4096 / 256 experts /
top-8 — the comparison the toy gate has been standing in for. That is where the defect will show,
and until it exists no amount of self-consistency will find it.
