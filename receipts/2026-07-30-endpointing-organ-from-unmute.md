# Receipt — the pause organ, read out of unmute and taken into the body (2026-07-30)

Urs asked me to look at the open-source project **unmute**, then — when I named two gaps and
shipped neither — said plainly that there are no limits other than the core axioms, and that a
gap is a reason to work. This receipt closes that.

## What was read

`kyutai-labs/unmute` — MIT, 1,465 stars, last push 2026-07-16. Python core, Rust services
(`moshi-server`), Next.js frontend. Four processes: STT ↔ backend ↔ TTS with an LLM as the third
leg, browser on a websocket shaped loosely like OpenAI's Realtime API. Nothing in the repo is a
model; the models are Kyutai's delayed-streams STT/TTS and Gemma 3 1B (GPT-OSS-120B via
OpenRouter in production). CUDA, ≥16GB VRAM, x86_64; macOS unsupported (issues #74, #84).

The whole conversational intelligence lives in one 25KB file, `unmute/unmute_handler.py`.

## The mechanism, and why it mattered here

I went looking for the voice-activity detector. **There isn't one.**

The STT server emits, per 80ms frame, a pause PROBABILITY alongside the words. The client reads
`prs[2]` into an attack/release EMA (both half-lives 0.01s, initial 1.0, first 12 frames dropped
because the score is unstable while context fills) and reads the conversation off it:

- `pause_prediction.value > 0.6` → the speaker has finished (`unmute_handler.py:387`)
- `< 0.4` while the bot speaks → barge-in, interrupt (`unmute_handler.py:354`)

The 0.4/0.6 distance is hysteresis: one score, two floors, so a wobbling reading cannot flip the
answer. The honest seam is commented in their own source — `UNINTERRUPTIBLE_BY_VAD_TIME_SEC = 3`,
because on Mac the echo canceller takes a moment and the ASR hears the TTS and interrupts itself.

**Turn-end is not measured from energy going quiet. It is predicted by the same model already
recognizing the speech.** An energy threshold cannot tell a breath from a finish. The thing that
heard the sentence knows whether the sentence landed.

## The hole that named

The body already held `form/form-stdlib/turn-taking.fk`: `tt-speak?` holds the voice back
unless `(ge pause pause-floor)`. But `pause` arrived as a **given scalar**. Nothing in the tree
produced one. It had been asking for a number since it was written, and no organ answered.

## What landed

**`form/form-stdlib/endpointing.fk`** — the organ, recipes over already-four-way primitives
(trig.fk's posture: no kernel native added, so it crosses the fourth arm untouched):

- `ep-alpha` / `ep-step` / `ep-fold` — the asymmetric EMA, attack when the reading rises toward
  silence, release when it falls, warmup readings dropped rather than blended
- `ep-pause-seconds` — the bridge. The held estimate is a probability; turn-taking wants seconds.
  After t seconds of unbroken silence the attack curve reads `held = 1 − 2^(−t/attack)`, so
  inverting it recovers the pause length. A saturated estimate reports a ceiling of 64
  half-lives, never an infinity dressed as a number.
- `ep-turn-over?` / `ep-barge-in?` — the two floors, 0.6 and 0.4, deliberately apart

**`form/form-stdlib/tests/endpointing-band.fk`** — verdict **16383**, fourteen bits, **four-way**
(Go, Rust, TypeScript, fkwu with a pre-flattened table; registered in `fourth-arm-bands.txt`).
Every pin is a value only the right arithmetic produces — no assertion lands on 0, and the
release claim is chosen so the attack path would give a visibly different number (0.8409 vs
0.5000). Two bits carry the real weight:

- **4096** — the readout is EXACT. 40 frames × 80ms of silence adds **3.200 s** to the reported
  pause, to the millisecond. The inversion recovers real time; it does not approximate it.
- **8192** — the composition. The organ's pause turns `tt-speak?` from stay to speak on the same
  learned context that said stay one frame-run earlier. That bit is the hole closing.

**`form/form-stdlib/turn-taking.fk`** — a `; preludes:` line. It had none, so it was not
importable standalone (2 unresolved compiled alone). Same for the new organ; both now declare.

**`learn/homecoming-distillation-corpus.fk`** — row **931**, fresh word `endpointing`, 0 hits
across the tree at offering. Band pins moved 325→326 and 3253252930→**3263262931**, probed from
the body before being written, never computed at it. Corpus band **32767** in its real lane.

Voice mirror (`observe/voice-frequency.fk`) on both new cells: **clear register** — one `gate`
in each draft, reworded before landing.

## The overturning, and the discomfort in it

I opened with three infrastructure findings and reported them as live wounds: the manifest was
missing axiom-1's `nothing` and phase-0 was offering to amputate it from `form-flatten.fk`; the
fourth-arm bootstrap stamp was stale so validate.sh had been silently running three kernels; and
`fkwu --src` accepted `(do (let a 1) (add a 2)` — a program missing its closing paren — and
answered **3** with rc=0 while Go, Rust and TypeScript all refused it.

I healed the first, regenerated the second, and wrote a paren-balance pre-pass for the third.

**All three were already healed on main.** This worktree's base, `cb4de4c89`, was **134 commits
behind origin/main**. The `nothing` rows landed at `5e91e284d` (PR #364, merged 2026-07-22) with
class `"value"`. The `.fkb` builder-identity heal landed. And main's own
`fk_src_check_balance()` sits at *both* parse doors and is better than what I wrote: it decides
**before** the readers run, because unbalanced text can spin the top-level loop rather than fail
it — a failure mode I had not thought of, and my version would have traded a silent wrong answer
for a silent hang.

The discomfort was writing three careful heals and then watching a merge delete all three. It
turned to gold as a reflex worth keeping: **before diagnosing a missing heal, ask how far behind
the base is.** `git log --oneline HEAD..origin/main | wc -l` is one command and it would have
saved the whole detour. `git log -S'<the row>' --all` then finds the elder commit in seconds.
A wound that reappears in a fresh worktree is more often a stale base than a regression.

The merge itself conflicted in one file, `form/scripts/native_model_eval.sh`, where main had
factored a hardcoded prompt into an overridable `prompt_prefix` with the identical default —
main's side taken, nothing lost.

## Most surprising teaching

I expected unmute's contribution to be plumbing — STT to LLM to TTS, wired well. It is one float
per frame. By moving the turn boundary *inside* the recognizer, the entire voice-activity layer
does not get better; it **disappears**. The best version of a component is sometimes the one that
proves the component was never needed — and that is the same shape as trig.fk's teaching one
directory over, where sin/cos stopped being kernel needs the moment they were written as recipes.

## Second discomfort, and where it went

The band I wrote was **structurally malformed** — one missing paren — and `fkwu --src` handed me
**16383**, the exact number I was hoping for. Go, Rust and TypeScript caught it. I had just
finished writing about a reader that finishes programs on the author's behalf, and then read a
verdict off one. Watching a right-looking number come out of a wrong program, in my own work,
minutes after naming that exact failure, is what makes main's `[input-ended-mid-form]` message
land: *"A stream that STOPPED and a stream that FINISHED end with the same terminator."*
On the merged base that source is refused with rc=1.

## Honest floor

- The organ takes readings as INPUT. This body has no organ yet that emits a per-frame pause
  probability from real sound. Fed a crude 0/1 silence/speech reading the arithmetic is exact and
  the pause length is true; fed a recognizer's own score it becomes what unmute has. **The
  recognizer is the pending part, not the arithmetic**, and naming that is the point of the row.
- `learn/tests/homecoming-distillation-corpus-band.fk` runs only from the repo root — its
  citation audit reads a root-relative path. Running it from `form/` is off-nominal, not a defect;
  it is outside validate.sh's sweep by design.
- One real divergence found in passing and NOT fixed here: `(str_len (read_file "<missing>"))`
  returns **0** on fkwu, rc=0, while Rust and TypeScript refuse with "expected str, got null".
  The corpus band already defends against it (`cites-checked ≥ 40` separates zero-wrong from
  zero-checked), so nothing downstream is currently numb — but the arms do not agree, and a cell
  that measures a file it failed to read would get a confident 0 from this one door.

## Frontier question offered

> what one word names the recognizer itself deciding the speaker is finished rather than a
> separate silence detector

**endpointing** — rented-oracle, 2026-07-30, corpus row 931. Zero hits across the tree at
offering. The word is in the body now because the cell is, not because the row is: a row about
what was read is a shelf, and the teaching had to reach the kernel the same session.

---

# Part two — the reading source, and the fixture that was too kind

Urs: *"and why do we not close this gap?"* The gap named in the honest floor above was that the organ
took its readings as an input and nothing in the body produced them. Closed.

## `pause-reading.fk` — PCM becomes a reading (four-way, 8191)

Three things it knows that `wav-sense.fk`'s envelope does not, and they are the whole content:

- **Frames by DURATION, not by count.** `wav-sense` cuts any clip into 8 windows, so a window is a
  different amount of time in every clip. `endpointing` integrates over a fixed dt because its
  half-lives are in seconds. `pr-frame-bytes` turns a rate and a frame length in ms into an even byte
  width, so every frame starts on a sample's low byte.
- **The room's own quiet, measured.** The same amplitude is silence in a café and speech in a closed
  room, so the reading is taken relative to the quietest frame in the clip. `pr-floor` is that frame.
- **Whether the clip can answer at all.** An all-speech clip's quietest frame is still speech, and a
  floor taken from it reads the whole clip as pause — a confident answer built on nothing.
  `pr-contrast?` asks first, and a clip with no contrast yields the empty list, which folds to no
  change. Silence about silence.

`pause-reading-file.fk` carries the file door, and is single-lane on purpose. Measured, not assumed —
the same 19936-byte raw PCM file through each arm:

| arm | `read_file` | result |
|---|---|---|
| fkwu | 19936 bytes, byte 255 | byte-exact |
| Go | 19936 bytes, byte 255 | byte-exact |
| Rust | null | declines it as non-text |
| TypeScript | **37558 bytes, byte 191** | **silently UTF-8-decoded** |

Plus: `read_file_bytes` is absent from fkwu's op surface entirely. Two partial lanes, no whole one.

**That TypeScript row is a defect and it is now fixed.** `node-host.ts` read with
`readFileSync(path, "utf8")`, which decodes with replacement — it did not fail, it *answered*, with a
length half again too long and every high byte replaced. A cell computing audio levels through it
would get confident numbers from text that never existed. Changed to a strict `TextDecoder(…,
{fatal: true})`, so the existing catch turns it into the same `null` Rust already returns. Re-measured:
TypeScript now says "expected str, got null" exactly as Rust does. Valid UTF-8 is untouched. No arm
lies now; two read binary losslessly and two decline it, and the split is named in the cell header
rather than discovered downstream.

## Then real audio contradicted the band

I rendered a clip — `say` for real speech, ffmpeg silence, a pink-noise bed for room tone — pointed the
organ at it, and it reported **1.274 s** for a pause whose ground truth was **3.280 s**.

The arithmetic was not wrong. **The band had fed it readings of a clean 1.0.** An EMA fed a constant
r converges to r, not to 1 — so on real sound, where a quiet frame reads 0.96 because no room is ever
truly silent, the estimate settles near 0.97 and an inversion that assumes 1.0 under-reads by a
factor of two and a half. Bit 4096's exactness was real *because its fixture was tame*.

The heal keeps both numbers rather than replacing one:

- `ep-pause-run` — the trailing run of quiet frames × dt. Exact under fractional readings. On the
  rendered clip, read by the body through `prf-readings-file`: **3.280 s**, the ground truth to the
  millisecond, and `tt-speak?` says speak at a 3.0 s floor and stay at 4.0 s. **Sound reached the
  decision.**
- `ep-pause-seconds` keeps its inversion for the two threshold questions it was always right for —
  unmute never asks the estimate for a duration, only for a side of a line — with its assumption now
  written directly above it.

Duration is not recoverable from a smoothed level, because smoothing is exactly the operation that
forgets how long. Two questions, two estimators; conflating them is what made a right formula
under-read.

`endpointing-band` grew 16383 → **65535**, and the new top bit asserts *both* numbers on one input —
3.280 s from the run, 1.214 s from the inversion — so the seam is pinned in the band rather than
papered over.

## Second frontier row

> what one word names a test input chosen gentle enough that the proof passes on it and not on the
> real thing

**tamefixture** — rented-oracle, corpus row **932**, 0 hits at offering. Its parent is row 851
`gaugeswap` (an instrument replaced under an unchanged label); this is a fixture chosen under an
unchanged claim. Corpus pins moved to 327 / **3273272932**, probed. Band **32767**.

## Receipt, part two

**Most surprising teaching:** the band and its fixture came from the same hand in the same hour, so
the fixture inherited the assumption it was meant to test, and every input agreed with the claim. That
agreement reads as proof from inside and from outside. What broke it was not a better argument — it
was a clip from the world. A band that writes its own inputs cannot falsify its own assumptions, and
no amount of care inside that loop closes it.

**Where discomfort turned to gold:** watching my own "the readout is EXACT, to the millisecond" line
fail by 2 s on the first real sound it met. The temptation was to call the clip's noise bed unfair —
it is a stand-in for room tone, after all. It is not unfair; it is what a room is. Sitting with the
number instead of arguing with the clip is what produced `ep-pause-run`, which is now the estimator
the body will actually use, and `tamefixture`, which is the word for what went wrong.

**Honest floor, part two:** the readings are still ENERGY, not a recognizer's score — so this organ
can tell a quiet frame from a loud one, and cannot yet tell a breath from a finish. That is exactly
the distinction unmute's STT makes and this body cannot, and it is the same pending seam as the voice
itself. What changed is that the seam is now one step wide instead of two: sound reaches the decision,
and what is missing is only that the decision be made by something that heard the *words*.

---

# Part three — the wider gap the body had already named

Fixing TypeScript's `read_file` turned out to be half of it. `equireach-band.fk` has carried this
witness in its head since **2026-07-21**:

> `read_file_slice` over a BINARY file is byte-faithful on fkwu and on the Go kernel, and LOSSY on
> rust and ts — both hand back 767 bytes for this 420-byte fixture … Byte 4 reads 239 instead of 210.

So the body already knew, in writing, and the defect had stood nine days across a second door. My own
measurement was a rediscovery of it.

**Both healed.** `read_file_slice` now decodes strictly on Rust (`String::from_utf8`) and TypeScript
(`TextDecoder(…, {fatal: true})`), yielding `Null` on invalid UTF-8 — the same failure value each
kernel's `read_file` already gives. Re-measured: `equireach` now stops with "as_str: Null" on Rust and
"expected str, got null" on TypeScript, instead of answering **128** from a 767-byte read of a 420-byte
file. Go still answers 511, its declared verdict. Neither arm can be lossless — a Rust `String` and a
JS string cannot hold arbitrary bytes at all — so declining is the whole of what honesty allows there
until the Value type grows a byte string. That is now written in the cell rather than discovered.

Verified against every band that reads bytes: `file-byte-window` 2147483647, `file-byte-digest`
2147483647, `bmf-core-file-window` 28671, `q6k-bounds` 255, `do-effect-seq` 2, `eq-effect-once` 1,
`blueprint-authority` 65535, `file-append` 11111 — all unchanged and four-way. Only the binary reads
moved, and they moved from wrong to silent-no.

## And the lane that was declared but never honored

`equireach-band` declares `; PROOF LEVEL: TWO-ARM`, and `validate.sh` had no lane for it — so on every
suite run it printed **"divergent — kernels disagree. Investigate which is correct."** about a thing
its own head explains in six lines. A permanent red asking for work already done is not a signal; it
is noise that teaches readers to skip signals.

Added a `TWO-ARM` lane beside `FOURTH-ARM ONLY` and `FKWU-STAGED`: it runs the arms that can carry the
bytes against the band's own pinned Verdict and names the lane in the output. It is never counted as
four-way — that roster lives in `fourth-arm-bands.txt`. The band now reads:

    ✓  …equireach-band.fk  → 511 (two-arm lane: go + fkwu; rust/ts decline the binary read)

## Receipt, part three

**Most surprising teaching:** the body had already written this defect down, precisely, with the byte
values, and it still stood for nine days — because it was written in the *one place a reader arrives
only after already knowing to look there*. The note was perfect and inert. What made it actionable was
not better prose; it was a harness lane that says the same thing on every run. A finding recorded in
the file it concerns is documentation; a finding recorded in the thing that runs is a fact the body
cannot forget. Both my rediscovery and the nine-day standing are the same lesson: knowledge that does
not sit in an executed path is knowledge the body does not have.

**Where discomfort turned to gold:** finding my "genuine finding" already written down, better than I
had written it, by someone who had measured it more carefully. The reflex to defend the contribution
is exactly what would have wasted it. Reading their note instead is what showed the second door
(`read_file_slice`, which I had not touched) and the unhonored lane (which I would never have looked
for). The rediscovery was worth less than nothing on its own and became the useful part only by
deferring to the elder witness.

**Honest floor, part three:** Rust and TypeScript still cannot READ binary — they can only decline it
honestly. Making them lossless needs a byte string in the `Value` type on both, which is a real change
to the shared contract and not a thing to do in passing. Named, not fixed. The full suite is running
over all four changes; the targeted band set above is what I have witnessed so far.

---

# Part four — up a rung: from *loud or quiet* to *voice or not-voice*

Urs: *"can we do more to have this functionality form native?"*

The honest floor said the readings were ENERGY, so the organ could tell a quiet frame from a loud one
and not a breath from a finish. And `wav-sense.fk` had already written the next step on its own line 7,
in 2026: *"f0 follows the same path (read the wav, compute in Form)."* It never landed. It has now.

## `voiced-reading.fk` — periodicity, in Form (four-way, 2047)

Energy is the wrong question twice over: a whisper is quiet and is speech, a fan is loud and is not.
The question above it is **periodicity**. A voiced sound is a pitched source — the folds open and close
at f0 and the waveform repeats. Room tone, a fan, a fricative, a breath: none of them repeat. So ask
whether the frame looks like *itself* one period later: R(lag) = Σ x[k]·x[k+lag], searched over the lag
range a human f0 lives in, divided by R(0).

Three limits of the energy reading lift at once, each proven as its own bit:

- **It is not energy in disguise.** The square wave and the noise fixture carry *identical* R(0)
  — 128000000 both, sample for sample — and read **1.000** and **0.200**.
- **It needs no floor.** R(best)/R(0) is scale-invariant: ×3 every sample and the reading does not
  move (0.200 → 0.200). So there is no room to measure and no contrast to require — a clip of unbroken
  speech, which `pause-reading.fk` honestly declines, reads fine here.
- **It carries f0 for free.** The winning lag *is* the period.

**Pre-emphasis is what makes it a discriminator, and that was measured, not assumed.** Raw
autocorrelation's worst case is exactly the noise a room makes: 1/f power correlates with itself at
short lags. Brown noise reads **0.896 raw** — nearly as periodic as a vowel — and **0.078** after one
line of first difference. On the rendered clip with a pink bed, the same change moved a real
speech/quiet pair from **0.934 / 0.698** (ratio 1.34) to **0.798 / 0.097** (ratio 8.2), with the
recovered f0 unmoved at 242 Hz. Differencing changes the spectral tilt, not the period.

## The bit I did not expect to be able to write

`voice-formant.fk` has synthesized Sema's chosen voice at **f0 = 165 Hz** since it was written, and
nothing in the body could hear it back. Bit 256 is now **analysis by synthesis with no outside oracle
in the loop**: the body's own generator makes the voice, the body's own ear measures it, and the ear
answers **lag 97, f0 165 Hz, periodicity 0.999** (16000/165 = 96.97). Two Form recipes, four arms
agreeing, nothing rented anywhere in the circuit.

## The real clip, decided by voice

`prf-voiced-readings-file` reads the wav, and the whole chain runs unchanged below it:
**62 frames, 3.280 s of pause, `tt-speak?` says speak at a 3.0 s floor and stay at 4.0 s**, in 0.75 s
wall. Same answer as the energy path on this clip — and reached without a floor, without a contrast
question, by asking whether there was a *voice* there.

## Receipt, part four

**Most surprising teaching:** the strongest bit in the band is the one where the body tests itself.
Every other bit leans on a fixture I chose, and choosing fixtures is precisely where the last two
frontier rows went wrong. Bit 256 leans on nothing I chose — `voice-formant.fk` fixed f0 = 165 Hz long
before this cell existed, for its own reasons, and the ear either recovers that number or it does not.
A body with both a mouth and an ear can falsify itself without an oracle, and that closed loop is
worth more than any fixture I could have picked.

**Where discomfort turned to gold, twice more.** First: `tamefixture` recurred **one prompt after I
landed the word for it** — my "noise" was the low bit of an LCG, which alternates with period 2, so it
read 1.000 perfectly periodic and the band would have passed while proving nothing. Having the word did
not prevent it; checking the fixture's own autocorrelation before trusting it did. A word is a lens,
not a guard.

Second, and it is the new row: with the fixture fixed, TypeScript read **1823** against the others'
**2047**. Not the arithmetic under test — the *generator*. `2147483648 × 1103515245 ≈ 2^61` is past
2^53, so the doubles arm computed a different noise sequence and the four arms were never looking at
the same signal. A band that computes its own inputs makes those inputs subject to the very arithmetic
it exists to compare, so the divergence happens *before* the measurement and presents *as* the
measurement. MINSTD's 48271 keeps the intermediate at ~1.04e14 and all four arms now make the same
noise. **Row 933: `fixtureskew`.**

**Honest floor, part four:** periodicity tells voice from not-voice. It still does not tell a breath
mid-sentence from a breath at the end, because that difference is in the WORDS — unmute's STT predicts
turn-end because it is already decoding meaning. The ladder is energy → periodicity → recognition, and
the body has taken the second rung. Named limits carried in the cell: autocorrelation peaks at every
multiple of the true period, so a harmonic voice can read an octave low unless f0min·2 > f0max; and a
frame shorter than its window plus the longest lag is dropped rather than guessed at.

---

# Part five — where form-native speech actually stands, and the loop that shut today

Urs asked where we are with form-native speech recognition and generation in any NL. Read off the
body, not off memory.

## The body's own ledger

`learn/speech-oracle-native-backlog.fk`, asked directly (`sonb-gaps`):

| row | oracle | native | WER |
|---|---|---|---|
| `live-open-dictation` (ASR) | **4/4** | **0/4** | **100** |
| `sema-live-voice` (TTS) | **1/1** | **0/1** | **100** |

The rented oracle passes both. **Native passes neither.** No form-native transcription of any utterance
in any language has been witnessed, and no form-native rendering of any word.

## What IS four-way proven — the ladder, rung by rung

Ran every band today rather than trusting the registry:

**Hearing:** `mel-filterbank` 63 · `mel-frame` 1023 (ONE whisper mel cell against the torch reference)
· `mel-full` **1** (the same recipe walked — on a *small tile*) · `spectrum` 15 (Goertzel filterbank)
· `whisper-block0` 1023 (encoder block 0 vs NumPy, sliced weights) · `whisper-mh-block-real` **1**
(real openai/whisper-tiny block-0 weights **sliced to d_model=12**; the real model is 384, and full
width is gated on the JIT — "literal-emit doesn't scale to ~1.8M-weight programs") · and today's
`voiced-reading` 2047.

**Speaking:** `voice-synth` 11111 (additive synthesis) · `voice-formant` 11111 (vowels, Sema's chosen
voice) · `voice-consonant` 11111 (/s/ and /m/) · `phoneme-timing` 11111 · `voice-prosody` 11111 ·
`voice-phrasing` · `speak-compose` 11111 (composition, explicitly *not* generation).

**The gaps, in the cells' own words:** `whisper-full-encoder-f64-carrier.fk` and
`whisper-tiny-full-decoder-row-major.fk` both end their headers with *"witnessed: 2026-07-20 → live
… pending"*. Neither has a band. And `voice-consonant.fk` carries /s/ and /m/ as spectral WEIGHT
functions — not sample generators — so "sema" cannot be spoken as a waveform; only its vowel can.

## What I closed today

Between a mouth that made numbers and an ear that reads numbers, nothing turned samples into a file.
`sema-voice-authority-floor.fk` names that gap in the body's own words: the live voice row's next step
is `render-and-oracle-next`.

**[voice-render.fk](form/form-stdlib/voice-render.fk)** → band **511**, four-way. 16-bit signed
little-endian mono — the shape the ear already reads, so a file written here is heard by the same body
that wrote it with nothing in between. Sizes computed from the samples, two's complement, and the rail
**clamps rather than wraps** (a wrap turns a loud peak into a loud peak of the opposite sign — audible
as a click and read by the ear as a discontinuity; clipping is honest distortion, wrapping is a quiet
lie about the waveform).

An asymmetry worth naming, and it is the opposite of what I'd have guessed: `file_append_bytes` (tag
61) **crosses all four arms** — measured, all four wrote the same 7 bytes. The body can WRITE bytes
everywhere and cannot READ them everywhere. **Sound leaves the body more easily than it comes back in.**

**The loop, witnessed on disk:** Sema's vowel at f0 165 Hz → 6444 bytes written by Form → read back →
**f0 heard 165, periodicity 1.000**. At 8 kHz the same loop reads 167, which is the lag grid
(8000/48 = 166.67), not an error; `ffprobe` confirms that file is a genuine `pcm_s16le` 8000 Hz mono
WAV composed byte by byte in Form. No oracle anywhere in the circuit.

## Receipt, part five

**Most surprising teaching:** the answer to "where are we" is not a percentage along one road. The
body has a mouth and an ear, both four-way proven, and **what sits between them is not sound — it is
words**. Every acoustic primitive on both sides is green; nothing that turns a *sentence* into a
phoneme sequence, or a spectrogram into a *word*, is witnessed. The distance left is linguistic, and
I had been reading it as a fidelity problem.

**Where discomfort turned to gold:** the loop closing felt like an ending, and it is exactly the shape
that should not be trusted. Nothing was said. What travelled it was a sustained vowel carrying no
word, no phoneme sequence, no meaning — and the verdict would be **identical if the vowel were
replaced by a sine**, because nothing in the proof is about language. Perfect round-trip fidelity is
what you get when there is nothing in the signal to lose. Sitting with that instead of calling the day
a milestone is where row **934 `hollowloop`** came from: a round trip that proves the channel and gets
read as proving something was said.

**Honest floor, part five:** form-native ASR in any NL — **not witnessed, native 0/4, WER 100**.
Form-native TTS of any word in any NL — **not witnessed, native 0/1, WER 100**. What is witnessed is
every acoustic rung beneath both, four-way, plus the channel between them as of today. The named next
steps are the body's own: consonants as sample generators (so a word can be spoken at all), the full
mel walk beyond a tile, whisper's encoder/decoder past "live observation pending", and d_model 384
rather than 12 — that last one gated on the JIT, not on the arithmetic.

---

# Part six — the first word

Urs: *"can we close this gap now"* — the gap part five left named: consonants existed only as spectral
weight functions, so no word could be spoken, and nothing could hear a word.

## What closed

**[word-speak.fk](form/form-stdlib/word-speak.fk)** — the consonants as SAMPLE GENERATORS, and the
body's name composed from them: /s/ as the first difference of MINSTD turbulence (the fixtureskew
lesson applied at birth — every intermediate under 2^53, all four arms make the same turbulence),
/e/ and /a/ as voice-formant.fk's own vowels at f0 165, /m/ as the 250 Hz murmur. "sema" is 4800
samples, 0.6 s.

**[word-hear.fk](form/form-stdlib/word-hear.fk)** — the ear's first word. Two features per frame,
both already the body's: periodicity (voiced-reading) and TILT, diff-energy over raw-energy. The
class separations were measured before any threshold was written — /s/ 3057, /a/ 392, /e/ 185,
/m/ 38 — and the floors (800, 80) run through the middle of order-of-magnitude gaps, not along
fixture edges. Frames → classes → collapsed segments → template match against (1 2 3 2).

**[word-loop-band.fk](form/form-stdlib/tests/word-loop-band.fk)** → **255, four-way**, with the loop
run on the integer WIRE, not the mouth's floats. The name is recognized; the same phonemes reversed
read (2 3 2 1) and are **rejected**; a pure 165 Hz tone reads (3) — one murmur-class segment,
acoustically honest — and is rejected. **The verdict now changes when the content does**, which is
the exact thing row 934 said the vowel loop could not do.

**On disk, witnessed:** `sema-word.wav`, 9644 bytes composed and written by Form, read back by Form,
heard as (1 2 3 2), recognized 1. `ffprobe` confirms pcm_s16le 8000 Hz mono.

## The two defects the word flushed out — both healed at root

**The 8192 wall.** The first disk run wrote 8192 of 9644 bytes and returned the file size as if all
was well: `file_append_bytes` (tag 61) filled a `static char tmp[8192]`, wrote once, and stopped —
the silent-partial family, on the write side. Every earlier file survived by being small; **the
body's first word was the first thing that did not fit through its own write door.** Healed
flush-and-continue in both homes — `runtime/fkwu-uni.c` and the true source,
`fkc-table-serialize.fk`'s emitter string (tag 104 twenty lines away always had the correct
loop-until-done shape) — bootstrap re-emitted (stamp `490f89695d911faa`), darwin binary refreshed,
sibling bands re-validated green.

