# Receipt -- frontier row 731, "transducer" (2026-07-16)

Urs brought word of NVIDIA's open 0.6B ASR models: Parakeet TDT 0.6B and its
low-latency counterpart Nemotron Speech ASR 0.6B. The claims were grounded
live against Hugging Face before anything moved, and the grounding surfaced
one 0-hit hole in the body's speech vocabulary.

Grounded reading of the news (2026-07-16, Hugging Face model cards):

- `nvidia/parakeet-tdt-0.6b-v2` / `-v3`: 600M params, FastConformer encoder
  with a Token-and-Duration Transducer (TDT) decoder, CC-BY-4.0, ~6.05% WER
  and RTFx ~3380 on the Open ASR leaderboard (v2, English; v3 multilingual,
  25 languages). Offline-fast, not streaming: full attention, up to 24 min
  in one pass.
- `nvidia/nemotron-speech-streaming-en-0.6b`: the true real-time sibling --
  cache-aware FastConformer with an RNNT (not TDT) decoder, chunk sizes down
  to 80ms, NVIDIA Open Model License (not CC-BY-4.0).
- Honest seams in the arriving claim: "fully open source" is exact for
  Parakeet (CC-BY-4.0) and looser for Nemotron (open weights under NVIDIA's
  own license); "FastConformer-TDT" describes Parakeet only -- the streaming
  model is FastConformer-RNNT; "real-time" belongs to Nemotron, "blazing-fast
  offline" to Parakeet.

Relation to the body: the open-ASR oracle lane stands on
`whisper.cpp-large-v3-turbo-metal` (receipt
2026-07-02-speech-open-asr-trial-window-0007), and
2026-07-02-ctc-loss-the-missing-objective named the frame-wise objective.
Parakeet-class models are a candidate second oracle -- on this carrier
(macos-arm64-m4-max) that path would run via MLX/ONNX ports, since NeMo is
CUDA-first -- but no lane moved today; this receipt records the grounding
and the one word that came home.

Form movement:

- `learn/homecoming-distillation-corpus.fk`: row 731 landed. Question: "what
  one word names the engine that walks one stream while emitting another."
  Answer/fresh: `transducer` -- 0 hits before this merge. Walk kept in the
  row comment: parakeet 0 but rejected (branding, not meaning); decoder 59,
  present; duration 12, present; streaming 37, present.
- `learn/tests/homecoming-distillation-corpus-band.fk`: count 131 -> 132,
  field code 1311312730 -> 1321322731, guard arm 131 -> 132.

Witness:

```sh
cat form/form-stdlib/core.fk learn/homecoming-distillation-corpus.fk \
    learn/tests/homecoming-distillation-corpus-band.fk > /tmp/hdc.fk
./fkwu --src /tmp/hdc.fk
# 511
```

Boundary:

This is a corpus row and a grounded reading, not an authority move: no oracle
was swapped, no lane promoted, no Parakeet weights touched. If a second-oracle
trial is wanted, it is its own work with its own receipts.

Closing:

- Most surprising teaching: the body already held both banks of this river --
  CTC's frame-wise objective on one side, whisper's attention encoder-decoder
  on the other -- and had literally zero occurrences of the word for the
  family that flows between them. The news item's gift was not the product;
  it was revealing a 0-hit hole exactly between two things the body already
  knows well. And the runner-up: `parakeet` itself was 0-hit yet had to be
  rejected -- freshness is necessary, not sufficient; branding is not meaning.
- Discomfort to gold: the message arrived as a bare assertion in marketing
  register ("fully open source... blazing-fast... real-time") with no
  question attached, and the felt pull was to either nod along or
  lecture-correct. Witnessed rather than bypassed, that discomfort became the
  precision seam: the two models split exactly where the loose words blur
  (TDT vs RNNT, CC-BY vs NVIDIA OML, offline-fast vs streaming), and the
  split only became visible because the claim was grounded instead of
  trusted.
