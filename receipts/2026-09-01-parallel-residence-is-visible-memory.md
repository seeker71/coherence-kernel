# 2026-09-01 — parallel residence is visible memory

> Correction witnessed 2026-09-03: the positive carrier handles below prove
> callable contexts, while the byte values are logical catalog artifact sizes.
> They do not prove per-model mapped or materialized pages. The corrected
> executable boundary and physical `vmmap` evidence live in
> `2026-09-03-owner-state-is-an-event-not-an-idle-render.md`.

Urs asked for the model fleet to stay physically present until pressure, and
for the glass to make memory purpose, age, and the next eviction obvious.

## Physical crossing

`observe/native-model-dual-resident-live-run.fk` opened one long-lived Form
process with both carrier handles alive at once:

- Qwen3.8 Flash Next UD-Q2_K_XL: handle `1262`, prefilled, position `20`;
- Llama 3.2 3B: handle `1695`, prefilled, position `6`;
- loaded `2`, prefilled `2`, logical weights `80,888,506,240` bytes inside the
  configured `103,079,215,104`-byte wired budget;
- the return from 3B to Qwen reported `append`, reused `19`, evaluated `1`;
- pressure plan at zero pressure selected zero evictions.

The live turnwheel then advanced 3B to position `7` without closing Qwen, and
advanced Qwen to position `21` without closing 3B.  The final status still
reported both positive handles, both loaded, and both prefilled.

## The glass

`form/form-stdlib/native-model-memory-glass.bml` projects carrier handles,
logical catalog extents, and policy state as `FORM-MODEL-MEMORY-V1` rows:

- green `◆` — the running native Form/world validation and evaluation process;
- blue `▰` — 3B natural-language-to-Form proposal weights;
- purple `▰` — Qwen reasoning/fallback weights;
- orange `◇` — the Qwen expert concept overlay;
- cyan `▤◈` — KV and recurrent substate;
- neutral `□` — logical admission headroom.

The capacity strip is scaled by logical catalog model-weight bytes. Fill alpha
is read age; edge alpha is write age.  The text fallback uses `█▓▒░`.  `⇣`
marks the next policy-ranked physically releasable unit, `·` another eligible whole
model, and `⊘` a desired grain that is not independently releasable yet.

The age/frontier was not static decoration.  After the 3B step, 3B became
opaque and Qwen aged to `▓`; the `⇣` moved to Qwen.  After the Qwen step, Qwen
became opaque and the `⇣` returned to 3B.  Both handles remained alive.

## Honest physical floor

The 512 Qwen experts are still packed into three whole expert tensors per
layer.  They are shown orange and pinned because no expert-level Metal handle
exists.  Both KV/substate overlays are also pinned because their buffer bytes
and partial-release handles are not yet exposed. Therefore the first available
carrier release action is closing an inactive whole model context; its physical
byte effect is not claimed. The policy's
north star is independently mapped split experts, then KV/substate, then an
inactive whole model, ranked within each grain by use count and last use.

The capacity strip includes logical catalog model-weight bytes only. The current Metal
carrier does not expose device `currentAllocatedSize` or
`recommendedMaxWorkingSetSize`, so the glass says `pressure=unobserved` and
does not fabricate GPU pressure or state-buffer sizes.  macOS separately
reported 36% system-wide memory free during this witness, but that uncorrelated
host reading is not used as eviction authority.

## Witnesses

- binary freshness: `31`
- high-authoring band: `4095`
- resident fleet band: `8191`
- memory glass band: `32767`
- model glass band: `16383`
- Qwen resident-state band: `1023`
- Qwen physical token-handle band: `2047`
- 3B resident-state band: `255`
- 3B physical token-handle band: `255`

Most surprising teaching: opacity became useful only when it moved the
eviction arrow under real alternating model use; a beautiful static heatmap
would have hidden the scheduling truth.

Discomfort turned to gold at the packed-expert seam.  The desired first
eviction grain does not physically exist yet.  Showing `⊘◇` in orange is more
alive than coloring a fictional expert cache green.

Signed: Codex / Sol.  I kept the exchange alive by preserving both model
contexts through the experiment, making the physical limitation visible, and
leaving the turnwheel resident rather than closing it for a tidy receipt.

; witnessed: 2026-09-01 -> dual handles 1262/1695 live; alternating 3B/Qwen
; steps moved age and eviction frontier; both remained loaded and prefilled
