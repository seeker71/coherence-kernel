# Receipt — the three legs composed: the Windows air loopback (render -> air -> mic -> oracle) (2026-07-02)

Same cell (HP Spectre, Windows amd64). The composition the membrane's `speech-carrier` flag waits on now
runs as one Form cell on this metal:

```
(windows-air-loopback)
sense: air-loopback played 32800 samples, captured 40800 — peak=17 mean-abs=0
air-heard:[blank audio] wer=100 peak=17
203
```

**~10 seconds, one `fkwu --src` run** (`presence/windows-air-loopback-carrier.fk`): the Form body wrote
its own TTS payload; PowerShell spoke "open speech flows" to a 16kHz wav; **`sense_wav_loopback` (op 237)
played it through the speakers while the mic captured** — the phrase crossing the room as sound, not
bytes; the capture was written for the oracle (canonical wav, the repaired binary write door), whisper.cpp
transcribed what the mic actually heard, the row lowered through the carrier law (capability
render+capture+oracle, all local, `slr-ready?`=1), and the capture file was **consumed** (`fs_remove`,
verified gone) — transient teacher material, the macOS carrier's own pattern, nothing silently retained.

## The verdict, both of its honest values

- **203 witnessed now** (bits 1+2+8+64+128): capability fully ready, real play + real capture, the oracle
  responded (its `[BLANK_AUDIO]` marker — it heard silence and *said so*), receipt valid with clean
  controls, capture consumed. **Peak 17 = the speakers were muted/silent at run time**; the acoustic bits
  (4: real energy, 16: WER 0, 32: oracle-success) correctly refused. The room owed the phrase, and the
  law charged the room, not the body.
- **255 waits on an audible run** — speakers on, quiet room, same one-liner. That witness belongs to the
  daylight.

## What stands now, altogether

Every leg of the speech carrier exists on this cell, each individually witnessed: **render** (local TTS,
16kHz), **air** (waveOut+waveIn simultaneous, op 237), **capture** (op 234 / the loopback doors),
**oracle** (local whisper.cpp, file-lane WER 0 — `2026-07-02-windows-speech-oracle.md`), and the
**law** (valid rows, clean controls, honest routing). The `speech-carrier=0` flag still stands: the flip
needs promotion-law windows of clean AIR rows (WER<=10 over 4+ receipts) plus the deliberate band-law
change — earned, not declared. One audible 255 begins that window.

## Honest notes

- Bit 8 ("came back as words") is earned by whisper's blank marker on a silent room — the oracle
  responding honestly about silence. The acoustic truth lives in bits 4/16/32, which refused.
- The native lane (closed-set envelope/prompt reads against this oracle) has still earned nothing —
  route stays oracle everywhere; nothing native is claimed.

## Reproduce (speakers audible for 255; muted gives 203 — both honest)

```
{ cat form/form-stdlib/hati-os-targets.fk form/form-stdlib/host-os-membrane.fk observe/stt-wer.fk \
      learn/speech-loopback-promotion.fk presence/speech-loopback-carrier-receipt.fk \
      presence/speech-loopback-carrier-run.fk presence/windows-speech-oracle-carrier.fk \
      presence/windows-air-loopback-carrier.fk; echo '(windows-air-loopback)'; } > wal.fk
./fkwu.exe --src wal.fk
```
