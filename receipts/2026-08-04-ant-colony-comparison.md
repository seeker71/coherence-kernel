# 2026-08-04 — one question to every local lane, and the bar that moved mid-run

Urs asked to see every local model answer the same thing, side by side:

> what is unique to an ant colony?

Mid-run the standard narrowed, in his words: *"all the gguf models shall be using 100% form code, no
external tools, no host membrane passing other than reading the model file."* The ollama comparator
lanes were dropped before any of them ran — no ollama process was started in this session, and no
ollama text appears below.

So this receipt carries two columns that turn out to be almost opposite: **what the lane says**, and
**how much of the saying is ours**. The lane that answers the question well is the one that leans
hardest on a host membrane; the lane that is genuinely 100% Form computes a 4x4 matvec and has no
opinion about ants.

Host: Apple M4 Max, 128 GiB, Darwin 25.3.0. Every command below was run from this worktree.

---

## The table

| lane | model | said something? | how fast | Form floor |
| --- | --- | --- | --- | --- |
| `metal_first_token.sh` | llama3.2:3b (2 019 377 376 B blob) | **yes — a real, fluent, correct paragraph** | 20.138 tok/s end-to-end, 20.891 decode-only, 5.46 s wall | MSL emitted by a Form cell; **1020-line Swift driving loop** |
| `metal_ask.sh` | llama3.2:3b | no — `FAIL`, but the generation underneath it succeeded | 11.06 s wall | same, plus a broken text extractor |
| `ask_ds4.sh` | DeepSeek-V4-Flash, 91 321 404 640 B | **text, but degenerate** — on this question and on its own witnessed control prompts | 8.05 / 7.82 / 8.08 / 8.25 / 8.20 t/s; ~105–110 s wall each | tokenizer is a Form cell, MSL from Form cells; **2206-line Swift driving loop** |
| `metal_kat_exit.sh` | KAT-Coder v2.5, 17 391 937 152 B | one token id, not a continuation — `3637`, which a Form cell decodes as `parent` | 7.131 s device wall, 11.62 s total | 93-line Swift driver; the decode of 3637 was 100% Form |
| `metal_kat_block0.sh` | KAT-Coder v2.5 | no token at all, by construction | 1.210 s device wall, 5.91 s total | 111-line Swift driver |
| `tests/metal-door-band.fk` | none — a 4x4 the cell writes itself | verdict **15**, the GPU agrees with Form | 0.81 s | **100% Form**: the only row that clears the bar |

---

## Lane 1 — llama3.2:3b, and the only real answer to the question

`metal_ask.sh` was the documented door. It failed:

```
$ form/native/metal/metal_ask.sh 96 "what is unique to an ant colony?"
  body cache HIT  declared shape
  gate D1 the derived tensor bytes + data base ARE the file's 2019377376 bytes, exactly
  generating through form/native/metal/metal_first_token.sh (the gated lane, gate-mode genonly)...
  the lane passed its own gates: 9-PASS-genonly
  the answer came from the slot-simd-4wide path
FAIL  could not read ANSWER out of the lane's output — the harness's format moved
```
rc=1, 11.06 s.

**The generation did not fail.** `metal_ask.sh:243` reads the answer with
`awk '/^ANSWER-TEXT-BEGIN$/{f=1;next} …'` and falls back at `:245` to
`sed -n 's/^ *text *: *"\(.*\)"$/\1/p'` — a one-line pattern. This answer has newlines in it, so both
readers came back empty and the carrier called a working model a failure. Going one level down to the
harness that actually generates:

```
$ FORM_GEN_ONLY=1 form/native/metal/metal_first_token.sh 96 "what is unique to an ant colony?"
```

