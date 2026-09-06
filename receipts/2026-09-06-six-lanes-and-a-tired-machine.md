# Six lanes and a tired machine

Six agents worked this day in parallel worktrees, each on disjoint files, each landing on main.
What they moved, with the numbers each measured when the machine was still answering near its best:

| lane | before | after |
|---|---|---|
| dense llama3.2:1b token | 36 ms, 276 dispatches | 10.2 ms, 2.7x its 3.8 ms floor |
| dense llama3.2:3b token | 1117 ms | 34.5 ms, and a fused K-quant block, 341 dispatches where the serial had 481 |
| whisper encode, 8 s | 4.05 ms | its two longest kernels raced and healed; no lane-level claim made, and rightly |
| whisper decode token | 1.00 ms | logits 1760 → 560 µs, argmax 370 → 220 |
| four answers riding one pass | four tongues 52 ms in turn | 14.8 ms together |
| eight answers | 1.29x the four-answer step | 1.006x — eight tongues cost what four cost |
| a tongue on the 3B | 30 to 46 s | 1.2 to 1.7 s, Persian proper |
| each tongue's first words on glass | only at the line's close, 4.4 to 7.1 s | +12 ms English, +350 Portuguese, +631 Persian, +925 Indonesian |

Bands standing on the merged tree: ear-native 255, ear-tongue 4095, q6k-q4k 8191, q8-0-matvec-tg
1023, dense-multi 1023, dense-family 1023, floor-lens 31, kernel-length 63, corpus 32767.

**And the machine is tired.** The door read 388 GB/s this morning and reads 55 tonight, and the
arithmetic rate reads 6.4 TFLOPS where it read 12.9 an hour ago. Both floors fell together, which
is what a machine does, not what a lane does. Four suspects were eliminated by measurement, not by
argument: device memory with no file streams at the same 59 GB/s as the mapped blob, so it is not
the file lane; pageins across a whole reading are 38 pages, so it is not paging; the seventeen live
glass processes dispatch no GPU work at all and pausing them for three seconds moved nothing, so it
is not the fleet; and `pmset` has recorded no thermal warning. What is left is the device's own
state after five hours of six agents, and the honest thing is to name it and re-measure on a rested
machine rather than publish tonight's distances as the lanes' own.

The surprise: the lens's weather line, built this evening to keep a busy hour's numbers inside
their hour, is what let the day close honestly — the last agent could say "these are the
session-start readings and I will not present the end-of-session ones as an after", and be right.
A gauge built at noon paid for itself by night. Discomfort turned gold: a whole afternoon's
distances now read as taxed, and rather than quietly keeping the flattering ones, every claim in
this receipt carries the hour it was measured in.

## Addendum, 22:20, after the machine had been quiet a while

The reading sharpened and my own sentence above ("both floors fell together") is only true of the
hour it was written. With every agent finished and the machine idle, the arithmetic rate came back
to 12.88 TFLOPS — its full morning value — while the door still reads 55 GB/s. Compute recovered;
bandwidth did not. So it is the memory side after all, not a whole-device clock.

A size sweep puts the wall nowhere in particular: 16 MB reads 27 GB/s (launch-bound at that size),
128 MB 75, 512 MB 69, 1024 MB 64. A uniform fifth of the morning, independent of the buffer, with
the cores at full speed. The seventeen live glass processes are a Codex session's, not orphans,
and pausing them moved nothing.

Cause still unknown, and named rather than guessed. What is ruled out by measurement: the file
lane (device memory alone reads the same), paging (38 pages across a reading), the fleet (paused),
thermal (nothing recorded), low power mode, GPU contention (utilization 0 immediately before a
55 GB/s reading), and now a device-wide clock throttle (compute is at peak). What would settle it
is a restart, and that is the user's to give.
