# The ear and the song

2026-09-22, Urs's Mac, with a song playing in the room and the glass standing on it.

Urs asked to walk the live multi-language transcriber home to equal-or-better than SOTA, and,
when I went to the fixtures first: *you clearly are not checking glass, since it should be able
to translate the song that is playing.* So this receipt is ordered the way the room was.

## What the glass heard

The live lane runs whisper-tiny, the body's own pass, at 200 ms hops. On the song, in one
minute: 40 `phantom` frames (a pass that returned only a tag), lines that were hallucinated
dialogue — *"- Tera? - No. - You know. - Why can't I be hearing? …"* repeated — and fourteen
`heard=` lines of one broken glyph, `蜂�`. The tongue lane dutifully carried what it was given
into en/pt/fa/id, the Persian line arriving with Chinese, Arabic and Vietnamese tokens inside it.
The model's own doubt read 0.49–0.65. The glass was working; the ear was songblind.

## What was measured, in order

| reading | result |
| --- | --- |
| turbo, three stored references (en/de/fa), WER door | **0 / 0 / 0**, 1.0–1.5 s per 4 s clip |
| tiny, same clips, raw greedy to cap | right sentence en/de but repeated (raw 700 / 658); fa noise (300) |
| turbo encoded at the window's own extent (tiny's path) | **100 / 100 / 163**, slower, witness fails — reverted |
| turbo in the live lane, 60 s beside the glass | encode 0.39–0.87 s, decode ~40 ms, nsp 0, 1 phantom, text **"you"** |
| turbo full-policy door on 12 s of the room (−35 dBFS) | **"Thank you."** |
| turbo plain no-timestamps greedy on the same 12 s | **"Anyway, there's a cold one"** — 4×: "cold weather" — 12×: "there is a cold weather" |
| witness `native-whisper-homecoming-run` on the reverted code | 8 of 9; `freed` reads 0 — one transcription alone retains 0 → 0 |
| `stt-wer.fk` naive distance on a 40-token line | two kernels at 100% CPU for 14 min; `sw-wer-bounded` returns |

Rows: `receipts/stt-wer-ledger.jsonl` (turbo before, turbo at the window's extent, tiny raw,
turbo reverted). Corpus rows 1583 `ownextent`, 1584 `songblind`; band 32767.

## What stands and what does not

- The turbo pass is live-viable: half a second an 8-second window, on this Mac, in-process.
- Turbo hears the song as words when asked without timestamps; the live lane asks with a
  timestamp prompt and gets whisper's music mark, "you". The full-policy door gets "Thank you."
  The lever is the decoding prompt and the lane's policy — not gain, not the model.
- The reduced audio context is a false lever for large-v3-turbo. Measured, reverted, written
  into `docs/native-whisper.md`.
- Whether *"Anyway, there's a cold one"* is what the song sings is our reading; nothing
  here claims it.

## Next rung, on purpose

The live lane on turbo with the no-timestamps greedy pass for its words, keeping its own pause
and full-window commits for the line's end, the tongue lane unchanged behind it; witnessed on
the song, our reading the judge. Then a reference corpus with ground truth so "equal or
better than SOTA" is a WER row and not a sentence.

## Rent

Zero for every pass. The arriving mind's tokens paid for the measuring — more than the step
warranted, and named here.

## Receipt

**The surprise:** the ear was never deaf to the song. Three doors, one model, one twelve-second
window: *"Thank you."*, *"you"*, and *"Anyway, there's a cold one"* — the same weights heard the
same air three ways, and what decided it was which tokens the body put in the model's mouth
before it listened. SOTA is not only a checkpoint; it is the prompt the body asks with.

**Where discomfort turned to gold:** being told I was not checking the glass. The fixtures were
green and the room was full of phantoms. Going to the spool first turned an abstract program
into one measured fact per door — and the cheapest probe of the day, three gains on a saved
window, is the one that pointed at the lever. Also: a chain that killed itself by grepping for
its own name — the wrapper-shell lesson already in memory, relearned at the cost of two runs.

## The next rung, walked the same evening

Urs said *next*. The lane gained three standing choices, each a file the body can write without a
rebuild: `.hearth/ear.model` (`turbo` opens the native large-v3-turbo; its live line is the plain
no-timestamps pass), `.hearth/ear.cadence` (pass on everything, close only on the full window), and a
drain that takes what the microphone buffered during a slow pass so the window stays at the present.

First witness, turbo without cadence, 90 s on the song: **76 of 94 frames were quiet** — the floor
follower had learned the steady song as the room's own quiet; the eight passes it allowed were
sub-second pause commits, each *"you"*. The gate, not the model, was the wall.

Second witness, turbo with cadence, 90 s: **192 frames — 124 growing lines, 7 full-window commits,
3 phantoms**; the glass's tiny lane in the same seconds: 94 phantoms. The lines:

| t | heard | avglp |
| ---: | --- | ---: |
| 859985 | Take me, won't you take me? She's a little bit about here. | −0.27 |
| 869248 | **Won't you take me? Take me.** | **−0.09** |
| 876900 | I wanna go to function down | −0.36 |
| 884316 · 892398 · 918853 · 926765 | Yeah. · Ciao! · Okay. · Thank you. | −0.35 … −0.94 |

The song was *Funkytown*; the ear heard its hook and misheard its title. Tiny, beside it, heard
*"Won't you tell me"*, *"Wonchuk-chan!"*, *"원주차!"*. Pass cost with cadence: 230–250 ms encode,
10–70 ms decode.

What remains for the glass itself: it runs from the main checkout on `main`; when this lands there,
writing `turbo` to its `.hearth/ear.model` and touching `.hearth/ear.cadence`, then letting the lane
restand, turns the room's ear over. The tongues will then carry lyrics.

**The surprise of the rung:** the model was ready an hour before the lane let it listen — a follower
built to hear speech over a quiet room heard a song as the quiet. **Gold:** the frames said so in two
numbers (floor 2,800, gate 5,600, level −24 dB) before any guess could.
