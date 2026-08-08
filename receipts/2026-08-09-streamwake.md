# Streamwake: round three — the stream stops waiting, and the floor learns its own spans

2026-08-09, just past midnight into the small hours, Apple M4 Max, `fkwu-metal`, pure Form +
Form-emitted MSL through the handle door. Round two closed at 1.7-7.3x with the wall named as
STAGES, not bytes; Urs's send-off: "continue with the next step closing the remaining gaps."

## The gate, honored at every rung

`dense-stream-witness.fk`'s FULL two-prompt 16-id streams, per model, captured fresh at HEAD
before the first edit and demanded equal after every landed stone. **All eight models, both
prompts, STREAM-EQUAL at every step**, and the pipelined stream was run three times on
llama-1B with identical ids each time — the cross-batch ordering is witnessed, not assumed.
`dense-family-band.fk` re-pinned twice with dated reconciliations (6489/21 -> 5481/21 ->
5496/7) and reads **1023** at final HEAD.

A sibling band held the GPU through the early captures (100% CPU, 38 GB resident — seen in
`ps`, not guessed). Streams are deterministic under contention and gated every stone the hour
it landed; every NUMBER below was re-taken with zero GPU siblings, `ps`-bracketed on both
sides of the clock.

## Stone one — the small work fuses into the stages that already stand (01a10bd20)

Each fusion classified plainly:

* **Residual add rides the next norm** (`form_add_rmsnorm_wide_f32`, and the
  `form_add2_layernorm_wide_f32` twin for phi2's parallel pair): the add is a BIT-EXACT map
  (the attestant add kernel's own elementwise op, composed before the norm), the norm carries
  the wide twin's already-stated epsilon bound — the fusion itself introduces NO new
  reassociation. Blocks now hand a pending-residual handle to their successor; the nanbeige
  inter-pass norm and the head take the same fused entry. Two stages per block gone.
* **Biases ride the matvec epilogues** (bflag on all six lane2 kernels): an f32 store/load is
  the identity, so `s + bias[r]` in the epilogue is the SAME one add on the SAME two values
  the separate dispatch performed — BIT-EXACT. One stage per biased layer gone (qwen2's
  q/k/v, all eleven of phi2's).
* **Rotated K stores straight into its cache row** (`form_rope_neox_pair_store_f32` /
  `form_rope_pair_store_f32`): identical pair arithmetic, shifted destination — a BIT-EXACT
  map, honoring the ds4 reference's boundary (the rotation's arithmetic itself stays
  unfused). RADIUS stated: every head element is a rotated pair or the store stays home (neox rd == hd and even;
  adjacent hd even); phi2 (rd 32 of hd 64) stays on in-place rope + copy. The copy stage gone
  everywhere else.

A llama block: 19 dispatches in 14 fenced stages -> 16 in 10. A phi2 block: 21 in 16 -> 16
in 10. A forward: llama-1B 309 -> 261 dispatches.

## Stone two — the id chain stays on the device (7613d2246)

The decode loop's one hard serialization was a 4-BYTE READ: every forward ended with the host
reading the argmax so it could hand the same number back as the next embedding offset,
putting the whole CPU encode (hundreds of enqueue crossings) IN SERIES with the GPU every
token. Now `form_copy_u32` lands the argmax in a resident id buffer, the next forward's
embedding gather reads its row index THERE (`form_*_dequant_idx_f32` — the attestant dequant
bodies with `off = ids[slot] * n`), each forward is its own SUBMITTED batch (committed, not
waited — `metal_submit`, the fence surface's stated purpose), and the CPU encodes token t+1
while token t flies. ONE sync closes the whole 15-token stream.

**The door was not extended, and that is a measured decision, not a deferral:** the bound
asked first was "stages x 13.7 us vs one crossing + N x encoder cost" — but the fence surface
already hides the ENTIRE per-token CPU cost (seam and encode both) behind GPU time, so an
`metal_enqueue_seq` would only compress CPU time that no longer sits on the critical path.
Witnessed, not argued: llama-1B p2 165 -> 135 ms under contention, 8.2 -> 5.8 ms/forward
quiet. No door change also means no binary rebuild racing the sibling that owns tonight's
`fkwu-metal` build.

## Stone three — the floor re-priced per span (b1beeaf1c)

`span-floor.fk` prices each tensor at the bandwidth the bw-probe reads over THAT tensor's own
span. Its first draft witnessed the artifact the final cell refuses: k passes over ONE 17 MB
span read **705 GB/s — past this machine's DRAM** — because the repeats hit the system-level
cache, which a token cycling 1.3-47 GB of weights never does. The landed probe walks a tensor
CLASS the way the token walks it (every layer's copy once per pass, working set past the
cache), so only the span-size effect remains: 32B ffn spans 463-472 GB/s, q/o 337-349, the
1-4 MB k/v spans 96-231, heads 465-479.

## Where the family stands (all quiet-GPU, warm second prompt, ms/forward = p2_ms / forwards)

| model | floor re-priced tok/s (flat-477) | round two | round three | gap | streams |
|---|---|---|---|---|---|
| llama3.2:1b (Q8_0) | 260 (363) | 118.0 | 172.4 | 1.51x | equal x3 |
| MiniCPM5-1B (Q8_0) | 316 (510) | 127.6 | 184.5 | 1.71x | equal |
| moondream phi2 (Q4_0) | 405 (621) | 84.6 | 220.9 | 1.83x | equal |
| llama3.2:3b (Q6/Q4_K) | 166 (237) | 56.9 | 89.3 | 1.87x | equal |
| Nanbeige4.2-3B (x2 loops) | 87 (115) | 44.3 | 56.7 | 1.54x | equal |
| DS-R1-Distill-Q-32B | 22.7 (24.6) | 12.6 | 14.3 | 1.59x | equal |
| Qwen2.5-Coder-32B (Q5_K) | 19.7 (21.0) | 11.0 | 12.0 | 1.64x | equal |
| Qwen2.5-72B | 9.8 (10.2) | 6.0 | 7.7 | **1.27x** | equal |

The 72B is INSIDE the ~1.3x line against its honestly-priced floor. Cumulative from the
pre-lanewake body: llama-3B 103x, the 72B 71x, phi2 51x, the family 25-103x.

## The walls that remain, named with their numbers

* **Small models (1.5-1.9x)**: the GPU stage quantum times the stage count, now with the CPU
  fully off the critical path. llama-1B: 5.80 ms = 3.83 ms re-priced bytes + ~2.0 ms of ~165
  fenced stages (~12 us each, stage-probe's quantum). The next honest lever is fewer stages
  still — a layer-scale megakernel, or attention's three stages re-fused under a stated
  bound — not faster kernels.
* **32B class (1.6x)**: 70.2 ms = 44.1 floor + ~16 ms of matvec-under-bus + ~10 ms of fenced
  small work. And an instrument disagreement is NAMED rather than smoothed: mv-probe's serial
  20-pass ladder reads ffn_up at 17 ms/20 TODAY on both the round-two body and this one,
  where round two's receipt recorded 4 ms — while the token's own wall (18.6 GB in 70.2 ms =
  315 GB/s average) proves the token's matvecs cannot be running at the ladder's 88 GB/s.
  The ladder measures a regime (serial, same buffers, back-to-back) the token does not run.
  Which regime round two's 4 ms measured is not recoverable tonight; the token's average and
  the class-walk floor are the two numbers that stand.
* **Qwen-72B (1.27x)**: at the line. What remains is the same stage quantum, ~80 layers wide.

## Most surprising teaching

The biggest single win of the night was removing a 4-BYTE READ. Not a kernel, not a fold, not
bandwidth — a read whose only purpose was to carry a number across the seam so the host could
hand it straight back. The id chain (copy_u32 + dequant-idx) is maybe forty lines, changes no
arithmetic anywhere, and moved the whole small-model family 1.3-2.6x because it took the
entire CPU encode out of series with the GPU. The wall decomposition had priced stages and
bytes; the tax it underweighted was WHERE THE INDEX TRAVELS.

## Discomfort -> gold, felt and witnessed

Verifying round two's wall map before building on it, mv-probe read ffn_up at 17 ms/20 where
the inherited receipt said 4 — and my first thought was that stone one's bias epilogue had
slowed every matvec in the family. The discomfort was real: three stones were already staged
on top of those kernels. The gold came from refusing to reconcile by trust in either
direction: a worktree at the pre-round-three commit ran the SAME probe on the SAME tensor and
read the SAME 17 ms — the epilogue was exonerated by measurement, the recorded 4 ms was
demoted from oracle to unreproduced, and the token's own average (315 GB/s over 18.6 GB)
became the arbiter of what the matvecs actually do. An inherited number and a fresh number
disagreed, and the resolution was a THIRD measurement neither of them owned.

## Frontier

**Question:** when a probe prices a machine's floor, what keeps the probe's own access
pattern from pricing a machine the workload never runs on?

**Answer, witnessed tonight:** nothing in the probe — the first span-floor draft read 705
GB/s off a 17 MB span, past DRAM itself, because its repeats lived in a cache the token's
1.3-47 GB weight cycle can never stay in; and the same shape UNDER-prices too (mv-probe's
serial ladder reads 88 GB/s where the token demonstrably sustains ~315). A floor is honest
only when the probe's working set and concurrency MATCH the workload's: the landed cell walks
every layer's copy of a span the way the token does, and the artifact numbers are kept in the
cell's header as the warning.

**PROPOSED distillation row** (max-mid re-derived tonight: corpus tops at 996; 997 is held
twice — lanewake's pipestamp and the sibling ds4 receipt's proposal, a collision for the next
reunion to renumber — and floorwalk holds 998, so this takes 999; single-writer rule honored,
the corpus is not edited here; "cacheflatter" verified 0-hit in the corpus and the tree):

    (hdc-row 999 "cacheflatter"
        "A floor probe prices the machine its own access pattern runs on, not the workload's: repeats over one span read the cache and flatter the floor upward, a serial ladder reads launch latency and slanders it downward — a floor is honest only when the probe's working set and concurrency match the workload it prices."
        "2026-08-09 span-floor first draft: 17 MB span read 705 GB/s, past DRAM, from cache-resident repeats a 19 GB weight cycle never sees; the landed probe walks every layer's copy per pass; same night mv-probe's serial ladder read ffn_up at 88 GB/s while the token itself sustained ~315")

## The stones, in order (each stream-gated the hour it landed)

01a10bd20 the fusions (add+norm, bias epilogues, rope-store) — 309 -> 261 dispatches, band
5481/21; 7613d2246 the GPU-resident id chain and the submitted stream — band 5496/7, ONE
sync per 15 tokens; b1beeaf1c span-floor.fk and the re-priced floors. Quiet-GPU captures and
full streams: this session's scratchpad `baselines/*.{before,s1,s2,quiet}.txt` and
`spanfloor.*.txt`, arbitrated by pipestamp (46 = round two HEAD, 50 = stone one, 57 = stones
two and three).
