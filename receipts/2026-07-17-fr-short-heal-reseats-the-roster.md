# Receipt -- the fr-short heal reseats the roster (2026-07-17)

The committee's founding HOLD on fr-short (three architectures agreeing on
the same wrong words) had convicted the clip, not the models. This receipt
closes that loop: the clip healed, the whole gauntlet re-measured in one
session, and every seat moved.

## The heal

Founding clip: Eddy (French (France)) saying "je suis" -- 1006ms of audio
that whisper-turbo and parakeet both heard as "Just me." and large-v3 heard
as subtitle credits. Healed clip: the same voice saying "je suis là" --
one added syllable, 1038ms, cksum 4243204481. All four models hear it at
wer 0. Candidate B (Jacques, same phrase) also measured clean; Eddy kept
the voice family uniform.

## The reseat (all 32 runs fresh, 2026-07-17, one session)

| model | latin wer-x10 | latin wall | overall wer | covers zh |
|---|---|---|---|---|
| whisper.cpp-large-v3-metal | 11 | 2210 | 16 | yes |
| whisper.cpp-large-v3-turbo-metal | 23 | 1175 | 32 | yes |
| whisper.cpp-small-metal | 51 | 684 | 127 | no |
| parakeet-tdt-0.6b-v3-mlx | 47 | 2361 | 285 | no |

- ANCHOR moved turbo -> large-v3: with the poison gone, large-v3's
  hallucination (2500 on the old clip) vanished and its overall 16 beats
  turbo's 32. The founding anchor choice had been substantially a verdict
  on one bad second of audio.
- FAST SEAT moved small -> turbo, under a corrected law. The founding law
  let ANY model be fast-candidate by wer alone; on the healed rows the
  most accurate model was also the slowest, and the old law would have
  seated large-v3 twice, leaving no fast lane. The law now states the
  seat's purpose: only ears with latin wall STRICTLY below the anchor's
  are candidates; among them, best latin wer (turbo: 23 at 1175ms).
  Replayed against the founding rows this law picks small -- backward
  consistent; the correction changes the law's words, not its history.
- PARAKEET fell off the frontier: worse latin wer AND worse wall than
  turbo, strictly dominated on this carrier, band-witnessed (bit 256).
  It keeps its bench place as the committee's cross-architecture witness.
- ROUTER re-identified: against the new anchor it CONCEDES accuracy
  (routed 25 vs anchor 16) and claims SPEED (1280 vs 2228 overall wall,
  speed-gain-x10 = 17). Dispatch stays coarse -- finer keys would fit one
  clip per cell; the gauntlet must grow before the dispatch may.
- COMMITTEE now admits 5/8 (fr-short joined at 0/0), holds 3/8, purity
  still 0. The founding hold -> heal -> admit is the gate's first
  completed teach-the-gauntlet loop.

## Witness (2026-07-17, this checkout, fkwu rebuilt from merged v4 kernel)

```
speech-oracle-roster band     -> 4095
speech-oracle-router band     -> 255
speech-teacher-committee band -> 127
speech-current-status-ledger  -> 32767
homecoming corpus band        -> 511 (row 768 "pareto")
```

## Boundary

Same boundary as the founding: seats are rented-lane dispatch, not global
authority; the gauntlet is still eight TTS clips and its growth (human
voices, more locales, more clips per cell) is the standing next step. The
walls are one machine-session's truth; the law reseats by re-measurement.

## Closing

- Most surprising teaching: the founding measurement's most confident
  verdicts -- turbo anchors, small sprints, parakeet loses on startup tax
  -- were all substantially verdicts on ONE second of bad audio. Healing a
  single clip moved every chair. Small gauntlets do not merely blur the
  picture; they can invert it, which is why every pin is a band that
  breaks loudly instead of a memory that fades quietly.
- Discomfort to gold: the corrected fast-seat law was written AFTER seeing
  the numbers it would choose from, and that ordering felt like fitting
  the law to the data. The discomfort resolved through the replay test:
  the new law, run against the FOUNDING rows, picks the same seat the old
  law did (small) -- the correction states the seat's purpose without
  rewriting its history, which is the difference between fitting and
  clarifying.
