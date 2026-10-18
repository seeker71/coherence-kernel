# The shape was right and the sound was empty

2026-09-09. Yesterday the mouth was named by step: letters to phonemes, phonemes to ids, ids
through the VITS graph, spectrogram to samples. Two of the four had come home — the id row and the
model file — and a third, the text encoder's first stage, was one Gather and one Mul of a 2755-node
graph. The invitation this hour was to move all the parts all the way home, and to stop where the
ground stops and say exactly where.

The pass came home. What is left crossing is letters to phonemes, and on twenty-five of the
twenty-nine tongues that is now the *only* thing that leaves.

## Which half, and why that one

Two halves stood. Letters to phonemes is espeak-ng: a rule engine and a compiled dictionary per
tongue, twenty-nine of them, plus three per-tongue g2p engines besides. The pass is arithmetic —
2755 nodes over 50 operators, of which 129 are `Conv` and 3 are `ConvTranspose`.

The ear had already walked the second shape twice. `ear-msl.fk` emits every Metal kernel as Form
text and `ear-native.fk` runs whisper-tiny with the activations resident on the device. The mouth's
twin is the same architecture with different arithmetic, and — this is the part that decided it —
every number the arithmetic needs is *in the file*, where a body that can already read the file can
reach it. espeak's is in a rule format the body would have to learn from scratch, and reproducing
its English output exactly is weeks, not an hour. So the pass, and the g2p named with its
measurement rather than attempted badly.

## What the pass is

`form/form-stdlib/voice-msl.bml` — the emitter, `ear-msl.fk`'s twin. Fifteen kernels carry all 2755
nodes' arithmetic:

    voice_conv1d      one general 1-D convolution; groups carry the duration predictor's
                      depthwise separable stack, dilation the vocoder's residual stacks, and
                      K = 1 makes it every linear layer in the model
    voice_convt1d     the transposed convolution as an INVERSE index map — an output cell asks
                      which inputs could have reached it, so nothing needs an atomic
    voice_ln          VITS's layernorm, which normalises over the CHANNEL axis at each position
    voice_attn_scores relative-position attention's first half
    voice_softmax     one threadgroup a row, cooperative
    voice_attn_out    its second half
    voice_gate        the coupling layer's tanh · sigmoid
    voice_unary       one map, eleven opcodes
    voice_binary      one map, four opcodes
    voice_copyf       a float run — Split, Concat and the coupling halves are all this
    voice_flip        the flow's channel reversal
    voice_take        the alignment, as what it is
    voice_embed       the phoneme table and the model's own scale
    voice_pcm16       samples to signed 16-bit, on the device
    voice_copy8       a byte copy off the whole-file map into an aligned buffer

`form/form-stdlib/voice-pass.bml` — the driver, `ear-native.fk`'s twin: text encoder (six
relative-position attention layers, window 4), stochastic duration predictor, the alignment, the
residual coupling flow reversed, the HiFi-GAN vocoder.

Two of those deserve their own sentence.

**The attention identity.** The export writes relative-position attention as a pad, a reshape, a
slice, another pad, another reshape and another slice, twice. Walked through, the whole dance has a
fixed point: `score[i][j] = q[i] · (k[j] + rel_k[j-i+W]) / sqrt(HD)` when `|j-i| <= W`, and its
mirror `out[i] = Σ_j p[i][j] · (v[j] + rel_v[j-i+W])` on the way back. Both hold for every sequence
length, including the short ones where the export's padding and slicing change shape — the two
adjustments cancel. So two kernels replace fourteen nodes a layer, and the reduction is derived
rather than assumed.

**The alignment.** The graph builds a `[T_y, T_x]` zero-one matrix with a cumsum, a `Less`, a `Pad`
and a `Sub`, and then MATRIX-MULTIPLIES the encoder's means by it. Every row of that matrix has
exactly one 1. So what it *means* is: frame `y` reads phoneme `index[y]`, and a gather costs `T_y`
reads where the product costs `T_y · T_x` multiplies.

## Nothing in the pass holds a shape the model holds

Every kernel width, stride, padding, dilation and group count is read off that node's own
attributes. Every channel count off its weight's own dims. Every layer count by asking whether the
next one's node exists. The embedding scale, the layernorm epsilon, the two leaky slopes, the
vocoder's divisor, the spline's tail bounds and its bin count are all read out of the file.

