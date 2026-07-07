# Receipt — the Windows cell's eye and ear open: MF camera + waveIn mic capture, measured not retained (2026-07-02)

Same cell (HP Spectre, Windows amd64). Asked directly — "can you add support for my camera and mic on this
laptop?" — the standing "don't" was re-read honestly: yesterday's refusal was *don't build the camera as a
side-effect*; an explicit ask IS the deliberate movement. Both carriers landed, witnessed live:

```
(sense_mic_capture 1500)
sense: mic captured 24000 samples (1500 ms) nonzero=5538 mean-abs=0 peak=2 — measured, not retained

(sense_cam_luma 15000)
sense: camera frame 640x360 mean-luma=5 nonzero=227601 — measured, not retained
```

24,000 samples in 1500ms is exactly the requested 16kHz — the ear is live (a quiet room's noise floor:
peak 2). The eye returned a real 640x360 NV12 frame — mean luma 5 because the room was dark, but 227,601
of 230,400 Y-pixels nonzero: the sensor sees. Both ops return integer measurements into Form
(`(samples nonzero mean-abs peak)` / `(w h mean-luma nonzero)`); **no audio bytes or image bytes are
retained** — the Android capture receipt's law, kept.

## The carriers

- **Ear — `sense_mic_capture` (op 234):** completes the existing plain-C winmm carrier (the seed already
  enumerated mics via waveIn): open PCM16 mono 16kHz, one buffer, capture, measure, free. No new
  dependency.
- **Eye — `sense_cam_luma` (op 235):** the Media Foundation carrier the hanging VfW shim demanded
  (`2026-07-01-windows-camera-carrier-probe.md`). Built under the same door discipline as the CUDA
  carrier: **LoadLibrary-only** (`ole32`/`mfplat`/`mf`/`mfreadwrite`), COM vtables called by slot index in
  plain C — zero new link libraries, the documented one-`cc` build line untouched. Asks the source reader
  for NV12 (Y plane first, any native format converted), reads one sample, measures luma, tears down.
  Runs on a bounded worker thread (caller-supplied timeout); a Windows camera-privacy denial prints its
  own honest message (`hr=0x80070005`).

Ops went through the ONE table (`flt-ops`) and the pure-Form regen — which post-shrink needs
`core.fk` catted before both passes (`str_find`/`int_to_str` now live in Form; the regen recipes use
them). Header diff: exactly the two new rows.

## Two honest misses on the way, both mine

1. First camera run refused with `MF_E_INVALIDSTREAMNUMBER`: I had invented `0xFFFFFFF4` for
   `MF_SOURCE_READER_FIRST_VIDEO_STREAM`; the real constant is `0xFFFFFFFC`. The refusal code named it.
2. First optable regen returned `-1` twice: post-shrink, the regen recipes themselves depend on the
   Form string ops — the passes need the `core.fk` prelude now. Named here so the next regen doesn't
   stumble.

## Honest floor

- `sense_report` / `sense_cam_health` still answer for the **legacy VfW carrier** (honest 0 + timeout
  note); pointing health at the MF carrier is the named next tidy.
- Capture is ONE leg of the speech-loopback carrier contract (render + capture + oracle); the membrane's
  `windows-amd64` row keeps `speech-carrier=0` honestly — the band law (bit 4096) still holds. The ear
  arriving is real progress toward it, not the carrier itself.
- Luma-only vision: no frames stored, no image files written. Retention, if ever wanted, is a separate
  consent and a separate receipt.
- Whole-cell regression on this build: `42 / 55 / 11111`, CUDA fixture 3/3, first-native-token " Once",
  Paris ask 8/8 — all hold.

## Reproduce

```
printf '(sense_mic_capture 1500)' > m.fk && ./fkwu.exe --src m.fk
printf '(sense_cam_luma 15000)'  > c.fk && ./fkwu.exe --src c.fk
```
