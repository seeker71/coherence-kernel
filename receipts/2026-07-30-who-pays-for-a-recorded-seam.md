# Who pays for a recorded seam

*2026-07-30, late. The one thing I named last turn and did not diagnose.*

## The debt

Last turn's receipt said, of a band I had found red: *"`nl-extract-band` — 255
with a tally of 1 error and no error line I could find printed. Not diagnosed;
named here rather than stepped around."*

Naming it is not carrying it. This is the diagnosis.

## Why no error line appeared

There was one. I had run the band with a warm cache, which reports

```
warning: cached image was compiled with errors; fix source and rerun to clear
```

— the replayed tally, not the error. Delete the artifacts and the compile speaks:

```
fkwu:1739:2: error: [unresolved-call] 'walk_recipe_here' matched no
  op/rewrite/fn/binding -- typo or missing prelude? Recovered to nothing
  (axiom-5); parse continues
fkwu: 1 error(s), 0 warning(s)
255
```

The same disease `gen-conformance-band` had two days ago, in a different chain:
**a green verdict over an unresolved call**, exit 1.

## Where it came from, and the part that is right

`cognition/tests/nl-extract-band.fk` → `hex.fk` → `form-ontology-loader.fk`,
whose last line is a top-level `(walk_recipe_here …)` installing the engine
constant family. `walk_recipe_here` is a Go/Rust/TypeScript native with no fkwu
counterpart.

The loader **says so, deliberately**, and has since 2026-07-17:

> Recorded, not shimmed: a numb walk would silently skip the generation it
> claims.

That is the right call and I did not touch it. Silencing a seam to make an exit
code green is the thing this body keeps refusing.

## The part nobody carried

fkwu resolves **every** call site in a prelude chain, not only the reached ones.
So every cell that preludes the loader inherits that one error and that exit 1 —
whether or not it ever wanted an engine constant. A band's exit code then cannot
separate *"my band failed"* from *"the loader's recorded seam"*, which is the
whole value of an exit code.

Measured rather than estimated:

| | |
|---|---|
| cells preluding `form-ontology-loader.fk` | **313** |
| of those, using any name from the engine half | **8** |
| using **none** of them | **304** |

`hex.fk` uses exactly one name from it — `fol-bp`, to look up the
`HEX-DECODE-ERROR` sentinel — and was charging the loader's walk to every fkwu
run of every cell that preludes hex.

## The fix

Split the loader along the arm boundary, the same move that healed the codegen
cells:

- `form/form-stdlib/form-ontology-bp.fk` — the reviewed bootstrap `bp` registry
  and its lookups, 749 lines, **arm-neutral**.
- `form/form-stdlib/form-ontology-loader.fk` — the engine/dialect generation and
  its `walk_recipe_here`, 126 lines, preluding the bp cell so **every name it
  used to export is still exported from it** and no consumer changes.
- `hex.fk` repointed to the bp cell.

The seam is not shimmed, not silenced, not moved. Its consumers are.

## Witnessed

```
nl-extract-band                 255        exit 0     ← was exit 1
gen-conformance-band            262143     exit 0
homecoming-distillation-corpus  32767      exit 0
review-ask-band                 511        exit 0
dialogue-covenant-band          111111111  exit 0
steiner-neutral-band            511        exit 0
neutral-rule-mdl-admission      65535      exit 0
native-learned-language-system  32767      exit 0
svg-emit / generate-step        11111      exit 0
```

And the loader's own deepest consumer, A/B'd across the split with caches
cleared: `form-ontology-parity-band` → **1506** on Go, Rust and TypeScript,
which is the number its own header declares — and **1506** with the pre-split
loader restored from git. No regression, verified rather than assumed.

## What remains, with its size

304 of the 313 consumers could be repointed at the bp cell. I repointed the one
that was producing a live red and verified it three ways; I have not audited the
other 303, and a 304-file mechanical sweep at this hour without per-arm
verification would be the kind of confident bulk move this week has been
teaching me to distrust. It is measured, it is in this lane, and it is next.

## The most surprising teaching

**An honest limitation is still a cost, and honesty about it does not stop it
propagating.** The loader's seam is one of the better-written comments in the
tree — it names the op, the arm, the consequence, and the reason not to shim it.
Everything about it is right except that nobody asked who would be paying, and
the answer was 313 cells, 304 of which get nothing for it.

## Where discomfort turned to gold

The pull was to treat "documented seam" as "case closed" — the comment is
thorough, it was written deliberately, and arguing with it felt like arguing
with someone more careful than me. Sitting with the discomfort of a red band I
could not explain away is what separated the two questions that had been fused:
*is this seam honest?* (yes, and untouched) and *who pays for it?* (304 cells
that never asked). A good decision about a cost is not the same as a decision
about who bears it.

## Frontier question

*What names a declared local cost that is silently charged to everyone
downstream?* → **externality**. 0 hits before offering. Named in economics for a
century, and the body had 313 cells paying one. Corpus row **954**.

Corpus band `32767`, 349 rows.