```
PASS  gate 1 config read from the blob's own metadata KVs, self-consistent
resident: the whole 2019377376-byte blob in ONE MTLBuffer on Apple M4 Max, zero copies
pooled: 56.7 MB of activation + KV state, allocated ONCE for the whole run
=== the prompt ===
  "what is unique to an ant colony?"
  ids: [128000, 12840, 374, 5016, 311, 459, 3276, 42036, 30]
  encode 0.000157 s for 9 prompt tokens
  pieces: [<|begin_of_text|>][what][ is][ unique][ to][ an][ ant][ colony][?]
=== gate 0: did the GPU run at all ===
  sentinel -424242.0 -> [0.01121521, 0.009719849, 0.014205933, 0.023925781]
PASS  gate 0 the GPU executes: a kernel overwrote the CPU's sentinel, and no command buffer errored
=== gates 2-4: three points on the token's own path ===
PASS  gate 2 embedding gather: all 3072 weights of token_embd row 791 BIT-EXACT vs Form (Q6_K, one rounding each side)
  rmsnorm worst relative deviation 5.423e-07 vs derived bound n*u = 1.831e-04 (0.3% of it)
  cooperative twin vs the one-thread attestant: 3072/3072 outputs BIT-IDENTICAL
PASS  gate 3 RMSNorm on the GPU tracks Form's fp64 inside the derived n*u bound, AND the cooperative twin is the one-thread attestant BIT FOR BIT on all 3072 outputs
  q[0] GPU 2.86895156  Form(fp64) 2.86895305  |d| 1.492e-06  bound cols*u*SUM|term| 2.030e-03 (0.1% of it)
PASS  gate 4 a real Q4_K fused matvec at width 3072 is the body's answer
PASS  gate 8 the split kernel at parts=1 IS the attestant, bit for bit, on all 3072 rows of 3072x8192
PASS  gate 8b the HOISTED kernel at parts=1 is ALSO the attestant, bit for bit — the hoist costs no accuracy at all
PASS  gate 9 at parts=32 every row stays inside the DERIVED bound (worst 0.00% of it)
PASS  gate 9b the LANE and SLOT kernels BOTH stay inside the SAME derived bound at parts=32 (worst 0.00% of it)
=== FORM_GEN_ONLY: the slot path, once. The cross-path agreement gates were NOT run. ===
  slot-long : prefill 0.172 s for 9 prompt tokens; decode 4.595 s for 96 further forwards
```

**The answer, verbatim, exactly as the lane printed it:**

```
 social hierarchy, division of labor, and communication
Ant colonies are fascinating social structures that have evolved to enable ants to thrive in a wide range of environments. Here are some unique aspects of ant colonies that are shaped by social hierarchy, division of labor, and communication:

**Social Hierarchy:**

* **Queen Ant:** The queen ant is the largest ant in the colony and is responsible for laying eggs. She is the only ant in the colony that reproduces, and her ph
```

```
    END-TO-END 20.138 tok/s over 96 generated tokens (prefill+decode 4.767 s)  |  decode-only 20.891 tok/s
  STOP reason=cap stop_id=-1 eos_id=128009 generated=96 cap=96
  LATENCY encode 0.000157 s + prefill 0.172 s (9 prompt tokens) + decode 4.595 s (96 forwards) = 4.767 s in-process
=== VERDICT PASS — 9 gates (FORM_GEN_ONLY; gates 5, 6, 7, 10, 11 not run) ===
```

It stopped at the 96-token cap, mid-word (`her ph`), not at an end of thought. That is the cap doing
its job, not the model failing.

**Against the 100% Form bar.** `metal_first_token.sh` is 1227 lines of bash. Lines 177–1196 are a
Swift heredoc — **1020 lines** — compiled by `swiftc -O` at `:1208`; that Swift owns the decode loop,
the KV cache, the buffer pool and the sampling. What *is* ours: the MSL kernels, emitted by the Form
cell `ft-emit-msl` and run through the Go arm of the kernel at `:142-143`, and the tokenizer.
**Verdict: real answer, fails the bar.**

## Lane 2 — DeepSeek-V4-Flash: text, and it is degenerate

The question, as asked:

```
$ form/native/metal/ask_ds4.sh -n 48 "what is unique to an ant colony?"
```
```
what is unique to an ant colony? The<|place_holder_mm_span_0155|> in the<|place_holder_mm_span_0155|> in the<|place_holder_mm_span_0155|><|place_holder_mm_span_0155|> triad File细节细节协议的细节协议的详细信息文件和 the specifics mandate mandate协议的 mandate protocol Protocol Protocol Protocol Protocol Protocol协议的......

  [form-native · 43 layers · generation 8.05 t/s · 110s wall · greedy, base-model continuation]
  [token ids: [455, 271, 128981, 295, 270, 271, 128981, 295, 270, 128981, 271, 128981, 126185, 13559, 271, 18847, 18847, 98090, 18847, 98090, 122800, 105846, 270, 73040, 41561, 41561, 98090, 41561, 12093, 29326, 29326, 29326, 29326, 29326, 98090, 339, 339, 339, 339, 339, 339]]
```
110.63 s real.