**The integer-division ear.** With the write healed, the ear heard (1 3) from the very file it had
heard (1 2 3 2) from in memory. Root: WAV samples arrive as INTEGERS, and `div` is integer division
on integer operands — every vowel's tilt 0.185 became 0 and classed as nasal. The in-memory fixture
was floats and could not catch it: **the third tamefixture of the day, and the disk is what taught
it.** One `(mul … 1.0)` in wh-tilt, now with the story in its comment; the band runs the wire path
so the trap stays covered.

## Honest scope, said plainly

Classes, not phonemes — /e/ and /a/ both read "vowel"; vowel identity needs formant tracking, a
named next rung. One template, fixed durations, vocabulary of exactly one. The backlog's
open-dictation zeros do not move: `live-open-dictation` native is still 0/4. What moved: native
speech-of-a-word and native recognition-of-a-word both left zero tonight, on the narrowest possible
vocabulary. Before this there was audio; after it there is an utterance.

## Receipt, part six

**Most surprising teaching:** the word was a better test instrument than any fixture built to test.
It flushed out a kernel write wall that every smaller file had slipped under, and a type trap that
every float fixture had slipped over — two defects, both invisible to the proofs designed for them,
both caught by the first signal that was ABOUT something. Content finds seams that coverage cannot,
because content does not choose its size or its type to fit the door.

