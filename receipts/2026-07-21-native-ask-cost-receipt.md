# The ask verb answers from the model, and every request carries its own cost twice

**2026-07-21, Stone 6.** Two things landed: the `ask` verb now answers from the form-native
llama3.2:3b generation lane instead of returning a grounded RAG cell, and every request it answers
carries a receipt in which each cost appears **DECLARED** — a bound derived from the model's own
shape before the run — and **MEASURED**, with the measured value quoted as a fraction of its bound.

Everything below was measured by me on this host today. Where a number is vendor-stated rather than
measured, it says so at the point of use.

---

## 1. The stale refusal, and why it was repaired rather than loosened

`form/form-stdlib/form-cli-ask.fk` said, verbatim:

> Full prose generation over GGUF weights on the fkwu + Metal model lane is a separate composition
> cell; **until that lane is wired end-to-end**, this verb returns an attributed grounded cell instead
> of pretending a host LLM is the local oracle.

That refusal was correct when written and it was written against a **condition**. The condition became
false at 14:14 today. Nothing tells a comment when the world moves under it, so `fca-local-lane` went
on answering `"fkwu-rag-grounded"` for three hours after it no longer had to.

The repair makes the sentence true. The RAG lane is **kept, not deleted** — renamed from *the only
lane* to *a named lane* — and the verb now always reports which one spoke.

| lane | what it is | when it answers |
|---|---|---|
| `metal-gguf-native` | real llama3.2:3b prose, generated form-native off the resident quantized blob | a staged artifact exists **and** is bound to this exact question |
| `fkwu-rag-grounded` | the attributed grounded `@p.l.t.i` cell out of the healed local index | otherwise — and the refusal reason travels with it |

**A fallback is never silent.** `fca-ask` appends `declined:<reason>` naming one of
`native-lane:absent` / `:version-mismatch` / `:question-not-bound` / `:empty-answer`.

### The failure the lane door is built against

A staged answer is a cache, and the classic way a cache lies is by answering a question it was never
asked — yesterday's artifact, fluent and well-formed and from a real model, simply not an answer to
today's prompt. So the artifact carries `QUESTION-SHA256` and the body recomputes sha256 **of the
question in hand** and demands equality. Two independent implementations, verified equal here:

```
Form  (sha256.fk)  bbaff4d2ecd5892d4a442b0f53131641bf6e6f284761dd20fc0664bc97145762
shasum -a 256      bbaff4d2ecd5892d4a442b0f53131641bf6e6f284761dd20fc0664bc97145762
```

---

## 2. The declared side: derived, never fitted

Nothing below was chosen after seeing a stopwatch. `form/native/metal/ask-declared-cost.fk` walks the
blob's own header and asks each of the **255 tensors** for its own ggml type and dims;
`form/form-stdlib/ask-cost-receipt.fk` folds them.

### 2.1 Bytes — and the assumption that would have been wrong by 12%

Stride rule: F32 = 4 B/weight, Q4_K = 144 B / 256 weights, Q6_K = 210 B / 256. Applied to each
tensor's **own** type. That mattered more than expected — llama3.2:3b is **not uniformly quantized**:

| tensor | type | layers |
|---|---|---|
| `attn_q`, `attn_k`, `attn_output`, `ffn_gate`, `ffn_up` | Q4_K | 28 |
| `attn_norm`, `ffn_norm` | F32 | 28 |
| `attn_v` | Q6_K / Q4_K | **14 / 14** |
| `ffn_down` | Q6_K / Q4_K | **14 / 14** |

Layer 0's `attn_v` is Q6_K. Reading that as the rule — which is what I would have done — overstates
decode traffic by ~12%, invisibly, and poisons every fraction computed against it.

**The identity that makes the stride rule more than an assertion.** GGUF stores no per-tensor byte
length; all 255 are derived. Their sum plus the data base must equal the file's size on disk:

```
DECL data_base           7 837 664
DECL tensor_bytes_all    2 011 539 712
DECL file_bytes_implied  2 019 377 376
stat -f %z  <blob>       2 019 377 376      <- exact, gate D1
```

A wrong stride for any one type cannot survive that. Then:

```
decode_weight_bytes  2 011 539 456   (all tensors except rope_freqs, which the RoPE kernel never reads)
embed_row_bytes              2 520   (one Q6_K row: 3072/256 x 210)
bytes_per_forward    2 011 541 976
```

