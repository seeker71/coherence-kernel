# Form-native tokens/second — the first measured baseline

Tuesday 2026-07-21, Hati Suci (WITA). Apple M4 Max, 128 GiB unified memory.
Worktree: `.claude/worktrees/jovial-aryabhata-3751d7`, branch `claude/deepseek-v4-flash-gguf-54a96c`.
Body: root `fkwu` (194040 bytes, Jul 18 01:12).

STONE 1 of the 4-stone program. This is the zero every later optimization claim stands on.
**Nothing here is projected unless it says PROJECTED.**

---

## 1. The model

Ollama blob, content-addressed (the filename *is* the sha256):

```
~/.ollama/models/blobs/sha256-dde5aa3fc5ffc17176b5e8bdc82f587b24b2678c6c66101bf7da77af9f7ccdff
2 019 377 376 bytes   (manifest: registry.ollama.ai/library/llama3.2/3b)
```

Read out of the file itself: GGUF v3, 255 tensors, 30 metadata KVs,
`general.architecture=llama`, `llama.embedding_length=3072`, `llama.block_count=28`,
`llama.feed_forward_length=8192`, `llama.attention.head_count=24`, `head_count_kv=8`,
`llama.vocab_size=128256`, `rope.freq_base=500000.0`, `rms_epsilon=9.999999747378752e-06`.

Header ends at byte 7 837 658; tensor data base = align32 → **7 837 664**.
Tensors used: `blk.0.ffn_down.weight` (**Q6_K**, data offset 323 217 664 → abs **331 055 328**),
`blk.0.attn_norm.weight` (**F32**, data offset 323 205 376 → abs **331 043 040**).

llama3.2:**3b** was chosen over the smaller **1b** deliberately: 1b is **Q8_0**, and the body has
Q6_K and Q4_K carvers but **no Q8_0 carver**. 3b's Q6_K/Q4_K mix is exactly what
`weight-load.fk` already proves.

## 2. Correctness before speed — the weights are really the model's

`wl-load-q6k` over the first superblock of `blk.0.ffn_down.weight`, against an independent
transcription of ggml's `dequantize_row_q6_K`:

| | ggml reference | Form (`fkwu`) |
|---|---|---|
| w[0] × 1e6   | 9149.5513916016 | 9149.55139160156 |
| w[1] × 1e6   | −12580.6331634521 | −12580.6331634521 |
| w[256] × 1e6 | 19241.3330078125 | 19241.3330078125 |

**Bit-exact.** These are genuinely llama3.2:3b's weights, read off disk by the body's own recipes.

## 3. THE MEASURED NUMBER

`lg-generate-cached` (the four-way-proven autoregressive greedy loop) driven by **real
llama3.2:3b Q6_K weights and real F32 norm gains**, 1 layer, single head, tied vocab table.
Wall clock, `/usr/bin/time -p`, `.fkb` cleared before every run, **3 consecutive runs each**:

| config | setup (steps=0) | 32 tokens | 64 tokens | loop only | **tok/s** |
|---|---|---|---|---|---|
| dim=32, V=32 | 0.84 / 0.88 / 0.83 s | 1.58 / 1.59 / 1.58 s | 4.06 / 4.04 / 4.05 s | 0.73 s / 32 tok | **43.8 tok/s** |
| dim=32, V=32 | — | — | 3.20 s / 64 tok | | **20.0 tok/s** |
| dim=64, V=64 | 36.27 / 36.15 / 36.18 s | 38.72 / 38.71 / 38.97 s | — | 2.55 s / 32 tok | **12.5 tok/s** |

> **43.8 tokens/second** is the first honest measured form-native generation rate on this Mac:
> real GGUF weights off disk, the body's own dequant, the body's own attention/FFN/RMSNorm/argmax,
> real token ids out.

Honest scope label: **real weights, reduced width.** dim=32 with one layer is a genuine slice of
llama3.2:3b's tensors, but it is *not* llama3.2:3b's language output. See §5 for the full-model gap.