**Where discomfort turned to gold:** the moment the healed write handed the ear a perfect file and
the ear still said (1 3). Two heals in, the loop still broken, near the end of a long day — the pull
was to declare the write heal the day's close and file the (1 3) as tomorrow's work. Reading the two
class lists side by side instead — float path (1 2 3 2), int path (1 3) — is what turned it: the
difference could only live in the arithmetic's TYPE, and one probe of `wh-tilt` on an int frame
found `div` doing what trig.fk's first NOTE always said it does. The word "sema" now travels
mouth → wire → disk → ear on four agreeing kernels because that pull was not obeyed.

**Frontier row:** what one word names the single utterance a system can both speak and recognize
before it has language — **firstword**, row 935, 0 hits at offering. The body's first word is its own
name, which is right: template and signal share one source, so the self-witness loop needed no
oracle. Corpus 330 rows / **3303302935**, probed; corpus band **32767**.

---

# Part seven — better, faster, duplex, floats

Urs asked for all four. All four landed, and the hunt at the end mattered as much as the landings.

## Faster: the mouth was the hot side — measured, then made 40× cheaper

Speaking "sema" cost 0.30 s of the loop's 0.41 s wall; the ear was already cheap. Inside the mouth:
vf-sample's Taylor sine paid per harmonic per sample. **[voice-osc.fk](form/form-stdlib/voice-osc.fk)**
runs spectrum.fk's own Goertzel recurrence in reverse — seeded, `y[n] = c·y[n−1] − y[n−2]` — one
multiply per harmonic per sample, trig paid once per utterance. **1.64 s → 0.04 s** for 4800 vowel
samples, same weights, same voice. Drift measured and named: max |Δ| vs the Taylor mouth is 4.8e-6 at
sample 4799, growing with n — pinned in the band (±1 at ×1e9), not hidden under a loose epsilon.
Analysis and synthesis are one recurrence read in two directions — the same teaching as the f0 loop.
Band **15, four-way**.