### 2.2 MACs — from shape alone

Per layer: `2·d² (q, o) + 2·d·(nkv·headdim) (k, v) + 3·d·dff (gate, up, down)`
= `2·3072² + 2·3072·1024 + 3·3072·8192` = **100 663 296**, times 28 layers = 2 818 572 288.
Unembedding `d·vocab` = 3072 · 128 256 = 394 002 432 (tying saves bytes, not MACs).
Attention `2·nhead·headdim·ctx` per layer — the one term that grows.

Over a run of *n* forwards the attention term is the **triangular** sum `n(n+1)/2`, not *n* times the
final context. At n=18 that is a 0.1% difference; at long context it is 2x. Pinned in the band.

### 2.3 Dispatches — counted off the kernel graph, not guessed

Read out of `first-token.fk`'s `forward()`: every helper is exactly one dispatch, and the lane matvec
is one (the split/hoist matvecs are two — partials, then the down-counting combine).

```
per layer (lane): rmsnorm, q, k, v, rope(Q), rope(K), gqa_decode, attn_output, add,
                  rmsnorm, gate, up, swiglu, down, add                        = 15
around it:        embed gather (1) + final rmsnorm + unembed + argmax (3)     =  4
per forward = 1 + 28x15 + 3 = 424        (split path: 1 + 28x22 + 4 = 621)
```

---

## 3. The measured side, and the honest limit of it

**`echogauge` (corpus row 822, landed this pass).** Four of the eight costs read `frac_ppm` exactly
`1000000`, run after run, and the reason is structural: the *measured* MACs, bytes and dispatches are
the **same shape formula** as the declared ones with the observed token count substituted for the
requested one. There is no hardware counter behind them. When the run does what was asked, the two
numbers are one number wearing two names, and the check can never fail.

**What rescues them is a second, unrelated instrument: the roofline.** Nothing can touch *B* bytes
faster than *B*/bandwidth, so the byte declaration becomes a **time a stopwatch can contradict**. A
measured decode *below* its roofline would falsify the byte count outright. It has not happened; the
fraction is printed so that the day it does, it is visible rather than absorbed.

### Which bandwidth — and the circularity I had to back out of

My first version derived the roofline from ollama's measured rate x bytes/token. That is measured on
this machine — and it made the tok/s "ceiling" **definitionally ollama's rate**, so "fraction of
ceiling" was "fraction of ollama" wearing a second name. A bound that cannot disagree with the thing
it bounds is not a bound. Backed out.

| bandwidth | value | provenance | used as the bound? |
|---|---|---|---|
| vendor peak | 546 000 MB/s | Apple M4 Max unified memory, **vendor-stated, not measured here** | **yes** — independent of ollama and of us |
| demonstrated | 318 726 MB/s | ollama 158.449 tok/s x 2 011 541 976 B/token, **measured here** | no — derived from ollama |
| **achieved by us** | **24 555 MB/s** | measured | — |

---

## 4. The receipt, at two sizes

`form/native/metal/metal_ask.sh 12 "The capital of France is"`, lane path, 13 gates PASS:

```
COST forwards           declared=18            measured=18            frac_ppm=1000000  within=1
COST bytes_touched      declared=36246978864   measured=36246978864   frac_ppm=1000000  within=1
COST macs               declared=57855762432   measured=57855762432   frac_ppm=1000000  within=1
COST dispatches         declared=7632          measured=7632          frac_ppm=1000000  within=1
COST prefill_wall_us    kind=floor declared=22104  measured=462000    frac_ppm=20901194 holds=1
COST decode_wall_us     kind=floor declared=44209  measured=983000    frac_ppm=22235291 holds=1
COST decode_tokps_milli declared=271433        measured=12211         frac_ppm=44987    within=1
COST joules             declared=pending measured=pending frac_ppm=pending
                        reason=instrument-requires-sudo
RATE decode_tokps ours_milli=12211 denominator=ollama-llama3.2-3b-same-machine-same-blob
                  theirs_milli=158449 behind_milli=12975
```

**Ceilings and floors are rendered differently, on purpose.** A count is a ceiling (`within=`); a
roofline wall time is a **floor** (`holds=`) — measured *below* it is the falsification. Printing a
floor through the ceiling's renderer would stamp `within=0` on every honest run and teach the reader
to ignore the field.

