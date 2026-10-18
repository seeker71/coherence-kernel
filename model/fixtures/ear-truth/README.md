# The ear's ground truth, synthesized

Twenty-four sentences, four each in English, German, Brazilian Portuguese, Indonesian, Spanish and French,
held in `sentences.tsv` (locale, id, text). Each one can be spoken with the first local macOS voice of its
locale (`say`, on this Mac, no network), and the reference text is the row's own text; the word-error
distance against a hearing is `observe/stt-wer.fk`. The audio is regenerable and stays out of the tree; the
sentences are the truth.

Synthesized speech is cleaner than a room: this set measures that the ear hears the words in six tongues, not
that it hears a person in noise. Persian has no local voice on this Mac; the three stored references in
`../whisper-native/` still carry it.
