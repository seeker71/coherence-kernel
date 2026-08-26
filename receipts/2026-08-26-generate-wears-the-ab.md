# 2026-08-26 — generate wears the A/B; GGUF still frozen

The next step toward vitality was not another census and not a 27B
gradient. `fcmg-teach` already calls `fqt-verdict-name`. That name
now runs the written adapter on the overlay's 64-d feature when
`.form-lora-adapter` is present and sized to match. Heldout centroid
stays on raw features so the teach band does not depend on a
gitignored file.

```
teach-layer-band     33554431   (before and after the 64-d write)
lora-writer-band     1023
writer-run           applied=1 dim=64
qwen-lora-flag       0
centroid=nothing live=nothing   (same face on this probe; the path ran)
```

Metal does not load A/B into Qwen weights. That is still owed. This
sitting is the Form-side residual generate already owned.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-26 -> applied=1, teach-band 33554431, fqt-lora?=0
