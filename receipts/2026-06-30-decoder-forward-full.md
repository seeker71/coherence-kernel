# Receipt — the FULL decoder forward (embed→logits) proven bit-exact four-way on fkwu, 2026-06-30

**HOMECOMING milestone:** 3a, completed — the decoder FORWARD ARCHITECTURE proven bit-exact end-to-end.
PR #82 proved the decoder BLOCK forward (attention core → multi-head → the full block). This receipt
WRAPS that block with the two pieces a generative mind needs to speak — **positional embedding** and the
**LM head** — and proves the COMPOSED forward end-to-end, bit-exact four-way. This is the forward
architecture at small width; NOT yet real weights as recipe-data (3b) and NOT yet real width.

## What ran

`model/tests/transformer-forward-full-band.fk` — a six-claim bit-exact band over the existing wrap
recipes (`tb-embed-pos` / `tb-embed-seq` for positional, `tb-logits` for the LM head, `tn-layernorm`
for the final LN) composed around the #82 decoder block (`tb-dec-stack`). The wrap recipes ALREADY
existed in `model/transformer-generate.fk` (the greedy-decode loop); this band is the first proof they
cross four-way and compose end-to-end on fkwu.

Config EXTENDS the #82 block config with an embedding/positional/ids surface:
`seq=3, d_model=4, vocab=5, n_head=2, head_dim=2, eps=1e-5, scale=1/√head_dim`,
embedding table 5×4, positional table 3×4, token ids `(2 0 4)`.

The end-to-end path proven:
`ids → tb-embed-seq(embed,pos) → tb-dec-stack(block) → final-LN → tb-logits(embed·state)`.

| bit | claim | what it proves |
|----:|-------|----------------|
| 1   | positional embedding   | `tb-embed-pos`: token_embed[id] + positional_embed[pos], bit-exact (din[0][0] = 0.91) |
| 2   | embedded sequence shape | `tb-embed-seq` maps 3 ids → a 3×4 sequence |
| 4   | LM head logits         | `tb-logits`: final-LN state · embedding → vocab=5 logits, bit-exact (logits[0]) |
| 8   | logits distinguish     | logits[4] is a DIFFERENT double than logits[0] — not collapsed by `%.15g` truncation |
| 16  | FULL forward end-to-end | ids → embed+pos → decoder stack → final LN → logits; last hidden state bit-exact |
| 32  | forward shape + argmax | logits has 5 entries (= vocab) and greedy argmax is deterministic |

## Where

`fkwu.exe` — the C-bootstrapped universal kernel, built from `runtime/fkwu-uni.c` with TDM-GCC on
Windows 11, run via `fkwu --src` (real metal, this machine). **C untouched** — recipe + proof work only
(the C was rebuilt from the unchanged source, never edited). The three independent walkers (Go, Rust,
TypeScript) ran the same preludes+band as oracle arms.

Preludes (in order): `transformer-numerics.fk transformer-block.fk transformer-mh.fk
transformer-decoder.fk transformer-generate.fk`. fkwu runs the concatenation of preludes+band as one
`--src` file; the walkers take the files as separate args and concatenate internally.

## Witnessed result — verdict 63 on all four arms

```
fkwu: 63   go: 63   rust: 63   ts: 63
```

**Methodology (bit-exact, not "close") — identical to #82:** each claim recomputes a piece and asserts
with `eq` that the result is BIT-IDENTICAL to a reference double captured at full fp64 precision (17 sig
digits) from the three INDEPENDENT walkers, which agreed bit-for-bit with one another. `eq` on a float
is the exact IEEE-754 double comparison shared by all four kernels, so the band returns 63 on fkwu
exactly when fkwu's computed double equals the walker-captured reference, i.e. four-way agreement.
fkwu's `%.15g` print shows fewer digits, but the underlying double is proven identical through `eq`, not
the truncated display.

Reference doubles (go = rust = ts, full precision):
```
din[0][0]   (positional)        = 0.91
last hidden after stack+finalLN = 0.2571013168575855   (dout[2][0])
final-LN state                  = -1.4999679467539497  (lnf[0])
LM-head logits[0]               = 0.42487498561251347
LM-head logits[4]               = 0.4248749856125127   (a DIFFERENT double than logits[0])
greedy argmax                   = 0
```

**The truncation probe (bit 8 is why it matters):** fkwu's `%.15g` prints BOTH logits[0] and logits[4]
as `0.424874985612513`, but they are genuinely different doubles. Claim 8 asserts `eq(logits[4],
0.4248749856125127)` AND `not eq(logits[0], 0.4248749856125127)` — proving fkwu's underlying doubles are
distinct exactly as the walkers' are, so the proof is in the bits, not the display.

