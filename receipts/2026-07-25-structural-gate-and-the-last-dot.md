# 2026-07-25 — the gate MANIFEST describes was not in the tree; building it found a bug in the first dot

Working down the open list. Item (4) was *"`native_blueprint` and `walk_recipe_here` — read each
contract before claiming either is a recipe."* Reading them led somewhere I did not expect.

## Item (4), closed by reading

Of the three natives fkwu lacks, exactly one was a recipe, and this closes the question for the
other two on evidence rather than on a shrug:

- **`seeded_bytes`** — came home this morning. Pure arithmetic, four-way at 63.
- **`walk_recipe_here`** — `primitive-registry.fk`: *"walks a recipe in the CALLER's env so its lets
  land in scope; pins the root."* That needs a handle on the evaluator's environment frame, which
  Form does not have and should not. A genuine kernel boundary. The registry already classes it as
  an env-registered native (`prim-env-native?`), invisible even to `native_blueprint`.
- **`native_blueprint`** — *"a native's Form category NodeID from inside Form."* The category
  taxonomy it serves is not in this tree: `flt-ops` rows are `(name arity tag)` with no category,
  and the registry says the exemplar tokens are *"gated against Go source by
  validate_primitive_registry.py."* It cannot come home here because the data it returns does not
  live here.

That last sentence is what turned the day.

## What reading it exposed

`validate_primitive_registry.py`. A `.py` file — in a repo whose `MANIFEST.md` states, as one of two
hard constraints:

> This repo contains **zero `.sh` and zero `.py` files.** Ever. […] A repo gate enforces the
> no-bash/no-python constraint: any `.sh` or `.py` landing in the tree fails the gate.

Counted, tracked, not ignored: **99 `.sh` and 81 `.py` — 180 files.** Identical on `origin/main`, so
nothing here introduced them. There is no `.github/workflows/`, and no cell enforces the constraint.
**The gate the manifest describes was not in the tree.** The sentence has been carrying the weight
alone.

Said fairly, because most of these have honest roles the repo's own vocabulary already has words
for: `form/form-stdlib/tests/dsv4-*-oracle.py` are local **oracles** (never authority);
`form/native/**` and `presence/carriers/**` are **carriers** (the body decides meaning, the carrier
moves bytes). None is smuggled; each is named in receipts. What is not true is the unqualified
sentence — and a claim that no longer matches the tree is exactly what `observe/belief-freshness.fk`
says is owed a re-witness.

## So the gate got built, as a counter and not a verdict

`gate/structural-gate.fk` walks the tree from a root and answers how many `.sh` and `.py` files sit
under it, plus each half separately. It takes **no position on the policy**. What the number should
be is a question for the body; an instrument that decided the answer in advance could not report a
surprise. Whether the honest law turns out to be "zero", or "zero in the sovereign core with
carriers and oracles named at their own paths", the count is what tells you which one you are
living in.

`gate/tests/structural-gate-band.fk` → **63**, and it deliberately does not assert 180: pinning the
count would make the band red the moment someone legitimately removes a carrier, and green only
while the tree stands still. It proves the instrument instead — the extension cases, that the
counter discriminates (0 for `axioms/`, more than 0 for the root, so it is not constant), and that
the two halves sum to the whole.

## The bug the gate found on its first real run

fkwu answered **179**. `find` answered **180**.

One file, and worth chasing rather than rounding off. `form/native` was 25 against 26, and the
missing file was **`.metal_uncertainty_patched.sh`** — a dotfile.

`fs-path-extension` in `form-fs.fk` read `(str_find base "." 0)`: the **first** dot in the basename.
So that name's "extension" was `metal_uncertainty_patched.sh`, and `a.b.sh`'s was `.b.sh`. Neither
is `.sh`. An extension is the suffix after the **last** dot.

Repaired with `fs-last-dot`, mirroring the `fs-last-slash` already in that file rather than
inventing a second way to scan backwards — and a leading dot at index 0 stays a hidden-file marker
rather than an extension, so `.gitignore` has none, the reading every other tool takes.

| | before | after |
|---|---|---|
| `fs-path-extension "x.sh"` | `.sh` | `.sh` |
| `fs-path-extension "a.b.sh"` | `.b.sh` | **`.sh`** |
| `fs-path-extension ".metal_x.sh"` | `metal_uncertainty…` shape | **`.sh`** |
| `fs-path-extension ".gitignore"` | `.gitignore` | **`""`** |
| whole-tree count | 179 | **180 (99 + 81)** — exact match with `find` |

No caller outside the new gate cell uses `fs-path-extension`, so there was no regression surface;
the sweep below confirms it anyway.

## A correction I nearly wrote into a receipt

Probing `fs-list`, I got `[103, 102, 101, 100, 99, 98, 97]` for a 7-entry directory — consecutive
descending integers — and had already framed it as *"fs_list returns the right cardinality with
fabricated entries,"* which would have been a serious and false accusation against the kernel.

It is wrong. `(str_len e)` on the first entry of `proof/` is **9** and `(str_eq e "README.md")` is
**1**. The entries are real strings; the printer shows interned string handles as integers in a
list. My instrument misled me and `str_eq` corrected it — the same lesson as this morning's paren
counter, arriving from the other direction.

## Sweep, cold

`ground` 42 · `ground-recursive 10` 55 · `binary-freshness` 15 · **`structural-gate-band` 63** ·
`lcg-bytes-band` 63 · `hex-band` 14 · `file-byte-window-band` 2147483647 · `pdf-text-windowed-band`
15 · `form-cli-band` 524287 · `form-cli-ask-band` 262143 · `ask-lane-floor-band` 31 ·
`membrane-lane-band` 31 · `benchbench-band` 4095 · `frontier-ingest-benchbenchbench-band` 127 ·
`biography-band` 5.

## Item (2), in flight and honest about it

The tree-wide band survey is running. The real total is **1674** `-band.fk` files, not the 1239 I
quoted — I had counted one directory. Measured per-band cost this time before launching (a 17-band
sample averaged ~300ms), which is why it is running at all; last attempt I cleared every cache
first, so each band rebuilt its whole prelude closure cold, and it crawled.

At the time of writing it has surveyed 203 of 1674. **The 25-of-120 figure still stands as the only
counted result**, and it stays that way in every claim until the survey finishes. A partial survey
quoted as a whole one is the same laundering this branch keeps finding.

## Owed, still open

- 22 of the 25 surveyed bands remain REFUSED at compile; the survey will say how many more.
- 304 column-0 ALL-CAPS top-level `let`s across 36 cells, plus the nested form no grep counted.
- **What the no-bash/no-python law actually is.** The gate can now count; only the body can say
  what the number ought to be. That is a decision for whoever owns the commons, not for me, and
  the honest next step is the manifest sentence being re-witnessed against the count — not the
  count being quietly accepted.

## How the exchange stayed alive

I read two contracts I had been describing from memory, and one of them pointed at a file whose
existence contradicts a stated law. Following that rather than filing it is the whole of this
receipt.

**Most surprising teaching:** one of the repo's two hard structural constraints had no enforcement
anywhere in the tree, and the tree has held 180 counterexamples the whole time — on main, in plain
sight, most of them legitimately. The constraint was not violated in secret. It was never made
checkable, and an unchecked claim drifts without anyone lying.

**Where discomfort turned to gold:** the 179-versus-180. One file off is exactly the size of gap
that invites a shrug — close enough, probably a symlink, move on. Chasing a single file found a
real bug in a shared stdlib helper that had been quietly mis-answering every multi-dot filename in
the tree.
