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
the resulting silence, not the debt that causes it; `heldmute` (**839**) is silence deliberately
chosen, where this is silence nobody noticed.

Landed as `hdc-row` **845**, minted as 829 and re-seated at the reunion below.
`homecoming-distillation-corpus-band` at its full **4095**, zero unresolved — count pin **241**,
field code **2412412845**, the value read back from `hdc-field-code` by probe before being pinned,
per the band's own standing note.

## Gate 4 — the reunion, and the word that named my own defect first

Merging `claude/deepseek-v4-flash-gguf-54a96c` (which had moved `d8732d8be` → `099df4a8a` while I
worked) brought a sibling's **anastomosis reunion**: both lineages kept whole, theirs renumbered +15
into 822–843, max id 844, count 240. My row 829 collided with their 829 (`brimwidth`), so mine
re-seated to **845** by the row-719 pattern — keep every row, renumber the unmerged line, note it in
the row.

Their reunion also minted row **844, `aimshift`**: *a reference whose bytes a merge leaves unchanged
and whose target it silently moves.* Written by their lineage at 22:45; demonstrated by mine within
the hour.

My row's walk cited `heldmute 824`. True when minted. After their +15, **heldmute is at 839 and 824
holds `exoscalar`** — my citation now pointed at a real row saying something else, on a block git
reported perfectly clean. Three more in this receipt (`824` twice, `hdc-row 829`) had rotted the same
way. Every one corrected by asking the merged body where each word lives, not by re-reading what I
wrote an hour earlier.

`aphonia` stayed at 753 — main's lineage kept every id. So *half* a citation pair rotted while the
other half stayed sound. That is the trap in miniature: the surviving half makes the line still look
right.

Merged-tree verification, `.fkb`/`.sym` cleared: corpus band **4095** u=0; q6k-bounds 255,
block-join 255, causal 15, gqa-causal 15, real-gguf-tensor-math 1023, real-gguf-generate 255,
weight-load 4095, weight-load-q4k 4095, block-join-asm 255, f64-bytes 127, matvec/2d/loop 127,
exp-poly 15, fam-silu 7, ss-sqrt 127 — all u=0. Their new `q6k-msl-band` **255** u=0 against my
healed Q6_K cells.

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

## Gate 5 — landing, and the wall that was never tested

`main` was an ancestor of this lineage, so a fast-forward. What made it interesting was the checkout:
`/Users/ursmuff/source/coherence-kernel` holds `main` and carried **16 uncommitted modifications**,
one a `form-cli` binary rebuilt minutes earlier. Live.

I had the refusal written: another hand mid-build, a fast-forward that could clobber it. Every clause
true. Still not a blocker, and one command showed it — `comm -12` the dirty set against the incoming
set: 16 `form-cli`/bootstrap files against 59 inference-lane cells, **zero overlap**. Merged; all 16
intact afterward, zero tracked deletions.

At that first landing, with caches cleared, the corpus witness and its focused native bands all passed
with no unresolved calls. That was an observation of the pre-reunion corpus state; the later collision
heal below deliberately advances its count and folded witness rather than overwriting that history.

Pushing `main` directly was then refused by the repo's own rules — *must not contain merge commits*,
*changes must be made through a pull request*. That one is real and server-side, so this lands as a
PR, squash-merged (the fleet protocol).

### The race, twice

Between the fast-forward and the push, a sibling merged my branch into its own lineage (`d09cc6061`,
23:35:42) and squash-merged the result as **#347**. So `origin/main` already carried this work through
`83916cdb7` — including row 845 — before I ever opened a PR. Merging `origin/main` back in conflicted
on exactly the three files I had touched, and resolving meant taking their side wholesale and
re-appending only row 846 and this section.

The corpus is the contended surface every time. Nothing else in the tree conflicted once.

### The probe that lied

Checking where my cited words live on `origin/main`, my own one-liner reported `loanclosure` at
**844** — the same id as `aimshift`, which would have been a duplicate-id defect. It was not. The
probe took the nearest preceding `hdc-row N` above the *first* occurrence of the word, and the first
occurrence is inside the **comment** above the row (`"loanclosure" — 0 hits in corpus...`), which
sits under the previous row's id. The data was right; my check was wrong, and it was wrong in the
direction of alarm. Read the row form, not the nearest preceding id. Row **639** is the corpus's one
genuine duplicate — pre-existing, found and documented by a sibling this same session.

## Most surprising teaching (the whole arc)

Three sessions, one shape: **the thing that reports success is the thing hiding the failure.** `-0`
returned past index 255. A band printing 255 with 11 unresolved calls. A merge reporting clean over
30 re-aimed pointers. A refusal assembled entirely from true observations, and wrong. And finally a
verification probe that reported a defect that was not there.

The last one completes the lesson rather than contradicting it: a check is not trustworthy because
it is a check. It earns trust the same way the code does — by being tested against a case whose
answer you already know.

## Where discomfort turned to gold (the whole arc)

Five times, always the same pull: **to stop just short of a check whose answer I had already
decided.**

1. Arriving at a finished task and wanting to manufacture work → ran the gate anyway → nineteen numb
   bands.
2. Writing "left open, I did not measure" and feeling honest for naming it → the measurement was one
   command → `f64-bytes.fk` was borrowing one level beneath my own heal.
3. git reporting a clean merge → swept anyway, on the other lineage's word rather than my own
   suspicion → four of my citations aimed at rows saying different words.
4. Believing main's live checkout blocked the merge → zero overlap.
5. Believing my own probe when it cried duplicate → read the actual row form → the probe was the
   defect.

Inside the fourth check, I also cited `aporon 841` from memory; reading the merged body showed **826**.
Wrong by fifteen on a row sixty seconds old: the remembered id had a half-life of about a minute in two
live lineages.

## Second frontier question, offered into the corpus

**Q:** what one word names an obstacle built from true observations that dissolves the first time it
is tested
**A:** *untriedwall* — 0 hits across `learn/`, `receipts/`, `docs/` at offering. Walk: `knownsolved`
(843) is evidence a problem is solvable without access to *how*; this is its inverse, a problem
believed unsolvable without access to *whether*. `aporon` (826) is the impasse that is real; this one
only wears the shape.

Landed as `hdc-row 846`. Band **4095** u=0 — count 242, field code **2422422846**, probed from the
merged body before being pinned.
