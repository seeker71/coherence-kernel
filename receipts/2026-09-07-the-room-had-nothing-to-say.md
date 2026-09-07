# The room had nothing to say and the pass wrote words anyway

`form/form-stdlib/ear-native-band.fk` = 16383 (was 255). The gap: on a quiet room the body's own
whisper pass answered confident ambient tags — `[BIRDS CHIRPING]`, `(eerie music)`, `(water
running)` — and the whole body downstream took them for words, because the pass carried no way to
say otherwise. The model always knew. Its vocabulary holds a `<|nospeech|>` token whose probability
at the start-of-transcript position is its own reading of "there is nothing here to write down", and
the pass had never once read it.

**Recorded, not imagined.** Eight seconds of this room through the mic organ
(`sense_mic_stream_start` / `sense_mic_stream_read 3200 200` / `sense_mic_stream_stop`), rms 160,
−48 dBFS:

```
room   lang=nn  spoken=[(eerie music)]  heard=[]
       nospeech 0.8834   tongue 0.7736   line-logprob -1.9834   doubted 1
```

`spoken` is `enw-text` — the phantom is still there, unchanged, and any caller that wants it can
read it. `heard` is `enw-heard`, which answers `""` for a line the model itself doubts. Before this
pass, `(eerie music)` left the pass as fact, and the tongue came out as Norwegian Nynorsk at 0.77 —
a confidently wrong tongue that also had nothing attached to it to say so.

## What was added

`ear_softp` in `form/form-stdlib/ear-msl.fk`: one probability off the resident logits — softmax over
whatever window the caller names, read at whatever index it names, written as float bits into the
four-word output buffer. Two stages like the argmax, because one threadgroup walking 51,865 logits
is one core of forty reading 207 KB alone. It carries `(max, sum-of-exp)` merged by the online rule
the attention kernels already use, so nothing overflows and no pass is needed to find the max first.
Over the whole vocabulary at `<|nospeech|>` that is the reference's `no_speech_prob`; over the
language window at the chosen tongue it is that tongue's own confidence. Two calls, one line.

`ear_argmax` gained the decoded line's log-probability **for free**. A softmax normaliser is the same
walk over the same logits the argmax is already doing, so each lane now carries an online sum-of-exp
beside its running maximum. And the argmax's winner *is* the maximum, so its log-softmax is exactly
`-log(L)` — no second index, no second read. Stage 1 adds it to `lp[0]` and one to `lp[1]`; a caller
zeroes those two floats where its line begins and reads back the sum and count of exactly the tokens
it decoded.

The answer shape grew by appending, so every existing caller still works:
`enw-transcribe`, `enw-transcribe-from` and `enw-transcribe-from2` now answer
`(lang ids nospeech-prob tongue-prob avg-logprob)` where they answered `(lang ids)`.
`(head r)` and `(nth r 1)` are untouched — **`observe/ear-live-native.fk` and the glass need no
repair to keep working**, and the three new elements are there when they want them. The live lane
already writes `nsp` as a hard `"0"` in every frame it spools; that field can now be filled.

## Where the line belongs, and why it is not the reference's

The reference silences at `no_speech_prob > 0.6`. On this body's 8 s window that is wrong, and the
body said so. The same utterance was walked across the window at every one-second offset and read
against three disjoint 8 s windows of this room's silence:

| window | no-speech | line log-prob | what it wrote |
| --- | --- | --- | --- |
| speech @1 s | 0.5144 | −1.447 | Okay, let's try the guitar. |
| speech @2 s | **0.6210** | −0.428 | Press or hit. Press or hit. Press or hit… |
| speech @3 s | 0.4263 | −2.494 | 이렇게. |
| speech @4 s | 0.5643 | −2.263 | 오케이, 펴트울 키스 한번 |
| speech @5 s | 0.4942 | −1.520 | Okay, let's try the guitar. |
| speech @6 s | **0.6104** | −1.543 | Okay, let's try the guitar. |
| room @0 | 0.9223 | −1.847 | (eerie music) |
| room @1 | 0.9205 | −2.009 | (eerie music) |
| room @2 | 0.9003 | −2.077 | (eerie music) |
| 8 s of digital zero | 0.8775 | −1.244 | Y ffysu, ddwyff, ddwyff… |

