# The ear's ground truth, synthesized

Twenty-four sentences, four each in English, German, Brazilian Portuguese, Indonesian, Spanish and French.
`observe/ear-truth-set-run.bml` speaks each one with the first local macOS voice of its locale (`say`, on this
Mac, no network) into `.hearth/ear-truth/`, writes the reference text beside each clip and a manifest the WER
door reads (`observe/stt-wer-fixtures-run.fk`). The audio is regenerable and stays out of the tree; the
sentences are the truth.

Synthesized speech is cleaner than a room: this set measures that the ear hears the words in six tongues, not
that it hears a person in noise. Persian has no local voice on this Mac; the three stored references in
`../whisper-native/` still carry it.
