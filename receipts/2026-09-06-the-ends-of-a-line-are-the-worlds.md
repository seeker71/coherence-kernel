# The ends of a line are the world's

Urs: "32 token cap is artificial and how much delay do we have?"

The cap is gone. A line now ends where the world ends it: end-of-text; the decoder's own clock
leaving the window (its clock runs ahead of the audio, so 1.5x the window plus a second); going
round (the last twelve tokens carrying four or fewer distinct ids); or more tokens than the
seconds of audio since the cut could carry (sixteen a second, plus eight). Lifting the cap to the
model's 448 first showed why the ends matter: one pass ran 440 tokens in 6.3 s on a loop the
short-period guard missed ("PENSEAN SOMENSEAN SOMENSEAN"). The mic slice is 100 ms now, so a
pass starts on fresher audio.

The delay, measured by stage on this room (25 s, 65 passes, every live frame carries encms,
decms and ntok; the glass shows the stages lane):

| stage | median | p90 | max |
|---|---|---|---|
| mic slice age | 0 to 100 ms | | |
| encode, 8 s window | 89 ms | 98 ms | 153 ms |
| decode (median 5 tokens, ~15 ms each) | 94 ms | 116 ms | 386 ms (28 tokens) |
| last sample to emit | 214 ms | 293 ms | 582 ms |
| emit to glass | 7 ms | | |

So a spoken word reaches the glass 200 to 400 ms after it is said. The heard line waits for the
pause (500 ms of quiet) and lands about 700 ms after the last word. The tongues are the delay that
matters: about 14 s a tongue a line on the body's dense llama lane, which prefills one token at a
time.

What stands between here and less: the encoder's gemms are one thread per output with no tiling
(89 ms that a tiled kernel would make ~15); the decoder pays 56 dispatches a token (a batched
prefill of the previous pass's tokens would make a hop cost only its new tail); the dense lane's
prefill is token by token. Named as the next stones, in that order of gain per token spent.

The surprise: 32 was close to the physical bound of English in 8 s, which is why it looked
harmless, and wrong for Persian and Portuguese, whose byte-level tokens run two to three a word.
Discomfort turned gold: lifting the cap before the ends were the world's cost one 6.3 s pass on
the glass, witnessed rather than reasoned away, and that pass wrote the fourth end.
