# Speech corpus adaptive acquisition

Batch 0004 should change what the next capture does. This receipt makes that
choice executable: high-yield lanes expand, medium lanes retry under A/B, and
low-yield lanes get repair recipes.

Observed input:

```text
en:    10/10, max WER 25
de:     5/10, max WER 250
es:     7/10, max WER 166
fr:     4/10, max WER 133
pt-br:  9/10, max WER 33
```

Decision:

```text
expand: en, pt-br
retry A/B: es
repair: de, fr
next lane: fr
next recipe: repair-voice-family-and-shorten-phrases
controls: choice-cut-fail-undo-timeout-clean
native neural parameters: 0
```

Witness:

```sh
cat learn/speech-corpus-adaptive-acquisition.fk \
    learn/tests/speech-corpus-adaptive-acquisition-band.fk > /tmp/speech-corpus-adaptive-acquisition.fk
./fkwu --src /tmp/speech-corpus-adaptive-acquisition.fk
```

```text
32767
```

Meaning: the algorithm moved from fixed acquisition to observation-conditioned
capture planning, without pretending the model itself is trained or globally
native.