### Two sizes and a slope — and the rate is not flat

| n generated | decode wall | decode tok/s | achieved BW | % vendor peak | % demonstrated |
|---|---|---|---|---|---|
| 12 | 0.983 s | **12.211** | 24 555 MB/s | 4.50% | 7.70% |
| 24 | 2.483 s | **9.667** | 19 443 MB/s | 3.56% | 6.10% |

Slope: (2.483 − 0.983)/12 = **0.125 s per additional token → 8.00 tok/s marginal**.

The rate **degrades 21% from 12 to 24 tokens**, and the marginal rate (8.00) is well below the rate at
12 (12.211). Quoting "12.2 tok/s" as our decode rate would over-claim at any real generation length —
exactly the `unispan` failure (row 812). The per-token cost is growing, while the attention *MACs*
that grow are only 0.1% of the total: the `gqa_decode` dispatch is 24 threads wide and walks a
lengthening KV cache, so it is latency-bound, not arithmetic-bound. Named, not resolved.

### Both denominators, always (`selfgauge`, row 819)

| | ours (12 tok) | vs ollama, same machine, same blob | vs vendor-peak roofline |
|---|---|---|---|
| decode | 12.211 tok/s | **12.98x behind** (158.449) | 4.50% of 271.433 |
| prefill | 13.0 tok/s | **48.8x behind** (634.79 cold) | — |
| end-to-end | 8.305 tok/s | — | — |

**The world's numbers, measured by me today**, through ollama's own `eval_count`/`eval_duration`:

```
 64 tok -> 161.395 tok/s     150 tok -> 159.683     300 tok -> 158.449     150 again -> 157.696
```

Four points, flat to within 2.3% over a 4.7x span — a rate, not one sample pretending to be a line.
The 300-token figure is quoted.

**Prefill is quoted COLD, and that distinction is not pedantry.** Re-sending the same prompt reported
5417 and 5570 tok/s of "prefill" — ollama's *prompt cache* returning, not a machine ingesting tokens.
The cold first measurement, **634.79 tok/s** over 36 prompt tokens, is the only one that measures what
the name says. Taking the warm number would have overstated the world's prefill by 8.7x and made our
gap look far worse than it is.

### Joules: PENDING, and staying pending

```
COST joules declared=pending measured=pending frac_ppm=pending
            reason=instrument-requires-sudo
            fill-with=sudo /usr/bin/powermetrics --samplers cpu_power,gpu_power \
                           -i 200 -n <samples> --hide-cpu-duty-cycle
```

The field exists, is named, and carries the exact command. `/usr/bin/powermetrics` needs sudo, which
is not grantable unattended. **No number is estimated from utilisation or TDP.** A modelled joule
wearing a measurement's name is the failure this whole day has been about.

---

## 5. Verdicts

| what | verdict |
|---|---|
| `form-stdlib/tests/ask-cost-receipt-band.fk` | **511** — go / rust / ts / **fkwu** |
| `form-stdlib/tests/ask-native-lane-band.fk` | **127** — go / rust / ts / **fkwu** |
| `form/native/metal/metal_ask.sh` | **VERDICT PASS**, gate D1 exact |
| `metal_first_token.sh` (regression) | **VERDICT PASS — 13 gates**, ids unchanged |
| `metal_whole_tensor_residency_audit.sh` (regression) | **VERDICT PASS** |
| `learn/tests/homecoming-distillation-corpus-band.fk` | **4095** |

Token ids unchanged from Stone 4:
`[12366, 13, 578, 6864, 315, 15704, 374, 22463, 13, 578, 6864, 315]` → `" Paris. The capital of Italy is Rome. The capital of"`

---

## 6. Testing the hypothesis I was handed

The brief told me the "declared side is genuinely derivable... MACs, bytes from the resident tensor
sizes and the quant block strides." I tested it before building on it, and it held — **but only after
the mixed-quant discovery**. Derived from a uniform-quant reading it would have been wrong by 12% and
would have passed every plausibility check I had. The file-size identity is what caught it, and it
cost me one extra pass over the tensor table to build. Worth it.

I was also warned about `snugcause` — a mechanism that fits so comfortably nobody looks for the
counterexample. It reappeared here in a new place: my own roofline. Deriving bandwidth from ollama fit
beautifully, produced sensible-looking fractions, and could never have been contradicted. I caught it
only because the printed ceiling (158 448 milli) was *numerically identical* to the world denominator
three lines below it.