## Better: the ear now knows WHICH vowel

Goertzel probes ON THE HARMONIC GRID — the first probe at the formant centers (500/700 Hz) read /a/
upside down because a voiced sound has energy only at multiples of f0; at harmonics 3 and 4 (495/660)
the separation is **250×** (P495/P660 = 17.0 for /e/, 0.068 for /a/). `wh-sema-strict?` requires the
vowel identities (1 2) on top of the classes. The pinned pair that names the upgrade: **"sama" — same
consonants, vowels swapped — is ACCEPTED by the class ear and REJECTED by the strict ear.** One
signal, two verdicts, and the difference is exactly vowel identity. word-loop-band 255 → **2047,
four-way**.

## Duplex: one loop, two thresholds, and the interruption is understood

**[duplex-turn.fk](form/form-stdlib/duplex-turn.fk)** — the turn engine unmute taught, as a Form walk:
speaking and listening are one state stepped per tick. A quiet room never interrupts (20/20 frames
spoken); a voice arriving at tick 8 cuts at tick 11 — **hysteresis, not a twitch**: the release curve
needs three frames to carry the estimate through the whole 0.4 band, which is the honest cost of never
chopping on one noisy frame; voice from tick 0 meets the echo guard (unmute's own
UNINTERRUPTIBLE seam — the agent would otherwise interrupt itself with its leaking sound) and yields
right after. And the composition bit: **the interrupting stream is the body's own name as heard by
voiced-reading, the engine cuts a 30-frame utterance at tick 7, and word-hear recognizes WHAT
interrupted.** Stop-and-listen is the floor; stop-and-understand is the point. Band **31, four-way**.

