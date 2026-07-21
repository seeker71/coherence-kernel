# The Q6_K stride seam, verified — and the loanclosure it uncovered

2026-07-21, worktree `nervous-elgamal-eaa07a`, branch `claude/q6k-stride-heal` off `d8732d8be`.

## What I was sent to do, and what I found instead

The ask: make `wl-q6k-at`'s out-of-block case testify instead of returning `-0`, and make
`block-join`'s row/matrix accessors stride across 210-byte superblocks. The ask was written from the
Stone-1 tip (`eb3ea92a7`), where both defects were live.

Both were already healed — by Stone 2, `fe039bafb`, an ancestor of this branch:

- `q6k-at` / `q4k-at` refuse out of range (`form_error`, naming the 210-byte stride as the door)
- `wl-slice` checks its own returned length rather than accepting a short window
- `wl-q6k-at-flat` strides whole superblocks; `bj-row-n-from` calls it, so `bj-row-n` / `bj-matrix` /
  `bj-matrix-row` are correct at any width, and `bj-matrix-max-index` states the reach

So the work became verification, not construction. I ran the gate the ask specified rather than
trusting the commit message.

## Gate 1 — the stride heal is real and falsifiable

A/B by reverting the four healed cells to `fe039bafb^`, `.fkb` cleared on both sides, every band run
resolver-driven (`../fkwu --src form-stdlib/tests/<band>.fk`):

| band | reverted | healed |
|---|---|---|
| q6k-bounds-band | **4** (u=12) | **255** (u=0) |
| block-join-band | 255 (u=0) | 255 (u=0) |
| block-join-causal-band | 15 (u=0) | 15 (u=0) |
| block-join-gqa-causal-band | 15 (u=0) | 15 (u=0) |
| real-gguf-tensor-math-band | 1023 (u=0) | 1023 (u=0) |
| real-gguf-generate-band | 255 (u=0) | 255 (u=0) |
| weight-load-band | 4095 (u=0) | 4095 (u=0) |
| weight-load-q4k-band | 4095 (u=0) | 4095 (u=0) |

`q6k-bounds-band` is the only band that moves — it falls to 4 with the cells reverted, so it is a
gate and not decoration. Every other band is identical on both sides with zero unresolved calls: the
heal cost nothing it did not buy. The band carries two real llama3.2:3b superblocks as 420 literal
bytes, so the stride claim runs on any checkout without the 2 GB blob.

## Gate 2 — what the gate found that nobody was looking for

