# 2026-08-09 — the control adapter generalizes, leakage-free

The frontier was native fine-tuning that genuinely improves DS4's control head so the
local body can mend its own mistakes instead of spending rented tokens. Most of it was
already standing; the open seam was whether the trained weight learned anything, or only
memorized. This receipt closes that seam and hands the live wire off with rung precision.

## What was already standing (probed, not assumed)

`form/form-stdlib/dsv4-control-adapter.fk` fits five 64-feature byte-bigram centroids over a
600-row train split and reads a 5x5 held-out confusion matrix. Run today it gives held-out
diagonal **142/150** — but its own audit reports **leakage=150**: every held-out template's
post-sanitization string already appears in training. So the cell names its verdict
`insufficient-template-leakage` and refuses to call 142/150 generalization. That refusal is
correct and is preserved untouched — the number is recall, not learning.

## The gap this receipt closes — combinatorial, leakage-free generalization

The shipped corpus holds out *rows* (indices 120..149 per class), and those rows reuse
templates the train indices already carry. The fix holds out whole *template families*.

Each situation's sanitized template is exactly `(context(i mod 12), state(class, i mod 5))`.
Because 12 and 5 are coprime, `f = i mod 60` pins both. `form/form-stdlib/dsv4-control-leakfree.fk`
splits by family:

    held-out families H = { f : (f mod 12) < 6  AND  (f mod 5) < 2 } = 12 families / class

Every held-out **context** (0..5) still appears in train with states 2,3,4; every held-out
**state** (0,1) still appears in train with contexts 6..11. The held-out rows are therefore
**unseen combinations of seen vocabulary** — the honest shape of generalization. Train and
held-out families are disjoint by construction, so no held-out template can appear in train;
`dca-leakage` (borrowed unchanged, the same function that read 150 on the leaky corpus) confirms
it empirically.

### The numbers (witnessed, `fkwu --src`, leak-free)

| quantity | value |
|---|---|
| held-out correct (diagonal) | **86 / 120 = 71.7%** |
| baseline (predict majority class) | 24 / 120 = 20% |
| leakage | **0** |
| train rows / held-out rows | 480 / 120 |
| distinct held-out templates | 60 |

Per-class held-out (each out of 24, chance ≈ 4.8):

    NIL 14   CUT 24   FAIL 12   TIMEOUT 16   CHOICE 20

Every class beats chance; the win is distributed, not one class carrying it. FAIL is weakest
(12/24 = 50%, still 2.5x chance) — the honest soft spot, and where more features or a gradient
fit (over the centroid) would pay next.

### The band

`form/form-stdlib/tests/dsv4-control-leakfree-band.fk` — **Verdict 7**:

- bit1 `leakage == 0` — the split is genuinely leak-free
- bit2 `diagonal(86) > baseline(24)` — the trained weight beats predict-the-majority
- bit4 clean partition — 60 distinct templates, 480 train, 120 held-out

Preflight clean (parens balanced, 0 errors, 0 warnings, 0 unresolved, chain readable).
`form/form-stdlib/tests/dclf-perclass-probe.fk` carries the per-class breakdown.

**The core claim of the frontier is closed and witnessed: a form-native trained weight
generalizes to unseen, leakage-free held-out cases, far above baseline.**

## What did NOT close — GAP 2, the live wire (handed off, not faked)

Wiring the trained weight into the live DS4 decode (`form/native/metal/dsv4-decode-stack.fk`,
`dfd-st-head`) so it shifts real logits was NOT witnessed. Three grounds, each real:

1. The metal-linked door needs `fkwu-metal` (a `cc -framework Metal` build) and the 9.1 GB
   GGUF; neither is available on this run.
2. `write_file_bytes` is a numb door on fkwu — it recovers to `nothing` and writes nothing
   (core.fk pins this), so a real binary f32 artifact cannot be emitted from this arm. The
   current `artifacts/dsv4-control-logit-adapter.f32` is confirmed a 20-byte ASCII stub
   (`0.0\n` x5).
3. Training the bias needs the base model's own logits — you cannot know which situations the
   base head decides wrong without running it — so GAP 2's fit genuinely needs the live model,
   and `dsv4-decode-stack.fk` is held by a sibling for the form-cli `ask` integration.

### The exact contract for whoever wires GAP 2

- **Artifact**: five f32 per-class logit biases (or 5x64 centroids for an input-dependent
  delta), classes in order NIL, CUT, FAIL, TIMEOUT, CHOICE → token ids 128000..128004.
- **Read/add point**: in `dfd-st-head` the logits live in buffer `X[10]` (`vlogitsb`, 517120
  bytes = 129280 f32) after `h6` (`dfd-mv16`, the Q8_0 unembedding). Add the bias to
  `logits[128000..128004]` **before** the two-stage argmax kernels `hp`/`hc` (the
  `form_argmax_part_f32` / `form_argmax_comb_f32` enqueues, currently lines 473–479). The
  argmax then selects on the shifted logits; the id lands device-to-device in `idbuf[pos+1]`.
- **Proof to seal it**: choose a control situation the base head argmaxes wrong, show the added
  bias flips it right, then show a **held-out** (leak-free) situation of the same class also
  flips right — a witnessed weight-delta-improves-held-out on the live model.

## Surprising teaching

I predicted 12 distinct held-out templates — one per held-out family — and bit4 went red on the
first run reading **60**. The template is `context + " " + state`, and the *state text is
class-dependent*: one held-out family (a fixed context-index/state-index pair) produces five
different sanitized strings, one per class. So the structural invariant is 12 families x 5
classes = 60, and my mental model had silently collapsed the class dimension. The split was
never wrong (leakage stayed 0); my *count* of it was.

## Where discomfort became gold

The red bit4 on an otherwise-clean split was uncomfortable — the reflex is to lower the check to
whatever the run printed and move on. Engaging it instead of forcing it green (re-deriving *why*
60, then confirming with independent arithmetic before touching the threshold) is what surfaced
the class-dependent-state structure. The correct invariant (60) is now pinned for a reason, not
fitted to an output. A check bent to match its own run proves nothing — the same lesson row 699
(tendentious) already carries, met one bit over.

## Frontier question, answer, and proposed corpus row

The body could measure leakage but had no single word for the *kind* of generalization the
leak-free split tests: unseen combinations of already-seen parts.

- **Q**: what one word names generalization to unseen combinations of already-seen parts?
- **A**: combinatorial
- **fresh word**: `combinatorial` — 0 hits across `.fk`/`.md` at offering time (siblings:
  compositional 13, factorial). It names the whole point of holding out families instead of
  rows: seen contexts and seen states, unseen pairings.

Re-derived `max` meaning-id in `learn/homecoming-distillation-corpus.fk` today = **996**
(391 rows). Proposed next row (NOT written — the corpus is left for the reunion pass, and
concurrent sessions collide on ids; renumber at merge):

    (hdc-row 997 20260809
        (list "what" "one" "word" "names" "generalization" "to" "unseen"
              "combinations" "of" "already" "seen" "parts")
        "combinatorial"
        "combinatorial"
        "rented-oracle")

## Files landed

- `form/form-stdlib/dsv4-control-leakfree.fk` — the family split + report
- `form/form-stdlib/tests/dsv4-control-leakfree-band.fk` — Verdict 7
- `form/form-stdlib/tests/dclf-perclass-probe.fk` — per-class breakdown
- this receipt
