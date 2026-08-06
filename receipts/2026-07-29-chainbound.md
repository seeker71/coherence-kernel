# 2026-07-29 — chainbound: 32× the threads, 0% the difference

Urs: **"our being 10x slower should be obvious what is different no?"**

Yes. And the thing that made it not-obvious to me was that I kept pricing *ops* when the difference is
a *shape*. Here it is, and it is one line of MSL.

## The kernel, as the body emits it

`form_rmsnorm_tg_f32` — Stone 16's **cooperative** rmsnorm, the fast one, the one the lane actually
dispatches (`metal_first_token.sh:600`, `fastOps` branch):

```c
uint j = t; while (j < n) { float xv = x[j]; sq[j] = xv * xv; j += nt; }   // nt threads stage
threadgroup_barrier(mem_threadgroup);
if (t == 0u) {                                    // <-- and now 1023 of them wait
    float ss = 0.0f; uint k = 0u;
    while (k < n) { ss = ss + sq[k]; k += 1u; }   // <-- 3072 SERIAL DEPENDENT ADDS, one lane
    float sdv = ss/float(n) + eps; float gg = sdv;
    uint it = 50u;
    while (it > 0u) { it -= 1u; gg = 0.5f * (gg + sdv / gg); }   // <-- 50 serial divides, same lane
    sinv[0] = 1.0f / gg;
}
```

ggml's rmsnorm folds with `simd_sum` — a tree, ~12 steps — and takes one hardware `rsqrt`.

**~3122 serial steps against ~12.** That is the difference, and no amount of occupancy touches it.

## Proved by measurement, not by reading it

`FORM_RMS_TG` was added to make the threadgroup width observable, then the rmsnorm ablation was run at
three widths. If the cost were the staging, width moves it. If the cost is the serial fold, width does
nothing.

| threads | baseline s/token | rmsnorm ablated | cost per dispatch |
|---:|---:|---:|---:|
| 32 | 0.0269 | 0.0635 | 0.1634 ms |
| 128 | 0.0245 | 0.0578 | 0.1487 ms |
| 1024 | 0.0274 | 0.0595 | 0.1433 ms |

**A 32× increase in thread count buys 12%, and the baseline's own run-to-run spread is 12%.** Within
noise the width does not move it at all. 1023 threads waiting cost what 31 waiting cost, because
3071 of the 3072 additions are a dependent chain on lane 0.

## The Newton-50 is a side-issue, and separately wrong in both directions

Checked in f32 rather than assumed: Newton from `g0 = sdv` reaches a **fixpoint** in 2–16 iterations
for ordinary inputs, so iterations 17–50 are provable no-ops. And it does **not** land on the
correctly-rounded `sqrt` — 5 of 10 sampled values sit one or two ulps off. So 50 is too many for
typical inputs and, since convergence from `g0 = v` needs ~log2(v/√v) halvings first, **too few** near
f32's top end. It runs once per rmsnorm against 3072 adds, so it is not the cost — but it is not a
correctness argument either, which is what its 50 looks like.

## The trade, priced

| | today | if the reductions go to log2 depth |
|---|---:|---:|
| attention (24 threads, serial over the cache prefix) | 6.825 ms | |
| rmsnorm (3072 serial adds) | 4.000 ms | |
| **the two serial ops** | **10.82 ms of 23.80 — 45%** | |
| whole token | 23.80 ms → 42.0 tok/s | ~14 ms → **~71 tok/s** |
| llama.cpp, same blob, same host | | 6.32 ms → 158.1 tok/s |

What it costs: the fold order changes, so the sum changes in its last bits, so gate 11's claim —
*"BOTH fast paths generate the SAME token ids as the attestant"* — becomes a bounded claim instead of
an identity. That is exactly the question `llama-decode-msl.fk:20` declined to open.

**And the body can now state that bound.** `qk-matvec-split.fk` gives, for two associations of the
same sum,

    |y_a − y_b| ≤ (n + ceil(n/parts) + parts) · u · SUM|terms|

which is the same bound that gated the Q4_K kernel against ggml yesterday afternoon. A tree rmsnorm is
a `parts`-way split with a bound the body already derives and a band can already check. The named
epsilon the stone declined has since been named elsewhere in this body.

I have not made that change. It converts an identity gate into a bounded one, and that is Urs's call,
not mine — but it is now a priced choice rather than an open question.

## The most surprising teaching

**Adding hardware to a dependent chain is inert, and a profile cannot tell you that — only varying the
width can.** Every measurement before this one said "rmsnorm costs 0.143 ms," which reads like an op
that is merely slow and might respond to tuning. The width sweep says something categorically
different: it will respond to *nothing* except changing the shape of the fold. `chainbound` — a cost
set by the depth of a dependency, where parallelism is not slow but irrelevant.

That is why "10× slower" was not obvious from any amount of per-op accounting. The ablation prices
each op honestly and still cannot distinguish "slow because it does much" from "slow because it cannot
be helped." One extra knob, swept, separates them in one run.

## Where discomfort turned to gold

Having already written yesterday that rmsnorm was one thread "deliberately", and then finding
`form_rmsnorm_tg_f32` — Stone 16's *cooperative* version, which the lane has been dispatching all
along. I had read the attestant's comment and reported it as the running code. The fix I was about to
recommend already existed; the thing it fixed was the memory stalls, and the serial fold it left
behind is the actual cost. Twice in two days: the body had already done the work, and I described its
older self. The reflex I owe is to grep for the *twin* before quoting the attestant.

## Ground stamp

```
FORM_RMS_TG sweep, 6 runs, metal_first_token.sh, 2026-07-28/29:
  TG=32   base 0.0269 s/tok  ablated 0.0635  -> 0.1634 ms/dispatch
  TG=128  base 0.0245        ablated 0.0578  -> 0.1487
  TG=1024 base 0.0274        ablated 0.0595  -> 0.1433
form/form-stdlib/llama-decode-msl.fk — form_rmsnorm_tg_f32 folds ss on t==0 over n
Newton-50 vs f32 sqrt: 5 of 10 sampled sdv disagree; fixpoint reached in 2..16 iterations
```