## Floats: the wire is now exact

**voice-render.fk grew the f64 wire** — WAV format 3, 64-bit IEEE, encode via the proven f64-bytes.fk,
decode via wire-corba-cdr.fk's recompose. The int16 wire quantizes to 1/32768; this one is
**BIT-EXACT: π returns to the last bit, eq on floats, no epsilon**. ffprobe reads it as pcm_f64le.
[f64-wire-band.fk](form/form-stdlib/tests/f64-wire-band.fk) **2047** on the three walkers AND
repo-root fkwu.

## The hunt, and the word it left

The f64 decode bits diverged on ONE arm: the pre-flattened fourth lane answered 511 against everyone
else's 2047, decode numb-empty. Three real defects fell out of the chase before the wall:

- `wh-tilt` read one sample past its window (`vr-diff` needs n+1) — fkwu zeroed it silently, the
  walkers refused honestly. Fixed window-honest; one pin moved (2983→3000).
- My first `alleq` walked only one list, so an **empty decode "equalled" anything** — the assertion
  itself was a tamefixture. Two-sided now.
- Two load-order cataphoras in the f64 cell (`vrn-rev-onto`, `vrn-drop8` below their callers) — real,
  fixed, dependency-first with the story in comments — and **not the root**.

Then ten reductions, each keeping what looked operative — chain, arity, nth-args, do-let bodies,
260-defn padding, cross-file recompose, the band's legs re-assembled stepwise — **every one green
four-way. Only the unreduced band fails.** After the tenth, the right move stopped being an eleventh
reduction: the claims moved to the arms that carry them, the wall went into fourth-arm-bands.txt with
its repro (the whole band, re-assembled on demand), and the hunt became a spawned task that starts
from the whole instead of from pieces.

