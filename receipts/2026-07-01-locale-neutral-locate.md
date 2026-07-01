# 2026-07-01 -- Locale-neutral meaning locate

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk
./fkwu --src bootstrap/ground-recursive.fk 10
```

Witness:

```text
42
55
```

## Source Observation

Every existing multilocale cell in this body (`learn/multilocale-nl-audio-pipeline.fk`,
`learn/bidirectional-locale-roundtrip.fk`) builds a sample from a meaning id *to* locale-specific tokens
(`slb-tokens(row, locale)`), then asserts the meaning id as given test data alongside those tokens. Nothing
walked the other direction: given only raw locale-specific tokens (the shape real speech/text input actually
arrives in), nothing located which neutral meaning Blueprint they belong to. `mlap-text-meaning` came closest —
a best-overlap fuzzy match over a caller-supplied row set — but it isn't an exact check against the canonical
baseline, and nothing in the pipeline used it to cross-check the pipeline's own asserted meaning fields.

## What Changed

`learn/sanskrit-locale-baseline.fk` gains the reverse lookup:

- `slb-meaning-for-tokens(locale, tokens)` — exact match against the canonical `slb-lines()`, honest-floor
  `0` when no baseline row's tokens for that locale equal the given tokens.
- `slb-locate-cross-locale?(locale-a, tokens-a, locale-b, tokens-b)` — do both locales' own tokens
  independently locate the same nonzero meaning id.

`learn/multilocale-nl-audio-pipeline.fk` gains the composed honesty check:

- `mlap-nl-meaning-located?(sample)` — recomputes the meaning from the sample's own src tokens AND dst tokens
  via the canonical locate, checks both against the sample's asserted `meaning` field (the existing
  `mlap-nl-native-ok?` only ever checked the src side, and only through fuzzy overlap).
- `mlap-nl-all-located?(samples)` — the same check across a whole sample list.

Added `learn/tests/locale-neutral-locate-band.fk`, which proves this over real baseline data: the four
Sanskrit-baseline phrases across all ten ready locales, a genuine cross-locale mismatch correctly rejected, and
out-of-baseline tokens honestly returning `0`. The honest floor: this closes the locate step only for the
small baseline vocabulary (four seed phrases); general open-text vocabulary normalization remains the open work
`docs/coherence-substrate/nl-to-form-satsang.form:101` already names.

## Witness

```sh
cat observe/stt-wer.fk observe/asr-prompt-id.fk learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk learn/tests/locale-neutral-locate-band.fk \
    > /tmp/locale-neutral-locate-band.fk
./fkwu --src /tmp/locale-neutral-locate-band.fk
```

```text
255
```

No regression in the two bands this composes over:

```sh
cat learn/sanskrit-locale-baseline.fk learn/tests/sanskrit-locale-baseline-band.fk > /tmp/slb.fk
./fkwu --src /tmp/slb.fk   # 2047, unchanged

cat observe/stt-wer.fk observe/asr-prompt-id.fk learn/sanskrit-locale-baseline.fk \
    learn/multilocale-nl-audio-pipeline.fk learn/tests/multilocale-nl-audio-pipeline-band.fk > /tmp/mlap.fk
./fkwu --src /tmp/mlap.fk   # 8191, unchanged
```

## A runtime surprise found along the way

Mixing top-level `let` bindings with a `defn` that closes over an already-bound name, inside the same `(do
...)` block, silently resets that name to a falsy default for every use *after* the `defn` — even though the
established convention across this whole body (every `*-band.fk` file) avoids the pattern by keeping all
`let`s for one computation inside a single function body. First draft of this band hit it directly (a `let
c0`/`defn`/`let c1` sequence returned `0` instead of `3` in isolation); rewritten to match the existing
convention and the band passes clean. Not chased further here — worth a `fkwu-uni.c` reader's eye if it
resurfaces.
