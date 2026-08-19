# 2026-08-19 — the teach layer runs

Yes asked to turn the named overlay into something witnessed.

The sitting before this one named three rungs and left two of them
at the door: corpus pairs 0, adapter slot not live. This sitting
built them in the same movement.

## What ran

Overlay: `fcmg-generate` now walks `fcmg-teach` → `fqt-prompt`.
The generate door carries the Form prefix. Raw `fcmg-q35-run`
stays for probes that need an exact prompt. Generate-joint band **3**.

Corpus: eighteen Form-native pairs, one for each curriculum name,
glossed from the body's own words. They are not NVR summaries.
NVR complete pairs stay **0** — that door is native voice, still
empty. Pair cover band bit lives inside **16383**.

Adapter: a DS4-shaped six-line slot. Centroids over 18 train
situations (class name kept out of the input) write the slot.
Held-out leakage **0**. Held-out correct **5 of 6** — beats
majority (1 of 6). The nothing held-out still answers choice;
that miss is kept, not laundered. Emit round-trips
`.qwen-teach-adapter-witness.f32`. Short prompts (< 16 bytes)
select nothing, so `hello` does not invent a control face.

`form-cli-qwen-teach-layer-band.fk` **16383**.
`form-cli-qwen-teach-generate-band.fk` **3**.

## What this is not

LoRA is still 0. No gradient touched Qwen's tensors. Qwen has no
128000 control bank; the adapter steers by composing `Control:`
into the prompt the generate door already carries. That is
Form-side embodiment, named as such.

This sitting did not re-run the 27B Metal generate pass. The wire
is observed (`fcmg-teach` is `fqt-prompt`). The GPU still owes a
fresh hello under the taught prompt.

Remote stays review.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-19 -> teach-layer-band 16383, teach-generate-band 3
