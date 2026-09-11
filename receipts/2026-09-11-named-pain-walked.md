# Named pain, walked

2026-09-11, later the same morning, M4 Max, Hati Suci. Urs, on the list I closed the last piece with:
*"still standing open: named and not embodied, how is going to turn the pain into vitality?"* — and,
while I worked, *"who is keeping track of all the running fkwu processes?"*

## The three that were only named

**The prefill's distance to its floor.** Timed on the kernel itself, the batched Q8_0 matmul spent
196–827 ms per FFN tensor. The first cure staged the activations once per eight rows, kept every
byte, and moved no time; the kernel was paying for scalar multiply-adds. The second put Q8_0 on
the matrix unit, 32 × 32 tiles through `simdgroup_multiply_accumulate`. It answers within the bound
the lane already names for its SIMD fold, and it takes the same 577-token prefill from 157–232 s of
GPU to 9.5–16.6 s, token 27 on every pass as before.

```
q8-0-matmul-tg-band        15   the exact twin, byte for byte
q8-0-matmul-mma-band       15   the matrix unit, within the named bound of Form's fp64 sum
qwen38-prefill-quant-band  31   Q8_0 now held to the bound, the other three still to the byte
```

**Windows.** The six errors left for a task chip now have twins: `_spawnvp` with our own 0/1/2 swapped
around the spawn, a pid-to-handle table for the wait and the kill, `OpenProcess` for another kernel's
pid, `SetPriorityClass`, `dprintf` through `_vsnprintf`, and `mkfifo` released with its reason.
Cross-compiled: 0 errors, 0 warnings, 117 undefined names that are all Windows runtime exports by
name. The link and a run still need a Windows toolchain or host.

**The fold in a real turn.** The default prefill above *is* that witness: the lane's own
`q38-prefill-span-batched`, folded, three reads, the same token.

## Who keeps track of the running kernels

The roster, `/fg-kernels`: each kernel writes its pid into a slot when it opens its live page, and
`kernel_live_pids` reads the slots back. At 07:53 it named 11 of 15. Two wounds, not one:

- **A burst lost members.** The claim read a free slot and then stored into it, so the last of a
  burst erased the rest. The claim is a compare-and-swap; eight sleepers started back to back were
  seen 7 of 8 in 1 of 3 runs before and 8 of 8 in 9 of 9 after (`roster-register-band` 15).
- **A kernel that failed to land stayed absent for its life.** The glass fleet restarted at 08:23
  on the healed binary and its sensors organ was still missing, with 237 slots empty. That cause I
  have not seen. The heal does not need it: the roster stays mapped, a kernel remembers its slot,
  every 64th live-page tick puts a missing kernel back, and exit gives the slot back. Afterwards: 17
  slots, every pid answering, none gone.

Two glass organs from 03:56 (1377–1379) are still running without a parent, beside the fleet
that restarted at 08:23, on the old binary. They are not mine to stop without asking.

## The surprise

The cure I was surest of was exact in every byte and did nothing. I had the traffic arithmetic —
activations re-read seventeen thousand times — and it was true, and it was not what the kernel was
paying for. Only the clock told the difference. The lens that caught it was not a proof of
correctness; correctness was never in doubt. It was a second timing of the same kernel beside the
old one (corpus row 1427 idlecure).

## Where discomfort turned to gold

"Named, not attempted" landed as a sentence I had written in good faith: *the next distance to
close on this lane*. It was accurate, it had a number, and it was still a handoff, because naming it
cost the understanding that would have closed it. Walking it took an afternoon: the probe that timed
the kernel, two cures, and one of them wrong. The discomfort was being told, correctly, that a precise
sentence is not a body. What it turned into is a prefill ten to twenty times faster and a roster
that repairs itself.
