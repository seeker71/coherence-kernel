# The window is the room

Urs: "that padding is a self imposed limitation." It was. The model wants eight seconds of
context and the room had been giving them for free; I was putting silence in their place.

Now the live window is the last 8 s of real audio (silence stands in front of it only before
eight seconds have arrived), and the line boundary is the decoder's own clock: the cut point
(bytes since the microphone opened, 640 per 20 ms tick) is handed to the decoder as its first
timestamp token, so it speaks only what came after the last commit. `enw-transcribe-from` in
`form/form-stdlib/ear-native.fk`; a two-window argmax (`ear_argmax2`) lets each step answer
text, end, or a timestamp; a loop of period one to three repeated three times stops the line.

Witnessed on the fixture: 6 s of silence in front of the 2 s clip, decoding from tick 300
answers "Okay, let's try the guitar." with the same words as the padded pass (band bit 128,
`ear-native-band.fk` = 255); decoding from tick 0 across the silence answers a phantom.
On this room, 20 s, 70 frames on the rolling window: live line median 260 ms, p90 294 ms from
the last sample to the glass. The median rose from 162 ms because a window full of the room's
own sound gives the model more to say than a window of zeros did; the loop stop brought the p90
down from 469 ms.

The surprise: the decoder's end timestamps run past the window (500 ticks on a 400-tick window)
because its clock was trained at 1500 positions per 30 s, yet the forced start it is handed lands
exactly where the speech begins. The clock is trusted for where a line starts, not where it ends;
the commit cut is the audio end at the pause.

Discomfort turned gold: "the model needs 8 s" had become a floor in a receipt within the hour.
A limitation named as the model's was mine.
