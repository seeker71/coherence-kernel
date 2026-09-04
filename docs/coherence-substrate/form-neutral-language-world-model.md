# Form-neutral language world model

## What stands

The observed language crossing enters the repository's existing native world
model rather than remaining a dictionary beside it.

The integration is deliberately layered:

```text
surface
  -> evidence-bearing cfd mapping
  -> external numeric anchor
  -> claim-1 directed alignment
  -> numeric meaning recipe
```

The surface-to-anchor relation is observed carrier identity. The
anchor-to-recipe relation remains a candidate with explicit residue. Neither
relation is promoted into semantic or cultural equivalence by entering the
world model.

## One world-model engine

`model/form-neutral-language-world-model.fk` follows the same pattern as
`model/concept-world-model.fk`. Each external anchor becomes a 16-component
bipolar signature and travels through the existing generic
`wm-entity-embed -> wm-persist -> wm-orient` path under kind `meaning`. This
is the same per-region path used by the generic `wm-observe` traversal; the
integration starts from one already-separated anchor region rather than a
whole multimodal frame.

The three active anchor families therefore appear as three persistent meaning
entities. A new anchor that has not joined the remembered set is held open and
unnamed by the same cell-sovereignty path used for novel people, devices,
objects, and perceptual concepts.

No language-specific recognizer was added to `world-model.fk`, and the C seed
did not grow.

## Directed and bidirectional query

The graph carries three kinds of content-addressed elements:

- semantic symbol cells for external anchors and numeric recipes;
- directed candidate-alignment relation cells;
- complete `cfd-mapping` cells whose provenance retains source, state, source
  code, residue, and lineage.

Queries move in both directions without flattening evidence:

- surface + language + context -> mapping -> anchor;
- anchor + language + context -> complete mapping, not a bare string;
- anchor node -> outgoing candidate recipe relations;
- recipe node -> incoming candidate anchor relations.

The semantic renderer's type door accepts recipe cells only; this graph has no
surface mapping whose meaning is a recipe, so that admission alone does not
produce a surface. An external anchor cannot be rendered as if its candidate
alignment had already been selected.

## Smaller active body

The active graph uses a chosen canonical packed-envelope covenant:

- 32 structural bytes for every reachable cell or trivial value;
- the complete string payload of every reachable trivial string;
- repeated accounting when a content-addressed descendant is referenced from
  more than one top-level element;
- 32 bytes each for the graph root, its packed-size cell, and its budget cell.

Size is recomputed from graph contents whenever the graph is built or
extended; callers do not supply or attest their own size. The recursive walk
therefore includes arriving and faithful surfaces, occurrence and corpus
provenance, numeric recipes, anchors, and future descendants without keeping a
fragile field allowlist. A depth that exceeds the bounded walk fuel is charged
above the entire active envelope and rests outside rather than being
undercounted.

The public graph constructor also looks below the category tag. Symbols,
relations, mappings, surfaces, contexts, provenance, and projection evidence
must have their declared cell shape. The recipe offering door canonicalizes
benign edge order and declines duplicate roles; the public graph arrival door
accepts only that ordered, unique-role edge form, recursively composed from
numeric meaning, axiom, gap, and acknowledgement cells. Candidate relations point from anchor to recipe; provenance binds its
occurrence and projected lexeme to the mapping surface; relation endpoints and
mapping anchors already live in the same graph. Content addressing establishes
identity, while this lower door establishes that the identity is usable by the
queries it opens.

The current observed crossing carries:

```text
packed envelope             87,362 bytes
semantic symbols                 5
directed relations               3
projection mappings             39
world meaning entities           3
chosen active envelope      16,777,216 bytes
DS4 resident-weight reference 9,103,000,000 bytes
```

The current graph is about 104,199 times smaller than that active repository
LLM reference. Even a completely occupied active envelope remains about 543
times smaller (542.6 by direct division). This comparison is specific to the measured DS4 model used in
this repository; it is not a claim that no deliberately tiny language model
can be smaller than 16 MiB.