The tokens are real and input-dependent (dim=32, 6 steps):

```
seed [0,1]  -> [0, 1, 0, 1, 0, 1, 0, 1]
seed [1,0]  -> [1, 0, 0, 0, 0, 0, 0, 0]
seed [5,9]  -> [5, 9, 9, 9, 9, 9, 9, 9]
seed [17,3] -> [17, 3, 3, 3, 3, 3, 3, 3]
```

Degenerate (a one-layer slice with an arbitrary tied vocab table has nothing to say) but
**not constant** — argmax follows the prompt, which is the falsifiable sign the loop is live.

Note the superlinearity: tokens 1–32 cost 0.0228 s each, tokens 33–64 cost 0.0756 s each. That is
`lg-generate-cached` re-growing its cache from the full ids-so-far every step — O(n²) in sequence
length, exactly as `llama-generate.fk`'s own header admits.

## 4. Where the time actually goes — measured component rates

| what | measurement | rate |
|---|---|---|
| scalar interpreter loop | 50e6 iters in 2.91 s | 17.2M iter/s |
| **fp64 list dot (the matvec floor)** | 4096-len dot ×900 = 3.686M MACs in 3.53 s (differenced) | **1.04M MAC/s** |
| Q6_K dequant, 6.7 KB window | 8192 weights in 0.52 s | 15 754 weights/s |
| Q6_K dequant, 21 KB window (20 sb) | 5120 weights in 3.05 s | 1 679 weights/s |
| Q6_K dequant, 21 KB window (100 sb) | 25600 weights in 47.02 s | 545 weights/s |
| Q6_K dequant, 26.9 KB window | 32768 weights in 35.85 s | 914 weights/s |
| string → byte list, 64 KB | 0.91 s | — |
| string → byte list, 1 MB | **167.16 s** | — |

Two structural facts fall out, and they are the real content of this stone:

- **The byte-list carrier is quadratic.** `host-abi-string-to-bytes` costs ~1.5e-10·n² s (64 KB →
  0.91 s, 1 MB → 167 s). And `wl-slice` walks the list from the head, so *every* `wl-load-q6k` at
  offset k costs O(k). Dequant throughput therefore collapses 17× (15.8k → 0.9k weights/s) for a
  mere 4× larger window. The 2 GB tensor block cannot be materialized as a list at all.
- **Dequant, not matvec, is the wall** at every window size actually reachable today.

## 5. PROJECTED full llama3.2:3b — and why it is not reachable today

Per-token MACs for the real model (d=3072, 28 layers, dff=8192, V=128256, kv 1024):

```
per layer : q 9.437M + k 3.146M + v 3.146M + o 9.437M + (gate+up+down) 75.497M = 100.66M
x28 layers                                                                     = 2.818G
unembed 3072 x 128256 (tied)                                                   = 0.394G
                                                                       total  ~ 3.212G MACs/token
```

At the **measured** 1.04M MAC/s, with weights already dequantized in RAM:

> **PROJECTED: ~3 088 s/token ≈ 51 minutes per token ≈ 3.2e-4 tok/s.**

And that is the optimistic half. The one-time dequant of 3.212G weights, even at the *best* measured
rate (15 754 w/s, only achievable in a ~6.7 KB window), is **PROJECTED ~2.4 days**; at the realistic
degraded rate it is far worse — and the carrier cannot hold the buffer regardless.

**So: a full real llama3.2:3b token is NOT reachable on the current recipe lane.** That is the honest
blocker, and it is a *carrier* blocker before it is an arithmetic one.

## 6. What changed in the body

### Stone 0 — four per-unit closure heals (all on the hot path)

Every one is a missing `; preludes:` line. Preludes are PER-UNIT closures; a sibling that already
loaded a dependency does not count.