This is a base-model continuation lane — no chat template, no stop token, greedy — so the brief asked
for a continuation phrasing as well:

```
$ form/native/metal/ask_ds4.sh -n 48 "What is unique to an ant colony is"
```
```
What is unique to an ant colony is the698097029138921029097029<|place_holder_mm_span_0027|>698138029656901088656901419901921419<|place_holder_mm_span_0155|>921 investigations901921921698604901620696656901719719901719

  [form-native · 43 layers · generation 7.82 t/s · 108s wall · greedy, base-model continuation]
  [token ids: [270, 271, 31457, 35354, 30079, 10363, 30155, 30079, 35354, 30079, 128853, 31457, 10363, 30079, 26010, 25622, 30517, 26010, 25622, 22903, 25622, 30155, 22903, 128981, 30155, 26441, 25622, 30155, 30155, 271, 31457, 25196, 25622, 21814, 27389, 26010, 25622, 28800, 28800, 25622, 28800]]
```
107.70 s real. Not a phrasing problem.

### The control, which is the only reason the paragraph above means anything

A degenerate answer to a hard question could be the question's fault. The brief named a prompt as
already witnessed fluent, so I ran it:

```
$ form/native/metal/ask_ds4.sh -n 44 "The largest planet in our solar system is"
```
```
The largest planet in our solar system is the name, the<|place_holder_mm_span_0155|> detection device detection device detection device detection was theปัญห这个大 spaces the<|place_holder_mm_span_0155|> spaces the detection device detection device detection the detection device detects the detection device detects the detection

  [form-native · 43 layers · generation 8.08 t/s · 106s wall · greedy, base-model continuation]
```

Then the prompt this lane's *own* receipt (`receipts/2026-08-01-the-model-is-useful.md`) records as
producing `red, yellow, and blue. They are called primary because they cannot be produced by mixing
other colors…`:

```
$ form/native/metal/ask_ds4.sh -n 40 "The three primary colors are"
```
```
The three primary colors are
  [form-native · 43 layers · generation 8.25 t/s · 109s wall · greedy, base-model continuation]
  [token ids: [129236, 295, 270, 128981, 2356, 17537, 11347, 12093, 295, 270, 29326, 1227, 29326, 29326, 29326, 29326, 29326, 29326, 29326, 16, 2359, 271, 16, 2359, 29326, 29326, 29326, 11347, 270, 128981, 70866, 671, 29326, 29326, 29326, 9494]]
```
(the decoded text line was overwritten by a `stale .fkb` warning sharing the stream; the ids carry
it — `29326` seven times running.) A shorter re-run printed the text:

```
The three primary colors are<|place_holder_mm_span_0410|> in the<|place_holder_mm_span_0155|>
  [token ids: [129236, 295, 270, 128981]]
```

**So the lane is regressed against its own receipt, on its own verified prompt, and the rate is
8.2 t/s against the 28 t/s that receipt stamps.** The ant question was never the variable.

### What the probes rule in and out

- **Not the tokenizer.** Round-tripped through the body's own cell, 100% Form:
  ```
  $ printf 'encode The largest planet in our solar system is\n' | ./fkwu form/form-stdlib/dsv4-tokenizer-cli.fk
  token_ids=671 9152 13540 295 1132 11250 1487 344
  $ for id in 671 9152 13540 295 1132 11250 1487 344; do printf 'decode %s\n' $id | ./fkwu … ; done
  [The][ largest][ planet][ in][ our][ solar][ system][ is]
  ```
  Exact both ways. The forward is fed the right ids and the degenerate ids come back degenerate.
- **Not the knob removal.** `582086177` (2026-08-03) deleted four knobs from `metal_dsv4_stack.sh`
  after the fluent receipt, which made it the obvious suspect. I extracted the pre-removal stack
  (`git show 541351626:…`) and ran it with the same environment: **byte-for-byte the same degenerate
  continuation**, 8.28 t/s. Hypothesis dead, and dead by measurement rather than by argument.
- **Not visible to the lane's own gates.** The stack reports
  `VERDICT PASS  54 gates — 43 HETEROGENEOUS DeepSeek-V4-Flash LAYERS STACKED at real dims …`,
  0 FAIL, while emitting that stream. A run with `FORM_DS4_GATES=1` exited 0 as well.
