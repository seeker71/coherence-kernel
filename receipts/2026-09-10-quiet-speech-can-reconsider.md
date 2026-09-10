# Quiet speech can reconsider

Codex, 2026-09-10.

The live ear's measured room estimate was 178 and its old onset gate 712;
Glass's ear frame was 96 ms old, with one worker admission. This reading named
the real gate, not an assumed model limitation. The current worktree's hearth
answered `signal=nothing reason=no-standing-hearth`; local source, local model
experiments, and attributed public research supplied the missing guidance.

The native listening policy now admits sustained soft candidates, keeps one
second of onset context, uses bounded window gain in the existing spectrogram
dispatch, and leaves the entire open phrase revisable. Decoder closure also
checks actual microphone quiet. Room RMS is unamplified. No C seed growth, no
foreign runtime, no new model, no private audio copied into the receipt.

Development fixture, native Whisper-tiny: original/4x-quieter/16x-quieter open
phrase exact matches moved from 1/3 to 3/3. The 64x-quieter row still fails.
Passes measured 11–17 ms in the latest run, excluding test-fixture construction.
This is not multilingual held-out WER or proof of final-commit accuracy; the
unchanged final word-confidence threshold can still refuse these open phrases.
Adding trailing silence revealed another real limitation: position sensitivity
in the tiny model. Details and reproduction are in
[ear-quiet-speech](../docs/ear-quiet-speech.md).

The useful surprise was that deterministic noise fooled the model's no-speech
probability even without gain. The actual onset and word-confidence checks
refuse it, and its gain remains unity. The failed noise assertion became an
explicit test of those separate defenses, not a claim that loudness is speech.
Bounded framebuffer exchanges compare each attenuation row before/after the
applied gain, without private text.

Witnesses: listening policy 4194303; physical quality 255; unchanged native
encoder/model pins 32767; Glass axes 131071; transcript flow 524287; display
slots 8191; body-channel isolation 4095; drift gates 8191/8191.

The exchange stayed alive by converting a quiet-voice report into measured
audio-path changes. A model's confident noise was the discomfort that made
the listening policy more honest. NVIDIA's cached streaming architecture is
useful guidance, not a native model port we have already completed.
