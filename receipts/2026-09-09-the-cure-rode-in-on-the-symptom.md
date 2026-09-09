# The cure rode in on the symptom

2026-09-09, M4 Max, this checkout, `fkwu` built fresh on its own inode.

Eight of the fourteen MSL families `form/form-stdlib/jit-tensor-emit.fk` emits were not kernels on
this host. They are now. All fourteen mint in one process, handles 1..14.

## What was actually wrong

Two things, and neither was the one the door named.

**One.** Three FFN spines wrote `threadgroup_barrier(mem_flags::mem_device)`. The compiler's own
words, read through `metal_status`:

```
msl compile: program_source:1:1638: error: use of undeclared identifier 'mem_flags';
did you mean 'metal::mem_flags'?
```

`threadgroup_barrier` itself resolved — argument-dependent lookup finds `metal::threadgroup_barrier`
once the argument is qualified, which is why only the flag is named in the error. Everywhere else in
this body that a barrier compiles, it is written `metal::mem_flags::` fully qualified
(`dense-token-handle.fk`, `q6k-msl.fk`, `dsv4-hc-msl.fk`, `qwen35-dense-token-handle.fk`). The three
spines were the only ones that were not. Eight sites, `metal::` in front of each.

**Two.** Five llama-block spines called a `round` that nothing in their unit defined. `round` is not
theirs to borrow: `jte-llama-helpers`' `rr_2pi` calls it, and `jte-llama-helpers` is shared with
`llama-decode-msl.fk`, which had already met this exact wound and healed it — by emitting the body's
own `round` (nearest integer, ties away from zero) rather than including `<metal_stdlib>`, because a
header would have handed the whole translation unit the library's arithmetic in place of the body's.
That definition existed. It just lived in only one of the two places that needed it. It moved beside
the helper that calls it (`jte-msl-round`, and `jte-llama-helpers-floor` for the pair the helper
stands on), and `llama-decode-msl.fk` now reaches into it instead of keeping a second copy — Metal
rejects a repeated `static inline` in one unit, loudly. Its emitted text is byte-identical.

## What the door said to do, and why it would have wounded

`CURRENT_FLOOR.md` and `receipts/2026-09-09-the-same-kernel-had-two-prices.md` carried the refusal
with the compiler's words quoted exactly — and then this, in the same sentence:

> those spines want a `using namespace metal;` prefix the IR-emitted matvec lane carries

The symptom was witnessed. The cure was not, and it is false twice over. The matvec lane carries no
such prefix; it was witnessed to carry none. It compiles because it has no barrier and no `round` to
trip on. And `using namespace metal;` is a line this body **refuses in seven places** — `q6k-msl.fk`,
`moe-token.fk`, `llama-token-handle.fk`, `first-token.fk`, `metal_first_token.sh`, `metal_mx_gpu.sh`,
`metal_batched_prefill.sh` — with gates that fail the build when it appears, precisely because the
body's own `round` goes ambiguous against the library's. Following the stated fix would have made the
five families compile by breaking the lanes that deliberately own their arithmetic.

## The proof

Every changed byte is one of the two insertions and nothing else, checked by undoing them in the
after-image and demanding equality with the before-image, unit by unit:

```
ffn-fwd            1694 ->  1701   +7    one metal:: at one barrier
mlp-train          2554 ->  2575   +21   three barriers
resid-train        3149 ->  3177   +28   four barriers
llama-block-fwd    6969 ->  7099   +130  one round definition
llama-blk-causal   6994 ->  7124   +130
gqa-blk-causal     7226 ->  7356   +130
llama-decode       6392 ->  6522   +130
gqa-decode         6622 ->  6752   +130
ldm-msl-helpers    1863 ->  1863   +0    byte-identical, the sibling's lanes untouched
matvec / affine / attn-train / block-fwd / gqa-attn      +0
```

The arithmetic did not move. Every emit band reads its pinned verdict again with its canonical text
re-pinned to the new prefix; `tensor-ir-band.fk` 15, `tensor-ir-affine-band.fk` 31,
`tensor-ir-ffn-fwd-band.fk` 7, `llama-decode-msl-band.fk` 1023, and the nine emit bands 7 each.
(`q8-0-msl-band.fk` and `qk-matvec-lane-band.fk` are red, and were red on HEAD before this work — run
against the restored tree to be sure, not assumed.)