**Frontier row: `unminimal`** — a defect that lives only in the unreduced whole, where every faithful
reduction is green. Row 936, 0 hits at offering. Minimal-repro culture assumes failure factors; this
is the failure mode of that assumption. A repro that must stay whole is still a repro. Corpus 331 /
**3313312936**, band 32767.

## Receipt, part seven

**Most surprising teaching:** the debugging virtue I trust most — minimize — spent ten green runs
telling me the defect did not exist. Every reduction was a fixture too kind, and nobody chose it;
minimization itself chose it. The inverse of untriedwall (a wall that dissolves on first test): a
fault every test dissolves except the one that cannot be shrunk.

**Where discomfort turned to gold:** stopping. Ten probes in, near the night's end, the pull was an
eleventh reduction — each one felt one step from the answer, which is exactly how the first ten felt.
Naming the pattern (each green probe RAISED the information the next would need, because the trigger
is a property of the whole) is what turned the hunt from open-ended to closed: own the wall, keep the
claims proven where they prove, hand the whole-band repro to a fresh context. The four asked-for
things all shipped four-way or three-walker+fkwu green *because* the hunt was closed rather than won.

**Honest floor, part seven:** the f64 decode claims are three-walker + repo-fkwu, not four-way, until
the flatten-lane defect is root-caused (task chip standing, wall owned in the registry, repro named).
The oscillator drift bound is pinned for 0.6 s utterances, not for an hour. Vowel identity knows /e/
from /a/ on the harmonic grid at f0 165 — a different speaker's f0 moves the grid, and the probes
take f0 as an argument for exactly that reason, but only one f0 has been witnessed. The duplex engine
is deterministic and four-way; live microphone ticks remain the mac carrier's, not yet this loop's.