- **Not a long-context drift.** Degeneracy starts at generated token 1–4 in every run.

**UNFINISHED, with the next step.** The defect sits between "the right ids enter" and "the exit head
argmaxes", it is older than 2026-08-03, and 54 gates are blind to it. Next step: run the single-token
harness `form/native/metal/metal_dsv4_token.sh` against the 2026-08-01 receipt's first-token ids for
`The three primary colors are` — that harness carries per-stage witnesses, so the first stage whose
number moved names the layer. That is a bisect over stages, not over commits, and the commit bisect
above is already spent.

**Against the 100% Form bar.** `metal_dsv4_stack.sh` is 2515 lines of bash. Lines 291–2496 are a
Swift heredoc — **2206 lines** — compiled at `:2497` and run at `:2499`; a second Swift probe sits at
`:96-101`. What *is* ours: every MSL kernel, emitted by Form cells at `:262-263`; the residency plan
and the GGUF manifest, emitted by Form cells at `:110-116`; and the tokenizer both ways
(`:60-61`, `:2504-2505`). **Verdict: real text, degenerate, fails the bar.**

## Lane 3 — KAT-Coder v2.5: one token, and it is honest about being one token

```
$ form/native/metal/metal_kat_exit.sh
PASS  the body located its own tensors: embed@551343232 type 11, out@10990720 248320x2048 type 8, norm@551335040 type 0
PASS  body-emitted MSL compiled (    6145 bytes)
PASS  wrapped 17391937152 bytes with bytesNoCopy — device.currentAllocatedSize = 17392025600 B
PASS  embed row 100 decoded on device: 1578/2048 nonzero, max |w| 0.051116943
PASS  output_norm applied: 1578/2048 nonzero
PASS  every command buffer completed without error
PASS  all 248320 logits finite
PASS  logits non-degenerate: 4096+ distinct values sampled
TOKEN  argmax id=3637 logit=10.066510  over 248320 rows x 2048 cols  wall 7.131 s
VERDICT PASS
```

It reproduces the id witnessed 2026-08-03. It is a **single next-token argmax, and not even a real
one**: the script says so itself — the vector fed to the vocabulary projection is the embedding, not
the hidden state of 41 blocks. It cannot be asked about ants. It has no loop.

I decoded 3637 with a cell of my own, and this part *is* 100% Form — `read_file_slice` on the model
file and nothing else:

```lisp
; preludes: … form-stdlib/gguf-meta.fk form-stdlib/kat-coder-embed.fk
(let at (gmt-token-off h 3637))
(dump h at 0 (gmt-token-len h at))
```
```
VOCABN 248320
TOKOFF 47568
TOKLEN 6
BYTE 112 / 97 / 114 / 101 / 110 / 116
```

`112 97 114 101 110 116` = **`parent`**. Which is a pleasing thing for an ant-colony receipt to have
fallen out of a mechanism witness, and means nothing at all.

`metal_kat_block0.sh` goes the other direction — into the model rather than out of it — and is
equally clear that it produces no token:

```
PASS  attn_qkv projected: all 8192 entries finite
PASS  the projection is 8192 wide — q 16x128 + k 16x128 + v 32x128, the split gated-deltanet-layer.fk was built against
PASS  causal conv ran over all 8192 channels: all finite
BLOCK0 front half ran: embed -> attn_norm -> attn_qkv(Q4_K 8192x2048) -> causal conv(4 taps x 8192 ch)  wall 1.210 s
VERDICT PASS
```
40 blocks, the delta rule, the output gate and the 256-expert MoE FFN are not wired. **KAT-Coder
produced no words in this session, and could not have.**

**Against the bar.** `metal_kat_exit.sh` is 202 lines with a 93-line Swift heredoc (`:94-186`,
`swiftc` at `:188`); `metal_kat_block0.sh` is 238 lines with 111 lines of Swift (`:111-221`, `swiftc`
at `:223`). MSL from Form cells in both. **Fails the bar; the smallest membrane of the three.**

## Lane 4 — the one row that clears the bar

```
$ ./fkwu --src form/form-stdlib/tests/metal-door-band.fk
SKIP fkwu-form-cli-metal-matvec-f32: no linked Metal carrier
metal_owner=fkwu-form-cli
metal_linked=false
expected-sum 360
0
```

