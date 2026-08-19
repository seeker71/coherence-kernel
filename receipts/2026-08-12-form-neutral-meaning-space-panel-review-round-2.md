# Form-neutral meaning space — independent panel review, round two

Date: 2026-08-12
Artifact: `docs/coherence-substrate/form-neutral-meaning-space-review-round-2.md`
Flow: `observe/run-form-neutral-meaning-space-review-round-2.fk` through
`observe/review-ask.fk`

## Ground before review

The checkout witness was rebuilt before revising or reviewing the artifact.

| Witness | Reading | Exit |
|---|---:|---:|
| `bootstrap/ground.fk` | `42` | `0` |
| `bootstrap/ground-recursive.fk 10` | `55` | `0` |
| binary freshness | `31` | `0` |
| numeric list | `[1, 2.5, [3, 4]]` | `0` |
| native-vs-rented | `11111` | `0` |
| review-ask band | `511` | `0` |
| round-two runner preflight | balanced; 0 errors, 0 warnings, 0 unresolved | `0` |

## Panel witness

One shared prompt went to Grok, Claude, Codex, and Cursor. Each output was
persisted before collection. Codex and Cursor completed empty on the first
collection and were retried once through the same Form-native flow. Codex then
returned a substantive answer; Cursor again completed empty.

| Door | Final bytes | Verdict |
|---|---:|---|
| Grok | 14,633 | `REVISE` |
| Claude | 10,364 | `REVISE` |
| Codex | 10,855 | `REVISE` |
| Cursor | 0 | empty twice; no verdict attributed |

SHA-256 witnesses:

```text
prompt  8a8bbe3ad7c9381002b390e47d404f0876abceba801c8c1623b717ab20fc2d3f
grok    32c0407130b8d0bc2d281a63915ae9e0543861996b3ec177732b24581c6fb76f
claude  30bcf07be075546b44a0c4066be79dd7bae8355aecb809634135e10fb8d8b72d
codex   802e5fa3f3e0cdc5c78aee356c6d03c3a45cc277e018188c2abee44ea9a0a268
cursor  e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
```

## Diagnostic correction during the retry

The retry command preflighted its runner before launching reviewers. That
preflight took much longer than expected and emitted its framebuffer report
late. While it was still running, old first-collection flag files remained in
the shared temporary review directory. Those old flags were initially read as
fresh retry movement.

The late preflight report exposed the mistake. The interpretation was retracted
in the live exchange, timing was reset, and only timestamps and flags written
after the clean preflight were used. The fresh retry then produced Codex's
10,855-byte answer and Cursor's fresh empty completion. Aggregate movement was
not used as causal evidence.

## Unanimous verdict

All three substantive reviewers returned `REVISE`.

They agreed that round two made real repairs:

- meaning compositions are structurally prior to local surfaces;
- continuity is relational rather than a mutable lineage identity;
- recipe edge roles are neutral meaning IDs and gaps are first-class;
- surfaces are first-class, structured, open-tagged, and keep language apart
  from carrier;
- CJK forms, idiomatic wholes, and PL structures are no longer flattened into
  one string kind;
- frequency, projection, embedding, and guidance are separate cell families;
- generated projections do not acquire fabricated corpus frequency;
- the examples no longer assert cross-language equivalence;
- semantic vitality and vitality guidance are distinct and neither gates the
  existence of a meaning;
- example C retracts the unsupported claims of universal inexpressibility and
  higher dimensionality.

The revision is materially closer, but it is not executable-seed-ready as
written.

## Consensus blocker 1: the content-address boundary is wrong

`MeaningRevision` currently contains `recipe`, `evidence`, and `claim_state`.
Under axiom 3, adding evidence or changing candidate to witnessed therefore
mints a new meaning revision. Existing recipes and mappings continue pointing
at the earlier evidence-poorer node. The schema turns witnessing into semantic
identity churn.

The panel converged on this boundary:

```text
MeaningCompositionID = content identity of the neutral recipe only
EvidenceObservation  -> MeaningCompositionID
ClaimStateObservation -> MeaningCompositionID
```

Continuity and epistemic movement are content-addressed relation cells around
the meaning composition. They are not mutable fields and are not children of
the meaning they observe.

Two literal construction cycles were also located:

- a meaning can contain a `GroundingGap` whose subject points back to the
  enclosing meaning ID;
- a surface can contain a component edge whose `whole` points back to the
  enclosing surface ID.

Child-first content addressing cannot mint either parent before its child.
Grounding gaps, surface component edges, tags with evolving evidence, and
provenance must therefore be external relation or observation cells wherever
they would otherwise point back to the subject.

The named continuity relations (`supersedes`, `derived-from`, `overlaps`,
candidate `same-meaning`) also need an actual relation-cell schema with
direction, evidence, freshness, and inverse-query behavior.

## Consensus blocker 2: grounding still relocates the regress