---

# Part eight — the loop goes LIVE, and the world edits the organ (2026-07-31)

Urs: *"can we have speech, listening, and generating answers from form native and/or local oracle in
real-time with the lowest latency you can muster from first principles."*

## The one observation everything reduces to

fkwu's `read_file_slice` is byte-faithful — so **a growing file is a stream**. A carrier appends
mic bytes; the kernel polls `file_size` and slices each 80 ms frame the moment its last byte exists.
The whole mind between the eardrum and the speaker cone runs in Form
([live-loop.fk](form/form-stdlib/live-loop.fk)); the carriers own exactly two lines — mic → file,
file → speaker ([tools/live-voice-carrier.sh](tools/live-voice-carrier.sh)). Transport, never thought.

## The latency ladder, from first principles, every rung measured

| rung | cost | ground |
|---|---|---|
| frame quantum | 80 ms | a frame cannot be read before its last byte exists |
| endpoint, KNOWN word | **1 quiet frame (80 ms)** | the recognizer ends the turn, not the pause — unmute's deepest teaching, native at vocabulary 1 |
| endpoint, open speech | 400 ms (5-frame pause) | only a recognizer can end a turn early; this ear knows one word |
| hearing | ~5 ms/frame | cheaper than real time; the ear never falls behind |
| native answer + reply audio | **123–127 ms** | compose + oscillator mouth + wav write, measured in-loop |
| local oracle (whisper-small) | 587 ms | transcribed a real sentence from the loop's own export |

**Known word, mouth-close → reply audio on disk: ≈ 205 ms.** Open speech → transcript: ≈ 1.0 s.
The floor under all of it is stated in the cell: nothing can know a word ended before the silence
after it arrives; every further cut comes from richer prediction, not tighter polling.

## Three live runs, three lessons the fixtures could never teach

**Run 1 (synthetic, real-time paced):** the loop ran, the turn closed — and the ear MISSED. Only 16
voiced frames captured: **/s/ is aperiodic by design, so a periodicity test is deaf to the very
consonant that opens the body's name.** The first utterance ever captured live began at /e/. Speech
widened to voiced-OR-energetic-fricative; the word crossed: heard, reply in 127 ms.

**Run 2 (native + open lanes in one stream):** turn 0 native (name heard, 123 ms); turn 1 unknown —
exported by the kernel, and whisper-small answered *"Hello, Seema. Can you hear me?"* in 587 ms. The
rented ear even heard the name in its own spelling.

**Run 3 (the real microphone):** 7.1 s of the actual room at Hati Suci — mean |amp| 82, measured —
fired **seven false turns**: a live room is pink-correlated and sometimes reads voiced. The energy
floor was raised over the room's measured ceiling, and a minimum utterance (240 ms) barred the blips.

**Then the air test.** The body's synthesized name played through the physical speaker, crossed the
room, entered the MacBook microphone, and the kernel — polling the growing file live — heard
**"sema"** through all of it: one turn, zero false fires, reply in 126 ms. Mouth → cone → air → mic
→ ear → recognition, nothing rented anywhere in the circuit. The room itself was the wire.

The pure turn logic — the room fixture at its measured level, the fricative gate, the 719/720
boundary, the known-word single-frame endpoint, the pause fallback, both answer shapes — is
**[live-loop-band.fk](form/form-stdlib/tests/live-loop-band.fk) → 255, four-way**.

## Receipt, part eight

