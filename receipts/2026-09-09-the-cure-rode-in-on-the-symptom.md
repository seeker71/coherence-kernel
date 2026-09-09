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