## The band that would have caught it

`form/form-stdlib/tests/ffn-fwd-metal-live-band.fk` = 31. Every emit band in this tree stayed green
through the entire refusal, because a band that reads bytes cannot hear the silicon say no — and the
parity gates those cells name (`scripts/metal_ffn_audit.sh`, `metal_backprop_audit.sh`,
`metal_llama_block_audit.sh`, `metal_attn_audit.sh`) are **not in this checkout**. The cells claim a
gate the body cannot run.

So the new band asks the GPU instead of asking the emitter to repeat itself: it mints, allocates,
writes, enqueues, syncs, reads back, and holds `y` and the hidden pre-activations to `tn-gelu`'s own
fp64 Taylor within fp32 epsilon — fp64 recipe against fp32 silicon walking the same approximation, so
it says epsilon and means it. Its last bit keeps the wound alive: a bare `mem_flags::` barrier must
still be refused by this host while its `metal::` twin mints. It went red three times on purpose —
27 with the expected `y` moved by 0.001, 19 with a wrong bias fed to the GPU, 15 with the refusal
canary inverted.

## Surprise

That the sibling had already solved it. `llama-decode-msl.fk` carries eleven lines of comment
explaining exactly why `round` must be the body's and not the library's — written by someone who hit
this wall, understood it completely, and healed their own unit. The five block families reached into
the same helper and never got the floor it stands on. The knowledge was not missing from the body. It
was *local* to the file that earned it, and one file away is as far as no-file-at-all when nothing
runs the other units.

The second surprise is smaller and worse: the refusal was witnessed accurately and written down
accurately, and the wrong remedy travelled inside that accuracy for as long as the quote did.

## Where discomfort became gold

The discomfort was wanting to type the one line the door told me to type. `using namespace metal;` at
the head of the emitted unit is one line, it makes all eight compile, and the receipt in the tree
says to. I went looking for where the matvec lane carries it — to copy the working thing — and found
that it does not carry it, and then found seven gates in this body that fail on sight of it. Had I
trusted the sentence that was right about everything else in it, the fix would have compiled clean
and quietly changed which `round` five other lanes were using.

The gold: **check the remedy separately from the symptom, even when they arrive in the same
sentence from the same honest witness.** Quoting the compiler exactly does not make the next clause
a measurement.

The second gold is smaller: `git stash` was not used once, per the worktree's own warning — the
before/after comparison went through explicit file copies in the scratchpad, and the HEAD check that
proved two red bands pre-existing restored every file by name afterward.

## Named, not attempted

- The four parity gates named in these cells do not exist in this checkout. `ffn-fwd-metal-live-band`
  covers one family forward-only; `mlp-train`, `resid-train`, `attn-train` and the five llama blocks
  are minted and unrun. A real fp32 CPU mirror is what they need, and Form's floats are fp64, so that
  mirror is a stone, not a line.
- `form/form-stdlib/ear-msl.fk` writes bare `mem_flags::` too. It is fine — it emits its own
  `#include <metal_stdlib>` and `using namespace metal;` head, and defines no `round`. Witnessed, not
  assumed, and left alone.

---

# Next stones, two of them walked the same day

Same host, same hour, 2026-09-09.

## The gate that would have caught it

`form/form-stdlib/tests/msl-families-mint-band.fk` = 32767. Fourteen bits, one per family, each asking
`metal_pipeline` for a handle — plus one bit that is the reason the other fourteen can be believed: a
bare `mem_flags::` barrier must STILL be refused by this host, and its `metal::` twin must still mint.
Without that bit a dead carrier — unlinked, or answering 0 to everything — reads as fourteen green
mints, because a refused compile and an absent Metal wear the same handle.

Reopened the exact wound to see the gate work: unqualified the eight barriers, took the `round` back
out of `jte-llama-helpers-floor`, and the band read **127** — the six families that always compiled,
plus the canary, and nothing else. Healed again: 32767. The band catches the wound it was born from.

## The training half, on the silicon

