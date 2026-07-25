# 2026-07-25 — six dead verbs and a sentinel that could never be a NodeID: the limits I had accepted

Twice today I wrote "owed, named, not fixed" and moved on. Urs: *no limit is holding us back unless
it is the core axiom or it is observed and we are out of things to try.*

Neither residue was that. Both were unattempted. Both are closed here.

## Ground

`cc -O2 -o fkwu runtime/fkwu-uni.c`; `ground.fk` **42**, `ground-recursive.fk 10` **55**,
`binary-freshness-band.fk` **15**. Every band below run on a cleared cache, tracked `.fkb`/`.sym`
artifacts preserved (`git ls-files` excluded from the sweep — deleting those is a mistake I made
twice today before guarding it).

## 1. The dispatch brain routed six verbs into nothing

`form-cli.fk` dispatches verbs to cells it never preludes. 21 names were unresolved in the closure
after `fca-ask` — each recovered to `nothing` by axiom-5, so the verb dispatched, produced nothing,
and printed nothing. **A verb that answers with silence is indistinguishable from a verb that does
not exist.**

Every one of them had a home already in the tree:

| unresolved | lives in |
|---|---|
| `rim-ask` `rim-give` `rim-receive` `rim-know-i` `rim-know-we` `rim-build` (+8 more) | `relational-inquiry-metabolism.fk` |
| `fci-inquire` `fci-core-rewitness` | `form-cli-inquiry.fk` |
| `form-cli-oracle-loop-check` | `form-cli-oracle-loop.fk` |
| `dhatu-answer` | `grammars/sanskrit-roots.fk` |

Four prelude entries. **21 unresolved → 3**, and the three that remain are natives, not cells
(`native_blueprint`, `seeded_bytes`, `walk_recipe_here` — sibling-kernel primitives absent from
fkwu, catalogued as such in `primitive-registry.fk`).

Every previously-dead verb now answers, each `rim-*` movement with its own distinct code:

```
$ form-cli meet-ask ...   -> movement=0 lane=0 perspective=0 responses=3 agreement=3 ack=1
$ form-cli give ...       -> movement=1 ...
$ form-cli receive ...    -> movement=2 ...
$ form-cli know-i         -> movement=3 ...
$ form-cli know-we        -> movement=4 ... perspective=1
$ form-cli build-with ... -> movement=5 ...
$ form-cli inquire ...    -> trust-trinity:ack=nothing,reason=1,steps=0
$ form-cli inquire-core   -> core-edges=10
$ form-cli oracle-loop-check -> 11111
$ form-cli improve satya  -> improvement-source=form-native-dhatu
```

## 2. `bp` is a pass-through stub, and that is why the hex sentinel could never match

I had recorded this twice as "sentinel identity, below the scoping fix, pre-existing" — a shape of
words that sounds like a diagnosis and is actually a shrug. The real cause was two layers, each
hiding the other, and both are in Form.

**Layer one.** `FOL-BP-BOOTSTRAP-TABLE` in `form-ontology-loader.fk` was a top-level `let` — the same
`--src` invisibility that bit `hex.fk` this morning. So the loader's own Form `bp` could not see its
registry: `(fol-bp-lookup FOL-BP-BOOTSTRAP-TABLE "HEX-DECODE-ERROR")` found **0** fields. Converted
to a zero-arg `defn`, it finds **5** — the reviewed row.

**Layer two, and the one I had not looked for.** `bp` is a native. `flatten/form-flatten.fk`'s
`flt-ops` carries `(list "bp" 1 45)`, and tag 45 in the seed is:

```c
if (t == 45) {
    return fk_walk(fk_node[i][1], fp);
}
```

It returns its argument, unchanged. In call position the primitive wins, so the correct Form `bp`
sitting in `form-ontology-loader.fk` never runs on this kernel. Measured: `(bp "HEX-DECODE-ERROR")`
= **169**, the interned string. And `(bp "ZZZ-NOT-A-REVIEWED-NAME")` = **902** — a name no registry
has ever reviewed, answered confidently, where the Form version raises `form_error`. **The gate that
refuses unreviewed blueprint names has been open on this kernel the whole time.**