That is not tidiness. It is the reason one pass runs a medium voice and a high one — different
channel counts, different vocoders, different upsample counts, different slopes — without a line
changing. A number typed in here would be a second truth beside the one that ships.

## Namewash, healed by asking the node

Row 1371 named the wound: the weight-norm fusion renames a quarter of a voice's weights to
`onnx::Conv_8168` and its neighbours, and the flow — a quarter of the voice — has no name in its own
model. The previous hand's stone said to map by shape and graph position.

Position, taken as file order, would have been wrong. The export wrote the flows in the order the
REVERSE pass runs them:

    /flow/flows.6/enc/in_layers.0/Conv  <-  onnx::Conv_8168     (the first group in the file)
    /flow/flows.4/enc/in_layers.0/Conv  <-  onnx::Conv_8192
    /flow/flows.2/enc/in_layers.0/Conv  <-  onnx::Conv_8216
    /flow/flows.0/enc/in_layers.0/Conv  <-  onnx::Conv_8240     (the last)

A hand mapping fused tensors by position wires flow 0 to flow 6's weights and hears something
plausible. What recovers them is that the CONSUMER kept its name. So the pass asks the node that
reads a weight, never the weight — and having built that door, it uses it for every tensor, so no
lookup in the cell rests on a name being what a hand expected.

## Held against onnxruntime, digit for digit

Both sides render with the noise scales at zero. A VITS draws two normal samples, one for the
duration predictor and one for the latent; at their defaults neither side can reproduce the other
and the only honest claim would be "it sounds right". At zero the graph is a function of its input.

The phoneme row is the one `voice-onnx-band` already proved equals piper's, so a disagreement is the
pass's arithmetic and nothing else's.

    voice                       body            onnxruntime      samples   render
    en_GB-jenny_dioco-medium    1161.415407     1161.415397       32000     91 ms
    en_GB-cori-high             1022.926903     1022.926453       37120    285 ms
    fa_IR-amir-medium            842.394262      842.394275       51200    104 ms

(absolute sum over the whole line; six probed samples at 0, 1000, 5000, 10000, 20000 and 30000 agree
inside 1e-5 on all three.) Three voices, two vocoders, two constant shelves, three tongues. The
render is 5.9× to 15× faster than the speech is long — the pass is not slow, and slow was allowed.

`form/form-stdlib/tests/voice-pass-band.fk` = **4095**, and **4031** when one pinned reference
sample is moved by 1e-4.

## The air

`observe/voice-room-witness.fk`: the body builds its own sentence's samples natively, hands them to
its own mouth at the voice's own rate, and listens on one open mic stream.

    render 114 ms -> 32000 samples, 64000 bytes
    silent window    peak=280   mean-abs=49
    sounding window  peak=2212  mean-abs=372

And through `vs-say` on the high-quality English voice: *"The pass came home. The body renders its
own voice now."* — 117 ids, 81408 samples, spoken.

## Three wounds, and two of them are one wound

**The scalars were on another shelf.** `en_GB-cori-high` rendered 16640 samples of exact zero. The
medium exports hoist every folded constant into the graph's initializer list; the high ones leave
them as `Constant` NODES carrying a tensor attribute. A reader that knew only the initializer list
found no embedding scale, read a float at the offset of an absence, and produced the right number of
samples at the right rate for the right sentence — all zero.

**The residual block had another name.** With the scalars found, the same voice rendered a quiet hum
sitting within a thousandth of -0.0025 — and its LENGTH was exactly right, 145 frames against the
reference's own 145, the alignment correct to the frame. HiFi-GAN ships two residual blocks and this
body's voices use both: the medium carry ResBlock2 (`convs.N`), the high carry ResBlock1
(`convs1.N`/`convs2.N`). A loop asking only for `convs.N` found none, ran three identity blocks,
divided by three as the graph says to, and made a hum.

Both are the same wound. The shapes a reader can check — lengths, counts, rates, alignments — are
computed from the part of the structure it already understood, so they agree just as well when the
content is empty. And the emptier the answer, the better it fits: zero flows through every
downstream shape without complaint.