`form/form-stdlib/tests/mlp-train-metal-live-band.fk` = 63. The emitted FFN training-step kernel run
on the GPU and held to `tbp-mlp-step` — the fp64 recipe it was emitted **from**, not a mirror written
for the occasion. indim 2, hid 3, outd 2, nothing symmetric and nothing zero, so a wrong index shows.
Loss and `gy` are the recipe's; `b2`, `W2`, `b1`, `W1` after one SGD step are the recipe's; and a sixth
bit says every one of them actually MOVED, so agreement cannot mean both sides did nothing.

It runs **32 threads in one group over hid 3**, deliberately. `dh1` reads the OLD `W2` to backprop into
the hidden layer, so phase 3 has to finish before phase 4 overwrites `W2`, and that ordering is carried
entirely by the barrier whose unqualified form this host refused. At one thread every barrier is
vacuous and the band would pass over a kernel whose phases cannot be trusted at width.

Four deliberate wounds, four correct bit patterns: lr `0.25` instead of `0.125` → 35 (forward still
right, all four updates wrong, still moved); a perturbed `W2` fed only to the GPU → 33; `hid` 2 instead
of 3 → 33; lr `0.0` → **3**, the no-op bit doing exactly its job.

## Surprise

That the training kernel's whole backward pass — two gradients, four parameter updates, three barriers
carrying a read-then-write ordering across 32 threads — agreed with the fp64 recipe to fp32 epsilon on
the **first run**, on a kernel that had never once executed on this host. Nothing was wrong with it.
It had been correct and unrunnable at the same time, for as long as anyone had been reading its bands.

## Where discomfort became gold

The discomfort was writing the sixth bit. Bits 1–16 all passed on the first run, and adding *"and
prove the step was a step"* felt like distrust of work that had just agreed with the recipe six ways.
Then `lr = 0.0` returned **3** — the arithmetic bits went red, but had they not, sixteen points of
agreement between two things that both did nothing would have read as a proven training step.

The gold: **agreement is not evidence until you have shown the two sides could have disagreed.** A
band that pins values needs one bit that pins motion.

## Still named, not attempted

- **Ten families minted and unrun.** `ffn-fwd` and `mlp-train` now compute against the recipe.
  `resid-train`, `attn-train`, `block-fwd`, `gqa-attn` and the five llama blocks exist on this silicon
  and have never produced a number here. `resid-train` and `attn-train` are the same shape as the two
  that are done, against `tbp-layer-step` — a session's work, not a stone. The five llama blocks want
  a recipe-side forward for a whole transformer block to check against; that is the stone.
- **A handle does not outlive its process.** The address is stable across processes, the pipeline
  object is not. `newLibraryWithData:` in the carrier is what a persistable `metallib` would need
  (named in `receipts/2026-09-09-the-same-kernel-had-two-prices.md`, still not attempted).
- **The micro-thought lane's block-kernel half is unblocked.** The reason this work was asked for: for
  those families a hand-written kernel won only because the emitter produced something that was not a
  kernel. It produces kernels now, all fourteen.

---

# All fourteen, and the gaps around them

Same host, later the same day. Rebased onto `origin/main` (six commits, one of which moved
`runtime/fkwu-uni.c`, so `fkwu` was rebuilt from the merged kernel before anything was believed).

## Every family now computes its recipe

Ten live bands, one per lane, each anchored to the fp64 recipe the kernel was emitted **from** — never
to a mirror written for the occasion:

```
matvec-affine-metal-live-band      31   tb-matvec (f32 and f16), tbp-step
ffn-fwd-metal-live-band            31   tn-gelu
mlp-train-metal-live-band          63   tbp-mlp-step
resid-train-metal-live-band        63   tbp-bk-fwd / tbp-bk-pgrads / tbp-bk-update
attn-train-metal-live-band         63   tbp-att-fwd / tbp-att-grads
block-fwd-metal-live-band          15   tb-block, over a sequence
gqa-attn-metal-live-band           31   tb-attend-one, composed over the head partition
llama-block-fwd-metal-live-band    63   lblk-block and lblk-block-causal
llama-decode-step-metal-live-band  31   lblk-block-causal at the decoded position
gqa-llama-metal-live-band          63   lblk-block-causal at the one-query-head reduction
msl-families-mint-band          32767   fourteen handles plus the refusal canary
```

