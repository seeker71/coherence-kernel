# 2026-08-24 — the phrase kept meaning open

The next missing surface was larger than one token. The dictionary could retain
words and symbols, but `we are`, `yes trust`, and a common meme phrase still had
no expression cell of their own. Adding a static English phrase list would have
made the language-neutral claim weaker, not stronger.

`cognition/session-neutral-phrase-graph.fk` now generates two/three-token cells
from actual adjacent arrivals. Its expression address contains numeric locale
bytes, exact surface bytes, and each token's expression facet. Its composition
recipe contains only neutral candidate and structural addresses. Candidate sets
remain nested, so an ambiguous token cannot quietly become its first sense.

There is no phrase table. Recurrence increments observation count and changes
the state to `repeated-expression-meaning-open`; it does not mint an idiom. A
single meaning appears only when an observed expression is explicitly bound to
a closed recipe of symbols already active in the neutral dictionary. That
meaning's namespace-8 address contains the recipe itself. Different locale
expressions therefore share an address only through the same neutral recipe,
not through an English key.

The directed graph uses existing structure symbols as relations: parts,
flow, and reference. Expression-to-component, candidate-to-next-candidate,
expression-to-meaning, and meaning-to-definition edges share one edge body and
can be queried from either endpoint.

## Working observation

The exact band returns `32767/32767` after clean preflight. It witnesses:

- exact locale expression identities across English, German, and Chinese;
- recurrence within and across turns without automatic meaning selection;
- one content-addressed meaning recipe shared by three locale expressions;
- closed-recipe rejection when a symbol is absent from the active dictionary;
- neutral evidence recipes on bindings;
- candidate-set field projection with all 13 axes;
- exact unresolved-residue participation without pretending it has meaning;
- directed part, flow, reference, and definition edges in both query directions;
- one or several bound meanings remaining distinguishable.

The live three-turn witness dynamically generated 89, 1,311, and 19 span cells.
After deduplication and occurrence folding, 1,152 expression cells were active;
118 had recurred, the largest count was 17, and their graph contained 397 unique
edges. The newest expression remained `observed-expression-meaning-open`.

A small multilingual binding demonstration mapped three observed locale
expressions to the same trust recipe:

```
[31013, 8, [[31013, 1, 490]]]
```

Its evidence is the neutral reference recipe `[[31013, 3, 14]]`; the metadata
states `authored-demonstration-not-speaker-reviewed`. The demonstration proves
the shared-meaning door, not the translations.

The temporary Claude watcher was deleted: it belonged to another session and
has no place in this movement.

The honest edge is now contextual binding evidence. Frequency of recurrence can
offer a phrase for attention, but people, cultures, programs, and contexts must
still be able to witness, revise, or refuse its meaning recipe. The graph makes
that future evidence attachable without changing the expression's arrival.

I kept the exchange alive by letting phrases become visible without forcing
them to mean. The surprising teaching was that a meme begins as recurrence but
becomes meaning only through a witnessed relation. Discomfort turned to gold
when the tempting shortcut—calling repetition semantics—became an explicit
`meaning-open` state and a queryable evidence edge instead.

— Codex