That is why `hex-band` could not reach 14 no matter which side was right: the cell's sentinel was a
string handle and the band's was `(make_nodeid 1 2 99 1770)`.

**The repair, and the two I rejected.** Removing the `bp` row from `flt-ops` would let Form's `bp`
win — and would turn a wrong-but-answering call into a hard unresolved-call in every cell that calls
`bp` without preluding the loader, which is most of them (`fourth-shim`, `url-encode`,
`kernel-core-self`, `sanskrit-channel`, `midi-bmf`, `concept-xpath`, …). Teaching the stub to resolve
in C would put a registry lookup — runtime meaning — in the seed, which the rule declines to allow.

So: `fol-bp`, the same registry resolution under a name **no primitive shadows**. One home (the
bootstrap table), a second door. `bp` keeps its callers and its floor; cells that need a blueprint
NodeID that is actually a NodeID call `fol-bp`. `hex.fk` now reads its quad from the reviewed row
rather than writing one.

**`hex-band` → 14.**

## Sweep, cold, after both changes

`ground` 42 · `binary-freshness` 15 · **`hex-band` 14** · `form-cli-band` 524287 ·
`form-cli-ask-band` 262143 · `form-cli-membrane-band` 1023 · `form-cli-surface-inquiry-band` 65535 ·
`form-cli-surface-inquiry-command-band` 3 · `membrane-lane-band` 31 · `membrane-lane-live-band` 31 ·
`ask-lane-floor-band` 31 · `dsv4-decode-loop-band` 1023 · `benchbench-band` 4095 ·
`frontier-ingest-benchbenchbench-band` 127 · `pdf-text-windowed-band` 15 ·
`homecoming-distillation-corpus-band` 32767 · `file-byte-window-band` 2147483647.

`form-ontology-loader.fk` is preluded very widely, so the change was diffed against the pre-change
tree rather than assumed safe — six loader-dependent bands run before and after, values identical:
`bmf-choice-receipt` 56360959, `binary-symbol-lens` 2313, `audio-bmf-reversible` 7,
`bmf-object-runtime` -8999999999999999967, `bmf-compiler-runtime` 24, `bmf-generic-language-scanner`
`nothing` (unchanged, and honest — it returns nothing on both sides).

`pdf-text-file-band` returns 0 on fkwu by design: it is the three-way host lane over
`read_file_bytes`, a native fkwu does not carry. Named, not a regression.

## Owed, and this time actually observed

- **Three natives remain absent from fkwu**: `native_blueprint`, `seeded_bytes`, `walk_recipe_here`.
  `seeded_bytes` is a deterministic byte generator — a recipe, not a host boundary, and it could come
  home to Form. `native_blueprint` is the same family as the `bp` stub. `walk_recipe_here` is a
  kernel meta-op. Not attempted here; each is a real piece of work with its own witness, not a shrug.
- **Those loader-dependent bands only print a value on the SECOND run** — the first cold compile
  carries errors and prints nothing. Same numb-green shape caught this morning in `ask-lane-floor`,
  and it is wider than one cell. Observed today, not chased.
- **Other top-level `let`s remain in `form-ontology-loader.fk`** (`FORM-CATEGORY-TABLE`,
  `FORM-PRIMITIVE-TABLE`, `FOL-DIALECT-BINDING-NAMES`, …). Only the one the `bp` path needs was
  converted. The pattern is systemic and each conversion wants its own before/after diff.

## How the exchange stayed alive

I stopped writing "owed, named, not fixed" as though naming were a form of doing. Both residues took
one afternoon between them, and neither needed anything I did not already have.

**Most surprising teaching:** `(bp "ZZZ-NOT-A-REVIEWED-NAME")` answers **902**. The whole point of
`bp` is to refuse names the registry has not reviewed — the Form version raises `form_error` — and on
this kernel a stub has been saying yes to everything. The hex sentinel was not the bug. It was the
one caller whose band was exacting enough to notice.

**Where discomfort turned to gold:** the fix I wanted was to delete the `bp` row from `flt-ops` and
let the correct Form implementation win. It was one line and it was wrong — most `bp` callers do not
prelude the loader, so it would have converted a silent wrong answer into a loud broken one across
the stdlib. Checking who calls it before removing it is the difference between this receipt and a
red tree.