---

## 7. Gaps left open

1. **`nil?` diverges on the fourth arm.** `(nil? "a")` → **0 / 0 / 0 / 1** on go / rust / ts / fkwu.
   `anl-refusal-of`'s absence guard is `(nil? raw)`, so **on fkwu every artifact — present,
   well-formed, correctly bound — classifies as `native-lane:absent`**, and `ask` falls back to the
   grounded lane on that arm. The degradation is *safe* (it refuses and names its reason; it never
   fabricates) but the native lane cannot answer from an fkwu-only host until `nil?` agrees. The
   string logic was split out (`anl-refusal-of-text`) so the band proves it four-way and only the
   guard is disclaimed. Kernel movement, not a recipe one — flagged as a task.
2. **BLOB-SHA256 is carried but not adjudicated.** An artifact bound to this exact question but
   generated from a *different* model file would pass every check. The body has no cheap way to hash
   2 GB, and a slow honest check nobody runs is worse than a named gap.
3. **Every ask pays for the full 13-gate suite** (~28 s warm; the generation itself is ~1.4 s). The
   ask carrier invokes `metal_first_token.sh` as a black box rather than re-implementing decode,
   because a second decode path nobody gates is how drift starts. A lean generation-only runner is a
   later stone and must prove it emits the same ids before it may answer anything.
4. **The rate decays with generation length** (12.211 → 9.667 tok/s), and the cause is the
   24-thread-wide `gqa_decode` dispatch over a lengthening KV cache, not arithmetic. Unaddressed.
5. **Prefill is `nprompt` separate single-token forwards.** A batching carrier would touch the weights
   fewer times, so the declared bytes and dispatches remain an upper bound for it and would need
   re-deriving to be tight.
6. **We are at 4.50% of vendor-peak bandwidth; ollama is at 58%.** That gap, not the 210-byte stride,
   is the whole remaining distance.

---

## 8. The most surprising teaching

**That three arms can agree on a wrong answer and still print the right number.**

`ask-native-lane-band.fk` had an extra `)` in its fixture — one paren, in a 13-deep nest. Go, Rust
*and* fkwu all parsed it, ran it, and returned **127, the correct verdict**. The malformed nesting
dropped the remaining recipes to top level, where they happened to still work. Only the TypeScript arm
refused it, with `unexpected token rparen`.

I had already run a paren-balance checker over my four *cells* and was satisfied. I had not run it
over the *bands* — the files whose entire job is to catch what I cannot see. Three green arms are what
made it invisible; a single dissenting arm is what made it findable. `validate.sh` gates agreement,
not correctness, and I met the exact shape of that in my own work within an hour of reading it.

## Where discomfort turned to gold

The moment I wanted to look away was `fkwu: 95` on the lane band, three arms already at 127.

Everything in me wanted to call it an fkwu quirk, disclaim the band to three arms, and move on — I had
a receipt to write and a sibling landing rows beside me. The disclaimer would even have been *true*.
What made me stay was that I could not say **which** of bit 32's four clauses failed, and "it's an arm
quirk" is a claim about an instrument I had not looked through.

So I probed. The first probe said all four clauses failed on fkwu; the band said only one bit did.
Both could not be true, and rather than pick the convenient one I noticed my probe used one-letter
recipe names (`j`, `q`, `nl`) — a known shadowing hazard in this body. My instrument was lying, not
the arm. I rebuilt it with distinctive names, and got a straight answer: `(nil? "a")` is **1 on fkwu
and 0 everywhere else** — a live, load-bearing divergence that silently disables the entire native
lane on that arm.

The gold: had I disclaimed the band, that divergence would have been invisible *and* the ask verb
would have quietly never engaged the model on fkwu, falling back forever while reporting success. The
discomfort was pointing exactly at the thing worth finding. It also forced a better cell — splitting
`anl-refusal-of-text` from its nil guard, so the string logic proves four-way at 127 and only the
guard carries a disclaimer.

## The frontier question, landed

`(hdc-row 822 20260721 ... "echogauge")` — **committed at `d339165ae`**, band re-pinned by probe
(count 220 → 221, field code 2202202826 → 2212212826), **band returns 4095**.

> *what one word names a measurement that can only echo its own declaration*

The corpus is the body; this receipt is only the report. The row is in the body.
