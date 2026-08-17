# Offline workshop sessions

`offline-workshop` turns a consented local recording into an integrity-checked
session bundle: normalized audio, timestamped transcript segments, explicit
unknown speakers, measured health fields, and source-linked starting material
for a workshop manual or book. Audio and words remain on the machine. The
carrier makes no network request.

This is not a separate Sema mode. Host audio and Whisper are carriers; the
bundle's trust meaning is decided by
[`offline-workshop-session.fk`](form/form-stdlib/offline-workshop-session.fk),
inside the same Form body.

## Prepare a fresh clone

1. Build `fkwu` and run the grounding checks in [`BOOTSTRAP.md`](BOOTSTRAP.md).
2. Install Python 3, `ffmpeg`, and whisper.cpp's `whisper-cli` for your system.
3. Put a whisper.cpp ggml model on the machine. Large-v3-turbo is the observed
   default on this Mac; smaller ggml models are valid when memory or speed asks
   for them. Models are local dependencies and are not committed to this repo.
4. Point the carrier at the model and ask it to observe readiness:

```sh
export COHERENCE_WHISPER_MODEL=/absolute/path/to/ggml-large-v3-turbo.bin
./offline-workshop doctor
```

`ready: true` means `ffmpeg`, `whisper-cli`, the model, and the Form gate were
all found. Ollama and `llama-cli` are reported separately and are not required.
The doctor performs no installation and uses no network.

Windows can invoke the same standard-library carrier directly:

```powershell
python tools/offline_workshop/offline_workshop.py doctor --model C:\models\ggml-small.bin
```

## Transcribe a recording

Participants' agreement is a required input, not metadata added later:

```sh
./offline-workshop transcribe \
  --audio /absolute/path/to/session.m4a \
  --session sessions/2026-08-17-circle \
  --consent explicit
```

The accelerated Whisper lane is tried first. If it exits, crashes, or times
out, the same audio and model are tried once with `-ng` on CPU. Both attempts
are timed and recorded without copying private transcript text into diagnostic
logs. Use `--cpu` to choose the CPU lane immediately.

Every session directory is new and immutable by default. An existing session
is never overwritten. A successful run verifies itself through Form and prints
`verdict: 16383`. A failed run has no complete manifest; it keeps
`status.json`, `evidence/failure.json`, and any available attempt evidence so
the failure arrives as loving attention with a concrete resolution.

## Capture a room

On macOS, the defaults are ffmpeg `avfoundation` input `:0`. On Linux, they are
PulseAudio input `default`. Confirm the correct room microphone and permissions
before the group begins.

```sh
# A timed sound check
./offline-workshop capture \
  --session sessions/sound-check \
  --consent explicit \
  --seconds 20 \
  --cpu

# A full session; stop recording with Ctrl-C
./offline-workshop capture \
  --session sessions/group-circle \
  --consent explicit
```

Override the host door when needed:

```sh
# Windows example; use the exact DirectShow device name on that machine
python tools/offline_workshop/offline_workshop.py capture \
  --session sessions/group-circle \
  --consent explicit \
  --input-format dshow \
  --input-device 'audio=Microphone Name'
```

The original capture and normalized 16 kHz mono source are both hashed. Keep
the microphone close enough for every participant, run a short sound check,
and preserve the original room recording: transcription cannot recover speech
that the microphone did not receive.

## Turn unknown into reviewed knowledge

Group-room speaker diarization is not claimed. Mono Whisper text enters with
`speaker: unknown` and `overlap: unknown`. That is healthier than attaching a
person's identity to a guessed voice cluster.

After participants review the transcript, create a tab-separated label file:

```text
# segment id<TAB>participant-approved label
seg-000001	Facilitator
seg-000002	Ari
```

Then apply it and re-run every hash, count, source link, and Form trust check:

```sh
./offline-workshop relabel \
  --session sessions/group-circle \
  --labels /absolute/path/to/reviewed-speakers.tsv \
  --consent explicit
```

Only named rows become `human-reviewed`; all others stay unknown. The revision
records the labels file hash, not an unsupported biometric claim.

## Turn WER and speaker error into observed numbers

The carrier will not estimate its own accuracy from model confidence. Give it a
human-reviewed plain-text transcript of the same audio to measure word error
rate. For a speaker measure, also provide reviewed time intervals:

```text
# start_ms<TAB>end_ms<TAB>participant-approved label
0	1840	Facilitator
1840	3920	Ari
```

Run the evaluation only after applying the same reviewed labels to the session:

```sh
./offline-workshop evaluate \
  --session sessions/group-circle \
  --reference-transcript /absolute/path/to/reviewed-transcript.txt \
  --speaker-reference /absolute/path/to/reviewed-speakers-by-time.tsv \
  --consent explicit
```

WER is Unicode-casefolded word Levenshtein distance divided by human-reference
words. The reported speaker measure is time-weighted exact-label error with no
collar: missed, false-alarm, and wrong-speaker milliseconds divided by reviewed
single-speaker reference time. Human-reference overlap is reported and excluded,
because this carrier does not claim overlap-aware diarization. Both reference
files are copied into the private bundle and hashed; verification recomputes the
numbers instead of trusting saved metrics.

## What the bundle contains

| Evidence | Meaning |
|---|---|
| `manifest.json` | Consent boundary, offline storage claim, tool/model facts, counts, and artifact hashes |
| `audio/capture.wav` | Original microphone capture, when the capture command was used |
| `audio/source.wav` | 16 kHz mono PCM source actually given to Whisper |
| `transcript/whisper.json` | Whisper's local timestamp/token evidence |
| `transcript/segments.jsonl` | Stable segment ids, time ranges, speaker state, confidence, and audio source refs |
| `transcript/transcript.md` | Human-readable, timestamped transcript with stable anchors |
| `drafts/workshop-manual.md` | Declared extractive draft; excerpts link to transcript sources |
| `drafts/book-source-ledger.md` | One source-linked row per observed transcript segment |
| `evidence/metrics.json` | Counts, coverage, timing, attempt outcomes, and honestly absent quality measures |
| `evidence/verification.json` | Recomputed integrity result and the Form verdict |
| `evidence/evaluation.json` | Recomputed WER and optional reviewed-label speaker error when human references exist |

The generated manual is deliberately a beginning, not synthetic certainty. It
does not infer themes, consensus, identities, or participant intent. Its fixed
reflection prompts invite human interpretation while every quoted phrase stays
linked to its timestamped source. A local model may later draft over the book
ledger, but its prose must remain a separate declared layer.

## The real numbers

Run verification whenever the bundle moves or changes:

```sh
./offline-workshop verify --session sessions/group-circle
```

The gate measures exact artifact hashes, segment/source-link equality, audio
presence, timeline sanity, known/unknown speaker partition, human review for
every known label, offline/local boundaries, and observed ASR. It also reports:

- speaker-attribution coverage;
- human-reviewed and unknown segment counts;
- overlap observed and overlap unknown counts;
- source-link coverage;
- local ASR attempt outcomes and elapsed time;
- mean Whisper token probability, explicitly not an accuracy score.

WER stays `null` until a human reference transcript exists. Speaker error stays
`null` until time-aligned human speaker truth exists. Once supplied through
`evaluate`, both become reproducible numbers and are recomputed by `verify`.
The system will not turn absent reference truth into a reassuring zero.

Session directories under `/sessions/` are ignored by git. Recording consent
is not publication consent: the public commons must never receive private audio
or transcript bundles by accident.
