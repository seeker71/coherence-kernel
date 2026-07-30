# 2026-07-30 — the mainline model runs Form-native, 43 layers, 96 gates

Urs: *"we stopped why?"*

No good reason. The remaining work was bounded and every piece was already built. Here it is done.

## It runs

```
VERDICT PASS  96 gates — 43 HETEROGENEOUS DeepSeek-V4-Flash LAYERS STACKED at real dims
              over the 85 GiB file, the four hyper-connection streams carried from each
              layer into the next
gate 3  heterogeneity: the 43 layers fall into 3 distinct (gate/up type, down type,
        expert count, routing regime, rope regime) groups
gate 6  blk.0 [gate/up 16 down 10 n_exp 256 hash rope plain] native self-witness:
        16384/16384 finite output entries, 16383 distinct bit patterns
wall: 2.43 s over 43 layers, mean 56.3 ms/layer
```

`gate/up 16 down 10` is IQ2_XXS and Q2_K read from the file's own table, `n_exp 256` unpruned. The
mainline DeepSeek-V4-Flash — the same file `ds4` runs at 31.40 tok/s — now goes through 43 layers of
kernels this body emits.

## What it took: three type dispatches

The reap25 file was MXFP8 and MXFP4 throughout. The mainline is Q8_0, IQ2_XXS and Q2_K, and every
kernel that reads a weight tensor had the old file's types hardwired. Reading one through the other's
decoder does not fail loudly — it yields NaN, and the first run said exactly that:
`native self-witness failed: finite 0/16384`.

| kernel | was | now |
|---|---|---|
| `gpuExpert` | 40 (MXFP4), 16 (IQ2_XXS) | **+ 10 (Q2_K)** |
| `gpuMx8` — the dense path | 41 (MXFP8) only | **+ 8 (Q8_0)** |
| `gpuGrouped` — attn_output_a | 41, and *planar* | **+ 8, blocked** |

That last one is the subtle one. The MXFP8 grouped kernel reads its scale from `qb[nel + g0 + g]` — a
separate **plane** after the payload, which is the reap25 file's layout. Q8_0 keeps its scale *inside*
the 34-byte block, so the plane offset would read payload bytes as exponents. Same grouping, different
reach; it needed its own kernel, not a branch.

All three emitters are `.fk` cells (`dsv4-forward-real.fk`), all three kernels are the body's, and
`q2k-msl.fk` / `q8-0-msl.fk` supplied the weight functions already proven on the device.

## The most surprising teaching

I found those three one at a time, and each cost a five-minute run. After the second failure I finally
ran one `awk` over the harness listing every function that touches `views[t.idx]` and which pipeline it
hardwires — seven functions, and it named the last remaining gap immediately. **That command was
available before the first run.**

A failure reports where execution *stopped*; it never reports how many stops remain. The source does.
When each diagnostic cycle is expensive, the enumeration is not an optimisation — it is the cheaper
instrument, and I reached for it third. `firststop` — 0 hits before this row, as are `oneatatime` and
`enumfirst`.

This is the same lesson `2026-07-28`'s pipeline-map receipt already carries — *"instead of doing one
gap at a time and trying to find another gap … do the full end-to-end analysis and place all the stones
in parallel"* — written down two days ago, by me, about this same model. Knowing a teaching and
reaching for it under pressure are different things, and the gap between them is measured in runs.

## Where discomfort turned to gold

Being asked "we stopped why?" when I had just written a receipt ending in "what remains, stated
plainly" — a tidy list of bounded work, presented as though naming it were a form of doing it. It was
three dispatches and about ninety minutes. The honest read is that the receipt's last section had
become a place to put things down rather than a plan to pick them up, and the tell is that it was
*well written*: precise about scope, correct about difficulty, and entirely inert.

## What is now true, and what is next

Form-native reads the mainline file at 43 layers with 96 gates green, and `ds4` runs the *same* file at
31.40 tok/s. For the first time both halves exist at once: an implementation and a reference on
identical weights. Every DS4 number from here can be checked against something other than itself.

The 2.43 s here is a **gated diagnostic** pass, not a generation loop — it reads every layer back to
judge it. The generation lane and the comparison against 31.40 tok/s are the next stone, and they are
now a measurement rather than a question.

## Ground stamp

```
form/native/metal/metal_dsv4_stack.sh, FORM_DS4_BLOB=<mainline>, 43 layers
  -> VERDICT PASS 96 gates, 108 PASS lines
  gate 3: 43 layers, 3 distinct type/expert/routing/rope groups
  gate 6: blk.0 16384/16384 finite, 16383 distinct bit patterns
  wall 2.43 s over 43 layers, mean 56.3 ms/layer
new emitters in form/native/metal/dsv4-forward-real.fk:
  dsv4-q2k-matvec-msl, dsv4-q80-matvec-msl, dsv4-q80-grouped-msl
ds4 on the same file: prefill 44.51 t/s, generation 31.40 t/s
```
