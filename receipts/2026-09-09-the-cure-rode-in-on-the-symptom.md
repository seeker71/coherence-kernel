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

---

# The category itself was the leak

Urs again, on the previous section: *"how is that healthy to name and not attempt? that is spending
nutrient in naming and then expecting someone else to spend more nutrients to figure out the same and
heal, that is double consumption."*

The section above conceded that "named, not attempted" was the **wrong category for that item**. That
concession was still too small. The category is mostly a leak, and the economics say why.

**Naming a gap well requires understanding it, and the understanding is most of the cost of healing
it.** By the time a name is accurate enough to be worth writing, the expensive work is done — it is
sitting in a live mind holding the whole shape at once. Stopping there discards exactly that, and
leaves behind prose, which the next mind must re-expand at full price: read the code, form the model,
rediscover the constraint. Two payments, one heal. Lossy compression of the one thing worth keeping.

It is worse than neutral, because the artifact left behind *looks like progress*. A named stone reads
as a decision when it is usually an unfinished first minute.

## The three that were standing, closed

**The GQA head partition.** I wrote that it "is covered by reduction to one query head plus a liveness
bit, not element-wise at genuine grouping — a whole-block GQA recipe would close the seam properly."
Writing that sentence required knowing exactly what the recipe had to be. `lblk-gqa-block-causal` is
now in `llama-block.fk`: it composes `tb-attn-seq-causal` verbatim over each head's q slice and its
group's k/v slice, and it is checked against the proven single-head block at NQ = 1 before being
trusted at NQ = 2. Both GQA kernels are now held to it element by element at genuine grouping — the
band reads **127**, up from 63, and setting `kvh = h` instead of `h / grp` drops it to 31.

Writing it also caught a false green I had introduced myself: the liveness bit truncated `wo` to
`d x KVD`, when `wo` is `d x dq` and the attention output stays dq-wide. The answer "moved" because the
kernel was reading a buffer tail the band never wrote. A liveness bit is the weakest thing a band can
say and the easiest to fool.

**The tensor-by-reference op** — closed in the section above, `tf32` restored, band 127.

**The persistable `metallib`.** This one closed differently, and the difference matters: it was
**retired by measurement**, not built. The remedy had been written down twice as "the carrier would
need to learn `newLibraryWithData:`" and had never been run once — the `quotedcure` shape again, in a
line I had repeated without testing. Re-derived on this host, M4 Max, 2026-09-09, five never-seen
kernel texts against five repeats of one, separate processes, `fkwu` startup as baseline:

```
cold   0.09  0.08  0.08  0.08  0.08     (~80 ms)
warm   0.03  0.04  0.03  0.03  0.03     (~30 ms)
base   0.00  0.00  0.00                 (fkwu startup, no mint)
```

macOS already caches the source→library compile across processes and returns ~50 of those 80 ms for
free. What stays warm is device creation and pipeline-state creation, which no archive removes. The
stone would buy a fraction of 30 ms on a path the OS has already made cheap. **Decided against, with
numbers, is a closed item; "named, not attempted" was never going to become one on its own.**

## Surprise

That retiring a stone can be the healing, and costs about the same as a small feature. Ten minutes of
measurement turned an item that had been carried in two documents into a decision — and the decision
was *don't build it*, which is a real result and was completely unavailable while the item sat in a
list. The list was not preserving the work. It was preserving the ambiguity.

## Where discomfort became gold

The discomfort was that I had already been corrected on this once in the same session and had produced
a smaller, safer version of the right answer: I agreed the category was misapplied *here* while
defending it as "a real category". That is the shape of conceding the instance to keep the pattern.

What made it gold was noticing the pattern had a cost I could count. Not a feeling about diligence —
an accounting: **understanding is the expensive input, and a name is a lossy export of it.** Whoever
picks the item up pays the full price again, and the export is convincing enough that they may not
even know they are re-deriving. The only honest exports are a working change, a measurement that
closes the question, or a specific pointer that removes the re-derivation (a commit hash, a line, a
number). Prose describing a gap is none of those.

The test that survives: before writing a not-yet, ask what the next mind would have to rediscover to
act on it. If the answer is "most of what I just learned", the item is not ready to be written down —
it is ready to be walked.

## What walking the three actually found

Closing them turned up four things that naming them never would have, and all four were in code, not
in the items themselves:

**A recipe I wrote that already existed.** `lblk-gqa-block-causal` went into `llama-block.fk`, tested,
red-tested, and was then deleted — `llama-gqa-block.fk` had carried `lgqa-block-causal` the whole time,
with RoPE scaling mine did not even have. My grep was `gqa` across two files; the body spells it
`lgqa-`, in a third. Third time in one day the thing named as missing was one file away, and this time
I built it before looking properly. The reflex that would have caught it costs one repo-wide grep and
belongs *before* the first line, not after the last.

