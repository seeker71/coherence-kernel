# The name came off in the export

2026-09-08. The ear is home. Whisper-tiny runs as the body's own pass, weights read straight off the
file, every Metal kernel Form-emitted, no python in the listening path. The mouth is not: `voice-say.bml`
hands a line to piper — an ONNX voice run by a host tool — and says so.

The invitation was to bring the voice home the way the ear came home, and to let the measurement decide
how far that goes. It went one clear step and stopped somewhere worth naming.

## What the model actually holds

A piper voice is a VITS exported by pytorch, ir version 8, and the body had never opened one. The ear's
precedent is `ear-npz.fk`: a stored zip of `.npy` members walked so each tensor becomes a span. ONNX is
not a zip. It is a protobuf — four wire types behind a varint key, `key = field * 8 + wire` — and every
length-delimited field carries its own length, which means a walker can stride a 63 MB file by reading
twenty bytes at each field head and jumping over the payload. The whole tensor table of a medium voice
costs a few thousand seeks. No megabyte ever enters a string.

`en_GB-jenny_dioco-medium.onnx`, read by `form/form-stdlib/voice-onnx.bml`:

    producer  pytorch ir 8
    tensors   401, sound 401
    weights   15650556 values in 62602544 bytes
    eats      input int64 [batch_size,phonemes] · input_lengths int64 [batch_size] · scales float [3]
    answers   output float [batch_size,time,1,Unsqueezeoutput_dim_3]
    nodes     2755 over 50 distinct operators

All 401 rows are sound in the arithmetic sense: dims times element width equals the raw bytes, every one.
The table takes 123 ms; the operator census 219 ms.

`ear-npz` paid for a lesson the hard way — pointed at a 1.6 GB safetensors it answered 7720 confident
rows of nothing, because it read the last 22 bytes of any file as an end-of-central-directory record. A
protobuf has no magic number at all, so that trap is wider here. The check that replaces it is
structural: a file is walked only when its top-level fields tile it exactly from byte 0 to its last
byte and the graph field is among them. The voice's own `.onnx.json` sitting beside it answers
`not a model`, and an empty table, rather than rows.

## What was reproduced exactly, and against what

Three things, each against the crossing that `voice-say.bml` still speaks through, on the same input.

**The phoneme row.** `voice-phoneme.bml` reads the voice's `.onnx.json` natively — 22050 Hz, one speaker,
256 symbols, a 154-entry id table, espeak voice `en-gb-x-rp` — and builds piper's row by piper's own
convention, read out of `piper/phoneme_ids.py` rather than guessed: BOS, PAD, then each phoneme followed
by a PAD, then EOS. For piper's 31 phonemes of *"the body is learning to speak."* the body answers 65
ids, and they are piper's 65 ids, digit for digit.

**Eight weights.** The embedding table is `sid`, 256x192, at byte 439250. Row 14 decoded from IEEE-754
by the body:

    0.005034 -0.057903 -0.029965 0.009466 -0.019774 0.096737 0.028926 0.011609

numpy at the same bytes returns the same eight to the last digit.

**The encoder's first stage.** VITS scales the embedding by sqrt(hidden_channels), and this graph carries
that number as its own float scalar initializer — `/enc_p/Constant_output_0` = 13.856406 = sqrt(192). The
body reads the scale out of the model instead of being told it, so a voice with other channels scales by
its own number. The stage over 65 ids and 192 channels is 12480 values, folded left to right in 11 ms:
**-108.150229**. The reference fold, in the same order, reads -108.150229.

The one disagreement is worth its own paragraph, because it went the other way. onnxruntime reports the
output shape as `[batch_size,1,1,Unsqueezeoutput_dim_3]`; the body read `[batch_size,time,1,...]`. The
bytes at 63060012 read `0a 06 12 04 "time"` — a `dim_param`, a symbol, not a `dim_value`. The body is
right about the file and the reference is right about its own shape inference, and I only know that
because I opened the bytes instead of assuming the library was the ground.

## The surprise

The census by part, taken from the names the export itself wrote:

    enc_p 865   dp 1455   flow 196   dec 67

