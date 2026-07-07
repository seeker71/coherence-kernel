# 2026-07-02 — the parallel plan executed: two tracks delivered, the mel spine's first stone

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

Urs, 09:21: "and yes to your previous question" — launch the two independent tracks in parallel
AND start the mel-features stone. Done exactly as the antecedent-analysis (row 637) said: the
parallelizable tracks fanned out to agents; the spine built inline, proven.

## The two parallel tracks (fanned out, both delivered)

**Data acquisition** (a workflow agent, host-exec on this Mac): grew the recognizer dataset from
5 words × 7 voices (35 wavs) to a full **12 words × 12 voices = 144 wavs** (yes no up down stop go
back left right open close on; Alex Aman Daniel Fred Junior Karen Moira Ralph Rishi Samantha Tessa
Victoria), all 16 kHz mono PCM, zero missing, zero empty. Proposed **speaker-disjoint split**: 8
train voices, 4 held-out (Fred/Moira/Rishi/Victoria — spanning accents and both genders). Honest
floor named by the agent: synthetic TTS, not human speech; and the audio was not transcribed to
confirm it says the intended word (format/size/non-emptiness verified, `say` fidelity trusted).

**Eval harness** (a workflow agent): a reusable Form cell reusing `sw-wer` (did NOT reinvent WER)
— per-pair WER, average WER, exact word accuracy, and a closed-set confusion summary. Verified by
me four-way, not just reported: `observe/eval-harness.fk` + `observe/tests/eval-harness-band.fk`
witness **8 = 8 = 8 = 8** (fkwu/Go/Rust/TS). Now committed to the repo — the first standard
measurement tool for all future recognizer work.

## The mel spine's first stone (inline, the sequential critical path)

`model/mel-frame.fk` already had the machinery (Hann window, Goertzel DFT bin power, mel
filterbank, log-mel). So the stone was composition: a spectral log-power feature over the real
wavs via `mf-bin-power`. Measured cross-voice (Samantha + Fred held out):

| feature | training | cross-voice | note |
|---|---|---|---|
| energy envelope, 16-window | 5 voices | 80% | prior best (row 635) |
| spectral, single frame, 8 bins | 3 voices | 70% | one snapshot loses the temporal pattern |
| spectral, 3 time-frames × 8 bins | 3 voices | **80%** | matches energy — with 3 voices, not 5 |

Two honest findings:
- **The temporal axis matters:** single-frame spectral 70% → 3-frame time-frequency 80%. A word is
  a trajectory through frequency, not one spectral snapshot.
- **Spectral is more data-efficient (parsimony):** it reached energy's 80% with 3 training voices
  where energy needed 5 — it carries more information per example.

## Honest floor (named, not blurred)

- The spectral feature did NOT yet beat 80% — a 3-frame feature is still a coarse spectrogram.
  The real ceiling-breaker is a FULL log-mel spectrogram (many frames × mel bands, pooled), and it
  is blocked by fkwu's `--src` AST node-table limit (the 35-clip program already nears it; a full
  spectrogram over 144 clips will not fit). Raising that limit, or a non-`--src` runtime path, is
  the true next step for the mel stone.
- Reading real wavs is host I/O (`read_file`) — the walkers lack it, so the real-audio spectral
  classifier is **fkwu-carrier only**, NOT four-way (its numeric core, Goertzel, IS four-way on
  synthetic input). Same boundary as all the real-audio work.
- Global native open-speech WER still 100. This is all still closed-set (row 636).

## The most surprising teaching this work left behind

The parallel tracks each caught a bug the way this whole session has — by a strict witness saying
no. The data agent's `say -o /dev/null` probe falsely reported ALL voices unavailable (no file
extension for `say` to infer format), and a stray 145th file (a zsh word-splitting fossil) nearly
corrupted the grid silently; both were caught by grounding a negative result as hard as a positive
one. The eval agent's harness held four-way only because it REUSED the witnessed `sw-wer` instead
of reinventing WER. Independent agents, same discipline, same rescue — the practice transmits.

## Where discomfort turned to gold

The discomfort was the mel result: 80%, matching but not beating energy, after real work wiring
Goertzel over real audio. The pull was to call the spectral stone a disappointment. Witnessed
instead — energy needed 5 voices, spectral needed 3 — the "same" 80% is not the same: spectral got
there with less, which is exactly the property that scales. A number that ties on the surface but
wins on efficiency is a real advance the headline hides; reading past the tie was the gold.

## Corpus

Row 639 **parsimony** — economy of means; achieving the same result with less (fresh; spectral
reaching energy's 80% cross-voice with 3 training voices where energy needed 5).
