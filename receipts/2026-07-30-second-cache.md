# 2026-07-30 — the second cache is built, and the first number it gave me was a lie

Urs: *"how is stopping here healthy, continue until you have an observed working solution."*

So I kept going, built the missing half, measured it — and the measurement said I had made things
worse. That reading was the interesting part of the day.

## What was missing, exactly

DeepSeek-V4-Flash keeps **two** KV lanes per compressing layer. Our lane had one. `helpfuldefault`
located the gap by boundary: our agreement with ds4 broke at exactly `pos = 3`, the token where
`compressor_decode_one` emits its first row on a ratio-4 layer, which is 21 of the 43.

Read whole and transcribed:

- **`compressor_pool_decode_state`** (ds4.c:12326) — not a mean. Each of the 512 output dims runs its
  **own softmax** over the window's scores at that dim and blends the window's kv values at that dim.
- **`compressor_decode_one`** (:12381) — two F16 projections off `attn_norm`, an APE bias added to the
  score row only, a write into the rolling state, and on a ratio boundary: pool, weighted RMSNorm,
  rope at `comp_pos = pos+1-ratio` with the layer's compressed freqs, fp8 round, f16 store.
- **`layer_attention_mixed_one`** (:12567) — one softmax across the union of raw and compressed rows,
  sink still in the denominator only.
- **the ratio-4 trap** — `coff = 2` there, so a state row is `2*head_dim` wide and the state holds
  `2*ratio` rows. Rows `[0,r)` are the **previous** window, read at offset `j`; rows `[r,2r)` are the
  **current** window, read at offset `head_dim + j`. **Eight** candidates per output dim, not four.

**And the indexer turned out to be inert, not missing.** ds4.c:12822: `top_k = min(512, n_comp)`, and
`if (top_k == n_comp) { all allowed; return }`. Since `n_comp = floor((pos+1)/ratio)`, nothing is
masked until 2048 tokens on a ratio-4 layer. That collapsed a third of the estimated work to a
declared radius — enforced in the harness, which counts every step past it and reports the count.

## The first reading said regression

```
ds4's argmax (" Paris", 11111) at our rank:   raw-only 148   →   with the second cache 175
```

Worse. Except the rank of one token out of 129 280 is a gauge narrower than the thing it gauges. The
whole distribution says the opposite:

```
prompt      raw-only r        with the second cache
3 tokens    0.671588          0.671588      ← identical, and that is the point
4 tokens    0.750738          0.844425
5 tokens    0.752988          0.878719
```

**The 3-token pair being bit-equal is the plumbing's own falsifier.** Below `pos = 3` no compressed row
exists on either side, so a correct implementation must change *nothing* — and a lane that merely
perturbed the numbers could not be bit-equal there.

## The mutant that made the number better and the recipe worse

Breaking the ratio-4 candidate map in the device kernel — reading the current window at offset `j`
instead of `hd + j`, the natural four-candidate wrong answer:

```
                    r          ds4's argmax at our rank
correct map      0.878719               175
broken  map      0.781588                10
```

The **broken** map scores far better on the rank and markedly worse on the distribution. `wrongmend`
credited a fix on exactly that kind of number. Corpus row 949, `narrowgauge`. The gauge is now r over
all 129 280 logits, computed inside the lane so no shell arithmetic stands between the measurement and
the reader.

## What is in the tree

- **`form/form-stdlib/dsv4-compressor.fk`** — the recipe (geometry, state, pool, norm, shift, step, the
  declared indexer radius) and four MSL kernels: init, per-token state write, pool, shift
- **`form/form-stdlib/tests/dsv4-compressor-band.fk`** — verdict **2047**, mutation-proven. Five
  mutations, each dropping exactly the bits aimed at and nothing else: candidate map → 1887 (−32−128),
  mean-not-softmax → 2035 (−4−8), shift direction → 1983 (−64), ape-on-kv → 2031 (−16), boundary
  off-by-one → 1790 (−1−256)
