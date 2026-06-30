# Receipt — stone S13: the eval-level type / contract system (2026-06-29)

**Stone S13 of the re-architecture (`docs/re-architecture-stones.form`).** A TYPE is the
interface a cell offers (axiom-4); the BLUEPRINT is the structural type (axiom-3);
type-checking is the boundary's trust decision, and a mismatch ACKS NOTHING (axiom-1 /
axiom-5). Landed as a **Form recipe layer over the EXISTING substrate** — the cell-card
facet shape and the kernel's already-native list / string / first-class-nothing
primitives. **No `runtime/fkwu-uni.c` change** (parallel-safe with the sibling stone-2c
runtime work in flight).

## What landed

- **`substrate/cell-type.fk`** — the type/contract recipe, three doors over `cell-card`:
  - `ty-of(cell)` → the structural type: the ordered list of facet KEYS the cell offers.
    Axiom-4 — the type IS the offered interface, read through the existing `cell-facets` /
    `facet-key` readers, never a parallel structure.
  - `ty-satisfies?(cell, iface)` → `1` / `0`, the boundary's structural verdict: the cell
    satisfies iff its offered type covers EVERY key the interface requires (axiom-3
    structural match; **width-subtyping** — offer at least the demanded shape). Pure
    list+string ops → the decision crosses four-way.
  - `ty-check(cell, iface)` → `1` (admit) | `(nothing)` (decline). Axiom-1 + axiom-5: the
    check OFFERS the cell to the interface; the interface ACKS `1` or the canonical
    first-class `(nothing)` (stone 2a). The `(nothing)` is returned LIVE as the tail value
    so the boundary's decline is observable via `nothing?` — a stored nothing loses the
    reduction-step identity; an offered-and-acked one keeps it.
  - `iface-make` / a contract IS an interface-cell whose facets name the required keys, so
    "check against a contract" and "check against a cell's offer" are ONE engine, the
    contract carried as data.
- **`substrate/tests/cell-type-band.fk`** — the band. Cells bound as NULLARY DEFNS (the
  fourth-arm-band-scope idiom): a composed cell stored through `let` degenerates on the
  `--src` seed, so each cell is a nullary recipe rebuilt at use — walker-portable.
- **`docs/re-architecture-stones.form`** — S13 marked `done` with the proof line.

## Gate — fkwu built `cc -O2 -o fkwu runtime/fkwu-uni.c`, run on `--src` (no Go)

**Structural decision — FOUR-WAY (preludes + band, concatenated, run on each kernel):**
```
substrate/tests/cell-type-band.fk  ->  1013   on  fkwu = go = rust = ts
   3      ty-of(person) offers THREE keys   (the structural type IS the offered interface)
 + 10*1   ty-satisfies?(person, Identifiable) = 1   (width-subtyping: name/where/age ⊇ name/where → admit)
 + 100*0  ty-satisfies?(bad,    Identifiable) = 0   (bad offers only name, missing where → decline)
 + 1000*1 ty-of(person) head = "name"               (the Blueprint shape is readable + ordered)
```

**The nothing-ARM — fkwu `--src` (axiom-1; NOT folded into the four-way number, since the
three walkers do not carry `nothing?`, exactly as stone-2a's receipt noted):**
```
(ty-check (person) (iface-id))             -> 1       admit ack
(nothing? (ty-check (bad) (iface-id)))     -> 1       decline acks the canonical first-class NOTHING
(eq (ty-check (bad) (iface-id)) 0)         -> 0       decline is NOT 0 (axiom-1: nothing ≠ 0)
```

## Honest floor (named, not papered over)

- The **structural type system is four-way** (1013): `ty-of`, `ty-satisfies?`,
  width-subtyping, and the ordered key-read all cross `fkwu = go = rust = ts`.
- The **decline ARM is fkwu-native `--src`** — a mismatch acking the canonical first-class
  `(nothing)`. This is **not** a divergence: the three walkers do not implement
  `nothing` / `nothing?` at all (they error `unbound function "nothing?"`), so it is a
  `3-kernel-only` arm with the missing op named, the SAME honest split stone 2a landed for
  nothing itself. fkwu owns the reduction-step nothing; the recipe rides it.
- **`let`-storage of a composed cell degenerates on the `--src` seed** (the pre-existing
  floor named across the offer-ack / stone-2a receipts). The band sidesteps it with the
  nullary-defn idiom; it is not introduced or regressed here.
- **Platform rows pending** — observed on Mac (`--src`, four-way) today; Windows / Android /
  iPhone receipts land as the metal sessions do. Pending is honest.

## Why it is a theorem, not bolted on

The type system DERIVES from the five axioms and adds zero new machinery: the type is the
axiom-4 offer (the keys a cell presents at its boundary), the Blueprint is the axiom-3
present shape (read through the cells the substrate already holds), and the verdict is the
axiom-5 offer/ack whose decline arm is the axiom-1 first-class nothing the reducer already
returns. One engine — a contract is just a cell, checking is just a structural read.
