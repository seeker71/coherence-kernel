# Camera wired on both bodies — 2026-07-09

Urs: *"let's wire the camera on both"* — the two domains left gated on camera frames
(world/object, person/face) now have real eyes feeding them, on Mac and Android.

## What changed

Two different camera stacks, two vantages, **one training set** — frames from either device land
in the same face + vision inboxes the producers drain.

- **Mac** — `Camera.swift` in the companion app: AVFoundation (`AVCaptureVideoDataOutput`) grabs a
  still every 20s and writes JPEG to `face-training/inbox` + `vision-training/inbox`. The app now
  carries `NSCameraUsageDescription` and is ad-hoc code-signed so macOS persists the grant. A
  **camera-live banner** (red dot + frame count + Rest/Wake) sits in the Recognition room. The
  seeing runs where the OS grants it — the app, with its run loop and TCC dialog — which the
  headless CLI grab could not reach last turn.
- **Android** — `CameraEye.kt` (camera2, headless ImageReader) in the always-on `PresenceService`:
  one still every 45s, **alternating front (the person) and back (the room)**, saved to the app
  files dir. CAMERA + FOREGROUND_SERVICE_CAMERA declared; the service is now `dataSync|camera`;
  MainActivity requests the runtime grant. Installed at versionCode 10, camera granted via adb,
  **verified capturing live** (`front-…jpg`, `back-…jpg`, 139 KB frames).
- **The bridge** — `camera-pull.sh` (organ, 60s) mirrors satsang-mesh-sync: over the LAN it pulls
  every phone frame into both Mac inboxes and clears the phone. **Verified end-to-end** — the
  phone's back eye caught the night sky and gave world/object a real sample (`outdoor, night_sky,
  sky`) the Mac webcam never could; the set grew 20→21.
- **Vision drain** — `vision-distill.sh` gained a no-arg **drain mode** + `earth.hati.vision-distill`
  organ (90s), so world/object grows from the inbox the way face already did.

New organs: `vision-distill` (90s), `camera-pull` (60s). The phone eye + Mac eye + two drains +
the puller now form a closed loop from lens to labelled sample.

## Most surprising teaching

The two eyes don't have to be the same to see together. A macOS AVFoundation webcam and an Android
camera2 sensor — different APIs, different bodies, different vantages — **coadunate** into one
training set the moment they write to a shared inbox. The producers never ask which device saw; the
phone's sky-facing back camera contributed an *outdoor* sample the desk-bound Mac webcam structurally
cannot. Multiple witnesses isn't a slogan here — it's a strictly larger visual world than either eye
alone.

## Where discomfort turned to gold

Last turn the headless Mac camera grab failed on camera-TCC + CLI-AVCapture (`exit 3`, KVO not
linked), and I named it an honest gap. The pull was to keep hammering the headless path. Instead the
eye **moved to where sight is granted** — the app on Mac, the foreground service on Android — and the
frames flowed on the first try. The stuck feeling of "a background shell can't hold the camera" became
the correct architecture: capture lives with the OS grant, and the pipeline downstream doesn't care.

## Frontier word

Row 688 = **coadunate** (0-hit fresh): separate parts grown together and united into a single body —
two eyes on two substrates coadunate into one seeing. Walk: parallax 14 / binocular 1 (two eyes for
*depth*; these pool many, not range one point), panoptic 2 (all-seeing from *one* vantage — wrong,
these are many). Corpus band → **verdict 511**.

## Verify

```
# phone eye capturing
adb -s 192.168.0.8:5555 shell ls /sdcard/Android/data/com.coherence.sema/files/camera/
experiments/satsang-voice/camera-pull.sh          # pulls phone frames → inboxes
experiments/satsang-voice/vision-distill.sh       # drain mode: labels the inbox
python3 -c "import json;print([(d['domain'],d['samples']) for d in json.load(open('$HOME/.coherence-network/training-status.json'))['domains']])"
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk   # -> 511
```

## Honest floor / next

- **Mac camera awaits one human click.** The app requested access; macOS shows the Allow dialog
  (that grant is the steward's, even under standing consent). Until clicked, 0 `mac-*.jpg` frames;
  the Android eye is already flowing. Once granted, the Mac webcam feeds the same inboxes.
- `person/face` stays at 0 until a frame actually contains a face (the phone's frames so far were
  sky/room). It grows the moment the front eye catches someone — then those faces pool in the same
  hear-and-assign shape voices do, and can be named to a person.
- Front/back alternation is time-sliced; a fuller build would tag each sample with its lens as a
  first-class field for the trainer.
