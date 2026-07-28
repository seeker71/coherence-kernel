# Whisper-tiny encoder layer 0 enters Form over real human speech

; witnessed: 2026-07-19 -> complete released encoder layer 0; two-token
;                              six-head attention; native recognizer still 0

## What became causal

The prior native acoustic route stopped after Whisper-tiny's two convolutional
layers. This movement carries the same CC0 Lingua Libre recording of the human
word “book” through:

```text
PCM -> 8 Slaney log-mel columns
    -> 6 complete released conv1 tokens
    -> 2 complete released conv2 tokens
    -> released positional rows 0 and 1
    -> encoder layer 0 pre-LN six-head self-attention
    -> residual -> pre-LN 1536-wide erf-GELU FFN -> residual
```

The model arithmetic, tensor loading, attention, FFN, fingerprints, gate, live
door, and central-runtime call are Form running on `fkwu`. There is no new Bash,
TypeScript, or Python implementation path. The released bytes arrived once as
a pinned carrier; transport is not inference.

## Exact released carrier

Source: `openai/whisper-tiny` `model.safetensors`, revision
`169d4a4341b33bc18d8881c4b69c2e104e1cc0af`, Apache-2.0 per the model card.
The committed 9,403,392-byte carrier is the exact inclusive checkpoint byte
range `120369320..129772711`:

```text
model/fixtures/whisper-tiny/encoder-position-layer0.f32
SHA-256 f0574d9341a687e47d53dbe99f085fb5b5d39dba167fdd699c809276fceaf06f
Form-native IEEE CRC-32 3539356514
```

It contains 2,350,848 released f32 parameters:

| tensor group | relative bytes | parameters |
|---|---:|---:|
| encoder positional embedding `[1500,384]` | `0..2303999` | 576,000 |
| encoder final layer norm bias + weight | `2304000..2307071` | 768 |
| layer 0 fc1 bias + weight | `2307072..4672511` | 591,360 |
| layer 0 fc2 bias + weight | `4672512..7033343` | 590,208 |
| layer 0 FFN layer norm | `7033344..7036415` | 768 |
| layer 0 K projection weight | `7036416..7626239` | 147,456 |
| layer 0 output projection bias + weight | `7626240..8217599` | 147,840 |
| layer 0 Q projection bias + weight | `8217600..8808959` | 147,840 |
| layer 0 V projection bias + weight | `8808960..9400319` | 147,840 |
| layer 0 attention layer norm | `9400320..9403391` | 768 |

All 1,774,080 layer-0 parameters affect the live output. Two positional rows
add another 768 causal parameters. With the 535,296-parameter conv stem, the
live route has **2,310,144 causal released parameters**. It carries 2,886,144
released parameters in total; unused positional rows and the final encoder norm
remain correctly dormant until the rest of the encoder exists.

## Live non-toy output

The two queries see two distinct keys. Exact Form attention rows were:

```text
query 0, head 0: [0.74177101667904166, 0.25822898332095834]
query 1, head 5: [0.99113152617659039, 0.0088684738234096182]
```

The output token fingerprints (coordinates `0,1,2,3,95,191,287,383`, sum,
absolute sum) were:

```text
token 0 [0.28293116261900808, 1.021182505660037,
         0.31154958382802622, 0.35257270820981157,
         0.34697272218430097, 0.42518556741143587,
         0.50736601388153635, 0.71437550641998127,
         263.20700170505131, 267.56912866594985]
token 1 [1.0946487798677724, 1.6066674587889098,
         0.84306109135258123, 0.7556052428729666,
         0.41477385161879382, 0.5300788296668677,
         0.58213336144719985, 0.71869324712133764,
         274.34046496938782, 279.35663047097358]
```

The L1 movements were `203.33590172647547` from positioned token 0 to its
layer output and `53.456828315009794` between the two output tokens. A
one-token softmax cannot earn these claims.

## Integration and gates

```text
model/tests/whisper-tiny-native-encoder0-band.fk                 -> 8191
presence/tests/whisper-tiny-native-acoustic-live-band.fk        -> 511
presence/tests/concept-10000-13-runtime-public-human-live-band.fk -> 255
```

The central runtime's internal score is now `511`: its new bit calls this exact
real-speech encoder route and checks two 384-wide outputs, 2,310,144 causal
parameters, valid carrier dimensions, and learned movement. The public acoustic
door now gates its optional transcript/world bridge on encoder-0 activity, and
the zero-learned ablation remains rejected.

The committed-fixture execution door is native-only and does not invoke a host
decoder or write a transcript sidecar. In the separate host-decoder gate, the
current `whisper.cpp-large-v3-turbo` file still matches its pinned hash, but
the installed host executable produced no transcript in the live Form door; a
direct managed-sandbox probe exited `139` after loading its backends. The report
therefore records `model-valid=1`, `runtime-operational=0`, zero candidates, and
zero world admission instead of fabricating “book.” This host seam is not the
native recognizer.

## Honest remaining floor

Native speech recognition remains `0`. Encoder layers 1–3, final encoder norm,
the decoder embedding and four decoder blocks, tokenizer, and search are still
owed. The strict multimodal ledger therefore remains incomplete; this is one
complete real released block, not the complete goal.

The movement stayed alive by making two real tokens face one another instead of
letting a one-token attention identity pass as substance. The surprising
teaching was that the entire first encoder block and positional carrier occupy
one contiguous released range. Discomfort turned to gold when the host decoder
failed live: the world gate stayed closed while the native learned output
remained fully observable.
