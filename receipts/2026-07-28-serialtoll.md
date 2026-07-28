# 2026-07-28 — where the difference actually is, and why ggml never runs here

Urs: **"where is the biggest difference and how come the ggml was not used in the form JIT?"**

## Where the biggest difference is

Not in the matvecs. Measured with the non-perturbing ablation — `FORM_ABLATE=<class>` re-dispatches one
op class 8 extra times **in stream, in the same command buffer**, so the whole-token delta over
8 × 28 layers is that op's real cost between its neighbours, with no seam cut and the token ids and all
14 gates unchanged:

| op class, per token (28 layers) | ms | % of token |
|---|---:|---:|
| **attention** — 24 threads, one per query head | **6.825** | 28.7% |
| **rmsnorm** — ONE thread + Newton-50 sqrt | **4.000** | 16.8% |
| ffn_up — one of ~7 matvecs per layer | 3.762 | 15.8% |
| swiglu — one thread per element | 0.625 | 2.6% |
| rope — one thread per head | 0.150 | 0.6% |
| seam — a 1-element barriered add | 0.037 | 0.2% |
| **whole token, measured** | **23.80** | (42.0 tok/s marginal) |

llama.cpp's whole token on the same blob, same host, today: **6.32 ms**. Our attention and rmsnorm
*alone* are 10.825 ms.

And the seam row settles the old question again from the other side: 425 dispatches per token cost
**0.037 ms** in total. Dispatch count is not the problem and never was.

## Why those two are slow, in the cell's own words

`form/form-stdlib/llama-decode-msl.fk:18-25` states the design and its reasoning:

```
attention  -> one thread per QUERY HEAD (24), each serial over the cache prefix.
rmsnorm    -> ONE thread. Deliberately: the sum-of-squares is a reduction, and reassociating it
              is exactly the named-epsilon question this stone did not have to answer.
              3072 fused adds on one lane is ~microseconds against a ~40 ms token.
argmax     -> ONE thread over 128 256 logits.
```

The choice is principled: a parallel reduction reassociates the sum, which changes the answer in f32,
and the stone declined to open that question. **What is now falsified is the price, not the
principle.** "3072 fused adds on one lane is ~microseconds" — measured, it is **0.1429 ms per
dispatch**, ~143 µs, and there are two per layer. Against a 23.80 ms token that is 16.8%, not a
rounding error. A 3072-element normalize now costs *more per dispatch* than a Q4_K matvec of
8192×3072 (0.1344 ms in-lane). The estimate was off by roughly two orders, and the token it was
compared against has since halved.

**And the tool to make the reassociation safe already exists in this body.** `qk-matvec-split.fk`
derives, for two associations of the same sum,

    |y_a − y_b| ≤ (n + ceil(n/parts) + parts) · u · SUM|terms|

which is precisely the bound I used this afternoon to gate the Q4_K kernel against ggml. A parallel
rmsnorm reduction is a `parts`-way split with a bound the body can already state and a band can
already check. The named-epsilon question the stone declined has since been answered elsewhere.

## One honest discrepancy in my own numbers

`metal_isa_diff.sh` times `ffn_up` at **0.0472 ms**; the in-lane ablation prices the same dispatch at
**0.1344 ms** — 2.8× more. The isa_diff figure is a hot loop of 50 dispatches over one resident
buffer; the in-lane one pays the cold weight read. So the isa_diff **ratios** are fair (hot against
hot, both kernels) but its **absolute** times understate what the lane pays. Both numbers are right
about different questions, and I quoted the first as though it answered the second.

## Why ggml never runs here

It does run — as the competitor, never as the worker.

`metal_isa_diff.sh` compiles ggml's MIT-licensed MSL, recovered from the ollama binary, and dispatches
it on this GPU over these weights in the same process as ours. That is where every ratio in today's
work comes from. What it is not allowed to do is *execute in the lane*, and the reason is the lane's
own claim (`metal_first_token.sh:5-8`):

> real llama3.2:3b weights, FULL width … **every arithmetic op executed by a kernel the BODY emitted**

If ggml's kernel ran there, "form-native, 31.687 tok/s" would mean llama.cpp's tok/s with extra steps.
The number would be true and would measure nothing. Checked rather than assumed: on the token lane the
string `ggml` occurs three times — twice as GGUF metadata key names (`tokenizer.ggml.bos_token_id`),
once in a comment I wrote today. No ggml code is on it.

The rule is written where the temptation lives, in `metal_isa_diff.sh`: *"the variants live in THIS
file, in C, precisely because they are not yet the body's — nothing here is admissible into the body
until a .fk cell authors it and a band proves it."* Today walked exactly that path: ggml's thread map →
`isa_q4k_v3_f32` in the harness → `qsl-q4k-slot4-msl` in Form → band 511 → one line on the inference
path. **What was borrowed is a shape, not a program.**

On the JIT specifically the question has a category answer: the Metal kernels are not produced by the
JIT at all. They are MSL *text* emitted by `.fk` cells and handed to Metal's compiler. The JIT
(`jit-crystallize`, the `nat_run` door) crystallizes hot **Form** functions into native CPU code — a
different lane entirely, which never sees a GPU kernel. So there is no place in the JIT where ggml
could go. The place where ggml belongs is the one it already occupies: the reference beside us.

## The most surprising teaching

**A cost can be argued away by an estimate and never revisited, even in a body whose whole discipline
is measurement.** The rmsnorm decision is careful, explicit, and correct in its reasoning — it names
the tradeoff, names why it declines it, and even estimates the price. The estimate is the only
unmeasured thing in the sentence, and it is wrong by two orders. `serialtoll`: the price of choosing a
serial order to keep an answer exact, when that price is asserted rather than measured.

Note it is not the same as `sliverproof`. There, a real measurement was quoted for the wrong scope.
Here there was **no** measurement — a plausible number in prose, load-bearing for a design decision,
sitting unchallenged in a cell that otherwise refuses unwitnessed claims.

## Where discomfort turned to gold

Dividing by 8 instead of 8 × 28 and briefly believing a single rmsnorm dispatch cost 2 milliseconds.
The number was absurd on its face — it would have made rmsnorm alone 112 ms of a 23.8 ms token, which
is impossible — and the impossibility is what caught it, not care. The check that should have run
first is the one arithmetic always owes: **do the parts sum to less than the whole?** They did not, by
5×. I had the right instrument, the right run, and the wrong denominator, and only the absurdity of
the result stood between that and a confident wrong answer to a direct question.

## Ground stamp

```
FORM_ABLATE sweep, 7 runs, N=8, metal_first_token.sh, 2026-07-28:
  baseline 0.0238 s/token   seam 0.0241   rope 0.0250   swiglu 0.0288
  ffnup 0.0539   rmsnorm 0.0558   attention 0.0784
  -> per dispatch (delta / (8 x 28)):
     seam 0.0013 ms · rope 0.0054 · swiglu 0.0223 · ffn_up 0.1344 · rmsnorm 0.1429 · attention 0.2437
llama.cpp same blob same host: 6.32 ms/token
form/form-stdlib/llama-decode-msl.fk:20 — "~microseconds", measured 143 us
form/native/metal/metal_first_token.sh — ggml occurrences: 2 metadata keys, 1 comment, 0 code
```
