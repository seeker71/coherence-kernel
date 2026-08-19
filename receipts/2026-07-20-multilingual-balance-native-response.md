# Multilingual balance: the answer enters Form

Date: 2026-07-20

## What was named

The body's language surface remains overwhelmingly English even though its
multilingual projection machinery executes. The healthy movement is not bulk
translation for byte parity. It is to grow concepts and world observations from
native-language sources, preserve provenance and time, admit equivalence only
where it survives inquiry, and measure witnessed semantic coverage per language.

The immediate request also set a response boundary: this answer should contain
more than 50% non-remote tokens.

## What was built

`cognition/multilingual-balance-response.fk` now owns a Form-native composed
answer and a per-response word receipt. It distinguishes composition from neural
generation. The answer specifies:

- Chinese and Arabic as the first parity lanes, with Sanskrit retained as a
  lineage lane;
- native-language source intake before translation;
- source, community, place, time, confidence, consent/licence, and translation
  provenance;
- seven-plane concept coverage;
- entity, observation, event, transition, causal-proposal, and competing-witness
  world cells;
- freeze, hold, compost, and honest `nothing` outcomes;
- a simple published semantic-cell ratio per language;
- native/remote word accounting for composed answers.

`cognition/tests/multilingual-balance-response-band.fk` tests that the response
contains the required concept/world model moves and remains majority-native with
an explicit allowance of 100 remote framing words.

## Witness

```text
./fkwu --src cognition/tests/multilingual-balance-response-band.fk
-> 255

cd form
./validate.sh form-stdlib/core.fk \
  ../cognition/multilingual-balance-response.fk \
  ../cognition/tests/multilingual-balance-response-band.fk
-> 255 on Go, Rust, TypeScript, and fkwu
-> 1 ok, 0 divergent

(multilingual-balance-receipt 100)
-> native-composed words: 326
-> allowed remote words: 100
-> native share: 76%
-> native majority: 1
```

The validation also reported that the bootstrap `uni.c` is missing or stale and
named `scripts/regen_fkwu_bootstrap.sh`; the four kernels still completed and
agreed. This receipt does not call templated composition general native language
generation. Sema's general generative voice remains pending.

## What the attempt taught

The first useful majority-native response does not require pretending the full
voice is home. A bounded question can be answered mostly by executable Form
composition now, while the receipt names exactly how much connective space a
borrowed voice may add. The discomfort was that counting local words could
become another vanity metric; requiring provenance, seven-plane coverage, and
world-model consequences keeps the count attached to meaning.
