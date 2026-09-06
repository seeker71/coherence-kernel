# A line pays once

The live transcript's tongue lane paid, on every line, for things that never change between
lines; the live lane paid, on every hop, for a copy of the whole window. Both lanes now hold what
is constant and pay only for the line. Measured on this M4 Max, in a worktree, with
`tongue-cost5.fk`'s shape (now `probe-a`/`probe-c` in the session's scratchpad) and the door
`observe/ear-glass-live.fk` run for 20 s with `.hearth/ear-okay-loud.wav` played twice into the
room.

## The tongue lane (`form/form-stdlib/ear-tongue.fk`, `observe/ear-tongue-native.fk`)

| stage, one line, one tongue | before | after |
|---|---|---|
| tokenize the prompt (15 tokens) | 354 ms — 336 of it one `gmt-u32` header walk for BOS | 1 ms, equal to `dtk-encode` |
| prefill the prompt | 45 ms a token, the head on every token | 22 ms a token, the head once, on the last; the tokens shared with what the cache holds are not prefilled at all |
| generate (the dense lane's own floor) | 46-50 ms a token, plus one `gmt-u32` walk for EOS (335 ms) | 46-50 ms a token, EOS read once at open |
| detokenize | 217-330 ms, one vocabulary walk | 0 ms, equal to `l3d-text` |
| outside the GPU, per line | about 1.1 s | about 1 ms |

The lane's state, opened once (`et-lane-open`) and carried line to line: the file's bos and eos
ids; the vocabulary as fixed 28-byte record buckets under `.hearth/vocab-<n>-r28/` (a token is one
`read_file` and a byte walk with `str_byte_at`, no substring; the longest indexed piece is 24
bytes, stated); every piece decoded through the byte alphabet into `.hearth/vocab-<n>.pieces`
behind an id index `.hearth/vocab-<n>.pidx` (a word is two `read_file_slice`s); and the ids the
KV cache holds, position by position. A new line prefills from the first token it does not share
with what is loaded. The second tongue of a line shares the whole text — "English: <text>\n" is
the same for Persian and Portuguese — so it prefills three or four tokens. The index builds once
from the rendered vocabulary in about 30 s; the door's first run in a fresh checkout pays it.

The 256-position cache is now a stated room: a prompt that leaves none answers empty tongues
(`room = 250 - len(ids)`), where the old lane would have written past `maxpos`.

`et-say` en→pt "Okay, let's try it." says "Ok, vamos tentar isso." — the same words as before —
in 1327 ms cold (first dispatch), fa after it in 381 ms, the next line's pt in 1071 ms. The rest
of a line is the dense lane's own per-token cost (its head is about 40% of a generated token;
its prefill runs one token a dispatch); not this lane's to change.

## The live lane (`observe/ear-live-native.fk`)

Per pass, Form time beyond the GPU (`livems − encms − decms` in the spool's live frames):

| | before | after |
|---|---|---|
| Form beyond the GPU, per pass | 90-110 ms | 0-3 ms |
| of which: the window's tail (`substring` on 256 KB, halve-and-concat) | 53 ms | gone |
| the level loop (1200 samples, `str_byte_at`) | 0 ms | 0 ms |
| the frame append, the bell fork | 1 ms, 3 ms | 1 ms, 3 ms (kept) |

The window is now the list of the mic's own chunks, oldest first: a hop appends one and cuts
the front to the last 8 s (a `substring` only on one 3200-byte chunk when the cut falls inside
it), the level reads the newest chunks up to 9600 bytes, and the window is joined into one
string only for a pass (about 2 ms). The frame fields, the bells and the door are unchanged;
124 frames in 20 s where the same room gave 66.

## Band

`form/form-stdlib/tests/ear-tongue-band.fk` = 255: the record tokenizer equal to `dtk-encode`
on an English, a Persian and a Portuguese prompt; the piece file equal to `l3d-text`; a line said
through a shared prefix equal to the same line through a cold cache; a said Portuguese line
non-empty and cut at its line break.

## Surprise

The "330 ms tokenizer" was not the tokenizer. It was `gmt-u32`, a walk over the header's
128 256-entry token array, asked for BOS on every line — and asked again for EOS inside every
generation. The bucket tokenizer itself cost about 20 ms. The live lane's "80 ms of Form" was
one call: the window's tail through the `substring` recipe; the sample loop everyone suspected
cost nothing measurable.

## Discomfort to gold

Using `dtk-encode` as the band's attestant costs 3.9 s a prompt — it walks the vocabulary at
every position. The slowness is the statement: it is the body's own wording of the
longest-match rule, and the fast lane agrees with it byte for byte on three tongues. And the
second door run heard a 300-token phantom loop from the tiny model; the new lane answered empty
tongues for it, which is where the cache's room became a visible number instead of a GPU write
past the end.

## Open

The dense lane's per-token cost (prefill one token a dispatch, the vocabulary head about 40% of
a token) is the floor now; it belongs to the sibling agents on `dense-token-handle.fk`. The
Persian tongue is as weak as it was — the model, not the lane. `.hearth/vocab-<n>-b4/` (the old
buckets, 139 MB) is read by nothing now and can go.
