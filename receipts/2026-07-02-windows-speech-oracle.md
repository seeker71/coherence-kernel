# Receipt — the Windows cell's first local speech oracle: "open speech flows", WER 0 (2026-07-02)

Same cell (HP Spectre, Windows amd64). The macOS teacher-carrier pattern (`say -> whisper.cpp`) lands on
this metal, fully local, Form-orchestrated end to end:

```
(windows-speech-oracle)
oracle-heard:[open speech flows] wer=0 route=oracle
255
```

**~5 seconds, one `fkwu --src` run** (`presence/windows-speech-oracle-carrier.fk`): the Form body wrote
the TTS payload itself (`write_file_text` to `C:/tmp` — outside the tree; the recipe is the author),
host-exec'd PowerShell `System.Speech` to render the spoken truth at 16kHz mono PCM, host-exec'd
**whisper.cpp base.en** (`C:/models/whisper`, v1.9.1 CPU build + ggml-base.en.bin — no cloud, no
network at ask time), tokenized the transcript through its own byte-level normalizer, and lowered the
row through the real carrier law.

## What changed on this cell, in the law's own terms

The capability row now says **oracle=1** — and for the first time on this cell `slr-ready?` passes:
receipts are **valid** (local + audio-present), controls **clean**, oracle WER **0**, oracle-success
under the band threshold. And the window still routes **"oracle"** — because the native lane has zero
successes, and the law will not hand it credit. This is the honest ladder's exact middle rung:
oracle-guided with valid rows, where before there was only control debt.

## Boundaries, named

- The route to `"native"` needs the native lane (closed-set envelope reads, the formant/prompt organs)
  to earn successes against this oracle — the same road every macOS/Android lane walked.
- The membrane's `speech-carrier=0` for windows-amd64 **still stands**: the full carrier contract is
  render+capture+oracle composed through the OS audio path as one organ; today's lane witnesses the
  oracle leg (file loopback), the tone carrier witnesses render+capture (air loopback, 231/255 by
  volume). Composing them — spoken truth through the speakers, captured by the mic, transcribed by the
  oracle — is the named next rung, and the day the flip is earned, the band law changes with it.
- The cell is self-contained over native byte ops: the C seed's function table cannot hold this lane
  plus `core.fk` in one `--src` run (the known mega-concat boundary — witnessed again as a silent
  `nothing` before slimming; the ceiling itself is C-seed shrink work, named).
- Whisper's cuBLAS build exists in the same release (the oracle could ride this cell's RTX); CPU
  base.en is sufficient at this phrase length and was chosen for the smaller honest footprint.

## Reproduce

```
# once: C:/models/whisper <- whisper.cpp v1.9.1 whisper-bin-x64.zip (Release/) + ggml-base.en.bin (HF)
{ cat form/form-stdlib/hati-os-targets.fk form/form-stdlib/host-os-membrane.fk observe/stt-wer.fk \
      learn/speech-loopback-promotion.fk presence/speech-loopback-carrier-receipt.fk \
      presence/speech-loopback-carrier-run.fk presence/windows-speech-oracle-carrier.fk; \
  echo '(windows-speech-oracle)'; } > wso.fk
./fkwu.exe --src wso.fk    # -> oracle-heard:[open speech flows] wer=0 route=oracle / 255
```