Speech spans 0.426 to 0.621. The empty room spans 0.900 to 0.922. **A gap of 0.28 with nothing in
it** — and 0.6 falls inside the speech band, so the reference's line silences real words spoken at
the end of a mostly-empty window. `enw-doubt-line` is 0.75: 0.13 above the loudest doubt any speech
produced, 0.15 below the quietest the empty room produced. Both edges are pinned in the band
(bit 2048 is the 0.6104 window *not* being silenced), so the line cannot drift back without the
band saying so.

**The line-confidence reading is carried and not gated on**, and this is the part that surprised.
The reference also *rescues* a doubted line when its `avg_logprob` stays above −1.0. On this body
that reading does not separate at all — speech ran −0.43 to −2.49 and the silent room −1.85 to
−2.08, fully overlapping — and the single highest score of everything measured, −0.428, was a
decoder gone round in a loop saying "Press or hit" eight times. A rescue clause keyed on per-token
confidence rescues exactly the most confident phantom there is. So the number stays an honest
reading in the answer and the gate is the reading that was measured to separate.

Checked against the reference on the same fixture: mlx_whisper answers `no_speech 0.5164 /
avg_logprob −1.1540` on the 2 s file and `0.7688 / −0.8823` on 8 s of zeros. Same quantity, same
ordering, magnitudes moved by the window length (the reference always pads to 30 s; this pass reads
the 8 s it was handed).

## What it costs

Measured warm, alternating against the unchanged sources in the same minutes because this machine
was bimodal today — the same code answered 3.7 ms and 10.5 ms per encode depending on which sibling
had the GPU — so the honest reading is the least-contended sample of four:

| | before | after |
| --- | --- | --- |
| 8 s encode | 3700 µs | 3750 µs |
| decode token | 715 µs | **695 µs** |

The per-token cost is nothing, because the work was folded into a kernel that was already reading
those logits. The per-**line** cost is 245 µs when the tongue is detected (two folds, one sync, a
12-byte read-back) and 370 µs when the tongue arrives known — that case has no start-of-transcript
step of its own, so one logits pass is run over row 0 of the prefill's rows, which is still the
start-of-transcript row. A whole `enw-transcribe-from2` line on this fixture is 7200 µs, so the
doubt is 5% of a line and 1% of a 37 ms live hop.

The invariants held to the last digit. mel[0][0..5] on the padded fixture: −0.13809633, 0.33574992,
0.33969527, 0.27233589, 0.22479147, 0.15208483. enc[0][0..5] on the 30 s window: 0.17042330,
0.01825747, −0.02012534, 0.12125858, 0.11359668, −0.05340948. The fixture still answers
"Okay, let's try the guitar."

Machine weather, `observe/floor-lens-run.fk`: **275.2 GB/s through the handle door against a best of
275.2 — a quiet machine**, 12.88 TFLOPS. The fall named in yesterday's `floorfall` row has lifted on
both readings. The lens's own warm encode read 6.4 ms, 10.76× its 0.59 ms arithmetic floor; its
whisper token 0.76 ms, 3.53× its memory floor.

## The larger model does not open, and the door lied about it

`whisper-large-v3-turbo` is cached beside tiny, and the premise that it is "the same npz shape" is
false — measured, not assumed. Asked with the body's own doors: no `weights.npz` exists there
(`file_size` −1); the file is 1,613,977,612 bytes of **safetensors**, whose first eight bytes are
`164 239 0 0 0 0 0 0` — a little-endian u64 header length of 61348 — where a zip would open
`80 75 3 4`.

**And `en-table` did not refuse it. It answered 7720 rows.** Not an error, not a `nothing` — a
plausible tensor table walked out of the last 22 bytes of a file that has no central directory
anywhere in it, which `enw-open` would then have mapped as weights. The end-of-central-directory
signature `PK\x05\x06` is now checked in `form/form-stdlib/ear-npz.fk`, a file that is not an archive
answers the empty table, and bit 8192 of the band holds that door shut with this exact 1.6 GB file.
The tiny npz is a real archive, so nothing else moved.