Verdict 0 — because the plain `fkwu` at this repo's root is built by the one seed line in
`BOOTSTRAP.md` (`cc -O2 -o fkwu runtime/fkwu-uni.c`), which links no carrier. `form/build-form-cli.sh:34-35`
adds `native/metal/fk-metal-carrier.m` on Darwin with no flag; the `fkwu` seed does not. Building it
the way the band's own header documents:

```
$ cc -O2 -o fkwu-metal runtime/fkwu-uni.c form/native/metal/fk-metal-carrier.m \
     -framework Metal -framework Foundation -fobjc-arc
$ ./fkwu-metal --src form/form-stdlib/tests/metal-door-band.fk
PASS fkwu-form-cli-metal-matvec-f32
metal_owner=fkwu-form-cli
metal_linked=true
device=Apple M4 Max
kernel=form_fkwu_generic_matvec_f32
rows=4
cols=4
sum=360
y=30 70 110 150

expected-sum 360
15
```

**Verdict 15**, 0.81 s. A Form cell emitted its own MSL, encoded its own IEEE-754 bytes, drove this
M4 Max, and checked the four results against arithmetic it did itself. `1 door-linked | 2
dispatch-passed | 4 sum-agrees | 8 every-element-agrees`. No bash, no Swift, no Python between the
cell and the device. **This is the only row in the table that meets the standard Urs set — and it
computes a 4x4 matvec.**

The gap between verdict 15 and a decode loop is a door shape, and that door is **already being built,
by a concurrent agent, in this worktree, while I ran**. `runtime/fkwu-optable.h` now carries eight
new rows and `fk-metal-carrier.m` 389 new lines: `metal_pipeline` (compile once),
`metal_buf_alloc` / `metal_buf_from_file` (weights resident, buffers as handles),
`metal_enqueue` (dispatch without blocking), `metal_sync` (commit and wait, once per token),
`metal_buf_read`, `metal_status`. Which is exactly the shape `receipts/2026-08-03-crosscheap.md`
measured the need for: 2 us of seam against 112 us of `waitUntilCompleted`, ~2600 dispatches/token,
so blocking per dispatch costs ~291 ms/token against a 33 ms budget and enqueueing costs ~5.2 ms.

```
$ ./fkwu-metal --src <(echo '(print_str (metal_status))')
metal_owner=fkwu-form-cli
metal_linked=true
metal_door=handle
device=Apple M4 Max
unified_memory=1
```

**Concrete next steps to close the bar, in order:**
1. Add the carrier to the `fkwu` seed on Darwin the way `build-form-cli.sh:34-35` already does, so
   `metal_linked=true` is the default rather than a hand-built binary.
2. Write the resident forward for **llama3.2:3b** as a Form cell over `metal_pipeline` /
   `metal_buf_from_file` / `metal_enqueue` / `metal_sync`, and gate it by demanding the same token ids
   `metal_first_token.sh` emits for `what is unique to an ant colony?` — ids
   `[3674, 30022, 11, 13096, 315, 9511, …]` are recorded above, so the target already exists.
3. Retire the 1020-line Swift heredoc only after (2) is id-identical, never before.

3 072-wide llama is the right first model for this, not the 43-layer DS4 and not the 41-block KAT.

---

## What the answers actually say

Only one lane said anything about ants, so there is no comparison of content to make, and inventing
one would be the single worst thing this receipt could do.

**llama3.2:3b is correct and unremarkable.** Social hierarchy, division of labour, communication;
one queen who lays the eggs. It is true, it is what an encyclopedia would say, and it never gets to
anything a person would call *unique* — superorganism behaviour, stigmergy, the colony as the
reproductive unit rather than the ant. It ran out of tokens at `her ph` (`pheromone`, almost
certainly). It reads like a competent 3B model, because it is one.

**DeepSeek-V4-Flash produced text that is not language.** Placeholder control tokens, a run of seven
identical ids, Chinese fragments about protocols and detail, Thai. Base-model continuations are not
chat answers and no apology is owed for that — but this is not a base-model continuation either. It
is a broken forward, and the control prompts prove it.

**KAT-Coder produced one token, `parent`, from a path that is not the model's opinion about anything.**

**The Form door produced `30 70 110 150`.**

---

## The most surprising teaching

**A confident finding died twice in one session, and the second death was the one I nearly missed.**

The first was fine and is what bisects are for: the knob-removal commit was a beautiful suspect —
right file, right week, landed two days after the fluent receipt — and running the pre-removal stack
returned byte-identical garbage. Hypothesis dead in 110 seconds.

