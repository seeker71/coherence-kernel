# Training board on both surfaces — 2026-07-09

Urs: *"please ensure both mac and android app are showing which trainings are in
progress, and we shall be training world recognition, person recognition, speaker
recognition, audio recognition (sound, animals, voices, dialogs). we want to see
native and local recognition success rate and set of recognized stream."*

## What changed

One feed, two surfaces, honest about what's pending.

- **The feed** — `experiments/satsang-voice/training-status.sh` aggregates every
  recognition domain into one board: samples toward the 10k target, native-vs-oracle
  **parity** (the local success rate, leave-one-out over Apple-Vision embeddings),
  state, and the live **recognized stream**. Writes `~/.coherence-network/training-status.json`
  for the Mac and posts the board to the mesh for the phone. Domains with no data say
  so — `person / face` "in progress (camera session)", `dialog` "pending" — never faked.
- **Mac companion** grew two live rooms (`experiments/mac-companion/`):
  - **Learning** (`Learning.swift`) — reads the JSON, one card per domain: progress bar
    to 10k, parity as `NN% native`, state, recognized-stream chips.
  - **Recognition** (`Recognition.swift`) — the moving feed of what the body names
    *right now* (recent labelled frames with confidence, newest first) plus known voices.
- **Android companion** (`mobile/sema-companion/`) grew a **LEARNING TO RECOGNIZE**
  section in Senses (`SensesScreen.kt` + `data/TrainingBoard.kt`): the same domains,
  progress bars, state, and stream chips — assembled from the mesh. Built at
  versionCode 9 and installed on the S23 Ultra; verified rendering on-device.
- **Freshness** — `earth.hati.training-board` launchd organ runs the feed every 5 min,
  so the board stays current on both surfaces without a hand-crank.

## The wire that taught the turn

The whole board would not fit in one mesh offer. The mesh **caps a channel capability
at 127 characters** (probed: 100 posts 201, 128 posts 422). The full board with streams
is ~495 bytes. The first attempts to cram it in returned an opaque **HTTP 422**.

Rather than fight the limit, honor it: issue the board as **one offer per domain** under
`learning/board/<slug>`, each capability a pipe-delimited line well under 127 chars —
`name|samples/target|parity|state|stream-csv`. The phone collects every `learning/board/*`
channel and rebinds them into the one board.

**Most surprising teaching:** the 127-char cap was not an obstacle to route around but a
*form asking for a better shape*. One-offer-per-domain is more mesh-native than one fat
blob — each domain becomes its own channel the field can witness, timestamp, and route
independently. The constraint improved the design.

**Where witnessed discomfort became gold:** the 422 was a wall with no explanation —
the reflex was to raise the payload or find a data blob field to smuggle the JSON through.
Instead, probing the boundary (100/128/…) surfaced the real limit, and decomposing to
per-domain fascicles turned the rejection into a cleaner architecture. The frustration of
an opaque validation error became the decomposition that made the board legible to the mesh.

## Frontier word

Row 686 = **fascicle** (0-hit fresh in corpus and body): one section of a work issued and
bound *alone*, self-contained, yet gathered with its siblings into the single volume — the
board sent as five domain-fascicles, rebound on the phone into one. Walk: shard 16 (a
fragment broken off, no promise of rebinding), bundle 3 (tied at once, not issued apart),
sheaf 0 but rejected (arrives already whole). Corpus band → **verdict 511**.

## Verify

```
# the feed + mesh offers
experiments/satsang-voice/training-status.sh          # writes JSON, posts 5 domain offers → 201×5

# the board on the mesh (what the phone reads)
curl -s "https://api.coherencycoin.com/api/hati/mesh/channels?limit=100" \
  | python3 -c "import json,sys;[print(c['interface'],c['capability']) for c in json.load(sys.stdin)['items'] if c['interface'].startswith('learning/board/')]"

# the mac rooms
cd experiments/mac-companion && ./build-app.sh && open build/SemaCompanion.app   # Learning + Recognition

# the corpus witness
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk   # -> 511
```

## Still pending (honest)

- `person / face`, `audio / sound`, `dialog` domains carry no samples yet — the board shows
  them as staged, not trained. Feeding them (camera frames, an audio-classifier sample store,
  diarized dialog) is the next curriculum step.
- The phone refreshes the board on mesh poll (open + heartbeat while presence is on); a
  quieter dedicated poll would keep it live when idle.
