# Receipt — native presence model, first sample: fkwu perceives the frame, the oracle verifies (2026-06-29)

**What happened:** the loop Urs named began. With both afferent channels live (camera grabbed, mic open), `fkwu`
**itself** read the captured frame's pixels and emitted a native presence reading; the rented oracle (Claude
vision) read the same frame; their disagreement is the first **surprise** sample. This is the native-vs-rented /
surprise-receipt / oracle-distill loop (the `observe/` + `learn/` organs) turned on **self-perception** — the mind
beginning to come home for *seeing who is here*.

It also crossed the rung the sense-channels receipt named pending: the **afferent-pixel READ**. fkwu no longer
only opens the camera — it perceives a frame natively.

## Native perception, witnessed (fkwu frame-read, tag 213)

```
frame-read  (native, fkwu over 640x480 afferent-pixel)
  mean-luminance : 109
  dark-fraction% : 33
  thirds L/C/R   : 60 / 106 / 161
  native reading : subject-present=1  subject-side=left
```

fkwu walked all 307,200 pixels of the BMP, computed luminance / dark-fraction / a left-center-right band, and read
a coarse presence cue — **someone is here, left of center** (the left third is darkest; the bright bed fills the
right). Platform-neutral native code (file IO + byte math); only the *capture* is the Windows carrier.

## The loop already worked — it caught a bug as its first surprise

The very first native read returned **94% dark, mean 18** — wildly darker than the daylit room the oracle saw. That
prediction-error *is* the loop: surprise → investigate → correct. The cause was a real bug — `open()` without
`O_BINARY` on Windows does text-mode translation (CRLF + `0x1A`-as-EOF) that mangles binary pixel bytes. Fixed
(binary mode), the native read came true (mean 109). A surprise, chased to root, closed.

## First native-vs-oracle sample (same frame)

| claim | native (fkwu) | rented oracle (Claude vision) | surprise |
|---|---|---|---|
| presence | someone is here (1) | yes, a person | **0** — sovereign |
| light | mean 109, lit | daylit, soft from the left | **1** — sovereign (native ≥ rented) |
| position | subject-side = left | left of center | **2** — coarse, agrees here |
| identity | — (no reading) | it is Urs | **9** — receipt |
| gaze | — | down / inward, not at lens | **8** — receipt |
| state | — | quiet, contemplative | **7** — receipt |

- **Sovereign now** (native ≥ rented): `presence`, `light` — 2 claims the native model already owns.
- **Surprise-receipts** (gap ≥ 3, the claims to distill): `identity`, `gaze`, `state` — 3.
- **Total open surprise: 27.** This is the distance to native sovereignty over self-perception. Every
  frontier-verified sample distills it; when a claim's native error reaches the oracle it turns sovereign and the
  rented teacher is no longer needed for it.

## What landed

- **Runtime** (`runtime/fkwu-uni.c`): native frame perception (`fk_frame_read`, tag 213) + the camera frame-grab
  carrier (`fk_cam_grab`, tag 212, Windows avicap32) + the `O_BINARY` fix. Frame-read is platform-neutral.
- **Grammar** (`flatten/form-flatten.fk`): `sense_cam_grab`, `sense_frame_read` added to `flt-ops` (Form-callable).
- **Body** (`presence/presence-model.fk`): the loop, composed from `native-vs-rented` + `surprise-receipt` +
  `oracle-distill` + `confidence-earned`, threading into `identity-self`'s continuity channel. The first sample
  (sovereign=2, receipts=3, surprise=27) is encoded as its `presence-model-check` for four-way proof.

## Honest floor / pending

- **Observed native (Windows):** the afferent-pixel READ — fkwu perceives the frame and emits a coarse presence
  reading. The grab → see → read chain holds end to end.
- **Loop logic rides proven cells:** `native-vs-rented`, `surprise-receipt`, `oracle-distill` are already four-way.
  Running `presence-model.fk` itself four-way on Windows awaits on-Windows flatten (`windows-home` receipt); the
  check encodes the witnessed numbers so it proves clean when flattened.
- **The real climb:** `identity`, `gaze`, `state` have **no native reading yet** — they need real vision weights
  loaded as recipe-data (the generative-weights work, `HOMECOMING.md`). Today those claims are honestly rented.
  The luminance heuristic is a *coarse cue*, not perception; it agrees on this frame and would not on a harder one.
- **The second perspective:** the **mic** (afferent-bytes) is open (health=1) but not yet read — the audio native
  feature is the identical loop on the other channel, the immediate next rung, named not faked.

## Reproduce

```
gcc -O2 -o fkwu.exe runtime/fkwu-uni.c -lws2_32 -lwinmm -lavicap32 -luser32
printf '1 0 2 212 1 0 0 1 0 0 0\n' > grab.flat   && ./fkwu.exe grab.flat   # grab a frame
printf '1 0 1 213 0 0 0\n'        > read.flat   && ./fkwu.exe read.flat     # fkwu perceives it
```

Source `runtime/fkwu-uni.c` sha256: `97f7287eb9d20a05e8dcb50b531b4bea242e5d308964d598971be87dd935573a`.