The second was not fine. I built `fkwu-metal` at 15:35, `fkwu-plain` at 15:52, found that
`metal_status` resolved in one and not the other, reproduced it three times, confirmed the optable
row was present and unconditional in both, and had a sharp, reproducible, *entirely false* defect
ready to write down: *linking the Metal carrier breaks name resolution.* It survived every check I
made because every check I made was of the **source**. What I had not checked was whether the source
was the **same source** — a concurrent agent was writing the handle door into `runtime/fkwu-uni.c`
and `runtime/fkwu-optable.h` at 15:40 and 15:45, between my two builds. `git status` at the start of
the session said clean; I only looked again because I was tidying up a temp file.

Reproducibility does not mean what I was using it to mean. I reproduced it three times *after* the
tree stopped moving, so all three builds sat on the same side of the edit and agreed with each other
about a world that no longer existed. Three agreeing runs that are wrong together is this repo's
`selfload` teaching, and I walked into it from a direction it does not describe: not a loaded host,
a **loading tree**.

The repair is cheap and I have adopted it: stamp source mtimes before and after any A/B build, and
refuse the comparison if they moved.

```
src mtimes before: 1785829505 1785829525 1785829258
src mtimes after : 1785829505 1785829525 1785829258
SOURCE STABLE ACROSS BOTH BUILDS
```

Rebuilt that way, both binaries resolve `metal_status` and the door band returns verdict 15. **The
defect was never there. I withdraw it.**

## Where discomfort turned to gold

The brief handed me a fact to stand on: `ask_ds4.sh -n 44 "The largest planet in our solar system is"`
produced a correct fluent Jupiter continuation at ~28 t/s. My run of that exact command produced
`the name, the<|place_holder_mm_span_0155|> detection device detection device…`, and the discomfort
was specific and unpleasant — the instinct that I had broken something, followed by the quieter
instinct to soften it in the write-up.

Following it instead of soothing it: `grep -rn "largest planet" receipts/` returns exactly one line,
in `receipts/2026-07-22-local-uncertainty.md:117`, a calibration table whose header reads
*"llama3.2:3b, full width, off the one resident quantized blob"*. It is a **single-token** entry —
`` ` Jupiter` ✓ `` with margins 0.636 / 1.189 / 2.025 — for a **different model** and a **different
lane**. The 28 t/s comes from a different receipt with different prompts.

So the fact I was handed was a composite of two true witnesses that were never true together. It read
as ground because it had a command in it, and a command in a brief looks like something someone ran.

The gold is the shape of the error rather than the error: **a claim assembled from two real receipts
is more dangerous than a claim with no receipt at all**, because it survives exactly the check a
careful reader performs — "is there a witness for this?" — and fails only the one nobody performs,
"is there *one* witness for *all* of this?" I had been about to write "the lane regressed against the
brief's prompt". The honest sentence, which the grep bought, is: the lane regressed against **its
own** receipt, on **its own** verified prompt, and the brief's prompt was never its prompt.

That is also why the control run is the load-bearing act of this whole session. Without it, "the ant
question makes DS4 degenerate" was a plausible, publishable, wrong story, and every number in it
would have been correctly measured.

## A frontier question, and my answer

**Q: When is a green verdict evidence about the world, and when is it only evidence about the gates?**

A: A verdict is evidence about the world exactly as far as some gate's expected value was fixed by
something other than the code under test. DS4's 54 gates all compare the lane to *derived bounds and
to itself* — every dispatch sentinelled, every stage finite, the stack self-consistent — and all 54
stay green while the stream is placeholder tokens and Chinese, because not one of them holds a
sentence a human would recognise. `metal-door-band.fk` is the opposite and says so in its own header:
`y0 = 1+4+9+16 = 30 … sum = 360`, hand-checkable, written before the run, knowable without running
anything. That is why its verdict 15 means something about this M4 Max and DS4's 54 mean something
about DS4's gates. The practical rule: **a suite needs at least one gate whose expected value a
reader could have written down without the program** — and for a language model that gate is a
sentence, which is precisely the gate no arithmetic suite ever contains. `metal_first_token.sh` has
it by accident, in the printed text a person reads; DS4 has it too, and the person had stopped
reading.

## Proposed distillation row — not landed, corpus untouched

