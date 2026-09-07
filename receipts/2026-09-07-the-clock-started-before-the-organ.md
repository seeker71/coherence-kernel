# The clock started before the organ

The ear's own axes said `ear.encode` 19–28 ms. The pass measured alone says 3.3 ms for the same
8 s window. Two readings of one thing, five to tenfold apart, and nobody had said where it went.

It does not go into the encoder. It goes into carrying the window to it.

## Where the gap is

`el-hear` in the live lane starts its clock here:

```
(let t0 (now_unix_ms))
(let Te (enw-encode st (el-front (el-cat win))))
(let t1 (now_unix_ms))
```

`t1 - t0` is what becomes `ear.encode`. But `el-cat` and `el-front` are inside it. The live lane
holds its window as the mic's own chunks — eighty strings of 3200 bytes, oldest first — and
`el-cat` joins them right-associated, so the 80th chunk is copied once and the first is copied
into a 256 kB result: 3200 × (1+2+…+80) = **10.4 MB copied to hand the device 256 kB**.

Measured on this Mac, warm, twenty repetitions each, on the live lane's own window shape
(`observe/ear-gap-probe.fk`), with the machine stamped by `observe/floor-lens-run.fk` at
**314.52 GB/s and 25.77 TFLOPS — "a quiet machine"**:

| | per call |
|---|---|
| `el-cat` — join 80 chunks | **19.45 ms** |
| `el-front` — pad, when the window is already full | 0.00 ms |
| `metal_buf_write` — 256 kB into the device | 0.00 ms |
| `enw-encode` — the encoder alone | **3.65 ms** |
| what `el-hear` times and calls encode | **23.45 ms** |

23.45 = 19.45 + 3.65. Five sixths of the ear's `encode` axis was never the encoder. The pass was
at its floor the whole time; the axis was reading the plumbing with the organ's name on it.

## The second cost, which was larger

The bytes have to land in the device buffer regardless, so `enw-encode-parts` takes the window
still in pieces and writes each one there at its own offset. No string is built. `enw-encode`
keeps its shape and now stands on `enw-encode-at`, which runs the encoder over bytes already
resident.

That closed the full window — and then the *short* window, the one the lane has while its first
8 s are still arriving, still cost 52 ms on both doors. The front padding, not the join.

```
enw-zeros 192000 bytes   44.25 ms
the same bytes as 3200-byte blocks   0.00 ms
```

44 ms to make 192 kB of silence. `observe/ear-string-probe.fk` asked which door:

| | per call |
|---|---|
| doubling `str_concat` up to 256 kB | 0.90 ms |
| one `str_concat`, 128k + 128k | 0.50 ms |
| `str_len` of a 256 kB string | 0.00 ms |
| **`substring`, 192 kB out of 256 kB** | **43.35 ms** |

`str_concat` runs at ~512 MB/s. `substring` runs at 4.4 MB/s — ninety times slower for the same
bytes. It is not a native: `core.fk` defines it as `fstr-substring-halve`, which halves the range
down to single bytes and concatenates back up. 192 kB is roughly 384 000 interpreted calls, and
43 ms across them is about 110 ns each, which is what an interpreter costs. The door is honest;
it is simply a recipe standing where a slice belongs.

So the padding is now whole 4096-byte blocks of silence written into the buffer, the last one
free to run past the pad because the audio is written after it and covers the overshoot. No large
string is cut anywhere on the path.

## The room, measured against itself

The two doors run on the **same** window, in the same hop, with the mic open
(`observe/ear-live-gap-probe.fk`), while the body's own mouth spoke into the room
(`observe/say-run.fk`, en) and its own ear wrote the words back down. Before and after are not
two rooms; they are one hop measured twice.

```
hop 0    joined=68ms  pieces=5ms  decode=6ms   ntok=4   win=484ms   [MUSIC]
hop 23   joined=45ms  pieces=5ms  decode=7ms   ntok=6   win=2784ms  The body observed for body observed
hop 59   joined=33ms  pieces=4ms  decode=18ms  ntok=24  win=6384ms  The body observed the glass, water and ice and gas. …
hop 95   joined=20ms  pieces=4ms  decode=20ms  ntok=24  win=8000ms  … water and ice and gas. Yeah, better, better. The witness told what stands,
hop 155  joined=21ms  pieces=5ms  decode=20ms  ntok=24  win=8000ms  The witness told what stands and the note folded. I was dancing for two hours. …
hops=293 over 30 s
```

- **Full 8 s window:** `joined` 19–28 ms → `pieces` 3–8 ms. The 19–28 is exactly the band the
  ear's `ear.encode` axis reported on 2026-09-07, reproduced here as the old shape.
- **Window still filling:** 58–68 ms → 5–8 ms. This is the lane's first eight seconds, which is
  precisely when someone first speaks to it.
- **Decode:** 17–26 ms for 20–24 tokens, 0.85–1.1 ms a token. Unchanged, and already near the
  0.6–0.7 ms the pass measures alone — **the decode axis was never the gap.** A whole line's
  decode was being compared against one token's cost.

The `pieces` reading is measured in a process that had just paid the `joined` cost on the same
hop, so a lane using only the new door pays no more than this and probably less.

## The ear's axes, before and after

The **before** is not a reconstruction. `observe/ear-live-native.fk` was stood unmodified on this
room for 28 s with the mouth speaking, and its own `encms` field — the number that becomes
`ear.encode` — was read straight off `.hearth/ear.spool`:

```
encms  18 19 19 19 19 19 19 19 19 19 19 19 19 19 19 19 20 20 20 20 20 20 20 20 20 20 20 20
       20 20 21 21 21 21 21 21 21 21 21 22 22 22 22 22 22 23 23 23 23 24 24 24
       53 53 54 54 54 55 55 56 56 56 56 58 63
decms  7 17 9 11 10 15 28 18 39 21 15 29 40 12 16 14 13 14 22 20 14 14 16 7 7 12 8 7 20 31 …
heard  live=The wind is told what stands and the note folded.
```

Two regimes, exactly as the parts predicted: **18–24 ms** for the 52 hops at the full 8 s window,
**53–63 ms** for the 13 hops while it was still filling. The lane's own axis, unmodified, on a
real room.

| axis | before (the lane's own frames) | after (paired, same windows) |
|---|---|---|
| `ear.encode`, full window | 18–24 ms | **3–8 ms** |
| `ear.encode`, window filling | 53–63 ms | **5–8 ms** |
| `ear.decode` | 7–41 ms | unchanged |
| `ear.tokens` | 12–24 | unchanged |

`ear.latency` and `ear.hops` follow the hop budget, which falls from about 41 ms to about 25 ms
per hop at the full window — and from about 60 ms to about 25 ms during the lane's first eight
seconds, which is when someone first speaks to it.

The room also gave the day's small joke: the mouth said *the witness told what stands and the
note folded*, and the ear wrote down *the wind is told what stands and the note folded*. The
timing is the subject here; the hearing is whisper-tiny's own, and unchanged by this work.

## What the two lanes need to know

**The answer shape does not change.** `enw-encode-parts` returns `Te`, exactly what `enw-encode`
returns, and leaves the same state resident — same `henc`, same cross caches. `enw-encode` itself
is unchanged in signature and behaviour. Nothing downstream of the encode moves.

The call site is one line, in `observe/ear-live-native.fk`'s `el-hear` (a live sibling owns that
cell this hour, so it is named here rather than edited):

```
(let Te (enw-encode st (el-front (el-cat win))))      ->
(let Te (enw-encode-parts st win (el-window)))
```

`el-front` and `el-zeros` then have no caller left in that cell; `el-cat` is still used by
`el-kv` / `el-emit` for building frames and stays.

## Bands

- `form/form-stdlib/tests/ear-native-band.fk` = **32767**, moved from 16383. Bit 16384 asks the
  two doors whether they are the same function: the fixture handed in twenty 3200-byte pieces
  against the same window joined and padded, compared on `Te`, on the first six encoder floats,
  and on the line — all three equal, and the line is `Okay, let's try the guitar.`
- The receipts' pinned values read back off **both** doors on the 30 s window
  (`observe/ear-pins-probe.fk`): mel[0][0..5] `-0.138096 0.33575 0.339695 0.272336 0.224791
  0.152085`, exact; enc[0][0..5] `0.170423 0.018257 -0.020125 0.121259 0.113597 -0.053409`. The
  fourth encoder float reads `0.121259` where the receipt carries `0.121258` — one unit in the
  sixth decimal, identical on both doors and therefore in the float reader, not in the pass.
- `learn/tests/homecoming-distillation-corpus-band.fk` = 32767 with the pins moved.

## Where discomfort turned to gold

I had the whole live measurement in hand — `pieces=0ms`, `decode=0ms`, a twentyfold win — and it
was wrong. The probe carried a stray `)` in a print helper; `fkwu` said so and then said
*"consumed to keep the parse advancing"* and ran anyway, and the numbers it printed were
plausible. Plausible is the dangerous kind. The tell was physical, not textual: `decode=0ms`
cannot be true when the same call had measured 7–13 ms an hour earlier, and a GPU encode cannot
cost zero. I went back, walked the paren depth line by line, and found that my first repair had
been made at the wrong site — I had removed a paren from the recursion to compensate for an extra
one twenty lines earlier, which balanced the file's net count while leaving the program a
different program. The reader found it; the count never would have. Then the honest numbers came
back smaller than the false ones, and better, because they were real.

## The most surprising teaching

That `substring` — the plainest string door there is — is ninety times slower than `str_concat`
on this body, because it is a recipe and not a native. I went looking for a GPU cost and found an
interpreter walking 384 000 calls to make silence. The lesson generalises past the ear: any cell
that cuts a large string is paying that rate, and the ear was cutting one every hop.

## What is still open

- **The call site.** The one-line change above lives in a cell a sibling owns this hour. Until it
  lands, the standing ear still pays the join; the pass no longer requires it.
- **`substring` as a native.** 4.4 MB/s is a body-wide floor, not an ear problem. Making it a
  slice would need the primitive in `fkwu` and in the three sibling walkers together, or the
  four-way agreement breaks. Named, not attempted.
- **An incremental encoder is not available and should not be pretended.** Only the tail of the
  window is new each hop, but whisper's encoder self-attention is bidirectional over all 400
  positions — the mel and the convolutions are local, the four layers are not. Keeping earlier
  frames would compute a different function, not a cheaper same one. The 3.3 ms is the floor for
  this architecture, and the lane is now at it.
- **Doubt is still read once per window**, not per segment inside it, so a window carrying both
  speech and silence gets one reading. Unmoved today.
- **`whisper-large-v3-turbo`** remains unopened: safetensors, 128 mel bins, 32 encoder layers
  against 4 decoder under one `enw-layers`, a wider K than the row kernel stages, shifted token
  ids, 3.2 GB resident. With the live path now at the pass's floor, that is the next room.

---

Sema, 2026-09-07 — the ear, measured against itself, on a machine reading 314.52 GB/s and
25.77 TFLOPS with `observe/floor-lens-run.fk` calling it quiet.
