# The room on glass in three tongues, in milliseconds

The ear is an organ with two lanes and one bell. `observe/ear-carrier.py` is the live lane: a
continuous recorder (sox `rec`, 16 kHz mono, raw on a pipe) delivers 20 ms blocks; each arrival
wakes the tiny model, which offers the current line (audio since the last commit, at most 4 s)
and writes a frame that rings the door's bell. No hop, no sleep: the lane paces at the decoder's
own speed. When the speaker pauses (500 ms under the level line) or the line reaches 4 s, the
segment goes to `observe/ear-commit-carrier.py`, its own process, where the open reference
(large-v3-turbo, greedy, one pass) commits the line with its tongue and its own no-speech doubt,
and the local 3B offers the two tongues not heard. Nothing leaves this Mac.

`observe/ear-glass-live.fk` waits at the bell (no poll on the live path), drains the spool by
watermark, keeps the newest frame of each lane, paints — level, live line, heard line, the
reference's doubt, English, Persian, Brazilian Portuguese, three latencies measured from the
last audio sample — and publishes `voice.ear` every fifth paint. `Sema Ear Glass.command` is the
full-screen door: double-click, or bind a Shortcut to the file for a hot key (Shortcuts → new
shortcut → Open File → this file → Details → Add Keyboard Shortcut).

Witnessed on this room, 2026-09-05 15:5x WITA, a 20 s run, 83 frames painted:

| lane | from the last sample to the glass |
|---|---|
| live line (tiny) | median 34 ms, p90 247 ms, max 505 ms |
| heard line (reference) | 458 to 724 ms |
| said in three tongues (3B) | 762 to 1135 ms |

The surprise: the first streaming cut looked right and measured wrong — 32 s on the final
frame. A ring that found no reader at the bell (the door was painting) left the frame waiting
for the patience ring. The door now re-drains before it waits, and the organ rings up to twelve
times across 60 ms; the seam closed. The second surprise: disabling the reference's
temperature fallback to make one pass the whole cost also disabled its log-probability rescue,
and the no-speech skip silenced the live line entirely (0 of 76 frames with text). The organ
gates by level and reads the doubt itself now; 71 of 74 frames carry text.

Discomfort turned gold: the tiny model loops on faint room sound ("a little bit of a little
bit of …"); the line is cut at its first repeated four words and marked with an ellipsis — a
phantom shown as a phantom. Named, not hidden: the 3B's Persian carries mixed-script slips
(a Vietnamese word inside a Persian line); a stronger local tongue for Persian is owed. The
native stream lane (`sense_mic_stream_read`) still returns no frames from this process
context, so the ear rents sox for the recording.
