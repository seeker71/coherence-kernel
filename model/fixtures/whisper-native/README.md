# Stored multilingual speech references

These are synthesized samples of the body's public sentence about finding its mouth, generated for the 2026-09-07 voice comparison. The accompanying `.txt` files are the stored Whisper large-v3-turbo reference transcripts from that comparison, not new reference execution and not corrected spellings. In particular, the Persian reference retains its colloquial contraction and its spelling.

The native witness reads the original 22,050-Hz WAVs and resamples them itself. No microphone, TTS process, MLX package or Python process runs during this witness. The samples come from the 2026-09-07 comparison of Piper voices heard by Whisper large-v3-turbo.

Voice attribution: [Cori](https://huggingface.co/rhasspy/piper-voices/blob/main/en/en_GB/cori/high/MODEL_CARD), trained from public-domain LibriVox recordings; [Thorsten](https://huggingface.co/rhasspy/piper-voices/blob/main/de/de_DE/thorsten/high/MODEL_CARD), CC0 dataset; [Amir](https://huggingface.co/rhasspy/piper-voices/commit/b22aa2aa275d5e8d528994782814cd8bf9d0337f), CC0 dataset. These files are generated audio, not model weights.

SHA-256 of WAVs:

| File | SHA-256 |
|---|---|
| en_GB-cori-high.wav | bb6758d32d18778790b81b56a5e9e489f57397384aeb0ce1ca6b644cde5d5527 |
| de_DE-thorsten-high.wav | 68e963983654cd8f39fda3c91895ae604a2cec8540cbf6da96872537737a1f59 |
| fa_IR-amir-medium.wav | ecefba11efeea0293bc0d166ef884aa0896bba19d2483617b311bdc5ab5c575e |
| lingua-libre-book-16k.wav | 1166acadc40e8d60baa82c6321ba3445fda5305a46539c3d1a0cc43e425de523 |

`lingua-libre-book-16k.wav` is one spoken word, "book", PCM-S16LE 16000 Hz mono, CC0, from [Wikimedia Commons LL-Q1860 (eng)-Simplificationalizer-book](https://commons.wikimedia.org/wiki/File:LL-Q1860_(eng)-Simplificationalizer-book.wav). The witness pins its exact tokens (`book.`, 5 sampled, 7 forward) and builds its two-window, silent and tail cases from it.