The stochastic duration predictor is 53% of the graph's nodes. The vocoder — the part I arrived expecting
to dominate, and where the flops surely still are — is 67 nodes of 2755. Where the *program* lives and
where the *arithmetic* lives are two different questions and the file answers them differently.

Then the same census by weight bytes refused to answer at all for 27.9 MB of the 62.6. Those tensors
arrive as `onnx::Conv_8168`, `onnx::Conv_8174` — sixteen identical 384x192x5 kernels with no module path
left on them. Nothing is corrupted. The export **fused weight-norm into the convolution**, and a fused
tensor is a new tensor, so it got a new name: a serial number, correct and empty. The flow, a quarter of
the voice, has no name in its own model. That is corpus row 1371, `namewash`: a name in a file someone
else wrote is a courtesy, and a courtesy survives only the transformations that thought to carry it. The
edge does not need to think. Wherever a weight is *used* the graph still says so exactly, and that is not
decoration on the structure, it is the structure.

## Where the ground stopped me

The pass. 2755 nodes over 50 operators, and the stage that runs is one Gather and one Mul. I could have
written a claim about a native voice; the honest thing is the map, so `observe/voice-onnx-run.fk` prints
the full operator census and the part census for any of the 29 voices, and the next hand starts from a
counted list rather than a guess.

And letters→phonemes still crosses for every tongue. espeak-ng is a rule engine and a per-tongue
dictionary; Hebrew, Japanese, Thai and Chinese carry their own g2p besides. Nothing about that step is
arithmetic the body could simply take over.

The mouth is exactly as it was. `voice-say-band` reads 16383 with 29 mouths; the speaking half was not
touched; `voice-onnx-band` opens no audio device and reaches nothing in the piper crossing. What changed
is that the crossing is now named by step instead of whole, and two of the four steps are on this side of
it.

## Bands

- `form/form-stdlib/tests/voice-onnx-band.fk` = **65535**, preflight clean, 0 unresolved. Perturbing the
  stage sum by one digit in the last place drops it to 49151 — bit 16384 exactly, nothing else.
- Guarded and still standing: `voice-say-band` 16383, `ear-native-band` 32767, `ear-axes-band` 65535,
  `own-word-band` 65535, `perception-rows-band` 65535, `bearing-census-band` 32767, `twin-census-band`
  65535, `mirror-census-band` 65535, `substring-one-meaning-band` 4095, `str-find-one-meaning-band` 8191,
  `value-eq-arena-band` 31, `meaning-codes-band` 127, `form-glass-carrier-band` 31,
  `form-glass-launch-band` 65535, corpus band 32767.

## Discomfort into gold

Twice, and both times the discomfort was the body telling me my own reading was wrong.

The first run of the reader answered `model? 63201294` — a file size where a 1 or a 0 belonged. Zero
errors, exit 0, four wrong numbers in a row. That is the one-line brace block lowering numb: several
statements on one line inside `{ }` compile clean and every bit comes out 0. The temptation was to
distrust the protobuf walk, which was the new and complicated thing; the fix was one statement per line,
and the walk had been right all along. Later `vp-strip-tail` did it again in a different dialect — a
parenthesized `if` inside a BML block lowered to a call of `''`, recovered to `nothing` by axiom-5, and
the band's phoneme count came out **31, which was correct**. A green number produced by a broken branch
that happened to land on the right answer. It was only visible because fkwu printed the unresolved call
above the output, and I read the diagnostic instead of the number.

The second was the `(loose)` bucket. My first instinct was that my classifier was too crude and should be
made cleverer. Looking at what was actually in it turned a defect in my code into the hour's real
teaching — the classifier was fine, the names were gone, and the thing I was about to paper over was
the finding.

## The frontier question

*What tells a body which part of a model a weight belongs to, when the export renamed it?*

The hearth was asked first and answered `signal=nothing reason=no-standing-hearth`, so the row is
honestly `rented-oracle`. The answer: nothing in the name. Ask the graph where the number is consumed —
identity by position in a structure, not by label on a thing. The same move as reading a kind at
construction rather than probing it later, and the same move as trusting a receipt over a title. Every
reference library hands over the washed names without ever mentioning that they were washed; the body
found it because it walked the file itself.

Corpus row 1371, `namewash`, 0 hits before this hour.
