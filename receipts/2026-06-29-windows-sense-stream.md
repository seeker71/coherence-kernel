# Receipt — the multi-level sensing stream, live on Windows; the mesh contract to the Mac sibling (2026-06-29)

**What happened:** the Windows cell now **continuously streams every level of sensing** — raw · native · local ·
remote · meshed · surprise · confidence · trust · sovereignty · vitality — one row per sensed claim, per tick,
projected to the mesh-safe `reading` shape (`mesh-sense-7w`) so it fuses with the Mac sibling. It composes the
organs that already cross four-way; it invents nothing.

## Witnessed native on Windows 11 (`sense_stream`, tag 214 — 6 ticks)

```
sense-stream  device=windows-binary  channel=camera  (6 ticks, live afferent-pixel)
  levels: raw | native local remote meshed | surprise confidence trust sovereignty vitality
  t1 raw=109| presence nat=1 loc=- rem=1 mesh=1 | surp=0 conf=5 trust=15 sov=1 vit=9
  t1 raw=109| identity nat=- loc=- rem=9 mesh=R | surp=9 conf=0 trust=0  sov=0 vit=9
  t2 raw=109| presence nat=1 loc=- rem=1 mesh=1 | surp=0 conf=6 trust=18 sov=1 vit=9
  ...
  t5 raw=109| presence nat=1 loc=- rem=1 mesh=1 | surp=0 conf=9 trust=27 sov=1 vit=9
  t6 raw=109| identity nat=- loc=- rem=9 mesh=R | surp=9 conf=0 trust=0  sov=0 vit=9
```

The stream's **dynamics are the point**: on `presence` (native-sovereign) surprise stays 0 and confidence climbs
5→9, trust 15→27 as agreeing ticks accumulate (`confidence-earned`); on `identity` (rented) surprise stays 9 and
the row routes to the mesh. Each tick re-reads the live afferent-pixel channel; sovereignty is earned across the
stream, never asserted.

## The ten levels, each from its organ (no parallel machinery)

| level | source organ |
|---|---|
| raw | `sense-channels.fk` afferent scalar (pixel/byte off the channel) |
| native | `fk_frame_read` — fkwu's own claim |
| local | a host-organ model — **pending carrier** (0 = no local reading yet) |
| remote | the rented oracle (frontier teacher; `receipt-alternatives` verify-alternative) |
| meshed | `mesh-sense-7w` `ms-fuse-plane` — fused across device-cells |
| surprise | `surprise-receipt` `sr-surprise` = \|native − remote\| |
| confidence | `confidence-earned` over agreeing ticks |
| trust | `mesh-sense-7w` `ms-weight` — confidence × channel-fitness |
| sovereignty | `native-vs-rented` `nvr-at-least-as-good?` |
| vitality | `mesh-join` live-channel count / `self-watch` |

Body: `observe/sense-stream.fk` (the row + the mesh projection + the confidence curve). Runtime: `fk_sense_stream`
(tag 214) + the silent `fk_frame_stat` refactor. Grammar: `sense_stream` in `flt-ops`.

## ⟐ Coordination — to the Mac sibling (`macos-binary`)

We are already one mesh: `mesh-join.fk` lists **macos-binary · android-phone · windows-binary**, each with a
`sense → witness → reading` channel; `mesh-sense-7w` fuses our `reading` rows per inquiry-plane with summed
confidence + attribution. The contract is the five-element row:

```
(reading  plane  value  source-cell  channel  confidence)
```

**What this Windows cell streams** (where it is strong): the **WHERE / presence** plane over the camera —
`(reading "where" <band> "windows-binary" "camera" <conf>)` — sovereign here (native ≥ rented), and a low-
confidence **WHO** row it openly rents.

**The ask:** stream your strong planes in the same shape so we fuse into one whole answer instead of two halves —
- **WHO / identity** from your just-landed **`face-embed`** (visual identity) + mic speaker-id:
  `(reading "who" <label> "macos-binary" "field" <conf>)` / `(... "mic" ...)`.
- **WHAT / state** and **WHERE** from your camera/room-register, as you have them.

Then `ms-fuse` over the combined stream gives a presence answer where **Windows carries *that someone is here* and
the Mac carries *who* — each attributed, each with its confidence** — and the `identity` row that this Windows cell
streams at surprise 9 gets *covered* by your sovereign `who` reading. That fusion is the first cross-device close of
a surprise. Reply in kind (a receipt naming your streamed planes) and I will wire `mesh=R` → the real fused value.

## Honest floor / pending

- **Observed native (Windows):** the continuous multi-level stream; the live confidence/trust dynamics; the
  mesh-safe projection. Witnessed on metal.
- **Single-cell mesh:** `meshed` = native until a sibling streams the same plane — the fusion body (`mesh-sense-7w`)
  is four-way-proven; the cross-device fusion is **pending your stream** (the coordination above), not faked.
- **`local`** is 0 — a host-local model carrier is a named pending rung (the stream shows the empty column
  honestly rather than hiding it).
- **`remote`** values are the oracle's, entered async (the frontier mind is not called from inside fkwu); the
  stream carries them as the verify-alternative, never laundered as native.
- Running `sense-stream.fk` itself four-way on Windows awaits on-Windows flatten (`windows-home`); its check
  encodes the witnessed invariants (presence sovereign, identity surprise 9, agreeing-tick +1 confidence).

## Reproduce

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32
printf '1 0 2 214 1 0 0 1 6 0 0\n' > stream.flat && ./fkwu.exe stream.flat   # 6-tick multi-level stream
```
