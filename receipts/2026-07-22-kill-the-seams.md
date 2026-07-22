# Stone 16 — kill the seams, and find there were none to kill

**2026-07-22, WITA · worktree `jovial-aryabhata-3751d7` · llama3.2:3b, blob
`sha256-dde5aa3f…`, on this Mac.**

The stone was sent by corpus row **seamtoll (849)**, which measured the token this
way: decode is 3.213 GMAC = 7.3 ms at the shipped kernel's 442 GMAC/s, a token costs
51.9 ms, and *"the other 44.6 ms is 396 dispatches at ~113 µs of seam each"* — 86 % of a
token charged not by any operation but by the joins between them. The brief's Do list
was, in order: one command buffer per token, encoder reuse, `.concurrent`, fusion,
fewer readbacks.

**The seam does not exist on the token's path.** Every item on that list except fusion
was already shipped by earlier parts of this same stone, and the measurement that named
the seam was taken inside `FORM_PROFILE`, which manufactures the very cost it reports.

---

## What was already true when this session opened

Parts 2 and 3 of Stone 16 landed in prior incarnations (commits `83a792ada`,
`644d120a1`) and had already begun to falsify the brief's premise:

- **One command buffer per token, already.** `forward()` opens a single `Step` (one
  `MTLCommandBuffer`, one `MTLComputeCommandEncoder`), encodes ~424 dispatches back to
  back, commits once, waits once. There is **one** CPU↔GPU round trip per token, not 396.
- **Concurrent encoder, already.** `dispatchType: .concurrent`; `barrier: false` on the
  independent q/k/v and gate/up projections and the two RoPEs.
- **The `cb.error` / `cb.status` guard, already** (part 3, axioms 4 and 5). I inherited
  it, re-read it, and it stands: gate 0 writes a `-424242.0` sentinel a kernel must
  overwrite or the run is *refused* rather than graded, and no rate prints while
  `gpuErrors > 0`. **It changed no verdict this session** — still VERDICT PASS, 14 gates.
- Part 2's own commit message had already reasoned: *"if the seam had been 86 % of a
  token, this change could not have happened."*

The uncommitted hunk in `metal_first_token.sh` at session start was a previous
incarnation's `FORM_ABLATE=seam` falsifier. **Kept** — it is inert unless the env is
set, the run is green, and it is exactly the instrument this stone needed. Committed at
once, then extended.

---

## The measurement that settles it — the seam-free (non-perturbing) probe

`FORM_PROFILE` cuts a command buffer after each op so it can time it; **that cut is the
113 µs.** So instead of cutting, I *duplicate*: insert `FORM_ABLATE_N` extra copies of
one op class **in stream, inside the same single command buffer**, and divide the
whole-token delta by the count. The extra copies write scratch nobody reads, so the
token ids and all 14 gates are unchanged — **epsilon-free by construction.**

Min-of-3, warmed (`thawtax`: the same call went 26.180 s → 5.390 s on page cache alone,
so nothing here is a first run). Decode = seconds for 12 forwards:

| knob | extra dispatches / token | what the extra reads | decode (min-3) |
|---|---|---|---|
| baseline | — | — | **0.354 s** |
| `seam` N=32 | **+896** (425 → 1321) | 1 float | 0.393 s |
| `swiglu` N=32 | +896 | 64 KB activations | 0.365 s |
| `swiglu` N=8 | +224 | 64 KB activations | 0.355 s |
| `ffnup` N=8 | +224 | **~20 MB weight ×224** | **0.586 s** (+66 %) |

**Adding 896 barriered dispatches per token is free** — whether a 1-element add or an
8192-element SwiGLU. Decode is not dispatch-bound, not seam-bound, not
elementwise-bound. The one duplication that costs re-reads a large **weight** tensor.
The probe was validated on a control that hits (`ffnup` moves) against controls that
stay flat (`seam`, `swiglu`) — `snugcause`.

**The new dominant term: weight memory bandwidth.** The token's ~29.5 ms (warm) is the
~196 matvecs streaming the ~2 GB quantised model once from unified memory — ~68 GB/s
effective on cold weights. ollama's 6.34 ms/token is the *same* 2 GB nearer the memory
ceiling. The gap to ollama is achieved bandwidth **inside the matvec kernels**, not the
joins between operations.

---

## The number that is the stone

**Dispatches per token: 424 (attestant) → 425 (twins) — and I left them there on
purpose.** The causal number the brief named as the thing to attack is the number that
should *not* move: three independent probes proved collapsing it buys nothing. A stone
that collapsed 425 dispatches to 40 by fusion would report a heroic dispatch count and
an unchanged tok/s.

