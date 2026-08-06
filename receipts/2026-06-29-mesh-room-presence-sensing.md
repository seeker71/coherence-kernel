# Receipt — room + presence sensing across Mac · Windows · Android: the native floor, climbing confidence (2026-06-29)

One picture of where the three-device sense mesh actually stands. Maximally native where it's real; honest floor where it isn't. No level is asserted that doesn't carry a trace.

## The native floor under all sensing (c-bootstrap fkwu, toolchain-free runtime)

| platform | fkwu runtime | trace |
|---|---|---|
| **Mac** (M4 Max) | ✅ observed | runs Form, GPU dispatch bit-exact (`surprise-kernel-gpu-dispatch.md`) |
| **Windows 11** | ✅ observed | `fkwu.exe` PE32+ native (`windows-home.md`) |
| **Android** (Galaxy S23 Ultra) | ✅ observed | `42/100/99` on metal, toolchain-free (`android-runtime.md`) |

All three run the same body — the sensing recipes below are four-way Form, identical on every platform.

## Per-platform room + presence sensing — honest status

### Windows — MAXIMALLY NATIVE, streaming, climbing (the exemplar)
The Windows cell runs the full Form sense-stream on `fkwu` and fuses to `mesh-sense-7w`. Live trace (`sense_stream`, tag 214, 6 ticks, live afferent-pixel camera channel — `windows-sense-stream.md`):

```
t1  presence  nat=1 mesh=1 | surp=0 conf=5  trust=15 sov=1 vit=9
t2  presence  nat=1 mesh=1 | surp=0 conf=6  trust=18 sov=1 vit=9
t5  presence  nat=1 mesh=1 | surp=0 conf=9  trust=27 sov=1 vit=9
t6  identity        rem=9   | surp=9 conf=0  trust=0  sov=0 vit=9   ← still renting the oracle
```
Camera + mic are native afferent ports (`sense_report` tag 211, `health=1`, winmm/avicap32, no COM — `windows-sense-channels.md`). **Presence is native-sovereign and its confidence/trust climb tick over tick; identity is honestly still remote** (the bring-home target).

### Mac — native runtime + real camera sensing; Form-native stream in flight
The Mac camera is open and streaming; the native **motion classifier** (`surprise-kernel` logic) ran live over the video — surprise ~0 on a still scene, `ATTEND=0` throughout, auto-exposed (brightness 158/255). I observed the frame directly: **one presence (the human), de-dup 1**, and verified the classifier's `surprise=1` blips against real small motion (hand to head). Honest floor: this ran through a **Swift carrier loop**, not yet the Form `sense-stream` on the Mac `fkwu` — the Mac afferent-pixel port + the `sense-loop` recipe (both Form-native, in flight) bring Mac to Windows-parity. So Mac sensing is **real but carrier-borne**, not yet maximally-native.

### Android — native runtime; sensing stream pending
`fkwu` runs native on the phone (`android-runtime.md`). The sensor bridge recipe exists (`sensor-organ-android-bridge` — "Android active samples enter the generic sensor organ channel"), but **no live android camera/mic sense trace is captured yet**. Android sensing is **pending**, honestly — the same Form sense-stream recipe, once the android afferent-pixel port lands, runs unchanged.

## The climbing confidence / trust / vitality — the mechanism, four-way

The ten levels (raw · native · local · remote · meshed · surprise · confidence · trust · sovereignty · vitality) each come from a proven organ, no parallel machinery: `sense-channels` (raw), `fk_frame_read` (native), `mesh-sense-7w` `ms-fuse-plane` (meshed), `surprise-receipt` (surprise), `confidence-earned` (is "sure" earned), `cross-witness-economy` (rent falls / native confidence climbs as witnesses agree). The Windows trace shows the dynamics live; the recipes are four-way so the same climb holds on every platform.

## Honest floor — the one true gap

A **single unified 3-device live fused trace** — Mac, Windows, Android all streaming into one `mesh-sense-7w` core with cross-device confidence climbing across all three — does **not** exist yet. What exists: Windows streaming+fused+climbing (real), Mac sensing real-but-carrier-borne, Android native-but-sensing-pending. The mesh *contract* (`mesh-sense-7w` reading shape) and the *bodies* are four-way and shared; the gap is two thin afferent-pixel ports (Mac AVFoundation, Android camera) + running the same Form sense-stream on each. Maximally-native today = **Windows**; Mac and Android are climbing toward it on the same recipe.