**A false green I had introduced myself, four hours earlier.** The liveness bit I wrote for grouping
truncated `wo` to `d x KVD`. `wo` is `d x dq` — the attention output stays dq-wide, head-major — so the
kernel read a buffer tail the band never wrote, and "the answer moved" for a reason unrelated to
grouping. It passed. A liveness bit is the weakest thing a band can say and the easiest to fool, and I
had reached for one precisely because the recipe I thought was missing wasn't there to check against.

**Two bands wearing a third band's header.** `observe-gqa-grouping-band` and `observe-llama-parts-band`
are byte-identical for 56 lines, both cloned from `llama-gqa-block-band` (31), both declaring
"Verdict 31" over five DECIMAL digits that sum to 11111, both describing claims neither one makes. A
copied header is the cheapest thing in a file to leave stale, because nothing runs it.

**Four claims wearing five digits.** `observe-gqa-grouping-band`'s fifth digit repeated its first
verbatim — `(eq s-shared 3871)` twice — so one digit could not fail on its own. It now asks whether the
grouping reaches token 1, where shared and per-head land on opposite sides of zero (258 vs -285): a
sequence-wide effect checked at one position is a per-position claim wearing a sequence's name. A wrong
pin drops it to 1111.

None of that was visible from outside. It became visible because the work was done rather than
described — which is the point, and is the second time today the same sentence has had to be learned
at a different depth.

---

# Embodied end to end

Urs: *"this does not sound fully embodied end-to-end."* He was pointing at the shape of the previous
section, and the sharpest instance was an hour old: I had measured the metallib question with
`/usr/bin/time` and shell loops — a claim about the body made from **outside** it. I looked for a clock
door for about ten seconds, said "no obvious clock door", and reached for the shell.

`host_monotonic_ms` and `host_cpu_us` are doors. They have been doors the whole time. I grepped the C
for name strings, missed them, and never grepped the cells that already print microseconds.

## The body reads its own price

`observe/mint-price-run.fk`, warming the Metal device on its own line so device creation is not charged
to the first kernel:

```
COLD  never-seen text        42 ms wall   1263 us cpu
WARM  same text, same proc    0 ms          13 us cpu
WARM  same text, third time   0 ms           3 us cpu
COLD  a second never-seen      8 ms         545 us cpu
WARM  that second again        0 ms          10 us cpu
```

The shell had said "cold ~80 ms, warm ~30 ms" and was mostly timing `fkwu` startup and device
creation, which a process-level timer **cannot separate from minting at all**. The body's own clock
puts the confound on its own line, and the real warm mint is **13 microseconds** — three orders of
magnitude from what I reported. The retirement conclusion survives; the number I published to support
it was measuring something else.

## The lane's wholeness is now the body's claim

`form/form-stdlib/tests/msl-lane-coverage-band.fk` = 31 walks the tests directory with `fs-list` and
`fs-read-text`, keeps only bands that call `metal_enqueue`, and demands all thirteen `-msl` entry
points appear in one — with bit 16 pinning the roster to the emitter's own count so a fourteenth family
cannot arrive uncovered. Until it existed, "every family is run" was true and lived in a receipt.

Writing it found two things about my own hands. `substring` is `(s, start, END)`, not `(s, start, len)`
— every prefix check in the tree uses `0 (str_len p)` where both readings agree, so a suffix check
written from assumption silently matched nothing. The body's own `fki-ends-with?` already had it right;
I wrote mine without reading it. And its bit 8 was born weak: the first witness was an emit-only band,
which touches no Metal at all and therefore cannot tell `metal_enqueue` from `metal_pipeline`.
Loosening the dispatch test left the verdict at 31 until the witness became `msl-families-mint-band`,
which mints fourteen handles and enqueues nothing.

## Following the thread all the way down

Three MLX bands were red — 137/255, 53/63, 49/63 — pre-existing, verified against the pristine carrier
both ways. The same 2026-08-25 consolidation that took `tf32` took **five** file-reading tokens:
`f32`, `q8`, `q4k`, `q6k`, `attn`. I had restored one and left its siblings, which is the same partial
move I had just been corrected for.

`q8`, `q4k` and `q6k` are back, ported through the current `fk_mlx_push` rather than the old raw stack
writes. The proof is the pins, not the port: **528** and **1056** out of the Q8_0 fixture, exactly the
values `mlx-q8-band` had been asserting into a void for two weeks.

Then the bands needed the vocabulary that IS. Their programs predate the one-int32 law, so each closes
with `i32` now; `m32x1` is gone under the same minimum law that retired `sub`, so the 32-wide ones
column is **built** — `32 iota 0 mul 1 add` — which is that law working exactly as intended.