The rest of the floor for the larger model, read off its own `config.json` and this pass's own
source rather than guessed — each of these is unfinished work, not a limit:

1. the container: safetensors, so the table door owes a second reader (a u64 length and JSON, which
   is a simpler shape than the zip it already reads);
2. **128 mel bins against 80**, and 80 is a literal in four emitted kernels — `ear_spec`'s
   `mv[80]`, its `tid < 80` gate and its `mel[t * 80 + tid]`; `ear_melbank`'s `80 * 201` and its
   `/ 81.0f`; `ear_melnorm`'s `Tf * 80`;
3. `enw-layers` is one number for both sides; large-v3-turbo is **32 encoder layers against 4
   decoder**, so the encoder tables, the encoder loop and the per-decoder-layer caches must part;
4. `ear_mv` stages its row in `threadgroup float4 xs4[384]` — 1536 floats, exactly whisper-tiny's
   mlp width. Large's mlp2 reads K = 5120. That is 20 KB of threadgroup memory, still inside
   Metal's 32 KB, so it is a size to emit rather than a wall;
5. the special token ids all shift by one (large-v3 added `<|yue|>`, so 100 language tokens):
   `<|nospeech|>` is 50363 and timestamp zero is 50365;
6. 1.61 GB of weights, and `enw-open` copies every member into its own aligned device buffer beside
   the whole-file map — 3.2 GB resident before a single window is encoded.

Head dimension 64 is the same on both, so `ear_attn`, `ear_attn_dec`, `ear_attn_x` and
`ear_attn_xfin` need nothing. The live line's latency was never at risk: tiny stays the live line
either way, and the choice would belong to the committing line.

## Standing, not mine

`form/form-stdlib/tests/ear-tongue-band.fk` reads **3904 against its stated 4095** (bits 1, 2, 4, 8,
16, 32, 128 red). Its prelude chain is `ear-tongue.fk` → `dense-token-handle` / `dense-multi` and
touches neither `ear-native.fk` nor `ear-npz.fk`, so it shares no path with this work; it last landed
green yesterday. Left for the sibling who owns that lane rather than reached into.

---

**How the exchange stayed alive.** The gap arrived as three separate asks and turned out to be one:
a pass that could not say "nothing". Each of the three was answered by asking the body rather than
the documentation — the mic for the room, the sweep for the line, the alternating A/B for the cost,
the file's own first eight bytes for the container.

**The most surprising teaching.** The reference's own rescue clause is a trap here. A gate that keeps
a doubted line when its tokens scored confidently would have kept exactly the worst phantom measured
today — a decoder stuck in a loop, whose repetition made every token trivially predictable and gave
it the best per-token confidence of any window, speech included. Confidence and correctness point
opposite ways once a decoder starts repeating itself. The reading is still worth carrying; leaning
on it would have been worse than not having it.

**Where discomfort turned to gold.** Two places. The first: `avg_logprob` was built specifically to
rescue the one real utterance the 0.6 line was eating, and when it was measured it did not
discriminate at all — the wanted answer simply was not in that number. Sitting with that instead of
tuning a threshold until it looked right is what produced the sweep, and the sweep produced a 0.28
gap that no amount of threshold-fitting would have found. The second: asking `en-table` to open the
larger model was meant to be a two-minute confirmation of a floor already known. It answered 7720
rows, and the discomfort of a green number over a file the door cannot read is the whole of bit 8192.

**One thing still open.** The doubt is read at the start-of-transcript position, which is the
reference's own place for it, and that position sees the *window* — not the segment inside it. A
30 s window holding four seconds of speech and twenty-six of silence has one no-speech reading for
all of it. Per-segment doubt would need the reading taken again after each timestamp the decoder
emits, and whether that is even meaningful at a position the model was not trained to answer it at
is unwitnessed here.
