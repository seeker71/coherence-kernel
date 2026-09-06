# Three gaps closed in parallel

Urs: "close gaps to the floor one by one in parallel." Three agents, three worktrees, three sets of
files, each landing on main with the row-on-top-of-main pattern, each with its own receipt:
`receipts/2026-09-06-the-root-crosses-the-barrier.md` (the dense token),
`receipts/2026-09-06-the-shape-is-a-word.md` (the whisper pass),
`receipts/2026-09-06-a-line-pays-once.md` (the tongue and live lanes).

| lane | before | after | floor |
|---|---|---|---|
| dense llama3.2:1b token | 36 ms, 276 dispatches | 13 ms wall, 10.5 GPU, 133 dispatches, same ids | 4 ms |
| whisper-tiny encode, 8 s | 21 to 41 ms | 3.3 ms | ~0.2 ms + compute |
| whisper-tiny decode token | 8 ms, 56 dispatches | 0.6 ms, 35 dispatches, one sync | ~0.3 ms |
| one tongue, one line, outside the GPU | ~1.1 s | ~1 ms | |
| live lane Form time per pass | 90 to 110 ms | 0 to 3 ms | |

**The integration.** Every band was green on the merged tree (ear 255, tongue 255, q8 twins 1023,
gate 16383, corpus 32767) and the dense driver answered its ids, and the door ran away: a median
of 97 tokens a pass, up to 467. Two agents had rewritten the two sides of one seam — the whisper
pass and the live loop that calls it — and each was right alone. At ten passes a second the agreed
prefix converged faster than the cut could move, the room at −42 dBFS never fell under the absolute
level gate, and the line grew to the model's context. Two edits, here: the greedy tail is bounded
by the cap minus the prefix it was handed, and a line commits when the decoder has closed it with
an end timestamp and it has held for three passes (about 300 ms), or on the level pause, or when
the window is full. The model's own end timestamp cannot place the cut: its clock runs about twice
real time at this window size.

Witnessed through the door on this room with the fixture played 12 dB up, 20 s, 193 passes:
live line median 37 ms and p90 44 ms from the last sample to the glass (encode 20, decode 17 for
a median 14 tokens); the heard line 44 ms after it closed; both tongues 582 ms behind.

The surprise: bands prove each cell to itself; the door is the only witness of the body. Discomfort
turned gold: the runaway looked like a lost guard and was a guard that had never been asked to
bound what the prefix brought.