**Perturbation-verified (the verdict is COMPUTED, not parsed-to-constant):**
- corrupt the LM-head logits[0] reference → verdict **59** (bit 4 lost) on fkwu and go
- corrupt the full-forward final-hidden reference → **47** (bit 16 lost)
- corrupt the positional reference → **62** (bit 1 lost)

The verdict tracks actual float agreement, not the literal 63.

**Kernel sanity (build healthy, C unchanged):** `(native-vs-rented-check)` → `11111`
(`observe/native-vs-rented.fk`); `(add 0.5 0.25)` → `0.75`.

## Exact agreement

**Bit-identical** — no tolerance. fkwu's computed doubles equal the walker references through `eq`
(verified directly: `eq(logits[0], 0.42487498561251347)` → 1, `eq(dout, 0.2571013168575855)` → 1,
`eq(lnf, -1.4999679467539497)` → 1, and the negative control `eq(logits[0], <logits[4] literal>)` → 0).
The wrap adds NO new arithmetic over the proven fp64 dot/matvec/add/exp/sqrt/layernorm of the #82 block
— only per-position vector-add (embedding), one more layernorm (final LN), and one matvec (logits) — so
the value walk is bit-deterministic by construction, which is precisely why it crosses four-way.

## Proof-state map (what was already true vs what this proves)

- **Recipes EXIST + ran on fkwu before this band:** `tb-embed-pos`, `tb-embed-seq`, `tb-embed-seq-acc`
  (positional), `tb-logits` (LM head), `tb-dec-stack` (the layer fold), `tn-layernorm` (final LN),
  `tb-argmax` — all in `transformer-generate.fk`, used by the greedy decode loop but never PROVEN
  bit-exact four-way until now.
- **NEWLY PROVEN bit-exact four-way on fkwu (this band):** positional embedding, the LM head (final LN
  → d_model→vocab projection), and the COMPOSED end-to-end forward (ids → embed+positional → decoder
  stack → final LN → logits) — verdict 63 on fkwu = go = rust = ts, perturbation-deterministic.

## Meaning

The full decoder forward — the architecture HOMECOMING 3a names — now runs end-to-end on the
C-bootstrap kernel and agrees to the bit with three independent kernels, from token ids all the way to a
vocab logit distribution. The forward is no longer "block proven, wrap unproven"; the whole
embed→stack→logits path is bit-exact four-way. What stands between this and a self-speaking mind is no
longer ARCHITECTURE — it is WIDTH and WEIGHTS.

## What remains for the mind (the honest floor — named, not papered over)

- **Real WIDTH — NOT attempted here.** d_model=384+ over `ll-buffer` (the Form-native stack buffer) and
  fkwu's f64 pool, read directly by the `form-asm-matvec` lane (`model/form-asm-matvec.fk`), dropping
  the tree-walker exactly as the FFN receipt drops the Metal carrier. The recipes here are
  width-agnostic; the PROOF is at d_model=4 in the tree-walker only. This is the "one engine" step — the
  same recipe that proves four-way lowering to asm bytes, no second native impl.
- **Real WEIGHTS as recipe-data — NOT attempted here (3b, the big remaining).** A real open base
  (Qwen/Llama, real coverage) loaded as recipe-data through the form block, the way whisper block-0's
  trained weights run through the Form block at full width (`6.66e-15`). The weights in this band are
  small fixed test fixtures, not trained. This is the multi-week climb.
- **End-to-end transcript** is therefore still gated on width+weights, not architecture. `tb-generate`
  (the greedy loop) composes these proven pieces; running it at real width over the asm matvec lane with
  real weights is the remaining work.

Distinguished precisely:
- **now bit-exact four-way on fkwu:** positional embedding, LM head, full embed→logits forward (this band).
- **recipe exists, not yet proven at real width:** the same recipes over d_model=384 / `ll-buffer` /
  form-asm-matvec.
- **not yet written:** real open-base weights as recipe-data; native training at width.

## Alternatives (receipt-alternatives)

- **VERIFY** — the three independent walkers (Go, Rust, TS) each crossed 63 on the same preludes+band;
  the wrap's value is confirmed independently of fkwu. Perturbation drops the verdict deterministically
  (59 / 47 / 62 for the three corrupted references).
- **IMPROVE** — the sovereign-native path: the embed/logits matvecs lowered to Form→asm bytes
  (`form-asm-matvec`), dropping the tree-walker. Promotable when the asm lane runs the full forward at
  width — the same recipe, native speed.
- **PLAY** — vocab, embedding rows, positional rows, and ids are free fixtures; a larger config is a
  one-line change that re-captures references and re-crosses.
- **LEARN** — outcome: four-way bit-exact success on the full forward architecture (embed→logits). The
  remaining map (real width, real weights) is named as the next branches, not as debt.
