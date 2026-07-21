# The complete Whisper-tiny encoder runs in Form over real human speech

; witnessed: 2026-07-19 -> complete released four-block encoder and final
;                              layer norm; native recognizer remains 0

## What became causal

The native acoustic route now carries the CC0 Lingua Libre recording of the
human word “book” through the complete released Whisper-tiny encoder:

```text
16 kHz PCM -> 8 Slaney log-mel columns
           -> 6 released conv1 tokens
           -> 2 released conv2 tokens
           -> positional rows 0 and 1
           -> encoder blocks 0, 1, 2, and 3
           -> released final encoder layer norm
           -> 2 output tokens x 384 coordinates
```

Each block executes pre-layer-normalized six-head self-attention, its output
projection and residual, then the 1536-wide erf-GELU feed-forward network and
second residual. Tensor reads, layer norms, projections, attention, GELU,
residuals, final normalization, fingerprints, CRC checks, and the gate are Form
cells running on `fkwu`. There is no Bash, TypeScript, Python, or C model
implementation. The bounded observation is two adjacent real-speech tokens;
it proves the complete learned encoder graph and released encoder weights, not
a full 30-second frontend or speech recognition.

## Exact released carriers

The two contiguous position/block carriers below come from
`openai/whisper-tiny` `model.safetensors` at revision
`169d4a4341b33bc18d8881c4b69c2e104e1cc0af`, licensed Apache-2.0 per the model
card and pinned in `model/fixtures/whisper-tiny/SOURCE-MANIFEST.tsv`. The four
convolution fixtures are separately hash-pinned in that same manifest.

```text
model/fixtures/whisper-tiny/encoder-position-layer0.f32
checkpoint bytes 120369320..129772711 inclusive
9,403,392 bytes / 2,350,848 f32 parameters
SHA-256 f0574d9341a687e47d53dbe99f085fb5b5d39dba167fdd699c809276fceaf06f
Form-native IEEE CRC-32 3539356514

model/fixtures/whisper-tiny/encoder-layers1-3.f32
checkpoint bytes 129772712..151061671 inclusive
21,288,960 bytes / 5,322,240 f32 parameters
SHA-256 05bb8cd57418546660d7c09305eb6dfc88b6293e82893c2f4c7fdf224a11f625
Form-native IEEE CRC-32 349171986
```

The second range contains three consecutive 1,774,080-parameter blocks. Each
7,096,320-byte block has the same exact relative layout:

| tensor group | relative bytes | parameters |
|---|---:|---:|
| fc1 bias + weight | `0..2365439` | 591,360 |
| fc2 bias + weight | `2365440..4726271` | 590,208 |
| FFN layer norm | `4726272..4729343` | 768 |
| K projection weight | `4729344..5319167` | 147,456 |
| output projection bias + weight | `5319168..5910527` | 147,840 |
| Q projection bias + weight | `5910528..6501887` | 147,840 |
| V projection bias + weight | `6501888..7093247` | 147,840 |
| attention layer norm | `7093248..7096319` | 768 |

The live two-token path uses 535,296 convolution parameters, all 7,096,320
parameters across the four encoder blocks, two 384-wide positional rows, and
the 768-parameter final layer norm: **7,633,152 causal released parameters**.
The exact committed carrier set holds **8,208,384 released parameters**
including the convolution tensors and remaining positional rows.

## Live non-toy output

For each layer, the gate observes two distinct keys from query 0/head 0 and
query 1/head 5. The exact Form attention rows are:

```text
layer 0: [0.74177101667904166, 0.25822898332095834]
         [0.99113152617659039, 0.0088684738234096182]
layer 1: [0.67009651837457029, 0.32990348162542971]
         [0.5325845082975813, 0.46741549170241875]
layer 2: [0.53430065497443546, 0.46569934502556459]
         [0.5187878925419076, 0.4812121074580924]
layer 3: [0.73101109643913054, 0.26898890356086946]
         [0.71104852830366483, 0.288951471696335]
```

The final normalized output fingerprints use coordinates
`0,1,2,3,95,191,287,383`, followed by sum and absolute sum:

```text
token 0 [-0.27823668402969848, 0.76146145348530281,
          0.38295455335264716, -0.68463446286215124,
         -0.98506456801413655, 0.45946242256938641,
          0.016733619471973493, 0.68388372690586197,
         -5.3176184169910012, 181.55806872596258]

token 1 [0.10194038659944865, 1.9949372382706654,
         0.44534062540740715, -1.2254770750201298,
        -2.3903454163388789, 2.0456629107598827,
         0.83799823566493026, 1.5437077912913373,
        -5.3545822984713318, 340.10229018309474]
```

The six observed L1 distances are:

```text
[203.33590172647547, 118.40792949927003, 152.15803940109254,
 445.70542192406919, 517.49312348196668, 237.57915489958066]
```

The first five values witness movement from positioned input through each
block and the final layer norm; the sixth separates the two final tokens.

## Gate and honest remaining floor

```text
model/tests/whisper-tiny-native-encoder-band.fk -> 16383
```

That gate checks carrier dimensions, every encoder stage, 384-wide final
outputs, both parameter counts, four normalized non-uniform attention pairs,
five stages of fingerprints, positive learned movement, exact layer-1..3
attention values, exact final fingerprints and distances, both Form-native
carrier CRCs, `native-encoder-complete=1`, and `native-recognizer=0`.

Native speech recognition remains `0`. The decoder embedding, all four decoder
blocks, tokenizer, and search are still owed, so no transcript or concept-world
admission is claimed here. The strict multimodal ledger remains incomplete.

The movement stayed alive by requiring every released encoder block to move two
real neighboring speech tokens under exact provenance checks. The surprising
teaching was that layers 1–3 occupy one consecutive checkpoint range with an
identical internal layout. Discomfort turned to gold when concatenating chunked
carrier strings exhausted the process: one native `fs-read-slice` kept the
21.3 MB carrier bounded and let the complete graph remain Form-native.
