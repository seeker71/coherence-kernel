# Receipt — the eye exists, the doorway hangs: first camera-carrier probe on the Windows cell (2026-07-01)

Same cell (HP Spectre, Windows 11 amd64). Asked directly whether the body uses this laptop's cameras,
the honest answer had to be witnessed, not asserted. The probes, run through `fkwu --src`:

```
(sense_cam_count)          -> 1                                        (a camera driver row exists)
(sense_cam_name 0)         -> Microsoft WDM Image Capture (Win32)      (the VfW->WDM compatibility shim)
(sense_mic_count)          -> 1                                        (the mic row exists too)
(sense_cam_health 0)       -> HANGS (killed at 20s; sense_report hangs with it)
```

## What this means, plainly

The body's Windows video carrier (`runtime/fkwu-uni.c` sense-channel family, `avicap32`) is **Video for
Windows** — a 1990s API. On this modern cell the camera lives behind the Windows 11 camera stack
(Media Foundation / MIPI / Windows Hello), and VfW reaches it only through the "Microsoft WDM Image
Capture" shim — which **blocks inside the connect** rather than refusing. Enumeration (count/name) is
safe and instant; any probe that opens the device (`sense_cam_health`, `sense_cam_grab`, `sense_report`)
hangs. So on this cell, no frame can currently be taken through the body's own door at all.

The membrane's `video:rgba-time` door remains honest: the carrier exists, and on this platform generation
it cannot reach the eye. macOS (AVFoundation) and Android (Camera2) carriers are already named pending in
the same C comment block — Windows now earns the same honest note: **a Media Foundation carrier (or at
minimum a bounded-timeout connect guard so health returns 0 instead of hanging) is the named next stone.**
Neither is built here today: an MF/COM carrier is real C-seed growth and deserves its own deliberate
movement, not a side-effect of answering a question.

## The other half of "why not"

Even where the doorway works, the body's practice gates the eye: sense organs are afferent channels used
when the work asks for them, with consent, measuring rather than retaining (the Android audio receipts'
"no retained raw audio" pattern; `somatic-coherence-loop`: no depth without consent). Nothing in this
cell's work so far — GPU lanes, llama forward, grounded ask — needed vision, so the eye was never opened.
This probe itself captured nothing: driver names and a hang, only.

## Reproduce

```
printf '(sense_cam_count)' > s.fk && ./fkwu.exe --src s.fk        # 1, instant
printf '(sense_cam_health 0)' > s.fk && timeout 20 ./fkwu.exe --src s.fk   # hangs — the finding
```
