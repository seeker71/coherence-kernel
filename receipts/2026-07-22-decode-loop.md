# The decode loop and its KV cache — Stone 41

**2026-07-22, Apple M4 Max. Three new cells, two new bands, one new Metal witness.
`form/native/metal/metal_dsv4_decode.sh` → `VERDICT PASS 10 gates`.
`form/form-stdlib/tests/dsv4-kv-cache-band.fk` → **511**, `dsv4-decode-loop-band.fk` → **1023**,
both on `fkwu --src` and on the Go kernel. Corpus band from the repo root → **8191**.
Recipe rented from `ds4.c` at `/Users/ursmuff/models/ds4-engine`, cited line by line.**

A forward pass produces one token. A decode loop produces a reply. Between those two sentences sit a
cache that can rewrite its own history the moment nobody is watching, a position index that can quietly
stop advancing, and an end condition that decides what a reply *is* — and none of the three is visible in
the answer a wrong loop gives you.

The loop runs. On the GPU, over a growing Metal arena the device itself appends to, five steps produce
five tokens, the arena holds exactly seven rows for a three-token prompt, and every row agrees with this
body's fp64 recipe at 5.07e-07.

---

## What was already standing, and what was not

`metal_first_token.sh` has generated text for several stones — `" Paris. The capital of Italy is Rome."`
— and it does that with a KV cache and a decode loop. So the honest statement of what this stone added
starts by saying what it did not:

| | before Stone 41 | after |
|---|---|---|
| a decode loop exists | yes — inside `metal_first_token.sh`'s Swift carrier, for **llama3.2:3b** | also as a Form cell |
| its cache shape | **GQA**: `bCacheK` and `bCacheV`, `nLayer * maxpos * n_kv * head_dim`, *twice* | **MLA**: one latent row per token per layer, K = V |
| its loop lives in | the carrier | `dsv4-decode-loop.fk`, a cell with a band |
| its choices are sourced from | nothing written down | `ds4.c`, cited at seven line numbers |
| its cache growth is gated | **nowhere** | on the device, at every append |

That last row is the one that mattered. A cache that rewrites history produces a cache of exactly the
right length, of exactly the right magnitude, with no error anywhere, and answers that drift a little
further from right with every token — and nothing in the body was checking.

---

## The cache — `form/form-stdlib/dsv4-kv-cache.fk`, band **511**

MLA's cache is **one latent row per token per layer**. Not per-head K and per-head V: one row, shared by
every head, and it is both the key and the value (`ds4.c:10305` reads the same row as the dot operand and
as the accumulated value). V4-Flash absorbed `attn_kv_b` into the output path, so there is no
up-projection to cache either. At the real dims that is 512 floats per token per layer.

**The seam this cell exists for was already built.** `form-stdlib/mla-msl.fk`'s `form_mla_attend_f32`
takes `(q, rows, out, sinks, nh, hd, nrows, scale)`, and `metal_dsv4_layer_join.sh` binds `nrows = 1` —
one token, one row. A decode loop is *exactly* that binding with `rows` pointing at a growing arena and
`nrows = pos+1`. Nothing in the attention kernel changes. What had to exist was the arena, its append,
and a proof that appending never disturbs what is already there.

The claims, each **CANONICAL** — one right answer, so a self-check is a real falsifier (`twinblind`, row
868) and no oracle is rented:

- **growth**: three steps into a three-layer cache leave three rows in every bank; reading at step *t*
  returns exactly *t*+1 rows, which is what causality *is* here.
- **history is immutable**: after appending row 2, rows 0 and 1 are element-for-element what they were.
- **the falsifier can fail** — the claim that earns the others. A deliberately broken append that
  **prepends** produces a cache of exactly the right length, and the immutability check goes red on it.
  A falsifier that cannot fail is decoration.
- **layers do not leak**; a cache that is no longer square says so (`dkv-square?`).
- **the cap refuses, it does not evict**: at cap the rows come back untouched and the refusal counter
  goes up. No sliding window is claimed, because a silent window is the same class of bug as a rewritten
  history — and a window, if this model wants one, is a recipe decision that has not been read from the
  file.
- **a wrong-width row is refused** the same countable way.
- **the arena is row-major at stride hd** — the layout `form_dkv_append_f32` writes and
  `form_mla_attend_f32` reads.