`mlx-home-band` was different again: its `rope`, `attn` and `softmax` were retired as *composable*, and
the carrier header listed `rope-pair` among the graphs living in `mlx-derived.fk`. It was not there.
Neither was `attn`. Retired in favour of a Form graph nobody wrote, for two weeks, with the header
asserting otherwise. Both are written now — `mld-attn` is matmul, the Form softmax, matmul; **9**, the
fused kernel's own answer in four dispatches instead of one, and that trade is named in the cell rather
than hidden behind "derivable".

`mld-rope-pair` took two attempts and the first one is the reason this section exists. I wrote it from
reasoning — build the rotation matrix and multiply — and it died on a reshape, because there is no
concat here and `[[cos,-sin],[sin,cos]]` has no expression. `take` is what makes it composable:
take(x,[1,0]) is (x1,x0), times (-1,1) is the sin term exactly. **I nearly shipped the first one.**

All five MLX bands are green: 16777215, 255, 63, 63, 127.

## Surprise

That `mlx-home-band`'s rope claim had been pinned at **position 0**, where RoPE is the identity — so
the check would have passed over a rope that did nothing at all. It is now pinned at 1 radian, where
7·cos 1 − sin 1 = 2.9408. A fixture chosen because it is easy to compute by hand is often chosen
because the operation vanishes there, and those are the same property seen from two sides.

## Where discomfort became gold

The discomfort was how ordinary the shell felt. Reaching for `/usr/bin/time` did not feel like leaving
the body — it felt like using a tool. That is what made it worth noticing: embodiment fails at the
places where the outside tool is *convenient*, not at the places where it is obviously wrong. I did not
decide to measure from outside; I failed to spend ten seconds asking whether the inside could.

The gold, and it is the same shape as `quotedcure` and `namedaway` one turn down: **a claim measured
from outside the body is a claim the body cannot hold** — it can only be told, and it will report the
number long after the number stops being true. The test is not "is this measurement correct" but "who
would notice if it stopped being correct". For the shell timing, the answer was nobody. For
`observe/mint-price-run.fk` and `msl-lane-coverage-band`, the answer is the body.

## Sweeping by declaration instead of by eye

The last thing embodiment changed was how the sweep itself works. All day I had compared band verdicts
against a list of "clean-looking" numbers I typed into a grep filter — which is a person deciding what
green means, one shell pipeline at a time. Comparing each band to the verdict it DECLARES in its own
header instead turned up six mismatches in one pass, and two of them were mine:

`observe-gqa-grouping-band` and `observe-llama-parts-band` read 11111 against a declared "Verdict 31"
— and the 31 was **my own prose**, a sentence I had written that morning describing the stale header I
was healing. A reader looking for the declaration finds the first `Verdict N` in the file, and I had
put an old one above the new one. Reworded; the only `Verdict N` in each file is now the true one. A
document that quotes a number it is correcting has planted the number it removed.

`mla-msl-band` declared 63 over what are now **seven** claims summing to 127 — a c64 was added and the
declaration was left behind. That is the quiet direction of this drift: a band reading MORE than it
declares fails nothing, and the extra proof simply goes uncounted by anyone who reads the header
instead of running it.

Three others — `form-knowledge-query-memory-shard-exec-band` (65535 against 262143),
`public-source-concept-index-band` (14149910 against 33554431) and `public-source-concept-shards-band`
(dies on str_len-of-nothing) — are in the knowledge/concept-index subsystem, prelude nothing this
session touched, and I have not diagnosed them. I am naming them as a boundary and saying plainly that
it is a boundary: three band names, verified independent of this work, is a pointer that removes the
rediscovery of "which are red and are they yours" — and it is not a diagnosis, which is the thing
`twicepaid` says not to pretend I am handing over.

---

# Heal the lanes; find what is absent, unavailable and stale

Urs, three asks in one turn: heal all MLX and Metal lanes, find more absent/unavailable/stale flows and
close them, and — arriving mid-work — **prefer native Metal over MLX wherever it can do the job.**

## The fatal path

Sweeping 74 lane bands against their own declared verdicts found three mismatches, and chasing the
worst one found something much larger than a band.

`mlx-matmul-band`'s bit 32 claimed "a shape mismatch is refused, giving 0". It has never tested that.
Running the mismatch **kills the process**: MLX's default error handler aborts, rc 255, and in a
pipeline the 255 launders to 0 — the body could die and read as success. The old carrier armed
`mlx_set_error_handler` and carried a paragraph explaining exactly this, ending *"a refusal has to be
survivable or it is not a refusal."* The 2026-08-25 consolidation took the handler out with everything
else. **From that day until today, any cell handing MLX a bad shape killed fkwu outright.**

Nothing found it in two weeks, and the reason is the sharpest thing here: the bands that would have hit
a shape error were failing one token EARLIER, on `mRxC` — a literal the same commit retired. Two wounds
in a row, and the first hid the second. A fatal path stays unnoticed not because nobody looks but
because nobody can REACH it.