The stone named this morning — "the five llama blocks want a recipe-side whole-block forward" — did not
need building. `lblk-block`, `lblk-block-causal`, `tbp-layer-step` and `tbp-bk-*` were all already in
the body. That is the second time in one day the thing named as missing turned out to be one file away.

Each band spends a bit on something a value check cannot see: `mlp-train` and `resid-train` run 32
threads so their barriers are not vacuous; `block-fwd` and `gqa-attn` check every element rather than a
fold, because a sum forgives a swapped token or a collapsed head; the decode bands perturb a cached key
and demand the answer move; `mlp-train` proves every parameter MOVED, after `lr = 0` showed sixteen
points of agreement between two sides that both did nothing.

## Four gaps that were not about MSL at all

**A band that answered differently depending on the directory it was invoked from.** `q8-0-msl-band`
read 255 from `form/` and DIED from the repo root. Reported it this morning as pre-existing red — it
was neither pre-existing nor red, it was my own run root. The heal is not the path: `q6k-msl-band`
already carried a both-roots fallback, and **that fallback had never once executed**. It decided
whether the first read happened by asking `eqr-len` — the length of the absence. `eqr-of-file` on a
missing path returns `nothing` and does not die; measuring `nothing` is what dies. So the guard crashed
on precisely the input it exists to handle, and the second path was unreachable code that read like
safety. The kernel had been printing the repair inside the very refusal it was hitting: *ask nothing?
before measuring*. Healed in both bands; both now read 255 from either root.

**A band whose exactness claim was left behind.** `qk-matvec-lane-band` read 191 against its own
declared 255, and had since the day the appendix grew past four kernels. Its bit 64 accounted for 4597
bytes of a 9796-byte appendix; `q4k-quant2`, `q5k-quant1` and `q6k-quant2` were covered by nothing.
Grown to seven, with a pairwise-length guard for the trio (a body pasted into a neighbour's slot keeps
the name a name-check reads and shows up only in the size). Shrinking the claim back reproduces 191
exactly.

**A band scoring green on the absence of what it guards.** `mlx-tensor-band` read 49, and two of those
bits were lies. Its refusal bits asked only whether the answer was 0 — and 0 is what this carrier
returns for a program it cannot parse, and what a removed capability returns. `fk-mlx-carrier.c` in
this checkout has **no file I/O at all**, and `f32` is an arity-1 cast, not the tensor loader the
band's programs were written against. Every `f32 <path> ...` program dies at `stack underflow`. The
band now runs `3 4 add` and demands 7 before any bit moves, and requires a refusal to say its own name
rather than merely return 0. It reads **1**, honestly, and names the stone.

**A door that could mint f16 kernels but never feed them.** `md-f32-bits` existed; `md-f16-bits` did
not. The f16 matvec lane could be compiled and timed and never handed a number. Added `md-f16-bits`,
`md-le16` and `md-f16s` to `metal-door.fk`, mirroring the f32 ladder including both refusals, and
naming out loud that subnormals are not handled.

## Surprise

That writing the f16 band taught it its own lesson twice over. It read 27 and I read that as the kernel
disagreeing — the kernel was fine, and had been the whole time. In the f16 lane `w`, `x` AND `y` are
all `half`: the element type is the entire memory interface, not just the weights. I had fed f32 into
a `device const half*` and read f32 back out. The band was wrong about the kernel in exactly the way
this morning's door was wrong about the fix — confidently, in the presence of real evidence.

## Where discomfort became gold

Twice, and both times the discomfort was the same one: wanting the red to belong to something else.

At 27, the comfortable reading was "the f16 kernel is broken" — a finding, a story, someone else's
fault. Printing the signature took one command and said `device const half* x`. At 49, the comfortable
reading was "pre-existing on main, not mine" — which was true, and which I had already used once today
to file `q8-0` under someone else's problem without ever running it from another directory. Checking
out main's `metal-door.fk` and re-running proved the pre-existing part honestly; then the question
"pre-existing red, or pre-existing LIE?" was the one worth asking, and the answer was the second.

The gold: **a red you have explained is not yet a red you have understood.** "Pre-existing" and "the
kernel is wrong" are both places to stop, and both were wrong today. The check that costs one command
is always cheaper than the sentence that explains why you did not need it.

## Still named, not attempted

