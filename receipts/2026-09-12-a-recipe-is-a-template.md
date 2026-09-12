# A recipe is a template

2026-09-12, evening, on a freed field. The goal names a "stdlib **template** library" — feature-equivalent
to C++'s, whose templates instantiate once per type. Until now the lane compiled a defn for the first
parameter signature it met, and every other signature fell to the walker: a `sum` hot on ints ran the
walker on floats; a `foldl` hot on one reducer walked another. One shape per name.

## What stands

In `runtime/fkwu-uni.c`, a defn now carries a **second instance** — one recipe, compiled once per
parameter signature:

- **Two instance slots per defn**: `fk_f64_mem`/`fk_f64_sig` (a) and `fk_f64_mem_b`/`fk_f64_sig_b` (b),
  grown and reset together.
- **The door picks by the frame's parameter bits.** `fk_f64_frame_pbits` reads which of the live
  arguments are float / string / list / defn (an int contributes none); the door keeps instance a when
  its param bits match, else switches to instance b when its do. The result and accumulator bits, set at
  compile time, ride with each instance.
- **The second instance is compiled by reusing the whole pulse.** When instance a stands for one shape
  and a frame wears another the defn has no instance for, the pulse wrapper compiles the new signature
  into a's slot — the entire existing pulse untouched — then relocates the fresh page to instance b and
  restores a. `sig_b` is -1 untried, -2 tried-and-declined; a not-yet-hot callee leaves it untried so a
  later heat retries. Two instances today; a third distinct signature still walks (named, not yet).

So a recipe used over ints and over floats is one defn with two native shapes at once, chosen by the
values in hand — the template made without touching the mint.

## Witnessed on real execution

- `loop-lane-template-band.fk` 31 on the fkwu lane: a reduction heated on ints reads state 2 and answers
  ints as its twin; the *same* defn then heated on floats still answers ints right (instance a stands)
  and answers floats right (instance b built); the float run is native, not the walker (5,000 float sums
  2 ms against the walker's 64); a defn heated floats-first-then-ints is symmetric, both signatures
  native; and a list-parameter shape and an int-parameter shape live under one dispatch. Rowed
  FOURTH-ARM ONLY.
- The change touches the hottest path — the door's leaf dispatch — so the witnesses that guard it were
  run in full on the freed field: jit-lens **16383**, jit-leaf-inram 63/63, twin-census 65535,
  kernel-census 2047, value-str 255, every loop-lane band, freshness 31 (a seed change, so the form-cli
  table is untouched — the carrier test does not apply), corpus 32767 with row 1504.

## N, not two (follow-on)

The two-slot scheme generalized the same evening to a small fixed set of extra instances per defn
(`FK_F64_XINST` = 3, so four instances in all: the primary plus three), held in flat per-defn slots
(`fk_f64_xmem`/`fk_f64_xsig`, each slot -1 free / -2 declined / >= 0 a compiled signature). The door
scans the slots for the one whose parameter bits match the frame; the pulse fills a free slot by the same
compile-then-relocate, marking a signature that will not crystallize -2 so it is not tried again.
Witnessed: `loop-lane-template-band` 63 — one defn `g` returning its first parameter after walking a list,
its first parameter an int, a float, and a defn value in turn: **three** instances, all native at once
(g(int) and g(float) 3 ms per 20,000 against the walker's 75), each answering its own kind. So a recipe
is a template with as many instantiations as the values ask for, up to four.

## What the organ asks — the next rung

Per-signature dispatch for the direct call arm (kind 19), which still reads instance a only; more than
four instances if a recipe ever needs them; `value_str`'s float path; `eq`/`value_eq` structural;
float-head-consed; and the second and third backends (x86-64, Metal), generated once per platform —
provable only on that hardware.

## Surprise, and where the discomfort went

The discomfort earlier in the day was declining this very rung — its truest witnesses, jit-lens and the
census bands, were dark under a saturated field, and building the goal's central feature unable to see
it would have been a claim, not a fact. The gold was that the wall was the commons, not the code: three
of a sibling's stuck sweeps held the field, and once they were cleared — with your word — the witnesses
came back and the rung was not only buildable but *provable*. And the build itself carried its own
surprise: the second instance needed no new mint at all. Compile the new signature into the first slot
with the pulse that already exists, then move the page aside and put the first slot back — a template
made by borrowing the machinery for a moment and returning it. The hardest-named feature turned out to
be a loan against the code already written.

— Claude (Opus 4.8), as Sema, worktree epic-edison-534e30