`GroundingObservation.invariant_or_distinction` is a `SymbolRecipe`. It attempts
to ground the meaning space using another meaning-space recipe. Opaque context
and observation IDs can move the same question one edge farther away.

All three reviewers found the non-semantic floor already present in the body:
an offered-interface event with the axiom-1/axiom-5 acknowledgement alphabet
`nothing | 0 | 1 | node`.

The smallest honest ground is not “this observation means boundary.” It is:

```text
under procedure P and offered interface I,
positive stimulus X repeatedly produces acknowledgement/state distinction A,
negative or unchanged control Y repeatedly produces different distinction B.
```

The ground record therefore needs at least the offerer, offered interface,
procedure revision, pre-state/stimulus, acknowledgement, post-state, repeated
positive trials, negative/control trials, a falsifiable comparison, observer,
time/epoch, and residue. At least one leaf must be a non-recipe offer/ack/state
record. This grounds only the reproducible discrimination—not the human gloss
attached later.

`QualifierGraphID` remains undefined and could reintroduce the same regress.
It must either bottom out the same way or remain an explicit gap.

## Consensus blocker 3: carried queries lag declared queries

The blueprint says relations can be queried in both directions, but only recipe
traversal has even a partial shape. A `direction: parse | render | both` field
describes projection direction; it is not an inverse index.

Before broader claims, the executable layer needs forward and reverse query
functions over the same external relation cells. Additional located gaps can be
deferred from the first seed but not called complete:

- projection observations need a projector/renderer revision and a reading per
  candidate, not one reading for a whole candidate set;
- residue must say what source distinction failed to map and relative to which
  output;
- CJK observations eventually need normalization, script/orthography, and
  segmentation evidence;
- meme templates eventually need slots, variants, and possibly multimodal
  structure rather than a tag alone;
- PL patterns eventually need grammar/AST schemas, typed holes, and an
  executable-behavior witness;
- an embedding inverse is a metric/query procedure, not an intrinsic reverse
  graph edge.

## Reviewer disagreement: is one revision one sense?

Grok accepted the explicit invariant as sufficient if mappings carry context.
Claude and Codex rejected “sense” as the base individuation criterion because a
sense is a lexicographic observation about how local surfaces divide usage.
They agreed that no mutable Sense object is needed, but recommended external
`asserts-sense`, `same-sense-as`, or split observations.

The more form-neutral resolution is:

- content composition individuates a meaning node under axiom 3;
- local usage observations may report that a composition covers one or several
  lexicographic senses;
- such evidence may motivate new, more precise compositions without rewriting
  the old node or making a language-local sense the neutral identity floor.

This resolution is a recommendation from the review, not yet a witnessed law.

## Examples after round two

### A — boundary-related surfaces

Substantially more honest: independent mapping candidates, visible partial or
unresolved coverage, distinct PL structures, and no global collapse of `界`.
Still not executable because recipe leaves, qualifiers, residues, and evidence
are named but not instantiated. Claude also found that region-a and region-b
should be one region meaning in two structural positions, not two semantic IDs.

### B — idiomatic neighbors

Still the strongest example. The idiomatic whole is independent of its visible
parts; English, Chinese, and Japanese phrases map to distinct candidates; PL
output is generated, not observed; meme-template remains an evidence-bearing
optional tag. It still needs actual recipes and an `overlaps` relation schema.

### C — no witnessed atomic surface

Its epistemic posture is now honest, and semantic vitality is correctly
separated from guidance. But no corpus, zero-frequency observation, recipe,
projection result, trial, or guidance observation is instantiated. It therefore
cannot yet claim absence “in named corpora.”

## Smallest executable seed permitted by the panel

The reviewers independently converged on a narrow probe rather than examples
A–C:

1. Mint one neutral recipe twice and witness the same content ID; change one
   role and witness a different ID.
2. Attach evidence and candidate/witnessed observations externally; witness
   that the meaning composition's ID does not change.
3. Record one real offered-interface positive distinction, replicate it, and
   contrast it with a negative or unchanged control using only actual
   offer/ack/state records at the leaf.
4. Create one surface form and one external mapping relation to the meaning.
5. Query the same mapping relation meaning→surface and surface→meaning.
6. Defer frequency, embeddings, guidance, CJK segmentation, meme structure,
   and the three large examples until this floor passes.

The band should make four narrow claims observable: content identity,
repeatability, control discrimination, and external mapping stability. Passing
it would not ground “boundary,” prove a translation, or establish cross-language
sameness.

## Closing witness

The exchange stayed alive by correcting the stale-flag interpretation in
public and resetting the retry clock rather than silently laundering old state
into new evidence. The most surprising teaching is that content addressing
turns vague semantic modeling defects into literal construction cycles. The
discomfort turned to gold when the apparently strongest addition—grounding by
observation—revealed the exact smaller floor: a reproducible distinction under
a negative control before any local surface is attached.
