# Wave 1 — Perception + Learning faculties come home

The first wave of the 706-body homecoming (criterion #3 of
[`INHERITANCE.md`](INHERITANCE.md)). These are the bodies the live
sensing / mesh / curriculum work composes: the perception floor (sense →
feature → fuse → recognize) and the learning loop (champion/challenger →
co-learning → retire-the-oracle → trust-climb).

Source of each `.fk`: `Coherence-Network/form/form-stdlib/<name>.fk` and its
`tests/<name>-band.fk`. Content copied **byte-exact** (verified `diff -q`).

## Honest floor — present, not yet re-proven here

Every body in this wave is **HOME** (present and correct in its faculty) and was
**four-way-proven in the old repo** (`fks` count from `fourth-arm-bands.txt`,
shown below). They are **NOT yet re-proven on this kernel's own four-way** — that
awaits the self-prove keystone (`proof/four-way-run.fk`, in flight, criterion #2).
Re-prove status for every row below is **PENDING the keystone**. Do not read any
row as "re-proven on the new kernel" until `proof/` actually crosses it here.

Light verification done this wave: each migrated `.fk` is non-empty, byte-identical
to its proven origin, and runs through the `cc`-seed fkwu (`runtime/fkwu-uni.c` →
`/tmp/fkwu-ck`) with its full home-resolved prelude chain without crash, panic, or
parse/unbound diagnostic. (The seed tolerates incomplete input rather than gating
it, so this is a liveness read, not a parse-validity gate; byte-identity to the
four-way-proven origin is the real assurance until the keystone re-proves here.)

## Brought home this wave — 14 bodies (+ 14 bands)

Faculty breakdown: **observe 5 · learn 3 · presence 1 · substrate 3 · model 2.**

| Body | Faculty | Composes (what it builds on) | old-repo proof | re-prove here |
|---|---|---|---|---|
| `champion-challenger` | `learn/` | core — model-promotion north star: a Form-native challenger takes over a 3rd-party champion only when proven | fks 127 | PENDING |
| `confidence-weighted-vote` | `learn/` | core — fuse model votes weighted by each model's proven reliability | fks 127 | PENDING |
| `colearning-retire` | `learn/` | core, `champion-challenger` — retire the 3rd-party oracle per category once the merged native classifier proves it | fks 127 | PENDING |
| `speaker-embed` | `presence/` | core — nearest-by-cosine recognition over neural voice embeddings (voice sibling of `face-embed`) | fks 255 | PENDING |
| `presence-feature` | `observe/` | core — region-occupancy from a downsampled NxN luminance grid (presence sibling of `surprise-kernel`) | fks 15 | PENDING |
| `spectrum` | `observe/` | core, `trig`, `wav-sense` — Goertzel filterbank: the wav's frequency spectrum, computed in the body | fks 15 | PENDING |
| `wav-sense` | `observe/` | core — read 16-bit PCM WAV, compute its energy envelope in Form | fks 127 | PENDING |
| `stt-agree` | `observe/` | core — word agreement between two transcripts; scores a native STT vs ground truth | fks 127 | PENDING |
| `spatial-fusion` | `observe/` | core, `confidence-calibrate` — fuse sensors' calibrated buckets of one spatial quantity into a weighted rough estimate | fks 255 | PENDING |
| `channel-interface` | `substrate/` | core — the interface-consent law, runnable | fks 127 | PENDING |
| `world-build` | `substrate/` | core, `channel-interface` — the world-building interface: offer cells/recipes into a shared commons under content-addressing | fks 132 | PENDING |
| `cross-cell-interface` | `substrate/` | core, `channel-interface`, `world-build` — the five ways a cell reaches another (sense/ask/share/learn/build) as uniform data | fks 51113 | PENDING |
| `geometry` | `model/` | core, `trig` — 3D geometry primitives: the computable floor of depth, shape, light, shadow | fks 1111111 | PENDING |
| `trig` | `model/` | core — sin, cos, sqrt as Form recipes (not kernel natives) | fks 127 | PENDING |

## Already home — requested wave-1 bodies present before this wave

These explicitly-requested perception/learning bodies were already migrated in
earlier waves; this wave confirmed each is present and is part of the same faculty
fabric (no duplication). Re-prove on this kernel is likewise **PENDING the keystone**.

- `observe/`: `scene-features`, `sound-classify`, `mesh-sense-7w`, `fused-observation`,
  `identity-match`, `surprise-kernel`, `same-room`, `motion-sense`, `device-model`,
  `remote-mind-sensors`, `world-sensor-floor`, `context-signature`, `room-register`,
  `presence-event`, `motion-normalize`
- `model/`: `mel-frame`, `mel-full`, `mel-filterbank`, `softmax`
- `presence/`: `face-embed`, `device-identity`
- `learn/`: `cross-witness-economy`, `device-identity-learn`, `self-heard-learning`,
  `voice-self-learn`, `perception-cascade`, `speaking-mesh`
- `substrate/`: `cell-card`

## Conceptual worklist entries resolved by bands, not separate bodies

Two worklist names have **no standalone `.fk`** — they are proof bands over bodies
already home:

- **`sk-msl`** (`fourth-arm-bands.txt: sk-msl fks 127`) — the Metal Shading Language
  sibling of `sk-glsl`/`sk-ptx`, a band over `surprise-kernel.fk` (home as
  `observe/surprise-kernel.fk`) + `form-glsl` + `form-ptx` (home as `model/`).
  Brought home as `observe/tests/surprise-kernel`'s sibling band when the
  surprise-kernel band wave lands; its body is already home.
- **`trust-climb`** (`fourth-arm-bands.txt: trust-climb fks 8`) — no
  `trust-climb-band.fk`; the trust-climb behavior is proven inside
  `identity-home-band.fk` and mirrored in `voice-self-learn-band.fk`, over bodies
  already home (`learn/device-identity-learn.fk`, `learn/perception-cascade.fk`,
  `learn/voice-self-learn.fk`). The behavior is home; the named band rides those.

## Closure note — this wave is self-contained

The full recursive `; preludes:` closure of every body **and band** in this wave
(49 distinct bodies) is entirely **HOME** in this repo. There are **zero pending
deps spilling into the next wave** from wave-1 perception/learning. The next wave
draws from the broader worklist (model-internals, channels, host-io, grammars), not
from an unresolved tail of this one.