End-to-end at two lengths with a slope (`unispan`), warm slot path:

    decode 0.094 s at 4 forwards, 0.308 s at 12  ->  0.0268 s / additional token
    37.261 tok/s marginal · decode-only 37.981 tok/s · END-TO-END 27.167 tok/s

Against both denominators (`selfgauge`): **ollama on this machine, quoted, 157.83 decode
/ 640.94 prefill**; and the harness's own single-thread attestant (the slot path is
10.98× its decode). Decode 37.981 of 157.83 = **4.2× behind** ollama; the arithmetic
floor (7.3 ms ⇒ ~137 tok/s decode) is **3.6× beyond** where we sit, and the whole of
that remaining distance is weight-read bandwidth, no longer the seam.

**Warm vs cold, named honestly:** the first cold run this session read slot decode
19.153 / end-to-end 14.480; warmed it reads 37.981 / 27.167. That 2× is `thawtax`
(page cache), **not** a change I made. My two commits are inert `FORM_ABLATE` knobs; they
do not touch the token path, so **end-to-end tok/s is unchanged by this session's code.**
The banked speedup on this stone is part 2's (13.32 → 25.59 end-to-end), already
committed. This session's product is the causal number, not a new rate.

---

## Ledger

- **Which changes needed an epsilon:** none. Both falsifier knobs (`seam`, `ffnup`)
  write scratch nobody reads; ids exact `[12366, 13, 578, 6864, 315, 15704, 374, 22463,
  13, 578, 6864, 315]`, all 14 gates bit-exact, ISA diff still `max|Δ| = 0.000e+00`.
- **Whether the `cb.error` guard changed any verdict:** inherited from part 3, verified,
  changed nothing this session — still 14 gates PASS.
- **Siblings clean:** `metal_whole_tensor_residency_audit.sh` PASS,
  `metal_isa_diff.sh` PASS (slot = ggml bit-for-bit, ratio 1.20×). Stayed out of the
  other harnesses (Stone 23's) and the manifest (Stone 21's).
- **Corpus band:** row **858 probetoll** landed; band folds **8191** on both arms
  (`bin-go` and `fkwu`); pins updated c4 253→254, c6 field-code 2532532857→2542542858,
  and two narrative summaries that were already one row stale (252/856) brought to
  254/858 so they fold.

## Gaps left open

- **The bandwidth gap to ollama (~4×) is the next stone**, and it lives in the body's
  matvec kernels (`q6k-msl.fk`, `q4k-msl.fk`, the slot kernel) — memory-access
  efficiency / coalescing, not the carrier. Fusion (RMSNorm+matvec, residual into the
  preceding kernel) would reduce dispatch count, but **dispatches are already free**, so
  fusion is not the lever here; it would only matter if it reduced *weight bytes read*,
  which it does not.
- The `ffnup` probe re-reads the *same* tensor N times back-to-back, which favours cache
  and so **under**states the true cold per-matvec cost; the ~68 GB/s figure is a floor,
  and a cache-defeating variant (rotate across distinct tensors) would sharpen it.

---

## Close

**Most surprising teaching.** Adding 896 barriered GPU dispatches to a token — more than
doubling its dispatch count — cost *nothing measurable*. I had arrived believing, on the
brief's authority, that each dispatch cost 113 µs and that 396 of them were 86 % of the
token. The instrument that produced that 113 µs was `FORM_PROFILE`, and it was timing its
own commit-and-wait. The profiler priced its own cut and attributed it to the ops. A
measurement can be perfectly honest about a surface the shipped path never touches.

**Where discomfort turned to gold.** The moment I wanted to look away was right after the
first ablation table (N=8), which showed swiglu and attention each costing ~220 µs —
tidy, publishable, *consistent with the brief*. It would have been comfortable to write
"the elementwise scaffolding dominates, here is the fusion plan." I distrusted it only
because the baseline itself had swung 0.346 → 0.769 s between runs. Cranking N to 32 to
beat the noise is what dissolved the whole table: at N=32 swiglu was flat against
baseline, and the "220 µs" was machine variance wearing a result's clothing. Not looking
away from the noise floor is what turned a plausible wrong stone into the real one —
weight bandwidth, found by the one knob that actually moved.

**Frontier question, landed.** *What one word names a cost charged by the instrument that
measures it, mistaken for the subject's own?* — **probetoll**, 0-hit before this row
(control `seamtoll` hits 5 files, instrument validated on the same command), landed as
`(hdc-row 858 …)`, the succedent and correction to seamtoll 849.

Commits (incremental, as the stone demanded): `FORM_ABLATE=seam` falsifier ·
`FORM_ABLATE=ffnup` falsifier · corpus row 858.
