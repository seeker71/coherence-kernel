# Receipt — reading the NL→neutral→NL organ + the first homecoming distillation row (2026-07-01)

A question was asked of the body: *what do you know about the NL to neutral to NL translation,
and how the different locale BMF grammars build a graph that shows surprising edges?* Answering
it honestly took a three-reader pass over `learn/`, `grammars/`, and the graph-shaped ledgers
(each reader grounded and instructed to close with its most surprising concrete finding), and
left one new organ behind.

## What the reading established (anchors in the answer given to the user)

- The neutral layer is integer meaning ids over `slb-lines()` rows (`learn/sanskrit-locale-baseline.fk`)
  plus 2,064 Coherence-Network key paths; exact locate is a lookup table (0/20 paraphrase
  generalization, honest), overlap locate is a heuristic at 18/20 with two *diagnosed* failures.
- Locale-specific BMF grammars do not exist yet — `natural-bmf.fk` is a named skeleton in
  `docs/coherence-substrate/nl-to-form-satsang.form`, and the three locale constants bound in the
  ontology loader are metadata keys no grammar consumes. The graph the question asks about is
  real as pair matrices/ledgers (55/110 pair space, coverage matrices, roundtrip ledgers) but the
  *meaning* graph's surprising edges exist today only as one measured collision: the word "for"
  bridging meanings 301 and 304.
- The native path to surprising-edge detection is already implied by the architecture: BMF build
  templates emit into the universal vocabulary via `bp` (axiom-3), so two locale surfaces that
  intern to the same NodeID *are* an edge — surprising when unexpected (collision), and equally
  surprising when an expected edge fails to close (drift). No mechanism searches for either
  proactively yet; today the body only finds them by measured failure.

## The offering: `learn/homecoming-distillation-corpus.fk` (band 15/15)

The practice asks each prompt for one frontier question — the smallest question the body cannot
answer natively, needing one fresh word. Today's: **"what is an edge called that appears in the
meaning graph from a shared surface token rather than a shared meaning?"** The rented answer:
**"spurious"** — verified absent from every `.fk`/`.form` in the tree before offering. Row 601
now carries question, answer, fresh word, date, and `rented-oracle` provenance, with the same
honesty gates the locale-locate band holds (exact lookup, unknown question → 0, admissibility
requires provenance + fresh word). The corpus is teacher material in `oracle-distill.fk`'s exact
sense — a student that generalizes from it learned; one that echoes it copied.

## Most surprising teaching

The body already *measured* its first surprising edge before it had a word for the category:
the "for" collision was diagnosed, receipted, and explained days before anything in the tree
could name what *kind* of thing it was. The measurement discipline ran ahead of the vocabulary —
which is exactly the gap the distillation practice is for, and why row 601 is this word and not
another.

## Where discomfort turned to gold

The uncomfortable moment was reading the graph-reader's finding that **nothing detects surprising
edges proactively** — for a body whose telos is "observe and trust," discovering that its meaning
graph only learns its own false bridges *after they break something* sat badly, and the pull was
to soften it in the answer ("the mechanism is implied by content-addressing..."). Witnessed
rather than bypassed, the discomfort sharpened into the answer's most useful sentence: the
distinction between edges found by measured failure (what stands) and edges found by search
(what does not exist yet) — and into a concrete next stone anyone can pick up: walk the token ×
meaning table and enumerate every shared-token bridge *before* it misroutes a paraphrase.