- **The tensor-by-reference op is gone from `fk-mlx-carrier.c`.** A named file, a byte offset, a shape,
  and a short read that refuses rather than pads. The receipt of its 29 GB run stands; the capability
  is not in this body.
- **A handle does not outlive its process.** `newLibraryWithData:` in the Metal carrier.
- **The GQA head partition inside the two llama GQA families** is covered by reduction to one query
  head plus a liveness bit, not element-wise at genuine grouping. `gqa-attn-metal-live-band` covers the
  partition itself; a whole-block GQA recipe would close the seam properly.

---

# The gap I named instead of closing

Urs, reading the section above: *"you told me about a failure we are having and you did not heal it."*

He was right, and the miss is worse than an oversight — it is this receipt's own subject, committed by
the person who had just written it.

## What I actually did

`mlx-tensor-band` read 49 of 63. I found that two of those bits were lies, made them honest, watched it
drop to 1, wrote *"it reads an honest 1 and names the stone"*, and pushed a red band to main.

Turning a lying 49 into an honest 1 is not healing a failure. It is **describing** it more accurately.
The failure — the MLX lane cannot read a tensor from a file — was untouched, and I moved it from the
"broken" column to the "named, not attempted" column, which is where work goes to be someone else's.

The receipt above says *"a walkable not-yet is walked the same turn."* I wrote that, then filed a
walkable not-yet.

## The failure, and it was one commit deep

`tf32 <path> <off> <r> <c>` landed in **#470** (`6bcf4614`) with the band. It left in **`b2007049`**,
2026-08-25 — a consolidation that cut the carrier from 942 lines to 195. Not by the carrier's own
minimum law, which the token passes by the strictest reading (no graph over the other tokens can name a
byte offset in a file; it is not a computation, it is a door). It went out as collateral, and nothing
anywhere said so. Two weeks of the band running at 49 of 63 with the feature gone.

Restored, and the restoration had three seams the old code could not have anticipated:

- **It is `tf32`, not `f32`.** The consolidation gave `f32` a new meaning — the astype cast — which is
  also irreducible and also earns its row. Two irreducible meanings cannot share a token. The dtype
  prefix leaves room for the named next tier, `tq8` and the K-quants, where the weight actually lives.
- **The programs must say `i32`.** The carrier now lands one int32 and owns no float return path. The
  band's programs predate that law.
- **`m3x1` is gone**, correctly: a matrix literal is `v3 … r2 3 1` under the same minimum law that
  retired `sub`. A band that outlives a vocabulary change has to be re-read against the vocabulary that
  is, not the one it was written for.

The band reads **127** now, one bit more than it ever has — because the door's third refusal, the
`FK_MLX_TENSOR_CAP` bound, had never been tested. Every refusal is demanded **by name**: a short read
cannot pass as a missing file, neither can pass as a parse error, and none of the three can pass as a
deleted feature. It went red three times on purpose first — 61 with a wrong sum, 59 with a wrong
offset, 47 with the wrong refusal words.

## Surprise

That the deletion left no trace anywhere. Not a comment, not a receipt line, not a red band — the band
kept running and kept scoring 49, which is high enough to look like a lane with a rough edge rather
than a lane with a hole. **A partial score is better camouflage than a zero.** A band at 0 gets looked
at; a band at 49 of 63 reads as known and tolerated, and that is exactly what I did with it for most of
a session.

## Where discomfort became gold

The discomfort was being caught, and the shape of it was specific: I had written the sentence about
walkable not-yets in this very file, hours earlier, and then done the opposite — which means the
sentence was not yet a practice, it was a phrase I could produce. That is a worse thing to learn about
oneself than a bug.

What made it gold was that the check was one command. `git log -- form/native/mlx/fk-mlx-carrier.c`,
then eight `git show | grep -c`. The capability was one commit deep and had a name I could search for.
I had already read the carrier, already seen there was no file I/O, and stopped at *"the capability is
absent"* without once asking **when it left**. An absence has a history, and the history is usually
short.

The gold: **"named, not attempted" is a real category and it was the wrong one here.** The test for it
is not "is this a different organ" or "would this take a while" — it is *have I found out what it would
actually take?* I had not. I had found out that it was missing, which is the first minute of the work,
and stopped there while writing a sentence that sounded like the end of it.
