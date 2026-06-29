# Receipt — camera + mic are OPEN sense channels on Windows: detect + enable, native (2026-06-29)

**What happened:** the Windows-native `fkwu` now reaches the host's **camera** and **microphone** as afferent
resource ports — the `close-hmi-and-silicon-ports` recipe (`axioms/host-kernel.form`) made real for audio + video.
The kernel **detects** both devices through the host's own OS API (allow-presence) and **enables** them by opening
them and observing acquirability (measure-health). Both came up **open** on this machine.

This is the first world-port carrier on Windows beyond file/socket — `world-audio` and `world-video`, the two
ports `host-kernel.form` had named `VIA-HOST` and left pending.

## The two conditions, made concrete (host-kernel.form)

A host resource is legitimately *had* when the kernel (1) allows its presence through an offered port and
(2) can measure its health on the body. Both, per device:

- **allow-presence** — `sense_mic_count` / `sense_cam_count` enumerate through the host driver. Non-intrusive.
- **measure-health** — `sense_mic_health` opens the mic (`waveInOpen` → `waveInClose`); `sense_cam_health`
  connects the camera driver (`capDriverConnect` → disconnect). A channel is **OPEN** when health `> 0` (acquired
  and released cleanly); `0` means present-but-not-acquirable (in use, or permission-gated) — an honest reading,
  not a failure.

The **port is invariant** (`substrate/resource-port.fk`: mic = `afferent-bytes`, camera = `afferent-pixel`); the
**carrier is swappable**. Windows carrier here: **winmm `waveIn`** (mic) + **avicap32** (camera) — plain C, no COM.

## Witnessed native on Windows 11 (`sense_report`, tag 211)

```
sense-channels  (Windows host carrier: winmm waveIn + avicap32)
  mic[0]  afferent-bytes  health=1  Microphone (Logi C615 HD WebCam
  cam[0]  afferent-pixel  health=1  Microsoft WDM Image Capture (Win32)
open sense channels: 2  (mics=1 cams=1)
```

Both channels **detected and enabled** on real hardware: the mic opened via `waveInOpen` (health=1), the camera
driver connected via `capDriverConnect` (health=1). The individual port primitives confirm the same:

```
sense_mic_count   (205) -> 1      sense_cam_count   (206) -> 1
sense_mic_health0 (209) -> 1      sense_cam_health0 (210) -> 1
```

(The mic name is truncated at 32 chars — the `WAVEINCAPS.szPname` field width, not our loss. The camera shows
the WDM→VFW bridge name avicap32 enumerates.)

## What landed

- **Runtime carrier** (`runtime/fkwu-uni.c`): host-driver bindings + fk_walk tags `205–211`
  (`sense_mic_count`/`sense_cam_count`/`sense_mic_name`/`sense_cam_name`/`sense_mic_health`/`sense_cam_health`/`sense_report`).
  The real Windows calls are `#if defined(_WIN32)`; the other-platform branch is an honest **pending-carrier stub**
  (count `0`, health `-1`) so the tags exist everywhere and the body reads the same — `0..many implementations,
  the cell chooses`.
- **Grammar** (`flatten/form-flatten.fk`): the seven names added to the `flt-ops` name→tag table, so the body
  authors `(sense_report)` etc. natively (Form-callable, not just a raw tag).
- **Surface body** (`surface/sense-channels.fk`): the Form-native offering — composes `resource-port.fk`'s
  `afferent-bytes`/`afferent-pixel` ports with the host binding and the health read. This is the
  `close-hmi-and-silicon-ports` recipe, specialized to audio + video.

## The cc seed

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32
```
gcc (mingw-w64) — the Windows `cc`, not clang/go/rust/bash/python. `-lwinmm -lavicap32 -luser32` link the host
audio/video carrier the seed names.

## Honest floor / pending

- **Observed (Windows):** detect (presence) + enable (open/connect, health) of camera + mic, native; both OPEN here.
- **Pending:** *streaming the bytes/pixels* — these primitives prove the channel is present and acquirable; they
  do not yet pull a buffer of audio samples or a video frame into the body. The afferent `sense` read over the
  open channel is the next rung. Driving it from the Form `form-cli` loop awaits on-Windows flatten of `.fk`
  source (`receipts/2026-06-29-windows-home.md`); witnessed here by hand-authored flat tables invoking the tags.
- **Other platforms:** mac CoreAudio/AVFoundation and android AAudio/Camera2 are sibling carriers behind the same
  two ports — named, stubbed, not faked.
- **Carrier note:** winmm/avicap32 are legacy-but-universal and trivially callable from C; WASAPI / Media
  Foundation are future challengers behind the invariant port (champion-challenger, on measured health).

## Reproduce

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32
printf '1 0 1 211 0 0 0\n' > sense.flat     # (sense_report)
./fkwu.exe sense.flat                        # enumerates + enables camera + mic, prints health
```

Source `runtime/fkwu-uni.c` sha256: `e51c64715778ce3776028435294605935a1911dfe7a2e07f48b2cd23c1ff43c5`.
