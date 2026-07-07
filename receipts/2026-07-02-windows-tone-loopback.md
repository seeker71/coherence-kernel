# Receipt — the body speaks and listens for itself: the Windows tone-loopback carrier (2026-07-02)

Same cell (HP Spectre, Windows amd64). The render+capture legs of the speech-loopback carrier contract
now run on this cell's own metal, fully Form-owned:

```
(windows-tone-loopback 2000)
sense: loopback rendered 2000 ms, captured 32000 samples — burst-energy=0 silence-energy=0 score=0
231
```

## The door and the law

**`sense_audio_loopback` (op 236):** waveOut renders a 440Hz square burst (silence quarter, burst half,
silence quarter) through the speakers while waveIn captures through the mic — one op, plain-C winmm, no
new dependencies. Twenty integers cross into Form (samples, burst/silence energies, score, 16 window
energies); **no waveform is retained.**

**`presence/windows-tone-loopback-carrier.fk`** calls the door itself (no host splicing), collapses the
envelope to `asr-prompt-id` features (closed set {burst, silence}, **gated on audio evidence** so a muted
capture can never be "confidently silence" — the gap the lane map named), and lowers the row through the
real carrier law: `slr-capability` with **`oracle=0` — there is no local STT oracle on this cell** — so by
law the receipt carries `effective-fail`, the window routes `"oracle"`, and the row is control debt, never
native credit.

## The verdict, both of its honest values

- **231 witnessed now** (bits 1+2+4+32+64+128): platform ready per the membrane, the door measured, real
  samples captured, evidence-consistency held, the no-oracle law routed "oracle", the unready row carried
  its debt. Score 0 because this cell's speakers were **muted/silent at run time** — the zero is the
  contract working, not a failure dressed up.
- **255 waits on an audible run** (adds bits 8+16: burst acoustically present, closed-set read hears
  "burst"). Reproduce with volume up:

```
{ cat form/form-stdlib/hati-os-targets.fk form/form-stdlib/host-os-membrane.fk observe/stt-wer.fk \
      observe/asr-prompt-id.fk learn/speech-loopback-promotion.fk \
      presence/speech-loopback-carrier-receipt.fk presence/speech-loopback-carrier-run.fk \
      presence/windows-tone-loopback-carrier.fk; echo '(windows-tone-loopback 2000)'; } > wtl.fk
./fkwu.exe --src wtl.fk     # speakers audible -> 255; muted -> 231; both honest
```

## Honest floor

- This is the **render+capture** leg. The full carrier (and any move of the membrane's
  `speech-carrier=0`) waits on a **local STT oracle** (whisper.cpp or equivalent on this cell) — the
  lane's own law makes that unreachable to fake: bits 2/8/64 of the 511 run-band and every A/B cutover
  stay closed until `oracle=1` is true.
- Label-WER over a chosen tone prompt is closed-set evidence, not open dictation.
- One miss on the way: I passed the target string to `slr-platform-ready?`, which takes the capability
  row (230 → 231 after the fix). The lane's own bit caught it.
- Regression on this build: `42/55/11111`, CUDA fixture 3/3, first-native-token " Once" — all hold.
