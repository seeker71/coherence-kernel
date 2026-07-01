# Speech corpus crossvoice capture batch 0004

This receipt grows the corpus in the direction the sufficiency gate asks for:
same translated keys, two host voices, five locales. It does not claim training
sufficiency or native speech authority.

Live shard result:

```text
candidate wav rows: 50
local-oracle-clean admitted rows: 35
local-oracle-rejected rows: 15
observed wav bytes: 2150026
max WER across screened candidates: 250
shards: en 10/10, de 5/10, es 7/10, fr 4/10, pt-br 9/10
native neural parameters: 0
rows used for training: 0
status: screened-crossvoice-corpus-audio-not-training-sufficient
```

Aggregate after this batch:

```text
live wav rows: 191
observed wav bytes: 6806882
captured corpus admitted rows: 99
data-sufficient training: false
```

Witness:

```sh
cat learn/coherence-network-self-corpus.fk \
    learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/tests/speech-corpus-crossvoice-capture-batch-0004-band.fk > /tmp/speech-corpus-crossvoice-capture-batch-0004.fk
./fkwu --src /tmp/speech-corpus-crossvoice-capture-batch-0004.fk
```

```text
8191
```

Live shard summaries were run with:

```sh
cat observe/stt-wer.fk \
    observe/wav-sense.fk \
    learn/macos-sema-teacher-acoustic-learning.fk \
    learn/coherence-network-self-corpus.fk \
    learn/speech-model-metrics-report.fk \
    learn/speech-learning-data-sufficiency.fk \
    learn/speech-corpus-acquisition-window.fk \
    learn/speech-corpus-crossvoice-capture-batch-0004.fk > /tmp/sccb4-live-LANG.fk
# append:
# (do (let r (sccb4-run-shard-receipt "LANG"))
#   (add (mul (sccb4-rec-count r) 1000000000000000)
#        (add (mul (sccb4-rec-ok r) 1000000000000)
#             (add (mul (sccb4-rec-max-wer r) 1000000000)
#                  (sccb4-rec-bytes r)))))
```

```text
en    10010025000451904
de    10005250000468296
es    10007166000404394
fr    10004133000350394
pt-br 10009033000475038
```

Boundary: these are screened corpus rows. Misses are retained as controls; only
local-oracle-clean rows are admitted. The model still has `0` native neural
parameters, `0` native vocoder authority, and the sufficiency gate remains
false.