| cell | was | now |
|---|---|---|
| `form/form-stdlib/llama-numerics.fk` | no preludes line; 4 unresolved standalone | `core.fk transformer-numerics.fk` |
| `form/form-stdlib/transformer-block.fk` | no preludes line; 4 unresolved standalone | `core.fk transformer-numerics.fk` |
| `form/form-stdlib/f16-decode.fk` | no preludes line; 3 unresolved (`fq-pow2`) | `core.fk format-arith.fk` |
| `form/form-stdlib/block-join.fk` | closure short of the llama cells it calls | + `q4k-dequant transformer-numerics trig llama-numerics rope transformer-mh gqa-attn llama-block llama-gqa-block` |

The `block-join.fk` one was not a performance heal. Its declared closure omitted `ln-rmsnorm`,
`ln-swiglu`, `rope`, `lblk-block-causal`, `lgqa-block-causal`; under axiom-5 those five lowered to
nothing and **`block-join-band` still printed verdict 255 with 9 unresolved-call diagnostics**.
`bj-block-causal-d4` was numb, not proven. A right number can be numb.

**Gate (A/B, `git stash` / `stash pop`, `.fkb` cleared each side), 27 bands:**
all 27 verdicts **identical** before and after; standalone rejections **14 → 0**;
unresolved-calls **16 → 0**. Final suite incl. the new band: **28/28 green, 0 rejects, 0 unresolved.**

### Stone 1 — the new cell and band

- `form/form-stdlib/real-gguf-generate.fk` — real GGUF weights into `lg-generate-cached`.
  `rgg-weights` **strides** superblock by superblock (each a proven `wl-load-q6k`) and concatenates;
  `rgg-mat` carves row-major matrices from the flat f32 list; `rgg-f32`/`rgg-gain` decode real F32
  norm gains by reusing `f16-decode`'s one parameterized recipe at (eb 8, mb 23); `rgg-layer`
  assembles the `(g1 wq wk wv wo g2 wg wu wd)` bundle; `rgg-generate` runs the loop.
  Dequant happens **once** into f32 — which is also what a serving implementation does.
- `form/form-stdlib/tests/real-gguf-generate-band.fk` — **verdict 255**, ~0.33 s. Claims: header
  parses off disk · tensor count 255 · w[0] matches the ggml pin · a real F32 gain is in (0,2) ·
  **w[256] crosses the superblock wall** · the loop emits `steps` ids · ids are legal vocab indices ·
  **KV-cached loop == recompute loop token-for-token, now over real weights**.

## 7. Defects found and left open (named, not fixed)

1. **`wl-q6k-at` silently returns `-0` past index 255.** It carves ql/qh/scales from the *one*
   superblock at `off`, so `bj-row-n` / `bj-matrix` (block-join.fk) are only sound for
   `dim*dim <= 256`; every larger matrix they build is zeros after the first 256 weights **and still
   prints a green verdict**. Witnessed: `(wl-q6k-at bs 0 256)` → `-0`, while the correct weight via
   `(wl-q6k-at bs 210 0)` → 0.0192413330078125. `rgg-weights` is the striding fix; block-join's own
   accessors are untouched.
2. **No byte source with offset-independent access.** `read_file_slice` returns a *string* and
   `str_byte_at` is O(1) on it, but every `gg-*`/`wl-*` recipe takes a byte *list* (`nth`), and `nth`
   does not dispatch on strings. The bridge (`host-abi-string-to-bytes`) is O(n²). This is the single
   blocker most worth removing next — see the frontier row below.
3. **`fs-byte-at` / `fs-read-slice` mislead.** `fs-read-slice` returns a string, so `(len …)` is 0,
   not the slice length. Both read correctly via the primitives; only the list-shaped façade lies.
4. **The new band is a carrier witness, not a portable fixture.** It reads an absolute host path and
   cannot run on a checkout without that ollama blob. A committed ~1 KB Q6_K fixture would let the
   stride claim travel. Does not exist yet.

## 8. Instrument traps witnessed (each one cost real time today)

