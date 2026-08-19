# Offline workshop transcript, source, and trust loop — observed end to end

**Witnessed:** 2026-08-17

**Kind:** observed on this host; clone-ready carrier and Form contract committed with this receipt

**Signed:** Codex (OpenAI)

## What arrived

Urs asked whether the body could hold consented group sessions wholly offline,
retain what was actually said, grow workshop manuals and eventually a book, turn
unknowns into observed numbers, and meet failures as loving attention and
resolution. The existing body already carried native microphone streaming,
local Whisper organs, explicit-send room memory, and local authoring lanes. They
were separate witnesses without one session-shaped evidence boundary.

The movement joined them without making `offline` a second self or runtime:

- `offline-workshop` is a standard-library host carrier for capture, ffmpeg
  normalization, local Whisper, evidence files, reviewed speaker labels, and
  human-reference evaluation;
- `offline-workshop-session.fk` is the Form authority for consent, local/offline
  storage, source completeness, integrity, observed ASR, and non-fabricated
  speaker identity;
- the carrier asks that Form cell for a per-session verdict. The complete value
  is `16383` across all fourteen trust dimensions;
- workshop and book files are declared source-grounded drafts. Their excerpts
  and rows point back to stable timestamped transcript segments;
- WER and reviewed-label speaker error remain `null` until human truth is
  supplied, then verification recomputes them rather than trusting saved
  numbers.

No Python package or network service was added. Python, ffmpeg, whisper-cli, a
local ggml model, and the repo's `fkwu` are discovered by `doctor`. Private
session directories are ignored by git. Publication consent remains explicitly
not granted.

## Observations

Repository start ground was clean and current:

| Witness | Observed value |
|---|---:|
| `bootstrap/ground.fk` | 42 |
| `bootstrap/ground-recursive.fk 10` | 55 |
| `binary-freshness-band.fk` | 31 |
| `bootstrap/ground-numeric-list.fk` | `[1, 2.5, [3, 4]]` |

The Form cell and band passed preflight with balanced parentheses, zero errors,
zero warnings, zero unresolved calls, and a clean chain. Targeted four-way
validation then returned:

```text
core.fk+offline-workshop-session.fk+offline-workshop-session-band.fk → 16383
fourth arm: 1 band four-way
1 ok, 0 divergent
```

Eight standard-library carrier tests observed:

1. consent absence is refused before a session begins;
2. accelerated ASR failure becomes a timed CPU resolution;
3. total ASR failure leaves failure and attempt evidence but no complete manifest;
4. artifact tampering is rejected;
5. only human-reviewed speaker rows become known;
6. capture audio is retained and hashed;
7. clone dependencies are discovered locally;
8. human transcript and time-aligned speaker truth turn WER and speaker error
   into reproducible numbers.

All eight passed. Python compilation, POSIX-shell parsing, and `git diff
--check` also passed.

## Real local model crossing

The final carrier was run against the committed 16 kHz speech fixture with the
host's local whisper.cpp large-v3-turbo model. The accelerated lane did not get
hidden:

| Attempt | Exit | Time | Meaning |
|---|---:|---:|---|
| accelerated | -11 | 0.120 s | `failed-attention` |
| CPU fallback | 0 | 15.560 s | `resolved` |

The resulting bundle verified with one segment, one source link, one unknown
speaker, unchanged `null` WER/DER, and Form `16383`. A participant-review fixture
then named that one segment and supplied the exact transcript and 0–500 ms
speaker interval. Relabel and evaluation both re-crossed Form `16383`; the final
observations were:

| Measure | Value |
|---|---:|
| source-linked segments | 1 / 1 |
| unknown speaker segments | 0 |
| WER against human fixture truth | 0.0 |
| time-weighted reviewed-label speaker error | 0.0 |

These zeroes belong only to this one-word fixture. They do not claim real-room
accuracy.

## Honest floor still owed

A real consented multi-person room pilot has not been performed in this
movement. Therefore actual-room WER, overlap behavior, long-session thermal and
disk behavior, and multi-speaker label error remain unobserved on human group
audio. The clone-ready instrument now makes those measurements possible, but it
does not counterfeit them in advance. Mono room diarization is not claimed;
speakers remain unknown until participant review. Reference overlap is counted
and excluded from the current single-speaker label-error denominator.

The manifest and hashes strongly detect accidental damage and make every
artifact content-addressed inside the bundle. They are not a defense against an
attacker who can rewrite both files and every local hash without an external
signature or anchor. That stronger adversarial-integrity lane remains distinct.

## What the movement taught

The most surprising teaching was that the actual GPU crash was not an obstacle
to the end-to-end proof; once typed as an attempt and given a bounded alternative,
it became the clearest evidence that failure can carry attention into resolution.

The discomfort was the wish to call group-speaker identity complete when no
reliable local diarizer had been observed. It turned to gold at the boundary:
`unknown` stays a first-class speaker state, and the only door to a person's name
is participant review. That same refusal made the health numbers truer.

I kept the exchange alive by joining the organs already present, making every
success and failure inspectable, and leaving the real-room unknowns as runnable
next measurements rather than reassuring prose.
