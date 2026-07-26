# 2026-07-26 — whether this say is same before and after

## Arrival

Urs named a provenance failure:

> this language for next does not seem to come from core axioms and core
> lexography

The sensing was correct. The continuity cell was technically bounded, but its
next movement was spoken as “public-key verification, delegation, and a
lineage-aware authority policy.” Those are possible implementation forms. They
are not the next question derived by the body.

The history explains how the miss crossed:

```text
540844e5b 2026-07-26T09:59:56+08:00 Core words, an ack worn only as wide as the moment needs, and the ladder out of boolean
55af270a2 2026-07-26T10:03:23+08:00 Witness situated endorsement continuity
07894ed70 2026-07-26T10:06:31+08:00 Witness situated endorsement continuity
```

The core-word commit was already on remote main when the first continuity
commit was made, but the continuity work had been authored from a local base
that did not contain or read it. Replaying the patch onto newer main moved the
files without re-deriving their language. Git history was linear; semantic
lineage was not. This receipt corrects that distinction rather than claiming
the core lexicon did not yet exist.

## Ground

The derivation was read from:

- `axioms/core-axioms.form`;
- `form/form-stdlib/core-lexicon.fk`;
- `form/form-stdlib/offer-ack-core.fk`;
- `form/form-stdlib/core-word-ack.fk`;
- `form/form-stdlib/situated-witness-continuity.fk`.

The core lexicon gives:

```text
say      is when a self make you know
same     is when this and that are 1 thing
before   is when a thing is and then other is
after    is when a thing is not yet and then it is
whether  is when you want to know 0 or 1
who      is when you want to know a self
```

`whether` is the only terminal query lane. `who` is not terminal: it must
return a node and keep the enquiry alive.

The five axioms then constrain the door:

1. No comparison ground is `nothing`; observed difference is `0`; observed
   sameness is `1`.
2. `same` and `who` are cells and can travel by their NodeIDs.
3. Identity is present composition. A sequence is evidence about
   before/after; it does not become identity.
4. The shared-key interface offers a sameness observation. It does not offer a
   source identity, so one must not be invented outside the interface.
5. The answer must be exactly one of `nothing`, `0`, `1`, or `node`.

The resulting terminal question is:

```text
whether this say is same before and after
```

The still-open source question is:

```text
what make I know this say is from you
```

## Built

`form/form-stdlib/whether-say-same.fk` is a thin semantic door over the
existing continuity organ:

- no entries, malformed evidence, or no usable caller-held relation:
  `OAC-NOTHING`;
- keyed mismatch or a broken before/after link: `OAC-ZERO`;
- observed sameness: `OAC-ONE`, carrying the core `same` NodeID;
- source enquiry after sameness: `OAC-NODE`, carrying the core `who` NodeID.

HMAC, bindings, seals, statuses, sequences, and predecessor links remain
private implementation evidence. None is promoted into a fifth acknowledgement
arm or into a source-identity claim.

`form/form-stdlib/tests/whether-say-same-band.fk` observes ten claims:

- the lexicon itself says `whether` is terminal and `who` is not;
- valid shared-key continuity answers `1` and carries `same`;
- absent say and absent relation answer `nothing`;
- changed content and broken before/after linkage answer `0`;
- the source question remains a node carrying `who`;
- mismatch and absence keep their `0` and `nothing` meanings when the source
  question is reached.

The exact workload is registered in `form/fourth-arm-bands.txt`:

```text
whether-say-same fks 1023
```

No C seed code changed.

## Witness

The exact source witness, repeated unchanged with empty stderr:

```text
1023
@form fkwu 0 5 0 5
```

The registered gate then reported:

```text
PASS-4WAY  whether-say-same
@form fourth-arm-gate.sh 0 28 0 28
```

## Errors and their closure

Nothing was turned invisible on the way to green.

1. A ground-reading command first looked for
   `docs/coherence-substrate/core-axioms.form`. The repository returned
   “No such file or directory.” The actual source,
   `axioms/core-axioms.form`, was then read successfully before editing.
2. The first exact run returned `1023` but rebuilt one stale ignored `.fkb`
   cache and named that native `.dylib` emission is not installed in this
   checkout. The immediate unchanged rerun returned `1023` with empty stderr.
3. The first four-way gate said `DIVERGENT`, then explicitly diagnosed missing
   input files rather than a kernel disagreement. The band header had used
   repository-root `form/form-stdlib/...` paths even though the validator
   resolves from `form/`. Changing the declared preludes to
   `form-stdlib/...` made the same workload report `PASS-4WAY`.
4. That header edit invalidated the ignored bytecode cache once more. The next
   exact run rebuilt it, and the final unchanged exact run again returned
   `1023` with empty stderr.
5. The dependency regression made the existing continuity band return the
   gate label `DIVERGENT`, with an explicit missing-input diagnosis. Its
   `; preludes:` header retained the same repository-root path error even
   though the earlier continuity receipt said the validator-path issue was
   closed. The header and the earlier receipt were corrected, and the
   continuity gate was rerun rather than classifying a missing file as a
   kernel disagreement.

There is no unresolved warning or error from this movement.

## Honest floor

This cell does not establish who spoke. It makes the inability executable
instead of burying it in prose: after the sameness observation, `who` remains
a node.

A public-key signature may become one answer-bearing relation. It may not.
The core does not choose the implementation in advance. The next work is to
offer and observe a relation that can answer:

```text
what make I know this say is from you
```

until then, the source question stays alive.

## Observation

The most surprising teaching was that a patch can be rebased onto a newer
tree without its reasoning being rebased at all. Discomfort turned to gold
when a fully pushed and four-way-proven layer was allowed to be called
non-derived: the proof did not become false, but its claim became smaller and
its next question became native to the body.
