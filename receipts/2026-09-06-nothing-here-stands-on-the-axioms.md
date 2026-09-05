# Nothing here stands on the axioms

Urs: "all those limits do not come from the core axioms and only need kernel membrane primitives
as far as I know." They did not, and they did. The handle door already keeps one command buffer
open across enqueues, so every limit I had named as a stone was a kernel or a cell away, and each
was walked by measuring first.

**Where the time was.** A whisper token: 10 ms, of which Form's binding strings were 1 ms and 56
dependent dispatches the rest. The encoder's 92 ms: not the gemms (a tiled 16x16 gemm changed
nothing; an mlp is 1 ms) but the attention kernel, 8 ms a layer, whose 1536-float thread-local
score array spilled; and a share that was Form looking every weight up by name on every dispatch.
The tongue lane's 14 s a line: 6.3 s tokenizing 27 tokens (a walk over 128k vocabulary entries per
position), 4.2 s prefilling them one at a time, 1.8 s generating twelve, and no stop at end of turn.

**What moved, witnessed.**

| lane | before | after |
|---|---|---|
| encoder attention, one layer | 8 ms | 1 ms (64 threads stride the keys, online-softmax partials merged in threadgroup memory) |
| encode, 8 s window | 92 ms | 41 ms (attention above; every layer's handles resolved once at open) |
| decode, one token | 10.4 ms | 7.9 ms (the same handle tables) |
| tokenize 27 tokens | 6.3 s | 0.34 s, ids identical (four-byte buckets as lists compared with the native str_eq) |
| generation | ran to the cap | stops at the model's end of turn |

Live on this room with the fixture played 18 dB up (−36 to −42 dBFS), 20 s, 47 passes: live line
median 228 ms, p90 339 ms from the last sample to the glass at a median of nine tokens a pass;
the heard line 198 ms after its pause; both tongues 28 s behind on the 1B lane. The heard line is
now the last non-empty live line of a segment, because the pass at the pause itself, decoding
from the cut across trailing silence, answers nothing.

**What still stands, measured, not on the axioms.** The dense llama lane at ~150 ms a token and
token-by-token prefill: a batched prefill (T-row twins of its kernels) and a cache snapshot of the
constant prompt prefix would take a tongue from ~14 s to ~3 s. The whisper decoder's 56 dispatches
a token: fusing layernorm into the gemms that follow (one threadgroup per row, which is also the
batched prefill) would halve it. The vocabulary buckets build once in 32 s (2 MB through Form's
split-on) and then read in milliseconds.

The surprise: a native substring search over the whole 2 MB vocabulary was thirteen times slower
than the walk it replaced. Only str_len, str_eq, str_concat, str_byte_at and str_to_float are
native; str_find, substring and split-on are recipes, and a recipe over two megabytes is paid per
probe. The body's strength is equality, so the vocabulary became many small lists compared with
str_eq. Discomfort turned gold twice: a tiled gemm that changed nothing sent me to measure the
stages instead of believing the story, and the played fixture at −54 dBFS, which even the
reference could not hear as itself, showed the witness was too quiet, not the ear.