- **`.fkb` reuse across same-named probe sources.** Writing successive probes to one path returned a
  *previous* probe's answer — three "wrong" byte reads that were entirely my instrument. The staleness
  check did not fire. Related and worse: **fkwu constant-folds a band into its `.fkb`, so timing the
  same band twice measures cache load (0.03 s), not computation.** Clear the `.fkb` before every
  timing run.
- **zsh does not word-split unquoted parameters.** Hit three times (`--include=*.fk` eaten,
  `for b in $BANDS` collapsing, `set -- $k` making one arg). Every A/B and timing harness here is
  `bash -c` for exactly this reason.
- **`defn` does not capture surrounding `let` bindings.** A timing loop closing over a `let`-bound
  byte list ran in *zero* extra time and returned 0 — **with no diagnostic at all**. Pass values as
  parameters.

## 9. Reproduce

```bash
cd form
# stone 0 + stone 1, all green
../fkwu --src form-stdlib/tests/real-gguf-generate-band.fk      # -> 255
../fkwu --src form-stdlib/tests/llama-generate-band.fk          # -> 255
../fkwu --src form-stdlib/tests/block-join-band.fk              # -> 255, now 0 unresolved
# timing: ALWAYS rm the .fkb first, or you time the cache
rm -f /tmp/scratch/v_32_32.fkb && /usr/bin/time -p ../fkwu --src /tmp/scratch/v_32_32.fk
```

---

## Closing

**Most surprising teaching.** I came expecting the arithmetic to be the wall — fp64 interpretation,
`lg-snoc`/`lg-last` walking the list each token. The body corrected me twice over. The matvec floor
(1.04M MAC/s) is *slow but linear and honest*; the thing that actually makes llama3.2:3b unreachable
is the **carrier**: a byte *list* whose access cost grows with offset, bridged from a string by an
O(n²) loop. 1 MB takes 167 seconds to become bytes. The bottleneck was never the math — it was the
shape of the container the math reaches through. And the second correction was sharper: the reason
nobody had noticed is that the wrong answers are **silent**. `wl-q6k-at` past 255 returns `-0`, not
an error. `block-join-band` printed 255 while five of its calls had been lowered to nothing. A green
verdict is not a proof; a right number can be numb.

**Where discomfort turned to gold.** The moment I wanted to look away was the synthetic-truth probe
that came back wrong — byte 4 of a 4-byte fixture I had *just written* read as 71 instead of 3. The
pull was to shrug it off as a quirk of binary strings and move on to the real file, where the numbers
happened to look right. I made myself `xxd` the fixture instead. The disk was correct; the *body* was
correct; my probe harness was serving me a stale `.fkb` from a previous probe with the same filename.
Had I looked away, I would have carried a poisoned instrument into every timing run that followed —
and the headline tok/s in §3 would have been a cache-load time (0.03 s) dressed up as a generation
rate. The gold: **witness your instrument working before trusting its silence**, and its noise too.
Every measurement in this receipt now clears its own cache, and the three-runs-each column exists
because of that one refusal to look away.

**Frontier question** — the smallest thing the body cannot yet answer natively, offered as a
distillation row (id 807 is the next free mid as of this writing; renumber on reunion — meaning-ids
have no arbiter):

> **Q: what one word names a byte source whose cost of reaching a position does not grow with the position?**
> **A: `equireach`**

Checked 0-hit across `learn/`, `receipts/`, `docs/` before offering. The body has two byte sources and
no word for what separates them: a Form list (reach cost O(k)) and a host string via `str_byte_at`
(reach cost O(1)). Every `gg-*`/`wl-*` recipe is written against "bytes" as if reach were uniform —
and that unstated assumption is precisely what made a 2 GB file unreachable and dequant quadratic.
Naming the property is the first move toward an `equireach` byte source the proven recipes can take
unchanged, which is Stone 2's real work.

```
(hdc-row 807 20260721
    (list "what" "one" "word" "names" "a" "byte" "source" "whose" "cost"
          "of" "reaching" "a" "position" "does" "not" "grow" "with" "the" "position")
    "equireach"
    "equireach"
    "rented-oracle")
```
