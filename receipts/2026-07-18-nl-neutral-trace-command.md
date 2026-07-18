# One command, one legible NL → neutral → NL trace

The ask that shaped this: the concept-query trace should be a single command,
and its response should carry enough pointers that any follow-up inquiry is
obvious from the response itself — native first, oracle only as fallback or
comparison.

## The command

```
./fkwu --src cognition/tests/nl-neutral-trace-live.fk
```

Default lanes: German `Wasser` and Indonesian `air`, plus the neutral-pivot
translation exemplar. A caller's own question is staged as one line
`<asserted-locale> <target-locale> <query text>` at
`.coherence-network/nl-neutral-trace-query.txt` and the same command is re-run
(direct-source Form does not yet read argv; the staged file follows the
`rag-query.txt` convention).

## What each stage line carries

Every stage names: the inquiry plane it answers (`observe/inquiry-planes.fk`
vocabulary — which/where computable, what/why learned), the recipe (code) with
byte size, the data cell with byte interval and size, the blueprint (type) with
arity, the paths available / tried / succeeded / failed / cut, the neutral
symbols or anchors matched, a follow-up pointer, and the north-star gap with
the concrete next step. The report closes with an overall north-star section.

Cut here has its backtracking meaning: alternatives pruned after a commit
(locale seats never tried once the first seat matched). Failures are
attempted-and-missed. Silences remain the choice-receipt timeout analog. The
stdlib `bmf-grammar.fk` engine carries no cut operator; the cut-capable seam is
the seedbank BMF `!` (`form/form-stdlib/seedbank/tests/bmf-cut-stop.fk`) and it
is not on this path — the trace says so instead of leaving cut counts implied.

## Witnessed on 2026-07-18 (live fkwu, Linux x86-64 checkout)

- `de "Wasser"`: 13 seats available, 7 tried, 1 succeeded, 6 failed, 6 cut;
  neutral anchor row 377, PWN 3.0 synset `14845743-n`, TSV bytes
  `[44250..44338)`; generated `id` label `air`; frame
  `Langsung: air — berakar pada tubuh.` re-detected as `id`; rag re-grounds
  `@10.3.99.377` at confidence 100; receipt trace 11/3/7/0.
- `id "air"`: walk hits `en` at attempt 1 on row 618 (the English *air*
  concept) — a real homograph fork, decided by the asserted seat to row 377;
  12 seats cut; receipt trace 5/3/1/0.
- `id "api"` (staged follow-up): row 454, synset `07302836-n`, `de` label
  `Feuer`; rag selects `@10.3.99.454`.
- Pivot exemplar: BMF rules `s prop` on both tongue grammars; production
  `prop` emits blueprint `property @1.2.99.33`; neutral symbols
  `sumber→n0 asli→n1 kernel→n2`, all ice-phase;
  `sumber adalah asli → the source is native`;
  a bare label through the property grammars is an honest PARSE-FAIL.
- Band: `cognition/tests/nl-neutral-trace-band.fk` → **1023** (ten bits:
  both lanes on row 377, both generation directions, walk counters including
  cut, the homograph fork, rag re-grounding both queries, a valid measured
  choice receipt with 0 silences, and the pivot round trip with `n0`/ice).

This organ opens committed files (`read_file`, `file_size`), so it is
fkwu-witnessed with its own band, not claimed four-way.

## Honest seams

- The rag rows are staged demo rows built over the real committed row bytes of
  `cognition/concept-nl-semantic-13-omw.tsv`; their `@10.3.99.*` coordinates
  are demo addresses, not substrate-resolved cells. The healed body index at
  `.coherence-network/rag-index/index.jsonl` is absent in this checkout and
  the report names that instead of pretending.
- The prelude stream still logs pre-existing `[unresolved-call]` axiom-5
  recoveries inside `json.fk`, `cache.fk`, `form-ontology-loader.fk`,
  `sha256.fk`, and `rag-embed.fk` — off this executed path, but they can push
  the process exit code to 1 even when the trace and band complete.
- All non-English labels used remain `mapped-unreviewed`; the trace witnesses
  bounded concept alignment, not fluency.
- Per-rule BMF attempt/cut counters are not yet instrumented inside the
  grammar engine; the trace reports rules available and the matched
  production, and names the instrumentation as north-star work.
- The oracle lane (local, then remote) is declared and deliberately not
  consulted while the native answer is sufficient; the automatic
  native-vs-oracle comparison for continuous learning is pending — the speech
  lanes (`learn/speech-model-auto-selection.fk`) already model the shape.

## Next, in order

1. Heal/regenerate the full `nodeid-rag-v2` body index (+ `rag-adaptive-k`).
2. Grow the `de` pivot column (then more tongues) from parallel corpora via
   `form/form-stdlib/nl-lexicon-grow.fk`.
3. Native-speaker review of mapped labels.
4. Per-rule BMF attempt/cut counters emitted as choice receipts.
5. Wire the oracle comparison lane so every native answer can be scored.
6. argv for direct-source Form so the staged query file becomes optional.
