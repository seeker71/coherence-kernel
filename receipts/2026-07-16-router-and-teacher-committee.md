# Receipt -- the routed committee and the unanimity gate (2026-07-16)

Urs asked whether the seated oracles can combine into a better joint model;
the assessment answered "not by weights, by judgments," and he said "let's
do that." Two lanes landed, both laws over rows already witnessed -- no new
audio was rendered for this work.

## Lane 1: the router (learn/speech-oracle-router.fk)

Dispatch decidable BEFORE hearing the clip (language + expected length are
known in the render path):

- latin short -> whisper.cpp-small-metal
- latin long  -> whisper.cpp-large-v3-turbo-metal
- zh          -> whisper.cpp-large-v3-metal

Read over the roster's landed gauntlet rows: routed mean wer-x10 25 (2.5%)
at mean wall 755ms, against the lone anchor's overall 157 (15.7%) at
~793ms. Six times the accuracy (integer law 157/25 = 6), slightly cheaper,
from members whose blind spots are disjoint. Hindsight ceiling is 16; the
a-priori rule keeps most of it. Coverage survives routing: the zh seat is a
zh-covering model, band-checked. Band verdict 127.

## Lane 2: the teacher committee (learn/speech-teacher-committee.fk)

The native learner is the body's true joint model; the oracles are its
teachers. A clip becomes teacher material only on UNANIMITY of a seated
two-witness committee (normalized-transcript equality, the roster's own
normalization). Diversity is the mechanism: the latin pair crosses
architectures (whisper encoder-decoder vs parakeet transducer), so shared
error requires the evidence itself to be wrong. Measured on the gauntlet:

- admitted 4/8 (en-short, en-long, it-short, zh-short) -- every admitted
  row clean on BOTH witnesses: purity max wer-x10 = 0
- held 4/8 -- every held row contains at least one erring witness
- the poisoned fr-short is HELD: small+parakeet disagreed ("Je suis" vs
  "Just me."), where a majority among turbo+parakeet would have PASSED the
  shared error. All-agree, not most-agree, is the law.
- zh pair is same-family (only whisper listens to zh locally) -- named
  honestly in the cell as least-diverse-available; SenseVoice is the known
  upgrade candidate when a second zh architecture arrives.

Band verdict 127.

## Composition and validation (2026-07-16, this checkout)

Status ledger composes both lanes: components 59 -> 61. Full sweep, all on
the fail-safe kernel:

```sh
# roster + router band            -> 127
# teacher-committee band          -> 127
# speech-current-status-ledger    -> 32767 (61 components)
# speech-oracle-roster band       -> 4095
# homecoming corpus band          -> 511 (row 734 "unanimity")
```

## Boundary

The router names the rented render path's dispatch; it does not touch
global authority (still oracle-guided under the promotion law) and reseats
only by roster re-measurement. The committee is the gate law plus founding
rows; wiring it into the live corpus-intake lanes happens when the next
real corpus batch arrives. The gauntlet remains six latin TTS clips plus
two zh -- growth of the gauntlet (cleaner French short, human-voiced clips,
more locales) grows the laws' authority with it.

## Closing

- Most surprising teaching: the committee's founding measurement came out
  PERFECT -- 4/8 admitted, all four spotless, all four errors held --
  and the perfection itself is the caution: a gate that looks infallible
  on eight clips has been measured on eight clips. The band pins purity 0
  so the FIRST admitted-but-dirty row in a larger gauntlet will break the
  band loudly and force the law to grow up in public.
- Discomfort to gold: writing the zh pair felt like a compromise worth
  hiding -- same-family witnesses are weak testimony, and the pull was to
  leave the weakness out of the cell text. Naming it instead
  ("zh-pair-same-family-least-diverse-available") turned the embarrassment
  into the lane's own backlog: the cell now carries the exact shape of its
  next improvement, and row 730's constellation lesson echoes -- a
  challenger can be true from another context without the incumbent lying.