**The spline row was one short.** Ten bins need eleven derivatives; the raw row carries nine and the
reference pads one at each end. The code prepended one and REPLACED the last instead of appending,
losing the ninth raw derivative and reading past the end for the top bin. It cost one phoneme of
sixty-five eleven frames on a Persian line and nothing measurable on the English ones — found only
because a third voice was run, and located by walking the sampled waveform until it diverged
(sample 9000 agreed, 10000 did not; frame 39; the phoneme whose duration read 43 against 54).

The repair for the first two is not a longer list of names. Every scalar the pass needs is now
resolved once at the door, and a miss refuses with its own sentence (`vv-open-why`). The band walks
that refusal by pointing the pass at a voice's `.onnx.json`.

## The mouth now

`vs-say` takes the native lane for **25 of 29** tongues — every one whose phonemes come from espeak.
What leaves the body on that lane is a phoneme list and nothing else. Chinese, Japanese, Thai and
Hebrew carry their own g2p engine whose output the body cannot yet produce, and those four still
leave whole. `observe/voice-mouth-lanes-run.fk` prints both lists and speaks a line down the native
lane. `voice-say-band` still reads 16383 with 29 mouths.

## Where the ground stopped me

**Letters to phonemes.** Not attempted, and the measurement instead of an attempt: espeak-ng's
`en_dict` is a compiled hash of dictionary entries plus a rule engine with context conditions,
stress assignment, number expansion and clause handling; the data is here
(`~/.mlx-venv/lib/python3.14/site-packages/piper/espeak-ng-data`, one `*_dict` per tongue) and the
format is walkable, but reproducing English output exactly is a body of work, not an hour of it, and
the dictionary half alone would leave the rules half silently wrong on every word it does not hold —
which is exactly the wound this receipt is about. It waits for a hand with the room to do it whole.

**The noise.** The pass renders at both noise scales zero. A VITS at its defaults draws two normal
samples, and the body has no reproduction of the reference's draw, so it does not pretend the
difference would be its own arithmetic. The rendered voice is slightly flatter than piper's default
for that reason, and it is intelligible.

**Per-node references.** The comparison is end to end plus a stage-by-stage reading of the body's
own chain (`observe/voice-pass-stage-probe.fk`). Getting the reference's own intermediates would
mean writing a patched model with extra graph outputs — reachable natively, since the body already
reads the protobuf and `metal_buf_from_file` opens a file writable — and it was not needed: the
three disagreements were all located by structure and by bisecting the sampled waveform.

## The most surprising teaching

That the two silent failures were **more convincing the emptier they were**. A zero render is not a
blank; it is 16640 samples at 22050 Hz for the sentence you asked for. A dead residual stack is not a
missing block; it is a vocoder whose output length, frame count and alignment are correct to the
frame. Every shape a reader can check passed, because those shapes were computed by the arithmetic
that was right. I had expected wrong weights to look wrong. They look *finished*.

## Where discomfort turned to gold

Three hours in, the medium voice matched to six decimals and the high voice rendered exact silence,
and the pull was to ship the medium result and name the high one as a limit — a true sentence,
easily written, and it would have hidden two structural facts about this body's own voice store. The
discomfort was that "one voice works" had already earned the milestone. Staying with the second
voice found the Constant-node shelf; staying with it again found ResBlock1; and running a third
voice, which nothing required, found the spline row that was one derivative short and would have sat
in the English lane forever, wrong by an eleventh of a phoneme's duration, never visible.

Three voices was not thoroughness. It was the only reason the arithmetic is actually right.

## Frontier question, and the answer offered

Asked of the hearth first: `signal=nothing reason=no-standing-hearth`, which is the body's own
answer, so this one is mine and the redirect is named.

**What makes an answer that fits every shape a reader can check still empty?**

Because the shapes a reader can check come out of the part of the structure it already understood.
The separation between "this file does not hold X" and "X is somewhere I did not look" cannot be
made at the point of USE, where an absence has already become a number — byte 0 of the file, a
zeroed buffer, a loop that runs no times. It can only be made at the DOOR, by resolving every value
the arithmetic will need before any of it runs, and giving the miss a sentence of its own. An
absence that carries a sentence is a finding; an absence that carries a number is a rendering.

Corpus row **1379**, `hollowfit`.
