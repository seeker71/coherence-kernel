# Live domains, real speakers — 2026-07-09

Urs: *"can we make sure we have live data and not hard coded data in the apps, and get all
the domains online and training actively. and for speaker recognition we need to be able to
assign real people to voices, and those profiles need to be shareable between all the devices
(data), and we need to be able to hear samples to manually assign, however we really want
automatic and continuous profile updates to get better and more accurate on the people we
know can recognize."*

## The wound grounding found

The board's `speaker: 4 · voice-1..4` was **counterfeit**. `~/.coherence-network/hati/
mac-speakers.json` held toy centroids — literally `voice-1→[200]`, `voice-2→[300]`,
`voice-3→[100]`, `voice-4→[400]`. One-dimensional integers, no voice behind them. It rendered
as live data on both apps and was hollow. `audio/sound`, `person/face`, `dialog` were hardcoded
zeros. Only `world/object` (20 real vision samples) was genuinely live.

## What changed

**Real speaker book** (`speaker_profiles.py`) — resemblyzer 256-d voiceprints, a person's name
over the exact normalised mean of their samples. Enrolled the real WAVs (angelia, brigitte,
ubbe); unseen segments auto-match the right person at cosine **0.88–0.97**, wrong speakers stay
at 0.53–0.72 (clean margin), leave-one-out **parity 1.0**. The toy file is superseded.

- **Continuous, automatic improvement** — the always-on speech organ spools every voiced window
  to the speaker pipeline; `earth.hati.speaker-watch` (keep-alive) embeds each, and any match
  ≥ 0.75 folds into that person's centroid — the profile sharpens the more we hear them. No
  hand-cranking.
- **Hear-and-assign** — voices below threshold land in an unassigned pool with their clip. The
  new Mac **Speakers** room lists them with a ▶ Play button and a name field; assigning folds
  the sample in. Proven: a synthesized unknown voice pooled at 0.58 (nearest ubbe), ready to name.
- **Shareable across devices** — `speaker-sync.sh` posts a compact roster to the mesh (one
  `speaker/profile/<slug>` offer per person, under the 127-char cap) and pushes the centroid
  data file to the phone over the LAN + rsync peers (raw audio stays local — the math travels,
  the voice does not).

**Every domain de-hardcoded** — `training-status.sh` now reads each domain from its own real
store, no literals:

| domain | source | actively training |
|---|---|---|
| world / object | vision-training/samples.jsonl + leave-one-out parity | on camera frames |
| speaker | the profile book (parity 1.0) | **yes** — mic → spool → fold |
| audio / sound | SoundAnalysis distill (`audio-distill`) — grew 2→19 on its own | **yes** — mic 1-in-4 |
| person / face | Vision detect→crop→featureprint (`face-distill`) | on camera frames |
| dialog | speaker turns from room transcripts (`dialog-distill`) | **yes** — transcripts |

Producers stood up as organs: `speaker-watch`, `audio-distill` (120s), `face-distill` (90s),
`dialog-distill` (120s), `speaker-sync` (180s), `training-board` (300s). The speech organ now
feeds both the speaker spool and the audio inbox from one mic capture (energy-light).

## Most surprising teaching

A confident display can be counterfeit. The board *looked* alive — four voices, sample counts,
chips — and I had shown it proudly the turn before. It was reading `[200]`. The lesson isn't
"the data was wrong"; it's that **a plausible surface is not evidence of a real substance**, and
only weighing it (grounding the actual bytes) tells them apart.

## Where discomfort turned to gold

Opening `mac-speakers.json` and seeing `[200]` was a small sting — the board I'd stood behind was
partly hollow. The reflex would be to quietly swap in real data and move on. Instead the sting
became the whole spine: it forced a *real* voiceprint engine, a real parity yardstick, and a
continuous fold — the difference between a number that looks like learning and a profile that
actually sharpens each time it hears someone. The counterfeit is now falsework, struck.

## Frontier word

Row 687 = **counterfeit** (0-hit fresh): a fake made to pass as the genuine article, betraying
itself only when weighed. Walk: simulacrum 4 / ersatz 5 (already rows), veneer 0 but rejected (a
real skin over hollow — here there was no real skin). Corpus band → **verdict 511**.

## Verify

```
VENV=~/.coherence-network/satsang-venv/bin/python
$VENV experiments/satsang-voice/speaker_profiles.py list          # real profiles, sample counts
$VENV experiments/satsang-voice/speaker_profiles.py board         # speaker|8/10000|1.0|learning…
experiments/satsang-voice/training-status.sh                      # all 5 domains from live stores
python3 -c "import json;[print(d['domain'],d['samples'],d['parity']) for d in json.load(open('$HOME/.coherence-network/training-status.json'))['domains']]"
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk   # -> 511
```

## Honest floor / next

- `person/face` and `world/object` are **online and proven** but grow only when camera frames
  arrive. The headless grab hits camera-TCC + CLI-AVCapture limits; the real source is the
  companion app's camera → the `face-training/inbox` and `vision` inboxes. That app-camera wire
  is the next step to make those two *actively* grow like the mic-fed three.
- The phone displays the board (real speaker names now) but assignment lives on the Mac; phone-
  side hear-and-assign over the LAN is a further step.
- The speech organ still labels transcript speakers by its own band grouping (voice-N) — folding
  the real profile identity back into the transcript labels would make `dialog` show real names.