Restored, and the generic `"mlx op failed"` no longer papers over MLX's own words: a bad matmul now
returns 0, keeps the process, and says `[matmul] Last dimension of first input with shape (1,3) must
match second to last dimension of second input with shape (2,2)`.

## Nine ops claimed and never written

`fk-mlx-carrier.c`'s header has listed `gelu layernorm scale axpy shift select clamp rope-pair attn`
as Form-emitted graphs since 2026-08-25. **Not one was in `mlx-derived.fk`.** Nine names, two weeks.

An overclaim in a header is not a lie that fails — it is a lie that CLOSES THE QUESTION. A reader
checking whether the carrier is minimal reads the list, sees the op accounted for, and stops looking.
A gap nobody is looking for outlives one that is red.

Seven written and pinned (`mlx-derived-band` 16777215 → **1073741823**, thirty ops): scale 21.000,
axpy 19.000, clamp at both ends (3 and 0, via `-max(-a,-b)` since max is the only comparison row),
gelu(1) = 0.841 with every constant an integer ratio because the carrier's only literal is int32, and
layernorm as centre-then-rmsnorm. `shift` and `select` are deliberately NOT written and are gone from
the header instead: select IS `where`, a shift by k IS scale by 2^k, and rows that compute nothing new
break the same law from the Form side.

## The three red bands, and what they were made of

`mlx-matmul-band` 57→63, `mlx-softmax-band` 1→63, `form-cli-gpu-band` 1009→1023 (the last via its BML
source, the authoring altitude, not the lowering). All three ran programs in a vocabulary the carrier
retired: `mRxC` literals and a `softmax` row. Rewritten in the vocabulary that IS — `vN … rN`, the Form
softmax, `i32` to land — and their refusal bits now name the refusal instead of accepting any 0, which
they could not do before because the refusal killed the process.

## Sixty-four paths that are not there

A grep of the tree's cells found **80 script paths named and 64 absent**, including all twelve
`scripts/metal_*_audit.sh` parity gates the tensor emitters cite as proof of their arithmetic on
silicon. Cells have been citing gates that are not in this checkout, in prose, for as long as the prose
has existed. Nothing fails, because a citation is not executed — it is read.

The twelve are repointed at the live bands that now do that work and DO exist. The CUDA pair is marked
absent in place, because there neither the script nor the GPU is here and repointing it at a Metal band
would be a worse lie than the one it replaces.

And the class is now the body's to watch: `lane-named-paths-band.fk` = 15 walks the lane's cells with
`fs-read-text`, extracts every `scripts/…` path, and demands each exist — with bit 8 as a live control
(the CUDA citation, absent by design) so a scanner that quietly matched nothing could not read green.
Adding one absent path to the emitter drops it to 13.

**The native Metal lane names 16 paths and all 16 exist.** The preferred lane is already honest about
what it cites.

## The preference, recorded where it will be read

Native Metal over MLX for new work, written into `fk-mlx-carrier.c`'s first paragraph and
`mlx-derived.fk`'s — the two places someone would extend that lane. Not a verdict on the repairs: an
honest organ beats a quietly broken one in either lane. But this carrier borrows a library's kernels
while `form/native/metal/` writes the body's own, and the quant doors restored today dequantize what
`q6k-msl.fk` and `q8-0-msl.fk` already dequantize in kernels the body emits and can read back. Two
paths to one meaning is the parallel path the minimum law exists to prevent, one level up from the op
table.

## Surprise

I wrote the absence back into existence. Healing `mlp-fwd-emit-band`'s stale citation, I explained it
by naming the missing path — and the scanner I had just built found it again, because a comment that
explains an absence by writing the absent path out **re-creates exactly what the scanner looks for**.
This is the second time in two turns: last turn I left a "Verdict 31" in prose describing a header I
was correcting, and the declaration-reader picked it up. A document that quotes what it removes has not
removed it. The fix is to describe rather than quote, and it is now written into that band.

## Where discomfort became gold

The discomfort was being handed "prefer native Metal" in the middle of an hour of restoring MLX doors
and writing MLX recipes. The cheap readings were both available: treat it as a correction and tear the
work out, or treat it as future-only and keep going unchanged.

Neither is right, and the distinction is worth having. **A broken thing and an unpreferred thing are
different problems.** The MLX bands were red AND lying — bits scoring green on the absence of what they
guard, a refusal path that killed the process. Leaving that because the lane is not preferred would
have left the body a dishonest organ AND a preference. Fixing it does not grow the lane; the preference
governs what comes NEXT, and the honest place to put it is the header of the file someone would open to
extend it.

The gold: **direction applies to the next stone, not to the wound already open.** Healing something you
are moving away from is not wasted work — an organ you are stepping back from should still tell the
truth about itself, because it stays in the body either way.