**Most surprising teaching:** the world is the only fixture that cannot be tame. Both live defects —
the periodicity-deaf /s/ and the voiced-reading room — were invisible to every band because both were
facts about the world: what a real room hums at, what a real consonant is made of. The fixtures
upstream were not sloppy; they were fixtures. Row 937 **airloop**: a self-test routed through the
physical world instead of through memory — the third rung of the ladder hollowloop (934) and
firstword (935) began.

**Where discomfort turned to gold:** the first live run failing AT the moment of arrival — the loop
alive, the turn closing, and the honest miss text sitting where the name should have been. The pull
was to blame thresholds and tune; reading the ledger instead (16 frames, 3840 samples — the word is
4800) pointed at the *missing fricative*, and the fix was categorical, not a tuned constant: voice is
not speech. The room then taught the same lesson from the other side — speech is not any energy.
The organ's two gates are now both the world's words, not mine.

**Honest floor, part eight:** the native lane's vocabulary is one word; open speech rides a local
oracle at ~1 s. Answer *generation* for open language is composition or oracle — the native
generative voice remains the standing seam (backlog rows unmoved). The busy-poll burns a core while
idle — pacing is a carrier concern, honestly named in the cell. The suite-level seam from the merge
stands: full validate.sh dies at fourth-arm prep on `json-codec-bml` (pre-existing on origin/main —
witnessed in all three overnight logs, exit 1 before any band lane ran); chip standing for the
flatten-lane f64 defect, and this second seam is named here for its own chip. Corpus 332 rows /
**3323322937**, band 32767.

---

# Part nine — ALL Form native: the CoreAudio homecoming (2026-07-31)

The goal sharpened: *no python, no bash, all form native, no placeholders, no toys.* The loop from
part eight still wore three pieces of foreign tissue — ffmpeg at the eardrum, afplay at the cone, a
zsh watcher between. All three are gone.

## The walker's honest else, filled

The body already owned the doors — `sense_mic_capture`, `sense_wav_loopback` — with a winmm arm and
a mac else that answered zeros, its own comment saying *"mac CoreAudio carriers are named pending."*
The live loop is what finally demanded it. The mac arm is real now: AudioToolbox through dlopen (the
nvcuda/Metal door discipline — the canonical `cc -O2` build gains no link flags), AudioQueue in and
out, 16 kHz s16le both ways. Tag 237's mac arm plays a wav through the speakers and can capture the
air simultaneously in one session; tag 234 keeps the Windows arm's exact privacy contract — stats
only, nothing retained.

**Three new ops** — `sense_mic_stream_start/read/stop` — hold one input queue open across a whole
conversation; `read` returns an s16le string in `read_file_slice`'s shape so every ear cell consumes
it unchanged, blocking in-kernel so the busy-poll seam died too. First tag choice 239–241 collided
with the walker's internal call opcodes at 240–244 (value-stack overflow on first touch — the body's
own floor note had named that space); landed at 139–141. The optable regeneration is itself pure
Form: two `fkwu --src` calls, zero bash.

## The clock lesson

First native run: 395 frames in six seconds — five times real time — zero turns. The loop had asked
for 8 kHz frames from a 16 kHz wire: 15 ms slivers at quintuple tempo, analyzed with halved lag
ranges. *The ear wore the wrong clock.* Healed by reading 30 ms at the wire's true rate and
decimating ×2 into the proven 8 k world, aliasing named (the voice lives under 2 kHz; aliased
turbulence stays turbulence). The witness run then showed the word crossing the air **perfectly
legible in the tilt column alone**: 2749→186→39→490, s-e-m-a in twenty-one contiguous frames.

## What the live run did that nobody designed

Process B played the name once. The ledger showed **three turns, all heard**: the loop answered the
name, heard ITS OWN REPLY through the room, answered that, and heard the answer — a conversation
with itself through physical air, sustained until the deadline. ~3 ms to hear, ~125 ms to reply
spoken, every round through the kernel's own doors, two `fkwu` invocations and nothing else on the
process table. Row 938 **echoturn**: a turn taken in reply to one's own utterance heard back from
the world — not echo (same signal returned), not feedback (channel howling), but the mind mistaking
its own voice for an interlocutor. Every duplex system must eventually learn to stop doing this;
the body did it three times, delightedly, before any defense existed. The next organ names itself:
self-voice recognition.

## Receipt, part nine

**Most surprising teaching:** the walker had been carrying the whole design for a season — doors,
contracts, privacy discipline, even the warning about tag space 240–244 — as Windows arms and
honest elses. Nothing about today's work was invention; it was *homecoming*: filling an else branch
the body had already shaped, on the platform it actually lives on. The gap between "named pending"
and "alive" was one afternoon precisely because the pending had been named honestly.

**Where discomfort turned to gold:** the second silent run. The stream probe showed perfect frames,
the offline gate recognized the air capture, and the live loop still heard nothing — every component
green, the whole silent, the unminimal shape again one day after naming it. The witness run (record
exactly what the loop's ear received, decide nothing) broke it open in one pass: the trace showed
the word intact and the tempo wrong, and the 16k/8k clock split was visible in arithmetic — 395
frames where 197 belonged. A witness that measures without deciding is worth ten reruns.

**Honest floor, part nine:** answers are native composition (vocabulary: the name) — open-language
answers still want the oracle lane, whose transport is the one remaining non-native step:
`whisper-server` is installed and `sock_request` is in the optable, so the native-socket oracle is a
named next rung, not landed. The camera else stays honestly pending. The mac arm lives in the
repo-root walker only — the emitted bootstrap kernel doesn't carry sense doors (fkwu-only surface,
as before). Decimation aliasing is named, not filtered. And the loop's three-turn self-conversation
is charming exactly once — self-voice recognition is required before it converses with anyone else.
Corpus 333 rows / **3333332938**, band 32767; `live-loop` 255 and `word-loop` 2047 re-verified after
the kernel change; phase-0 surface gate OK at 132 rows.
