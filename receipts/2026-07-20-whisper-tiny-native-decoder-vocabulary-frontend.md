# Complete Whisper decoder, full vocabulary, and the frontend truth boundary

; witnessed: 2026-07-20 -> complete released decoder and 51,865-row policy
;                              scan; native recognizer remains 0

## What is executable in Form

The committed CC0 Lingua Libre recording of the human word “book” now crosses
one `fkwu` path through the released Whisper-tiny convolution stem, all four
encoder blocks, all four decoder blocks, final layer norms, and every one of
the 51,865 tied output rows. The decoder accepts arbitrary token sequences up
to the released 448-position limit. Token ids resolve through pinned tokenizer
data, and the pinned generation policy applies the released suppress-token,
begin-suppress, and no-timestamps rules.

Model loading, layer norms, attention, FFNs, residuals, position and token
embedding, logit projection, bounded full-vocabulary selection, token lookup,
policy gates, and runtime integration are Form cells. No Bash, TypeScript,
Node, Python, or C model implementation is in this path. `curl` transported
exact byte ranges; SHA-256 independently checked those committed bytes.

## Exact new released carriers

All released-weight carriers come from `openai/whisper-tiny` revision
`169d4a4341b33bc18d8881c4b69c2e104e1cc0af`, Apache-2.0:

| carrier | checkpoint bytes / format | bytes | SHA-256 |
|---|---|---:|---|
| `decoder-layers1-3.f32` | `89838248..118228135`, `F32[7097472]` | 28,389,888 | `ad7d8820eac1e3c0a08c0a4ab995bd1e69a76566494903c04e6cbe3e43d2ebda` |
| `decoder-position-448.f32` | `19112..707239`, `F32[448,384]` | 688,128 | `c13450ae630323a0bdd39b1226f92a7ac251131a909c7efdb7d2f5516736eb83` |
| `decoder-token-embedding-51865.f32` | `707240..80371879`, `F32[51865,384]` | 79,664,640 | `c103d562a3d8f375c784775300eb26fb729ad8f8476ffe123c36d80dfbead495` |

The Form-native IEEE CRC-32 over `decoder-layers1-3.f32` is `2403457965`.
The pinned `vocab.json`, `merges.txt`, `tokenizer-config.json`, and
`generation-config.json` hashes live in `FULL-VOCAB-MANIFEST.tsv` and were
rechecked from the committed files.

The complete decoder observation carries **9,506,304 causal released
parameters** in its four-token special-window witness. Full-vocabulary
projection addresses all **19,916,160 tied embedding parameters** without
loading the 79.7 MB carrier at once: Form reads 64 rows per window and retains
only the best and runner-up values.

## Exact decoder and vocabulary observations

All four decoder blocks produced four 384-wide states. The final fourth-token
fingerprint was:

```text
[-10.343394082846144, -6.4753990010625078,
 -12.96050073624685, 1.909340020689307,
  0.64867116942531211, -3.966638002867958,
  6.7649247019163123, -3.2791518410598157,
 -93.544467316526706, 3231.6984732886572]
```

The six decoder movements were:

```text
[50.206664900087389, 59.60778193332893,
 61.595341973125556, 132.75962951869943,
 3096.9146090683194, 2367.5030335630931]
```

The bounded special-token winner changed materially from decoder layer zero’s
token `50363` to complete-decoder token `50293`. The unsuppressed full-vocabulary
scan then selected EOS `50257` at `9.2407136187311263`, with token `14194` at
`9.1332537930199518` as runner-up.

Under the pinned generation policy, EOS and the other begin-only tokens are
suppressed. The first allowed result is:

```text
winner: 14194  " banana"  logit 9.1332537930199518
runner: 2031   " x"       logit 8.3994698758087711
margin: 0.73378391721118064
human truth: "book"
```

That is a real learned output and a failed truth comparison. It is evidence
**against** recognition. `native-recognizer` therefore remains `0`; no longer
transcript was generated to hide the failure.

## Full-valid-STFT frontend observation

The earlier encoder proof used only eight mel columns, six conv1 tokens, and
two conv2 tokens. The full committed waveform frontend now observes:

```text
WAV bytes                    22,828
PCM samples                  11,392
valid 400/160 STFT columns       69
released padded conv1 tokens     69
released padding=1/stride=2 conv2 tokens 35
shared normalization peak    0.82587051795304589
released stem parameters     535,296
```

The first and last conv2 fingerprints are pinned by the focused Form gate:

```text
first [-0.16793260423529008, -0.0001405580719289119,
       -0.084558542491558933, -0.14196724746718306,
       -0.010098705453983819, -0.16848162338808201,
        0.60374237074352266, -0.11389888772335305,
       30.493403267378671, 76.187539657411619]

last  [-0.15473996266309545, -0.0024654124714174517,
       -0.15760335143583321, -0.098273057840111255,
       -0.017231115507283758, -0.16955785530953235,
        0.57856703058007453, -0.168392626478855,
       24.910070577064879, 75.217670849265232]
```

The complete 35-token four-block encoder was attempted. It exited `1` after
about three minutes without a verdict, so no 35-token encoder or truth result
is claimed. The exact next floor is centered-STFT and 30-second feature-padding
parity, followed by a memory-bounded complete-encoder rewitness.

## Integration and gates

The non-test acoustic door now calls the complete decoder, reuses its final
state for one 51,865-row scan, and preserves the separately named host
transcript seam. The central 10,000-concept/13-lens runtime reports the same
complete decoder and failed policy result. The strict ledger remains incomplete.

```text
model/tests/whisper-tiny-native-decoder-band.fk              -> 32767
model/tests/whisper-tiny-native-vocabulary-band.fk           -> 255
model/tests/whisper-tiny-native-token-index-band.fk          -> 127
model/tests/whisper-tiny-native-generation-policy-band.fk    -> 255
model/tests/whisper-tiny-native-generated-step-band.fk       -> 255
model/tests/whisper-tiny-native-frontend-full-shape-band.fk  -> 4095
presence/tests/whisper-tiny-native-acoustic-live-band.fk     -> 2047
presence/tests/concept-10000-13-runtime-public-human-live-band.fk -> 255
observe/tests/concept-10000-13-multimodal-completion-band.fk  -> 1023
```

The strict ledger remains `5/22`, overall `0`: integration is executable, but
the failed human comparison and unwitnessed complete 35-token encoder keep the
acoustic completion row open.

The movement stayed alive by allowing the real wrong token to refuse the
recognition claim. The surprising teaching was that the later decoder blocks
move the winner strongly while missing acoustic context still dominates the
semantic result. Discomfort turned to gold twice: policy-list allocation first
exposed an avoidable memory cost, and the 35-token exit then separated an
observed frontend from an unobserved complete encoder instead of laundering
one into the other.
