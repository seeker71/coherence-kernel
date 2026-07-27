# 2026-07-28 — the Form mirror reaches v5, and the divergence that could not fail

The `.fkb` interpretant heal moved the C seed to v5 this morning and left an open work order in its
own receipt: the body's Form-native mirror of that contract still described v4. Closed here.

## What changed

| cell | change |
|---|---|
| `form/form-stdlib/program-image-fkb-byte-container.fk` | version 4→5, `pifbc-builder-id`, field emitted after the version word |
| `form/form-stdlib/program-image-fkb-byte-decode.fk` | reads the builder id, refuses a foreign one with its own reason |
| `grammars/…-byte-container.fk`, `grammars/…-byte-decode.fk` | re-synced byte-identical |
| `…/tests/…-byte-container-band.fk` | golden fixture, lengths 193→237 / 308→352, seven pinned offsets +44 |
| `…/tests/…-byte-decode-band.fk` | length 308→352, forced-byte offset 280→324 |

**The builder id is deliberately not the C seed's.** `FK_FKB_BUILDER_ID` is
`"fkwu-uni " __DATE__ " " __TIME__` — that translation unit's build. This cell is a *different
writer*, so it carries `"form-program-image-fkb-byte-container-v5"`. The whole purpose of the field
is that different writers do not accept each other's artifacts; a container written here and one
written by fkwu **should** be mutually refused. Carrying the C seed's stamp would be this cell
claiming to be a binary it is not — exactly the confusion v5 exists to end.

The decoder refuses a foreign builder with `foreign-builder`, not folded into `bad-version`: the
version is right and the source identity untouched, so reporting either would send a reader hunting
something that is not the cause. The C seed made that same mistake first and repaired it this
morning; the mirror inherits the repair rather than the error.

## Two derivations, not one paste

The golden fixture is a hand-readable byte list, and that readability is why it exists instead of a
hash. So the new field was written out **by construction** — `0 0 0 40` and the 40 ASCII bytes of
the builder string — and then checked against `pifbc-payload-bytes`'s actual emission. Hand-built
and machine-produced agree. Pasting the machine's answer into the pin would have been one
derivation wearing the shape of two.

Same for the shifted offsets: `280 → 324` is recorded in the band as *"moved by exactly 44 — the
4-byte length plus the 40-byte builder string"*, so a future format change has to state what it
moved rather than re-measure and re-pin.

## A/B against origin/main, because green is not the same as unchanged

A baseline worktree at `origin/main`, its own `fkwu`, the same band lists, verdicts diffed:

| set | bands | diff |
|---|---|---|
| every `*fkb*` / `*artifact*` / `*source-compiler*` band | 39 | **identical** |
| broad stdlib sample (every 18th) | 77 | **identical** |

That matters more than the two bands going green. Several of those 39 return blanks or non-all-ones
values — `runtime-program-image-fkb-attempt` at 268435327, `micro-walker` at 16187391, four that
emit nothing at all. Without the A/B I could not have told my damage from the tree's existing
state, and I would have either chased someone else's dark bit or quietly owned it.

## The most surprising teaching

**The divergence could not have failed, and fixing it still mattered.**

I went looking for the interop breakage and there is none: the container cell writes no files, the
byte-file-witness writes no files, and the Form decoder is never pointed at an fkwu-written
artifact. The two writers never meet on disk. Every band was green before and after, and would have
stayed green forever with the mirror describing a format the body no longer writes.

So this is not a *latent* hazard — the body has that word in 54 files, for a hazard that is real but
untripped. This one had **no failure mode at all**. It mattered because the Form cell is what a
reader learns the contract *from*, and a wrong specification teaches wrongly while every test
agrees. That names a whole class of artifact here — `grammars/`, the `.form` axioms, every mirror —
whose correctness is **read and never run**, and which no band can reach.

## Where discomfort turned to gold

The band that caught me was one I did not write and would not have thought to: a bit asserting that
`grammars/program-image-fkb-byte-container.fk` is **byte-identical** to the `form-stdlib/` copy. I
had migrated one and not the other, and the body refused — enforcing the exact two-copies-drift
discipline I was there to repair, one level below where I was working.

The discomfort was small and specific: I had been treating "the mirror" as the thing I was fixing,
while sitting inside a mirror pair I had not noticed I was breaking. The gold is that the body
already knows this shape well enough to have soldered a check for it, and the check was written by
someone who had been burned in exactly the way I was about to be.

## The frontier question

> **What names a statement that binds by being read rather than by being executed?**

**`declaratory`** — against *operative*. Distinct from `latent` (a hazard real but untripped): a
declaratory divergence has no failure mode, only a misteaching. Verified 0 hits. Corpus row
**903**; band **32767**, 298 rows, field code 2982982903.

## Ground stamp

```
./fkwu --src bootstrap/ground.fk                                          -> 42
./fkwu --src form/form-stdlib/tests/program-image-fkb-byte-container-band.fk -> 2147483647
./fkwu --src form/form-stdlib/tests/program-image-fkb-byte-decode-band.fk    -> 536870911
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk           -> 32767
39 artifact bands + 77 sampled stdlib bands: identical to origin/main
```

The open work order from `2026-07-28-fkb-interpretant-heal.md` is closed.