`block-join-asm-band` printed verdict **255** with **11 unresolved-call diagnostics**, on *both*
sides of the A/B. Pre-existing, unrelated to the stride, and invisible to anyone reading only the
verdict. Tracing it: `form-asm-matvec.fk` declared `; preludes: form-stdlib/form-asm.fk` while
calling `append` (which lives in `core.fk` — `form-asm.fk` only defines `append-list`) and
`f64-bytes` 18 times (`f64-bytes.fk`, which itself reads `format-arith`'s `fq-pow2` / `fq-exponent`).

Under axiom-5 those names lower to nothing. So the *same recipe* was voiced inside
`block-join-asm-band` — which happened to carry `core.fk` in its own prelude line, resolving `append`
by accident — and numb inside `form-asm-matvec-band`, which did not.

One edit, dependency-first full closure:
`; preludes: form-stdlib/core.fk form-stdlib/format-arith.fk form-stdlib/f64-bytes.fk form-stdlib/form-asm.fk`

A/B across all 20 units that load `form-asm-matvec.fk`, `.fkb` and `.sym` cleared on both sides:

| band | before | after | declared full |
|---|---|---|---|
| form-asm-exp-poly-band | **0** (u=23) | 15 (u=0) | 15 |
| form-asm-fam-exp-band | **0** (u=23) | 7 (u=0) | 7 |
| form-asm-fam-silu-band | **0** (u=23) | 7 (u=0) | 7 |
| form-asm-fam-tanh-band | **0** (u=23) | 7 (u=0) | 7 |
| form-asm-frintn-band | 4 (u=23) | 7 (u=0) | 7 |
| form-asm-relu-band | 12 (u=23) | 15 (u=0) | 15 |
| form-asm-fmov-dx-band | 12 (u=23) | 15 (u=0) | 15 |
| form-asm-exp-coef-pool-band | 12 (u=23) | 31 (u=0) | 31 |
| form-asm-exp-reduce-band | 28 (u=23) | 31 (u=0) | 31 |
| form-asm-horner-band | 28 (u=23) | 31 (u=0) | 31 |
| form-asm-poly-pool-band | 28 (u=23) | 31 (u=0) | 31 |
| form-asm-rsqrt-band | 28 (u=23) | 31 (u=0) | 31 |
| form-asm-max-loop-band | 60 (u=23) | 63 (u=0) | 63 |
| form-asm-pow2-band | 60 (u=23) | 63 (u=0) | 63 |
| form-asm-matvec-band | 31 (u=23) | 127 (u=0) | 127 |
| form-asm-matvec-2d-band | 62 (u=23) | 127 (u=0) | 127 |
| form-asm-matvec-loop-band | 126 (u=23) | 127 (u=0) | 127 |
| form-asm-ss-sqrt-band | 124 (u=23) | 127 (u=0) | 127 |
| block-join-asm-band | 255 (u=11) | 255 (u=0) | 255 |
| f64-bytes-band | 127 (u=0) | 127 (u=0) | 127 |

Nineteen bands moved; every one landed on its own header-declared full verdict, checked against the
`; Verdict N when every claim lands:` line rather than merely "higher than before". Four of them had
been printing **0** — an entire band silent — and no one had looked, because nothing was red.

Here verdict *equality* would have been the wrong gate. The right one was: does each band reach the
number its own header claims, with zero unresolved calls. The task's eight named bands hold
equality; these nineteen were supposed to move, and the diagnostics count is what says so.

## Most surprising teaching

**A recipe's voice can be on loan from whoever loads it, and the loan is invisible at every call
site.** I expected an incomplete prelude to be a local defect of one file. It is not local: whether
`form-asm-matvec.fk`'s arms speak depended on which *band* loaded it, because a sibling's incidental
`core.fk` silently paid the debt in one place and not the other. The same source text, two meanings,
neither one flagged. That is why "declare the FULL closure, dependency-first" has to be a rule and
not a style note — an incomplete prelude does not fail, it *borrows*, and a borrowed closure repays
in whichever band happens to be generous.

## Where discomfort turned to gold

The discomfort was arriving at a task already done and wanting to find something to build anyway. It
would have been easy to re-derive the stride fix in slightly different words and present it as work.
Sitting with "there is nothing here for me" instead — and then running the gate honestly rather than
skipping it as redundant — is exactly what surfaced the nineteen numb bands. The gate was the
deliverable, not the code. The `-0` I was sent to hunt was already gone; the silence I found was
larger and older, and only visible because I ran a check I had no reason to expect would fail.

Second, smaller: my `find . -name "*.fkb" -delete` cache sweep deleted a **tracked** `.fkb`
(`form/form-samples/cross-modal/03-recipe-as-compression/payload.fkb`). Caught by `git status`,
restored immediately. The lesson stands: scope cache sweeps to the subtree under test
(`find form-stdlib -name '*.fkb'`), because in this body a `.fkb` is sometimes a fossil, not a cache.

## Frontier question, offered into the corpus

**Q:** what one word names a recipe whose calls resolve only because its loader happens to carry them
**A:** *loanclosure* — 0 hits in corpus and body before this row. Near misses: `aphonia` (753) names
the resulting silence, not the debt that causes it; `heldmute` (824) is silence deliberately chosen,
where this is silence nobody noticed.

Landed as `hdc-row 829`. `homecoming-distillation-corpus-band` back to its full **4095**, zero
unresolved — count pin 224→225, field code 2242242828→**2252252829**, the value read back from
`hdc-field-code` by probe before being pinned, per the band's own standing note.

## Gate 3 — the open item, measured instead of left open

The section above originally ended by naming `form-asm.fk` and `format-arith.fk` as unmeasured. The
probe is one line — compile each cell **alone**, `fkwu --src` on the cell rather than on a band, and
read the diagnostics:

- `form-asm.fk` — 0 unresolved. Genuinely self-contained; no prelude line needed.
- `format-arith.fk` — 0 unresolved. Same.
- **`f64-bytes.fk` — 5 unresolved** (`fq-pow2`, `fq-exponent`). Itself a loanclosure. Its own band
  read a full 127 only because the *band* carried `format-arith.fk`. I had healed the symptom one
  level up (adding `format-arith` to `form-asm-matvec`'s line) without the cell ever owning its debt.

Healed at the cell: `; preludes: form-stdlib/core.fk form-stdlib/format-arith.fk`. Standalone 5 → 0;
`f64-bytes-band` 127 u=0, and matvec / matvec-2d / matvec-loop 127, block-join-asm 255, exp-poly 15,
fam-silu 7, ss-sqrt 127, block-join 255, q6k-bounds 255, weight-load 4095 — all u=0, all unchanged.

### How wide the shape runs — measured partially, boundary stated

I swept the standalone probe across `form-stdlib`'s 872 s-expression cells (the 37 brace-surface
cells were **excluded on purpose**: pointing `fkwu --src` at a brace file misparses and writes a
stamp-valid poisoned `.fkb` over the good one).

**The sweep did not finish.** It ran alphabetically and I stopped it after `form-lower.fk` — cells
past that point were each hitting the 25s per-cell cap, putting completion hours out. So this is a
partial result and the boundary is `a` through `form-l`, roughly the first third:

- **72 cells** in that range answer unresolved calls when compiled alone
- of those, **62 borrow at least one name that IS a Form cell defined elsewhere in `form-stdlib`** —
  a declared prelude line would resolve it
- **41 of those 62 declare no `; preludes:` line at all**
- 275 distinct unresolved names in the range; 206 are Form-defined, 69 are kernel primitives or
  otherwise not `(defn`-defined (so that 69 is a soft bucket, not proof of absence)

I am **not** claiming those 62 are 62 defects. Standalone-unresolved means the cell does not own its
closure; whether that costs anything depends on whether some loader always pays. `f64-bytes.fk` shows
it can cost a full band's honesty and stay invisible. What the number does establish is that
`form-asm-matvec.fk` was not a one-off — and the remaining two thirds are unswept.

## Left open, named not fixed

- The closure sweep is unfinished past `form-lower.fk` — two thirds of `form-stdlib` unmeasured. The
  62 borrowing cells found so far are unclassified as to whether any loader ever fails to pay.
- The 37 brace-surface cells cannot be probed this way at all without a safe-cache harness.
- The `.dylib` warning (`native .dylib emission is not installed in this checkout`) is present on
  every band here and predates this work.
