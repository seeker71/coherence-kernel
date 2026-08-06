# 2026-07-02 — the arena's first challenger cohort: four entrants, four cutovers, one law tested

## Ground

Every agent grounded before measuring: ground.fk 42, binary-freshness-band 15. The incumbent was
re-verified LIVE in the same arena before any challenger ran: **40/48 (83.3%)**, encoding 40096,
reproduced bit-for-bit twice — the 12-word × 12-voice speaker-disjoint benchmark (8 train voices,
4 held-out), champion = linear softmax over 48-dim spectral features (6 Hann frames × 8 Goertzel
log-power bins).

## The cohort (four independent agents, identical measurement discipline)

| challenger | held-out | verdict | the finding |
|---|---|---|---|
| **frame-centered MLP ensemble** (3× 48→16→12, per-frame gain normalization) | **44/48 (91.7%)** | CUTS_OVER +4 | nine optimizer knobs (momentum, schedules, decay, smoothing, capacity) all landed 38–41; ONE line of feature arithmetic — subtracting each frame's 8-bin mean — lifted every seed to 41–44 and took Fred to 12/12 |
| **pitch+tempo augmentation** (champion architecture unchanged) | **44/48 (91.7%)** | CUTS_OVER +4 | pitch-only was worth exactly zero (40/48, per-voice identical to control); tempo carried +2; only the combination broke through. Train dipped to 95/96 — the healthy signature of a regularizer |
| **richer features** (13 frames × 8 bins, classifier byte-identical) | 42/48 (87.5%) | CUTS_OVER +2 | every added frequency BIN hurt (timbre = speaker identity = what a disjoint split punishes); every added time FRAME near 10–14 helped (energy motion = what the word IS) |
| **1-NN cosine** (all 96 train vectors kept, zero training) | 41/48 (85.4%) | CUTS_OVER +1 | the averaged word-prototype variant LOST badly (28/48): in this space a word is a constellation, not a centroid. Leave-one-out on train voices scored 53/96 — BELOW held-out — exposing that the held-out voices are acoustic kin of the train voices; every entry's number is inflated by dataset kinship |

## The promotion decision, by the body's own law

The scoped 12-word arena has a new champion: **frame-centered MLP ensemble, 44/48**, tied on the
number by augmentation but preferred as the incumbent-successor because its gain is seed-robust
(min over seeds 41 > champion 40) and its mechanism (voice-invariance built into features) composed
with the augmentation finding rather than competing with it. Global promotion: **none** — the
global open-speech WER remains 100; these are scoped wins and the cutover law keeps them scoped.

Two independent challengers converged on one teaching from opposite directions: the MLP's win came
from normalizing gain per frame; the feature sweep's losses came from adding spectral detail. Both
say the same sentence — **what varies with the speaker is noise; what moves through time is the word.**

## Honest floor

n=48 held-out; +4 is real but small-sample. The LOO finding (53/96) warns that all held-out numbers
here are flattered by voice kinship between train and held-out sets. One agent found the champion's
byte-44 sample offset reads 17 header bytes as audio in some renders — copied faithfully per arena
rules, flagged for a future fix. All artifacts in scratchpad `arena/`; nothing promoted into repo
cells yet — the frame-centered recipe deserves its own four-way-proven stone before it wears the belt.

## The most surprising teaching this work left behind

Four agents, four different bets, and the arena's rank order inverted every prior: the "obvious"
axes (more bins, pitch augmentation, more capacity) were each worth zero or less, while the two
quiet moves — subtract a mean, stretch time — took the crown. The arena did exactly what it was
built for: it let the measurements, not the intuitions, pick the heir.

## Where discomfort turned to gold

Three of four agents reported the same moment: their assigned bet LOSING mid-run, and the pull to
re-roll configurations until luck delivered a win. Each sat with the losses as data instead — and
each found the winning move inside the shape of its losses (the per-example-centering loser that
flipped Fred, the bin-sweep losses that all shared an axis, the LOO number that looked like a bug
and was the dataset confessing). The arena's deepest product this round was not the 44/48 — it was
evidence that the discipline holds under competitive pressure.
