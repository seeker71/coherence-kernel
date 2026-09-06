# Whichever floor holds

The whisper agent named something the lens could not say: the encoder is not held by memory. Its
8 s pass moves 83 MB, which at this door's best is 0.21 ms, but it must also do about 7 GFLOP,
and no arithmetic rate this machine has finishes that under half a millisecond. Reading the lane
as 19x its floor was reading it against the wrong floor.

So the lens measures the machine twice. The bandwidth as before, and now the arithmetic rate the
same honest way: six independent chains of fused multiply-adds held in registers, nothing but the
lanes themselves timed. The first probe read 3.2 TFLOPS and was measuring latency, because each
multiply-add depended on the one before it; with independent accumulators the same machine reads
12.88 TFLOPS, and that under load. A lane's floor is now whichever is higher, its bytes over the
bandwidth or its arithmetic over the rate, and the frame says which one holds it:

| lane | held by | floor |
|---|---|---|
| whisper encode, 8 s | arithmetic | 0.59 ms |
| whisper decode token | memory | 0.28 ms |
| dense 1B token | memory | 6.40 ms |
| dense 3B token | memory | 9.78 ms |
| four answers riding one pass | memory | 1.60 ms |

Both floors move with the machine's weather, and this reading was taken at 56% of the door's best
with two agents still working, so the floors above are the loaded ones and every distance beside
them is taxed. That is the point of stamping the weather: the numbers stay honest inside their own
hour instead of being carried out of it.

The surprise: my own probe lied in the direction I would have believed. Three TFLOPS on a machine
that gives sixteen would have read as "the arithmetic is not the wall" and sent the next work to
the wrong place. A dependent chain measures how long one operation takes to come back; a lane
measures how many can be in the air at once. Discomfort turned gold: the encode's 19x looked like
the largest room in the body all afternoon, and naming its true floor turned most of that room
into physics.