- **`form/form-stdlib/mla-msl.fk`** — `form_mla_attend_mixed_f32` beside its sibling; `mla-msl-band`
  extended to **127** with a read-back that pins the shared max and shared denominator (mixed with an
  empty compressed set *is* the plain attention, bit for bit; splitting the same rows across the two
  arguments changes nothing)
- **`form/native/metal/metal_dsv4_stack.sh`** — the compress ratios are now read **before** the want
  list, because the ratio decides which tensors a layer has; the four compressor tensors join residency;
  gate 100 witnesses that every layer holds exactly `floor(steps/ratio)` rows, all finite, none past the
  indexer radius. `FORM_DS4_NO_COMPRESSOR=1` keeps the A/B. **104 gates, VERDICT PASS.**

## What is still open, named rather than rounded off

**At 3 tokens, with no compression anywhere on either side, r = 0.672.** So the raw half is not exact
either, and nothing in this work touched that. Yesterday's receipt called the no-compression path "a
faithful port" on the strength of whole-function reading; the number says reading was not enough. That
is the next boundary to bisect, and now there is an instrument for it.

**A diagnostic I surfaced and did not resolve, said out loud rather than stepped around:**
`fkwu --src` has no `print`, only `print_str`, so `mla-demo.fk`'s printer functions recover to nothing
under the walker — seven errors, pre-existing at HEAD, verdict unaffected only because those functions
are never called. That is the axiom-5 silent-lowering family and it deserves its own healing, not a
footnote; it is spawned as its own task with the agreement requirement (byte-identical float rendering
against the Go kernel) written down.

## The most surprising teaching

**A correct change can read as a regression, and a broken one as an improvement, when the instrument is
narrower than the thing it measures.** I have chased this for two days through two receipts. What made
today different was not more reading — it was replacing a one-number probe with one that looks at the
whole distribution, and then *deliberately breaking the kernel* to see which way each number moved. The
mutant is the evidence; the correct version alone could never have shown it.

## Where discomfort turned to gold

Building the whole second cache, running it, and reading **175 where 148 stood** — the flat statement
that a day's careful transcription had made the model worse. The pull was to explain it away or to
start rewriting the compressor. What I did instead was distrust the ruler, and the ruler was the thing
that was wrong. The gold is narrow and hard-won: **when a result contradicts a well-grounded change,
audit the measurement before the change.** `overfine` (row 934) said the reference can be the less
faithful side; this says the *metric* can be.

## Ground stamp

```
ds4.c:12326 compressor_pool_decode_state, :12381 compressor_decode_one, :12567 layer_attention_mixed_one,
:12822 indexer_allowed_decode_one — all read whole
indexer top_k = 512 (ds4.c:558); inert below 2048 tokens on ratio 4 — declared, counted, reported
model: DeepSeek-V4-Flash-IQ2XXS-w2Q2K-AProjQ8-SExpQ8-OutQ8-chat-v2-imatrix.gguf (86 720 111 488 B)
reference: ds4 --raw-prompt --dump-logits, prompt_tokens 3/4/5, argmax 270 / 344 / 11111
r over 129 280 logits, computed in-lane:
  n=3  raw 0.671588  comp 0.671588   (identical — no compressed row exists below pos 3)
  n=4  raw 0.750738  comp 0.844425
  n=5  raw 0.752988  comp 0.878719
device falsifier: ratio-4 candidate map offset j instead of hd+j -> r 0.781588, rank 10 (better rank,
  worse distribution)
bands: dsv4-compressor-band 2047 (5 mutations, each dropping exactly its own bits), mla-msl-band 127,
  dsv4-kv-cache-band 511, corpus band 32767
harness: 104 gates, VERDICT PASS, 43 layers, 21 compressed rows after 5 steps, all finite
corpus: 343 rows, max-mid 948, field 3433432948 — counts asked of the body
open: r = 0.672 at 3 tokens with zero compression on either side — the raw half, untouched by this work
open: fkwu --src has no `print` (7 unresolved-call errors, pre-existing at HEAD) — spawned as its own task
```
