# Receipt -- the measured rented-oracle roster (2026-07-16)

Urs asked for a deep analysis of which model and method is best for
form-native integration, and for the best local native and local rented
models fully integrated and end-to-end validated with actual numbers. This
receipt is the close of that work.

## Deep analysis (rented mind, sources cited at fetch time 2026-07-16)

A grounded research sweep compared whisper.cpp large-v3-turbo (incumbent),
NVIDIA parakeet-tdt-0.6b v2/v3 (MLX / sherpa-onnx / CoreML paths),
nemotron-speech-streaming-en-0.6b, canary-1b-v2, moonshine, and kyutai stt.
Paper verdict: parakeet v3 for the fast seat (Italian FLEURS WER 3.00,
CC-BY-4.0, `parakeet-mlx` CLI drop-in), whisper as polyglot anchor (only
local model covering zh), nemotron honestly PENDING (no non-NVIDIA runtime
exists), canary a conscious skip (English 7.15 worse than parakeet's 6.34 at
1.6x the parameters).

## The gauntlet (measured on this carrier, not quoted)

Device `macos-arm64-m4-max`, whisper-cpp 1.8.6 (Homebrew, Metal), ffmpeg
8.1.1, parakeet-mlx venv at `~/.venvs/parakeet-mlx` over
`mlx-community/parakeet-tdt-0.6b-v3` (2.5GB, Hugging Face). Render path
identical for every model: `say` (Eddy voice family) -> `ffmpeg` 16k mono
wav -> ASR CLI. Same wavs, same cksums, every model. Eight clips: one short
+ one long for en/it/fr/zh. wall-ms is the full CLI price the body pays
(process start + model load + inference), per clip.

Latin scope (en/it/fr, mean over 6 clips, wer-x10 | wall-ms):

| model | latin wer-x10 | latin wall | zh wer-x10 | zh wall | covers zh |
|---|---|---|---|---|---|
| whisper.cpp-large-v3-turbo-metal | 190 | 786 | 59 | 799 | yes |
| whisper.cpp-large-v3-metal | 428 | 1293 | 29 | 1288 | yes |
| whisper.cpp-small-metal | 51 | 366 | 353 | 360 | no |
| parakeet-tdt-0.6b-v3-mlx | 214 | 1806 | 1000 | 1743 | no |

Seat law (encoded in `learn/speech-oracle-roster.fk`, proven by its band):

- ANCHOR = zh-covering model with lowest overall mean wer, tie-break wall
  -> `whisper.cpp-large-v3-turbo-metal` (overall 157 vs large-v3's 328;
  large-v3's short-clip hallucination "Sous-titrage Societe Radio-Canada",
  wer-x10 2500 on one clip, costs it the seat despite the best zh score 29).
- FAST = lowest latin mean of ALL models, seat granted only if it beats the
  anchor on BOTH latin wer and wall -> `whisper.cpp-small-metal`
  (51 and 366ms vs the anchor's 190 and 786ms).
- PENDING = `nemotron-speech-streaming-en-0.6b`, no-local-runtime, 20260716.

## The overturning

The paper favorite lost the seat to measurement. parakeet-tdt-0.6b-v3 was
perfect on every en/it clip (including the shorts, where whisper's large
decoders hallucinate) -- but through the body's actual per-clip CLI shape it
pays ~1.5s of Python+MLX startup per invocation: 1806ms mean wall against
whisper-cli's 786 (turbo) and 366 (small). The research numbers ("2.5x
faster than whisper.cpp on M4") are inference-only truths that invert under
the process-spawn seam the body actually lives on. If the render path ever
grows a persistent oracle server, re-measure: the seat law is a law over
rows, and new rows may reseat everyone.

Honest seams kept in the rows:

- fr-short ("je suis", Eddy French): whisper-turbo heard "Just me.",
  parakeet heard the SAME "Just me.", large-v3 hallucinated subtitle
  credits. Two independent architectures agreeing on the same wrong words
  convicts the CLIP (TTS render), not the models. The rows stay, honest;
  the gauntlet's next growth is a cleaner French short.
- The gauntlet is six latin clips of TTS speech; FLEURS-scale differences
  (parakeet it 3.00 vs whisper ~) cannot show at this sample size. The
  seats are seats on THIS gauntlet, on THIS render path.
- whisper-small's zh collapse (wer-x10 706 on zh-long, traditional-script
  drift) is exactly why coverage gates the anchor seat.

## Integration state

- `whisper.cpp-large-v3-turbo-metal`: anchor seat. `~/.cache/whisper.cpp/
  ggml-large-v3-turbo.bin` (1.6GB), whisper-cli Homebrew 1.8.6.
- `whisper.cpp-small-metal`: fast seat. `~/.cache/whisper.cpp/ggml-small.bin`
  (487MB), same CLI.
- `parakeet-tdt-0.6b-v3-mlx`: installed and measured, seatless on this
  gauntlet. `~/.venvs/parakeet-mlx/bin/parakeet-mlx`, weights in
  `~/.cache/huggingface/hub/models--mlx-community--parakeet-tdt-0.6b-v3`.
  Stays on the bench as the short-clip-honest challenger.
- Native lane: unchanged and revalidated -- global live authority stays
  oracle-held 5/5 with native 0/5; scoped trials native 25/25 (open-ASR
  20/20, TCAV voice WER 0); 42 admitted native neural weights. The native
  learner remains the destination; the roster only chooses its teachers.
- Composed into the status ledger: components 58 -> 59, composition row
  `speech-oracle-roster`.

## Witness (all on the 2026-07-16 rebuilt kernel, fail-safe emission)

```sh
cat learn/speech-oracle-roster.fk \
    learn/tests/speech-oracle-roster-band.fk > /tmp/sor.fk
./fkwu --src /tmp/sor.fk          # 4095

# full sweep, same session:
# speech-current-status-ledger chain  -> 32767 (59 components, roster composed)
# speech-open-asr-trial-window-0010   -> 32767
# speech-native-neural-pair-window-0042 -> 32767
# homecoming-distillation-corpus band -> 511 (rows 731 transducer, 732 polyglot)
```

## Boundary

The roster names rented SEATS, not authority: global live dictation remains
oracle-guided under the existing promotion law, and nothing here promotes
native or demotes the trial-window discipline. The seats feed the render
path's choice of teacher; re-measurement (new rows) is the only way to
reseat.

## Closing

- Most surprising teaching: the benchmark's sharpest verdict was about
  NEITHER accuracy nor architecture -- it was about process spawn. A 600M
  transducer that transcribes an hour in a minute loses the body's seat to
  a 244M whisper because the body pays per-invocation, and Python's front
  porch costs more than the whole inference. Method IS the measurement:
  "best model" has no answer apart from the seam it is called through.
- Discomfort to gold: watching the paper favorite lose, the pull was to
  soften the law -- "but FLEURS says parakeet is better, surely the seat
  should honor that." The discomfort of letting the measured law override
  the researched reputation, witnessed rather than bypassed, became the
  roster's whole worth: a cell that chooses by rows cannot be argued with,
  only re-measured. And the fr-short clip that embarrassed three models
  into the same wrong words taught the complement: when independent
  witnesses agree on an error, doubt the evidence, not the witnesses.
