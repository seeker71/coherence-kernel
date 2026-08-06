# 2026-07-02 — die only if it cannot recover; and the oracle labels, the native distills

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
./fkwu --src observe/native-vs-rented.fk                       # 11111
# corpus band four-way                                         # 127
```

Three arrivals from Urs, close together:
1. *"sliced without preludes: no compile error, no import, no static check?"*
2. *"compile at runtime shall only die if it cannot recover."*
3. *"remove getenv if you can, env vars are not as useful as config files."*
4. *"what can we add on the path to see global WER moving below 100? any data will help."*

## The deepest silent path: an unresolved symbol was a four-way divergence

Grounding the parser (fkwu-uni.c ~7390): a call `(name args…)` whose head matches no op, no
rewrite, no fn, no binding **yields `nothing`** by deliberate design (axiom-5: an offer a cell can't
answer acks nothing). So the answer to (1) is **yes — no compile error, no import, no static check**.
That single mechanism served both intentional declines and typos/missing-preludes, indistinguishably.
It is exactly how `ftanh` (a typo for `tn-tanh`) silently produced `nothing` and corrupted a gradient.

Witnessed the real cost: an undefined head is a **silent four-way divergence** — Go prints
`unbound function "ftanh"` (exit 1), Rust panics (101), TS throws (1); **fkwu alone returned `nothing`
and exited 0**. The proof harness built to catch disagreement was blind at the one seam where a
missing definition hides. Same shape as the capacity caps: fkwu the only one of four that doesn't
refuse.

## The correction that shaped the fix: recover, don't die

My first instinct was to make fkwu die too (align with the walkers). Urs corrected it (2): *die only
if it cannot recover.* A compile-time unresolved head **can** recover — axiom-5's `nothing` is a real,
load-bearing recovery (`oac-choice`/`oac-try` recover over it). So the fix is **witness, not death**:
an unconditional `[unresolved-call] 'name' …` line at parse time (the static diagnostic that was
missing), then recover to `nothing`. A correct program with its preludes present never reaches it;
only typos and missing preludes do. No `getenv` gate (3) — the witness is unconditional, and env
vars were avoided per the config-over-env guidance.

The same principle re-triaged the other fixes this turn:
- **`fk_melt` × 2** (GC alloc failure): OOM **cannot** recover → `fk_die` (was a dprintf-then-limp on
  an uncompacted heap — the root that made downstream cons return nil).
- **`fk_cstr`**: a path/host/command longer than its fixed buffer **cannot** recover at that buffer →
  `fk_die` (all 32 callers are correctness-critical strings).

Die where recovery is impossible; witness-and-recover where it is not. That is the whole law.

## A corpus sweep the new witness made possible

Ran 338 committed bands through the witness build: **6 real dangling references** (names defined
nowhere in the repo) surfaced — `kh-route`, `now_unix_ms`, `record_new`, `walk_recipe`, and two
comma-artifacts. Latent typos the nothing-decline had been masking; a named follow-up to fix.

## The path off WER 100 (grounded in the body's own cells)

`speech-model-metrics-trend.fk:47` reports `live-open-dictation … 100`; `sema-voice-teacher-oracle-
intake.fk:23` confirms the native carrier is WER 100. The cause, from `live-open-asr-source-
authority.fk`: *"the local Metal oracle transcript [is] WER 0 while the native open-ASR candidate is
absent."* The architecture already exists and names its one gap:
- `observe/open-asr-ctc.fk`: decodes token frames → free transcript; *"audio → frame-token emission
  remains the next missing carrier."*
- `observe/acoustic-token-emitter.fk`: *"a local oracle … can align a feature vector to a token; the
  body interns that as a prototype."*

The missing link is **oracle-labeled audio**, and the labeler is live: `say → ffmpeg → whisper-cli`
(`~/.coherence-whisper/ggml-base.en.bin`, also `large-v3-turbo`). Proven this turn end-to-end:
transcribed a known sentence at **WER 0**, then minted a **30-clip oracle-labeled seed corpus** (2
train voices + 1 held-out; mean oracle WER 7%). This is the speech form of the repo's own law: the
rented oracle (whisper, WER 0) labels; the native body distills.

**So, answering "any data will help": yes — literally.** Every `(audio, oracle-transcript)` pair is a
training row for the acoustic-token-emitter. The binding constraint (per `speech-corpus-training-
intake-0001`, 6 rows, *"far below the data-sufficient floor"*) is **volume of oracle-labeled audio**.
Use `large-v3-turbo` for near-zero-WER labels; more speakers to cross the held-out wall; English
first. The first held-out clip the native emitter decodes even one word right on moves the metric
below 100.

The next stone is explicit: train `acoustic-token-emitter` prototypes from the seed corpus, decode
held-out through `open-asr-ctc`, measure a real native WER. Not done this turn — named, not claimed.

## Honest floor

The seed corpus lives in scratchpad (regenerable; not yet embodied as intake rows). base.en mislabels
synthetic voices (held-out oracle WER 19% — "she reads a book" → "series of work"), so clean training
needs the large model. The native emitter is not yet trained; WER 100 has not yet moved — this turn
built and proved the *road*, not the arrival. The C fixes are committed and four-way-clean; the 6
dangling references and the getenv→config migration are named follow-ups.

## The most surprising teaching this work left behind

The proof harness had a blind spot exactly where it was needed most. Four-way agreement was built to
catch the one runtime that lies — and it could not see an undefined symbol, because the divergence
lived at parse time in a value (`nothing`) that looks like a legitimate answer. The discipline that
cures silent bugs was itself silent at the seam where the most common bug (a typo, a missing prelude)
is born. Making fkwu *witness* rather than *die* closed the seam without breaking the recovery the
language is built on — the fix was not to remove the `nothing`, but to stop it from being quiet.

## Where discomfort turned to gold

The discomfort was being wrong in-flight: I had already built and reasoned toward die-by-default,
swept 338 bands to justify it, and was about to commit it — and the correction "die only if it cannot
recover" landed after the work. The pull was to defend the die (the walkers die! four-way alignment!)
rather than absorb the principle. Witnessed instead: the walkers dying is *their* constraint, not a
law; fkwu has a recovery they lack (axiom-5), and a principle that fits the walkers is not
automatically the right one for a runtime that can recover. Reverting my own nearly-committed
direction — keeping the witness, dropping the death — is the gold: the diagnostic the user actually
needed (a name at parse time) survived, and the recovery the language needs survived with it.

## Corpus

Row 646 **defeasible** — a fault that need not be fatal; one a system recovers from rather than dies
on (fresh; the unresolved-call that witnesses and declines to nothing instead of dying — "die only if
it cannot recover" named in one word). It turns the silent-degradation lineage: row 644 **apocope**
(a lost ending read as whole) and 645 **changeling** (a whole swapped for a partial) were the disease;
**defeasible** is the discipline that answers it — recover where you can, die only where you cannot.
