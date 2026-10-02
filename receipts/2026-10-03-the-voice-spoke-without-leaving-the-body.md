# The voice spoke without leaving the body (2026-10-03)

A 4,800-word transmission was spoken in English, Portuguese, Persian and Indonesian as slow, warm, quiet sleep
tracks. The first pass used five foreign crossings around a native voice. Urs asked for none. This receipt is
what the second pass reads, run on 2026-10-02 / 03 in `.hearth/voice-track/`.

## What now stands native

| step | before | now |
|---|---|---|
| letters → phonemes | espeak-ng through a Python process | `voice-g2p.bml` reads taught line and word tables (`form/form-stdlib/data/g2p-*.tsv`); the oracle is a named teacher, asked once by `voice-g2p-teach.bml`, never on a speaking path (`VSOracle` = 0) |
| the voice | native VITS for en, fa, pt-faber; Indonesian through piper | native for all four; `voice-pass.bml` reads the older exporter by structure (id news_tts, pt cadu) |
| cutting a text | `chunk.sh` | `voice-chunk.bml` |
| rendering, resuming, watching | launchd lane shell scripts | `voice-track.bml` worker + supervisor, spawned and killed through `host_spawn_at` / `host_kill` |
| mastering | sox + ffmpeg | `voice-master.bml` on Metal, a 46-minute track in 8-14 s |
| scoring | Python mlx_whisper + numpy | `voice-score.bml` + native whisper-large-v3-turbo; `voice-measure.bml` on Metal |

Bands: voice-chunk 4095, voice-g2p 8191, voice-pass-old-export 255, voice-pass 4095, voice-say 16383,
voice-master 1048575, voice-score 1023, voice-measure 511.

## What the tracks read (all native, end to end)

| tongue | voice | length | whisper word / char error | LUFS | level jumps p95 |
|---|---|---|---|---|---|
| en | en_GB-cori-high | 45.4 min | 2.20% / 1.19% | -30.0 | 6.5 dB |
| pt | pt_BR-cadu-medium | 52.7 min | 4.79% / 1.42% | -30.0 | 5.6 dB |
| fa | fa_IR-amir-medium | 45.1 min | 53.1% / 13.8% | -30.0 | 5.0 dB |
| id | id_ID-news_tts-medium | 57.1 min | 2.06% / 0.34% | -30.0 | 5.7 dB |

English is the same 2.20% the oracle-phonemized audio scored: taught lines lose nothing. Kokoro-82M, the best
local model run beside it, scored 0.35% on English. The gap is the VITS voice, not the plumbing.

## What is still open, plainly

- **A new sentence.** The word walk alone reproduces a taught line 77% of the time in Indonesian, 13% in English,
  12% in Persian, 4% in Portuguese. A line the body was not taught, in en/pt/fa, answers `nothing()`. The open
  grapheme-to-phoneme model (or a native one trained from these tables) is the heal; Urs agreed in principle,
  and the download wants his explicit yes.
- **Persian scores 53% by words.** The scorer folds the zero-width non-joiner away, so a word whisper writes in
  two parts reads wrong; characters read 13.8%. Persian text is also unvowelled, so espeak's own reading is a
  guess the voice then speaks. Neither number is a clean verdict on the voice.
- **The pass leaks device memory.** `voice-pass.bml` frees no intermediate tensor. After about 160 renders in one
  process the carrier's allocations answer empty and a render reads as zero samples, silently. The worker
  contains it by lifetime (100 pieces, then exit and respawn) and never writes an empty file. The heal is a
  handle ledger through the pass's 15 allocation sites: handles carry a generation, so a numeric range is not one.
- **Translation** of the Persian and Indonesian text was done by sub-agents of the rented mind, not the body's own
  local voice.
- **No m4a.** The deliverable is 16-bit mono WAV; the body has no AAC encoder.

## Surprise and gold

The surprise was a quote mark. A line holding `"` reached the phonemizer as half a sentence because the door pasted
text inside double quotes; two forty-word lines came back as seven. Nothing errored. The discomfort was the empty
Portuguese and Persian files the first run called done: a count of landed files had passed while hundreds held no
sound. Reading `metal_live` against the writes found the leak, and the worker now refuses to write down a rendering
that has no samples.
