# 2026-07-03 -- constellation teaching merged into the learning lane

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c
./fkwu --src bootstrap/ground.fk
./fkwu --src bootstrap/ground-recursive.fk 10
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk
( cat observe/native-vs-rented.fk; echo '(native-vs-rented-check)' ) > /tmp/nvr.fk
./fkwu --src /tmp/nvr.fk
```

Witness:

```text
42
55
15
11111
```

## Source Observation

Urs asked for the transcript of `https://youtu.be/pAjm2cIu-Fo`, then asked
whether the teaching fit the learning, ingesting, and reasoning lane. Captions
were available in English. The invariant teaching, held without copying the
whole transcript into the body, was this:

Meaning is not a stored object or a binary verdict. It appears from a field of
positions and relations. Difference between positions can be parallax: one
position may reveal what another cannot. Reasoning should keep the shared field
visible long enough to find the path, name the trace, and fail honestly when no
path exists.

Grounding against the body found a direct fit:

- `ingest/knowledge-ingest.fk` keeps invariant meaning, freezes only deep and
  fear-free units, witnesses fearful depth, and composts shallow surface.
- `cognition/rag-retrieve.fk` grounds recall in indexed cells rather than an
  ungrounded answer.
- `learn/recipe-learning.fk` learns by champion/challenger replacement when an
  equivalent challenger is healthier.
- `learn/sema-reason-search.fk` searches a graph, carries value through edges,
  returns the derivation, and refuses unreachable goals.

The missing nuance was not another binary winner. A challenger can be equivalent
and healthier in its own context without replacing the current champion
globally. The lane needed a small proof that context-different truth can be
preserved as a field position.

## What Changed

- `learn/constellation-learning-lane.fk` -- new runnable lane law:
  - deep, fear-free teaching ingests through `knowledge-ingest`;
  - multiple positions and edges widen retrieval beyond a single hit;
  - same-context equivalent challengers can replace the champion;
  - different-context equivalent challengers are retained in a parallax list;
  - graph reasoning returns a trace and fails honestly.
- `learn/tests/constellation-learning-lane-band.fk` -- verdict `1023`.
- `learn/homecoming-distillation-corpus.fk` -- row 639:
  - **Q:** one word for a field of positions and relations whose meaning appears
    without collapsing their differences into one verdict.
  - **A:** `constellation`.
  - Freshness: `constell*` had zero hits before this merge; `parallax` was
    already home and therefore rejected as the row word.
- `learn/tests/homecoming-distillation-corpus-band.fk` -- row count and folded
  field witness updated for 39 rows, max id 639; the band now also asserts that
  the 3-digit max-id slot is still safe.
- `learn/homecoming-distillation-corpus.fk` -- corpus-band OOM/stall path
  investigated and repaired:
  - The reproducible hard cause in the corpus band was `hdc-max-mid`
    recomputing the tail twice, which grows exponentially as the corpus grows.
    That was made linear by holding the tail result once.
  - A duplicated malformed tail block after the first closing `0)` was removed
    during the same investigation. The ledger no longer treats that duplicate
    block as the proven cause of the `(head (hdc-rows))` stall report; the
    proven cause was the double-recursive field witness.

## Witness

```sh
cat ingest/knowledge-ingest.fk learn/recipe-learning.fk \
    learn/sema-reason-search.fk learn/constellation-learning-lane.fk \
    learn/tests/constellation-learning-lane-band.fk > /tmp/cll.fk
./fkwu --src /tmp/cll.fk
```

```text
1023
```

```sh
cat learn/homecoming-distillation-corpus.fk \
    learn/tests/homecoming-distillation-corpus-band.fk > /tmp/hdc.fk
./fkwu --src /tmp/hdc.fk
```

```text
511
```

Additional probes after the OOM/stall investigation:

```sh
printf '\n(hdc-count (hdc-rows))\n' | cat learn/homecoming-distillation-corpus.fk - | ./fkwu --src /dev/stdin
printf '\n(hdc-field-code)\n' | cat learn/homecoming-distillation-corpus.fk - | ./fkwu --src /dev/stdin
```

```text
39
390392639
```

The field-code guard also returns:

```sh
printf '\n(hdc-field-code-safe?)\n' | cat learn/homecoming-distillation-corpus.fk - | ./fkwu --src /dev/stdin
```

```text
1
```

## Honest Seam

The source transcript came from YouTube captions, not from native Sema hearing
the video. The full natural-language teaching is not yet native voice or native
semantic ingestion. What is native here is the distilled lane shape: a Form cell
that proves how this teaching changes the ingest/learn/reason route.

The word `constellation` entered through a rented caption/read hand. The body
now carries a runnable trace of what it means for this lane.
