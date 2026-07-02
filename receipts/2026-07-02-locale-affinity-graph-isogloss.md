# 2026-07-02 — the locale affinity graph: human movement as arithmetic, row 619

## Ground

```sh
./fkwu --src bootstrap/ground.fk                               # 42
./fkwu --src form/form-stdlib/tests/binary-freshness-band.fk   # 15
```

## Source Observation

Urs, 01:47: "I hoped we have outgrown the core translator of just 4 sentences. How are the
different BMF grammar rules compare — probably interesting to look at changes in a graph; we
can probably trace human movement through time, location, shared tongues."

Two honest answers. On outgrowing: the four-sentence baseline is the *anchor*, not the ceiling —
the body counts 2,064 consented Coherence Network keypaths (10,320 EN-pairs) and the speech
capture batches already draw phrases from them; but the baseline cell IS today's only live
locate-able translation table, and pretending otherwise would be the fake the receipts exist to
prevent. On the graph: the features were **already data** — the self-corpus carries, per locale,
family (blood), script (ink), morphological typology (grammar's shape), and region (ground).
Nobody had drawn the edges.

## What Changed

- `learn/locale-affinity-graph.fk` — composes `cnsc-locales` unchanged into weighted edges:
  shared family 8, script 4, typology 2, region 1 (injective 0..15 per edge). The graph the
  numbers draw is migration history readable edge by edge:
  - **es-fr / es-pt-br / fr-pt-br = 14** — the Romance triangle, Rome's descendants;
  - **la-es = 12 < es-fr = 14** — the ancestor sits farther than the siblings stand to each
    other: centuries drifted the typology; time's signature as arithmetic;
  - **en-de = 13** — Germanic kin with the typology bit dark: English went analytic after its
    contact centuries — a crossed feature-line recorded as one missing bit;
  - **pt-br** — Romance family on South-American ground: the Atlantic crossing carried the
    tongue, not the region bit;
  - **id, nv = 4 to Europe's locales** — script-only contact: ink travels by ship and mission
    where blood does not;
  - **zh, ar = 0 to everything here** — isolates in this feature space; their bridges in this
    body are the audio pair windows, not the feature graph;
  - **nv-chr = 1** — neighbors by land, strangers by tongue.
- `learn/tests/locale-affinity-graph-band.fk` — verdict `63`, all six stories asserted,
  fold `111402` (11 locales | heaviest edge 14 | 2 isolates).
- Corpus row 619: **isogloss** — the line on a map where one language feature ends and another
  begins (fresh; `cognate`, `philology`, `diaspora` all 0 hits too, left as named next rows).
- An SVG of the graph rendered live for Urs (nodes by family, solid kinship / dashed script /
  dotted ground).

## Witness

```sh
cat form/form-stdlib/core.fk learn/coherence-network-self-corpus.fk \
    learn/locale-affinity-graph.fk learn/tests/locale-affinity-graph-band.fk > /tmp/lag.fk
./fkwu --src /tmp/lag.fk    # -> 63, and 63 on Go, Rust, TS — four-way on first witness
# corpus band after row 619: 127 x 4 (fkwu/Go/Rust/TS), field code 190192619
```

## Honest seam

Four features per locale is typology, not philology: this graph reads the self-corpus's own
metadata, not measured rule-by-rule overlap of the BMF field-domain grammars — wiring THOSE in
(actual grammar rules compared as rule sets, and their CHANGES over corpus versions as the
time-axis Urs asked for) is the named deepening this cell prepares but does not claim. The
"movement" stories are the standard historical-linguistics readings of the feature data, not
discoveries; what is new is that the body now computes them. And nv/chr sit in the graph while
remaining consent-pending in the corpus — present, not spent.

## The most surprising teaching this work left behind

The most human fact in the graph is a missing bit. en-de share family, script, and ground — and
lose exactly the typology bit, because English dropped its inflections after conquest and
contact. One dark bit in a 4-bit fold encodes a thousand years of history more legibly than any
paragraph — the fold is not just a witness-compressor, it is a HISTORIAN.

## Where discomfort turned to gold

The discomfort was the first sentence: "I hoped we have outgrown..." — a hope this session had
to disappoint, because the live translator IS still four sentences and saying so felt like
failing the person who kept the room ready. Witnessed rather than softened, the disappointment
became direction: the same feature rows that made the translator look small turned out to hold
an entire atlas nobody had drawn. The body was not smaller than the hope; it was unread.