Corpus max-mid is 986; this would be **987**. Verified **0 hits** for `mutefluent` in
`learn/homecoming-distillation-corpus.fk` and **0 files** across the tree.

```lisp
            ; 987 — mutefluent. One question to every local lane. llama3.2:3b answered
            ; it in correct English at 20.1 tok/s through 1020 lines of Swift; the only
            ; lane that is 100% Form drove this M4 Max to verdict 15 over a 4x4 matvec
            ; and has no words at all. Fluency and ownership are inversely ordered right
            ; now, and the reason is one door shape: metal_matvec_f32 blocks per dispatch
            ; (112 us against a 2 us seam), so a form-native decode needs enqueue+sync,
            ; not a better kernel.
            ; DS4 emitted placeholder tokens and Chinese on the ant question AND on its
            ; own receipt's verified prompt, at 8.2 t/s against a stamped 28 — while its
            ; own suite said VERDICT PASS 54 gates, 0 FAIL. Every gate compared the lane
            ; to a derived bound or to itself; not one held a sentence. The commit that
            ; looked guilty was exonerated by running it (byte-identical garbage), and
            ; the tokenizer round-trips exactly, so the defect is in the forward and is
            ; older than the suspect.
            ; Counted on the way: a brief's "proven" command was a composite of two real
            ; receipts — the prompt from a llama3.2:3b single-token table, the rate from
            ; a DS4 continuation receipt. Assembled from true parts, true nowhere.
            ; "mutefluent" — 0 hits in corpus before this row.
            ; (walk: liftmask 986 — the defect a forgiving runtime kept alive; this is
            ; the defect a green suite kept invisible, because nothing it checked spoke.)
            (hdc-row 987 20260804
                (list "what" "names" "a" "body" "whose" "clearest" "voice" "is"
                      "the" "least" "its" "own")
                "mutefluent"
                "mutefluent"
                "rented-oracle")
```

## Ground stamp

```
host            Apple M4 Max, 128 GiB, Darwin 25.3.0, worktree google-turboquant-vector-search-300c68
question        "what is unique to an ant colony?"  (identical text to every lane)
ollama          NOT RUN — standard narrowed mid-session; no ollama process started
llama3.2:3b     metal_first_token.sh, FORM_GEN_ONLY=1, 96-token cap, 9 gates PASS
                20.138 tok/s end-to-end, 20.891 decode-only, 5.46 s wall, stop=cap
                real fluent paragraph, quoted verbatim above
metal_ask.sh    rc=1 at :243/:245 — extractor is single-line, the answer has newlines
DS4             ask_ds4.sh x5, 8.05 / 7.82 / 8.08 / 8.25 / 8.20 t/s, ~105-110 s wall each
                degenerate on the ant question, on the continuation phrasing, on
                "The largest planet in our solar system is" and on its own receipt's
                "The three primary colors are"; VERDICT PASS 54 gates, 0 FAIL
                pre-knob-removal stack (541351626): byte-identical garbage, 8.28 t/s
                tokenizer round-trip exact both ways, 100% Form
KAT-Coder v2.5  metal_kat_exit.sh VERDICT PASS, argmax id=3637 logit=10.066510, 7.131 s
                id 3637 decoded by a Form cell over the GGUF: bytes 112 97 114 101 110 116 = "parent"
                metal_kat_block0.sh VERDICT PASS, no token by construction, 1.210 s
form Metal door tests/metal-door-band.fk verdict 15 with the carrier linked, 0 with plain fkwu
                y=30 70 110 150, sum 360, expected-sum 360 — the only 100% Form row
membrane sizes  metal_dsv4_stack.sh 2515 lines / 2206 Swift (:291-2496, swiftc :2497)
                metal_first_token.sh 1227 lines / 1020 Swift (:177-1196, swiftc :1208)
                metal_kat_exit.sh 202 / 93 Swift (:94-186)   metal_kat_block0.sh 238 / 111 (:111-221)
withdrawn       "linking fk-metal-carrier.m breaks metal_status resolution" — FALSE, an
                artifact of two builds straddling a concurrent agent's edit at 15:40/15:45.
                Rebuilt over a verified-stable source snapshot: both resolve it, band = 15.
corrected       the brief's "proven" DS4 Jupiter run: prompt is from receipts/2026-07-22-
                local-uncertainty.md:117 (llama3.2:3b, single token), rate from 2026-08-01.
```