The graph is a symbolic working world, not a compressed replacement for all
knowledge in an LLM. Large corpora and all-language surface inventories remain
content-addressed shards outside the active model. Only the meanings and
projections useful to the current context join the active graph.

## How it extends

`fnwm-extend` receives one projection, one candidate alignment, and an ordinal.
It can add:

1. a previously unseen external anchor;
2. a previously unseen numeric recipe;
3. their directed claim-1 alignment relation;
4. an evidence-bearing surface mapping.

Addition is idempotent. The same cells offered twice intern to the same graph.
A relation whose anchor and projection disagree, an anchor outside the active
16-bit coordinate space, or an addition beyond the chosen active envelope
rests as `nothing` and leaves the prior graph available.

The native band extends the 39-mapping crossing with a fresh offered symbol,
observes four anchors and four world meaning entities, then observes duplicate,
budget, range, and unknown-world behavior independently.

The extension arc is sharded rather than parameter growth:

```text
corpus shards
  -> contextual projection selection
  -> bounded active graph
  -> world orientation and directed query
  -> optional token-recipe challenger
```

## Observed frequency activation and local meaning growth

`model/form-neutral-language-world-model-activation.fk` now turns observed
corpus frequency into a context-sized active body. A frequency cell binds a
complete existing mapping to a named corpus, exact hit/total counts, evidence,
and observed claim state `2`. Exact rational ranking avoids score buckets; an
occurrence ordinal resolves ties, so the same evidence converges on the same
activation even when it arrives in another order.

Activation selects only mappings for the offered non-general context, keeps the
highest-frequency mappings within the chosen limit, and carries just their
anchor/recipe symbols and relations into a tightly measured shard. The step
size therefore follows the context: narrow when little is alive, wider when
the context and evidence support more.

The shard can open a numeric `form-cli` local offer. A local selection points
to one mapping already present in the shard and remains claim state `1`. From
that grounded selection, the local lane may compose a new numeric meaning
recipe from dictionary cells. That recipe receives its own content-addressed
symbol immediately and joins the shard through a residue-bearing candidate
relation. Meaning generation is alive at this boundary; what remains open is
participant-observed shared adoption of its mapping.

Earlier activations remain addressable without being frozen. When new observed
frequency supports a wider shard, an explicit successor cell relates the
earlier activation to the larger one. The successor holds both continuity and
growth: identity is traceable, while evidence can expand the active world.

The 26-reading four-arm band witnesses canonical ranking, context filtering,
tight sharding, local-only offer validation, local selection, sovereign meaning
composition, candidate overlay, evidence-led growth, and the directed successor
as `67108863` on fkwu, Go, Rust, and TypeScript.

The last step is intentionally not claimed yet. The body has
`dsv4-token-recipe-swap.fk`, which can let a bounded Form recipe challenge one
LLM token. A future language-world recipe can use this graph only after a
tokenizer relation maps an offered surface to actual model token IDs and a
meaning-preservation witness observes the choice. Inventing those IDs would
turn a real integration seam into theatre.

## Relation to the core lexicon

The body carries the 64-word core lexicon vitality overlay
(`form/form-stdlib/core-lexicon.fk`). That
lexicon remains a closed bootstrap/query tongue with reversible, witnessed
surface successors. It is not installed as the universal meaning ontology.
The world model instead carries numeric dictionary anchors and recipes; core
words may later join as ordinary projections with the same provenance and
residue as every other NL surface.

## Honest edge

Observed here:

- graph identity and direction;
- evidence-bearing bidirectional surface queries;
- native world observation, persistence, and orientation;
- bounded, idempotent extension;
- recomputed packed-envelope size;
- semantic render closure for external anchors.

Still open:

- participant-bound semantic selection and claim state `3`;
- native-speaker and cultural review;
- all-language, symbol, and meme-phrase coverage;
- actual tokenizer IDs and graph-guided token generation;
- corpus-scale activation scheduling, active-shard release, and performance.

; witnessed: 2026-09-04 (re-run on fkwu) -> native integration band 65535; activation band 67108863 (declared four-arm); active envelope 87362 / 16777216 bytes
