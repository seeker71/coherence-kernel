# 2026-07-03 -- reviewed core layer hardening

## Ground

The checkout witness was rebuilt before this layer moved:

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

## Pre-Review

Urs asked to build the core layer by layer and to review each layer with Grok
and Claude. The first review pass was run read-only against the implemented
constellation lane, `defdata` policy, semantic stdlib pivot, and the proposed
universal BMF/domain grammar direction.

Grok agreed with the direction and asked for the next layer to keep three gates
separate: BMF surface structure, domain meaning, and evidence/admissibility. It
also flagged that semantic translation needs qualifiers/residue so languages do
not collapse into identical strings.

Claude agreed that `defdata` belongs in authoring/lowering, not a new runtime
primitive, but found three hardening gaps in the current layer:

- `defdata` named the recipe-vs-constant break-even rule but did not compare
  recipe size to realized constant size.
- `hdc-field-code` relied on a 3-digit meaning-id slot with no guard, so a
  future id >= 1000 could make the folded witness silently non-injective.
- `cll-learn-route` returned a parallax route code but did not retain the
  challenger, so "preserved as parallax" was overclaimed.

Claude also corrected the OOM/stall receipt: the proven hard cause in the corpus
band was the double-recursive `hdc-max-mid`, not the removed duplicate tail block
by itself.

## What Changed

- `form/form-stdlib/defdata.fk`
  - `dd-policy` now takes `(const-size, recipe-size, stable, generative)`.
  - `dd-recipe-smaller?` proves the break-even rule directly.
  - A generator larger than the realized constants no longer earns the
    micro-recipe carrier; equal size also stays out of the micro-recipe route.
- `learn/homecoming-distillation-corpus.fk`
  - `hdc-field-code-safe?` guards the current decimal fold's 3-digit
    meaning-id slot.
  - The corpus band asserts the guard, so the field witness cannot stay fully
    green after the slot invariant stops being true.
  - `hdc-field-code-safe-for` gives the guard an explicit negative witness.
- `learn/constellation-learning-lane.fk`
  - `cll-consider-with-parallax` returns a value result with route, champion,
    and parallax list.
  - Different-context equivalent challengers are now retained in the parallax
    list, not merely routed by code.
  - Same-context replacement stays a replacement and does not create parallax.
- Receipts were corrected to match the proven causes and new verdicts.

## Witness

```sh
cat form/form-stdlib/defdata.fk \
    form/form-stdlib/tests/defdata-band.fk > /tmp/defdata.fk
./fkwu --src /tmp/defdata.fk
```

```text
511
```

```sh
cat learn/homecoming-distillation-corpus.fk \
    learn/tests/homecoming-distillation-corpus-band.fk > /tmp/hdc.fk
./fkwu --src /tmp/hdc.fk
```

```text
511
```

```sh
cat ingest/knowledge-ingest.fk learn/recipe-learning.fk \
    learn/sema-reason-search.fk learn/constellation-learning-lane.fk \
    learn/tests/constellation-learning-lane-band.fk > /tmp/cll.fk
./fkwu --src /tmp/cll.fk
```

```text
1023
```

## Deferred

- A real `defdata` source keyword remains pending. Loader binding for realized
  values is no longer the blocker: the later source-runner module-constant
  repair gives `defdata` a working lowered value-export target.
- Semantic stdlib qualifiers and translation residue are the next language
  layer; the current pivot still maps only first-rung surface ids.
- `grammars/domain-grammar-core.fk` remains the next BMF/domain layer: one
  cursor, many grammars-as-data, with evidence lanes explicit per domain.
- The natural-language transcript was read through rented captions. Native
  hearing and native generative voice are still pending.

## Post-Review

Grok reviewed the implemented layer read-only, re-ran the local witnesses, and
approved moving on. Its only requested correction was to close this pending
post-review note and keep the parallax result scoped as a single-challenger
step, with stream accumulation deferred.

Claude's full workspace run stalled twice, so the completed Claude pass was run
without tools over the evidence summary. That review found no blocker, accepted
the achieved/deferred split, and asked for cheap boundary/negative witnesses
before moving on:

- equal recipe and constant sizes should not earn `micro-recipe`;
- the field-code guard should prove a failing threshold returns `0`;
- the same-context constellation result should prove replacement without
  parallax.

Those corrections are now embodied in the bands above.