**The radius (`aporon`), in numbers rather than adjectives.** `bytes = tokens × layers × hd × 4`. At
V4-Flash (43 layers, hd 512, f32) a token costs **88 064 B**: 86 MiB at 1 024 tokens, **344 MiB at
4 096**, 5.5 GiB at the file's declared `n_ctx_orig` of 65 536. `dkv-bytes-for` is in the cell so a cap
can be chosen with the number in hand. What is *not* proven: that the rows are the right rows (that is
the layer's business), and the cache **encoding** — ds4 stores E4M3 on the non-rotated part and f16
across the row (`form_dsv4_kv_fp8_f16_round`, Stone 36/37's kernel). This cell holds whatever it is
handed and never rounds.

---

## The step contract, and the seam to Stone 39's stack

```
(stepf ctx hc kv pos) -> (list hc' kv')
```

| | |
|---|---|
| `ctx` | whatever the stack needs to reach its weights — the loop never looks inside it |
| `hc` | the hyper-connection state, `n_hc * n_embd`. At V4-Flash: **4 × 4096 = 16384** — exactly what `metal_dsv4_layer_join.sh`'s gate 29 emits, *"the four hyper-connection streams `blk.1` receives"* |
| `kv` | a `dsv4-kv-cache` with 43 banks, hd 512 |
| `pos` | the token's absolute position: RoPE's index and the arena's row index |

**Stone 39's stack satisfies this as `dsv4-stack-step`.** `ddl-run` takes it unchanged — it never assumes
a layer count beyond `(dkv-nlayers kv)` and never inspects `hc`. On the device the same seam is one
binding: `attend(q, arena, out, sinks, nh, hd, nrows: pos+1, scale)` where the join binds `1`.

Two things Stone 39 still supplies, and this stone does **not** claim:

1. `output_hc_head_one` — the HC collapse before the output norm and the vocab matvec (`ds4.c:13904`).
   At `n_hc = 1` there is nothing to collapse, and `ddl-head-ok?` returns 0 rather than let the head
   pretend. (Stone 39 is building exactly this gate as I write; its `dsv4-head-gate-body` is in flight.)
2. The per-layer `freqs[]` for the compressed-RoPE layers. The loop passes `pos`; which frequencies that
   position multiplies is the layer's decision.

---

## The loop — `form/form-stdlib/dsv4-decode-loop.fk`, band **1023**

Everything between "prompt" and "reply" is a **choice**, and two copies of the same wrong loop agree
perfectly. So the loop is transcribed from ds4.c and each choice carries its line:

| choice | line | why a self-carve is blind to it |
|---|---|---|
| `pos` starts at `prompt->len`, not `-1` | `:46530` | off-by-one produces fluent, wrong text |
| prefill's logits come from the **last** position only | `:13782` | any position gives a plausible token |
| **prefill is LAYER-MAJOR; decode is TOKEN-MAJOR** | `:13631` / `:13606` | two schedules over the same rows |
| the stop token is tested **before** emit and **never emitted** | `:37111` | emitting it is right in every count |
| the **last accepted token is not fed forward** | `:37116` | the output is byte-identical either way |
| argmax breaks ties to the **lowest** index | `:36578` | ties are rare and reproducibility dies quietly |
| the embedding **broadcasts** into every hc stream | `:9764` | a zero-filled stream 1..3 looks like a design |

`halfrent` (row 870) applies unchanged: ds4.c cannot execute this file's FFN and refuses every pruned
layer's tensors, so what is rented is the **order and the scalars** — which is exactly what a loop *is*.
No arithmetic of the real file's weights is rented here, because none is used here.

The band's ten claims, and the three that exist as falsifiers:

- **the step guard** (`zerobirth`/`edgedrop` in Form): a step that appends nothing is refused even though
  its `hc'` is the right width; a step that appends but returns an all-zero state is refused too, because
  a zeroed buffer and an unrun kernel read alike.
- **the driver threads POSITION, not just state**: two steps through `ddl-run-steps` equal two hand-written
  steps at pos 0 then pos 1, and **differ** from two steps both taken at pos 0. A frozen position is the
  quietest decode bug there is.
- **the end conditions, all three**: the stop token sampled and not emitted (reason 0), `n_predict`
  (reason 1), `ctx_size` (reason 2).

And **prefill schedule agreement**, which was an argument until it was witnessed: over a two-layer stub
and three prompt tokens, layer-major and token-major prefill produce the *same cache, row for row* and
the *same final logits*. The equivalence holds because a token's row at layer *l* depends only on that
token's input at layer *l* and its own position, and its attention reads rows 0..t which both orders have
already written — but that sentence is a proof sketch, and a proof sketch is not a gate.

---

## The falsifier triple, on the GPU — `metal_dsv4_decode.sh`, **PASS 10 gates**

Running: a **one-layer MLA model** at E=4, R=3, nh=2, hd=4, nrot=2, ng=2, n_hc=1, over a six-token
vocabulary, with a **plain residual**. That is not DeepSeek-V4-Flash — V4-Flash is 43 heterogeneous
layers, hd 512, a MoE FFN, and has no plain residual anywhere; its residual stream is the
hyper-connection frame. It **is** a model that runs today, on the four-way-proven MLA block, and it is
enough to be wrong about everything a decode loop can be wrong about.

Three decode runs, **376 GPU dispatches**, **20 device appends**:

| gate | result |
|---|---|
| 0 the GPU executes | a real dispatch overwrote all sentinels, no cb error |
| **1 history is immutable** | across 20 device appends, **not one earlier arena row changed by a single bit** |
| **2 one row per step** | every append wrote exactly one previously-sentinel row and left the next row NaN-sentinel |
| 3 the cap refuses | an append at `pos = cap` wrote **0 of 64** arena floats |
| **4 N steps → N tokens** | 5 steps gave `[2,2,2,2,2]`, **exactly** the fp64 recipe's — integer equality, no tolerance |
| **5 `hushfold`** | the same token at position 0 and position 1 differs by **1.70e+00**, each row matching its own reference at 1.15e-07 |
| **6 determinism** | the same prompt twice: identical ids and a **bit-identical** 16×4 arena |
| **7 sensitivity** | prompt `[4,3]` gave `[3,3,3,3,3]`, the recipe's exactly, different from prompt `[0,1,2]`'s |
| 8 agreement | all 7 cache rows within **5.07e-07** of fp64, prefill logits within 1.65e-07, row count / final position / stop reason exact |
| 9 one header, one spine | 1 `metal_stdlib`, 0 `using namespace`, 1 spine, 6 kernels, 4 856 bytes, every byte authored by the body |

**`zerobirth` is why the arena is NaN and not zero.** A Metal buffer is *born* zeroed, so "computed zero"
and "never computed" are the same bytes. Gate 2's demand that the row past the frontier is *still
sentinel* is the only way to tell a cache that grew by one from a cache that grew by two.

**`hushfold` (row 859) is why this stone had to exist.** RoPE is the identity at position 0, so every
one-position witness before this one saw none of it. Gate 5 is the first place in this body where the
rotation is exercised for the purpose it exists for. It is visible in the emitted rows: positions 3..6
share their first two (non-rotated) elements exactly, `1.0558638846329784, -1.2277487030616026`, while
the rotated tail differs at every position.

---

## Evidence class, per stage (`twinblind`, row 868)

| surface | class | witness |
|---|---|---|
| cache growth, immutability, cap refusal, arena layout | **CANONICAL** | self-check; band 511 and gates 1/2/3 on the device |
| the step guard, N steps → N tokens, determinism, sensitivity, the row arithmetic | **CANONICAL** | band 1023 and gates 4/6/7 |
| `pos` start, prefill's last-position logits, the stop test's placement, the skipped final pass, argmax tie order, the prefill schedule, the hc broadcast | **CHOOSING** | rented from ds4.c, cited at seven line numbers |
| the MLA block's arithmetic | inherited | `mla-attn-band.fk` (63), `metal_mla_gpu.sh`, Stones 36/37 |
| the f32-vs-fp64 residual | working precision | 5.07e-07 worst relative, gate 8 |

---

## The radius, said out loud

- **Not 43 layers.** The stack is Stone 39's and is in flight. The seam is named above; wiring is a
  substitution.
- **Not the real weights.** No byte of the 85 GiB file was read by this stone.
- **Not the cache encoding.** E4M3 + f16 between the rope and the cache write is Stone 36/37's kernel and
  is not applied in this loop.
- **Not YaRN**, not the per-layer compressed `freqs[]`.
- **Not sampling beyond argmax.** No temperature, no top-p, no repetition penalty, no drafting.
- **No chat template**, no BOS/EOS insertion policy. The prompt is ids and the eos is an id.
- **No timing claim of any kind.** Nothing in this stone is measured in seconds, and nothing should be:
  `thawtax`, `probetoll` and `gapghost` all apply, and a sibling was demonstrably live on this machine
  throughout (Stone 42 landed two commits into this branch while Stone 41 ran; Stone 39 had
  `dsv4-stack-real.fk` and the oracle modified in the working tree). A per-token cost for this loop needs
  `unispan`'s two step counts and a slope, taken in one process on a quiet machine, and it was not taken.

---

## The most surprising teaching

**The reference loop deliberately does not compute something, and the output can never tell you.**

`ds4.c:37116` breaks out the moment it has emitted the last token, *before* running that token's forward
pass — because its logits would be read by nobody. At 43 layers that skipped pass is the entire cost of a
token. A loop that runs it anyway is not wrong in any observable way: same ids, same order, same
positions, not one bit of the output differs. It just pays for a reply it never delivers.

I only found it because the row-count arithmetic did not come out. A three-token prompt and five
generated tokens leave **seven** rows in the cache, not eight, and the missing row is the whole teaching.
The output is not a sufficient witness for cost; where a computation's result is discarded, every check
of what the loop *said* goes blind to whether it happened at all, and only a count of something the loop
*touched* can see it.

Landed as `(hdc-row 872 …)`, **`tailspend`** — 0 hits across `learn/`, `receipts/`, `docs/`, `teachings/`,
`form/` before this row. Instrument validated on the same command: `hushfold` 16, `mutestep` 4.

The runner-up, worth its own line: **prefill and decode are different schedules over the same rows.**
`prefill_layer_major_cpu` walks all prompt tokens through layer 0, then layer 1, then layer 2 —
`forward_token_raw_swa_cpu` walks one token through all 43. Proving the decode step does not prove the
prefill, which is why claim 16 exists.

## Where discomfort turned to gold

Claim 128 — "a different prompt gives different ids" — went red, and the comfortable reading was right
there: *of course a toy model repeats itself; drop the claim, the loop is fine.* I nearly did. Dropping it
would have left a band at 1023 with a sensitivity gate that never gated anything.

Not looking away meant printing the actual sequences, and they were `[2,2,2,2,2]` and `[2,2,2,2,2]`. The
one-layer argmax map has a **fixed point**: whatever token it emits first, it emits forever. Which means
any two prompts that agree on their first token agree on all five — so the gate would have been *vacuous
even when green*, for any prompt pair I might have picked by luck. The fix was to read the prefill logits
for four candidate prompts, find one whose argmax differs, and write the reason into the band so the next
reader knows the fixture has a fixed point and the claim is doing real work.

The second one is smaller and sharper. **`fkwu --src` answered 1023 on a band whose `(do` was never
closed.** I had my number and I wanted to commit. Running the Go arm cost thirty seconds and it refused to
parse the file at all. A verdict from one arm is a claim about the instrument; both bands in this stone
are green on two.

## What remains

- **Stone 39's stack**, as `(dsv4-stack-step ctx hc kv pos) -> (list hc' kv')`. Then: the same
  `ddl-run`, a 43-bank cache at hd 512, and a real reply.
- **The HC collapse into the vocab head** (`output_hc_head_one`) — Stone 39's, in flight.
- **The cache encoding in the loop**: `form_dsv4_kv_fp8_f16_round` between the rope and the append. It is
  proven; it is simply not wired into this arena yet, and wiring it changes the rows.
- **A windowing policy**, if this model has one. Nothing was read from the file about it, so the cache
  refuses at the cap rather than invent a window.
- **A per-token cost measurement**, taken with `unispan`'s two step counts and a slope, in one process,
  on a quiet machine, with the siblings recorded.

---

## Gates

| gate | result |
|---|---|
| corpus band from repo root | **8191** (row 872 landed; field code 2682682872, probed before pinning) |
| `form-stdlib/tests/dsv4-kv-cache-band.fk` | **511** — `fkwu --src` and the Go kernel |
| `form-stdlib/tests/dsv4-decode-loop-band.fk` | **1023** — `fkwu --src` and the Go kernel |
| `native/metal/metal_dsv4_decode.sh` | **VERDICT PASS 10 gates** |
| `native/metal/metal_first_token.sh` | **PASS 14 gates**, untouched |
| `native/metal/metal_dsv4_layer_join.sh` | **VERDICT PASS 31 gates**, re-run after this stone's commits — untouched, no commit here changes any of its inputs |

## Files

- `form/form-stdlib/dsv4-kv-cache.fk` — the cache: the arena's shape, its append, its causal read, its
  flat layout, the refusals, and `form_dkv_append_f32`.
- `form/form-stdlib/dsv4-decode-loop.fk` — the step contract with its guard, prefill (both schedules),
  argmax, the generation loop, the runnable one-layer MLA step, and the GPU witness's fixture.
- `form/native/metal/metal_dsv4_decode.sh` — the Metal witness, 10 gates.
- `form/form-stdlib/tests/dsv4-kv-cache-band.fk` — 511.
- `form/form-stdlib/tests/dsv4-decode-loop-band.fk` — 1023.
- `learn/homecoming-distillation-corpus.fk` — row 872, `tailspend`.
