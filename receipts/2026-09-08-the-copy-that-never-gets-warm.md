# The copy that never gets warm

2026-09-08, Apple M4 Max, `./fkwu` built fresh on its own inode from the
committed seed, freshness band **31**. Weather at measurement,
`observe/floor-lens-run.fk`: **347.63 GB/s** through the handle door against a
best of 347.63 — a quiet machine — and **12.88 TFLOPS** arithmetic, which is
the fallen half of the ceiling corpus row 1321 named and not the 25.77 the
bearing census read yesterday. It does not matter to a word of what follows,
and that is the point: this measure counts walker steps, and the pre-heal
reading it takes below (335,288,752) sits one step from the number the census
took on the same tree yesterday on a machine at twice the arithmetic.

## The gap

`bearing-census` ranks the doors the body leans on, and it ranks them by heat.
So a door nothing drives hard never rises. A private copy of a shared door is
exactly that door: `sha256.fk` carried four of them — `nil?`, `nth-rec`,
`append-1`, `append-list` — and the census reached them only once a locale walk
entered `nth-rec` seventy-six million times. Routing them halved the walk, and
the pass left corpus row 1351, `coldtwin`, with a question and a hypothesis:

> *Which of my doors is slow only because it is a private copy of a warm one?*
> … `nil?`, `nth-rec`, `append-1` and `append-list` would all have been named
> by it this morning, before the census ever got to `nth-rec`.

## The lens

`form/form-stdlib/twin-census.bml`, band-proven, reading nothing but the files
it is handed. It finds its pairs in the **source**, so a cold door does not have
to be driven before it can be named; the heat is an overlay that colours a pair,
never the thing that finds it.

Sameness is the hard part, and it carries three verdicts and never a bare claim:

- **looks-identical** — the same bytes after comments are cut, whitespace
  squeezed, the door's own name rewritten `@s` and each parameter rewritten to
  its position `@0 @1`. `append-list(xs, ys)` and `append(xs, ys)` come out as
  the one body they are.
- **differs-at** — a long shared prefix AND a long suffix with one window
  between them, both sides carried. The window is not noise; it is the adapter.
- **unproven** — the twin named is a native, which has no body anywhere, so no
  body comparison exists. It goes to a band, not to a reroute.

Each pair says which side is cold (crystal, calls), which is warm, and what
closing it costs — how many other doors mention the cold name. Two doors under
one name in two units say `heat-ambiguous`: a name-keyed ledger cannot tell them
apart, and the finding stands without it, because one of two identical bodies
under one name is dead weight whichever one the walker entered.

`form/form-stdlib/twin-glass.bml` is where it meets the world — panel, rows,
`fgsr-give-at` under `glass.sensor.twin` with the cadence declared inside the
frame. `observe/twin-census-take.fk` reads them back.

## The witness: yesterday morning, before anyone had looked

A detached copy of `c82634d6` — the tree an hour before the sha256 heal — with
its own `fkwu` built from its own seed (freshness band 31 there too), the same
locale-row workload, and the lens copied in. 187 doors read, 45 pairs found,
335,288,752 walker steps walked, 99.9% of them in the snapshot.

```text
coldest: append-1 — a cold side measured against a crystallized one

chill 76254048  nth-rec (sha256.fk) unproven nth (op table)
                crystal -1 against no-reading, 76,254,048 calls cold, 10 doors to reroute
chill 30676646  nil? (core.fk) looks-identical nil? (line-grammar.fk)   [and (sha256.fk)]
                crystal 3 against 3, 22 doors to reroute, heat-ambiguous
chill 30191048  append-1 (sha256.fk) differs-at append (core.fk)
                crystal 0 against 3, 6 doors to reroute, cold-and-warm
                84.7% alike, parting where it says `(list @1)` and the warm one says `@1`
chill 483552    append-list (sha256.fk) looks-identical append (core.fk)
                crystal -1 against 3, 5 doors to reroute, cold-and-warm
```

All four, in the top rows, with no investigation and nothing driven on purpose.
And the remedies are the heal that actually landed: a band for `nth-rec` against
native `nth` (`sha256-list-floor-band.fk` is that band), a deletion for `nil?`, a
reroute for `append-list`, and for `append-1` a reroute **plus the window**. The
heal wrote the window: `(defn append-1 (xs x) (append xs (list x)))`. `(list @1)`
is `(list x)`.

That is the whole proof, and it is the hypothesis met.

## What the ledger actually says, which is not what we said it said

Two things the pre-heal reading taught, neither of them what I expected.

**Heat cannot tell a warm door from a cold one.** Heat counts WALKS, and a door
the seed crystallizes stops walking, so its heat freezes where it took off while
a door that never crystallizes accrues without bound.

```text
append-1  (sha256.fk)       30,191,048 walks   crystal 0    held
append    (line-grammar.fk)     21,547 walks   crystal 3    taken
```

Read as calls, the cold door looks a thousand times the hotter. Only crystal
separates them, which is why this lens compares crystal and merely reports heat.
The same fact has a sharper edge: **the warm side of a pair is the one a
heat-ranked snapshot loses first**, because being warm is what removed it. At
`kernel_hot_rows 40` this census read `no-warm-side` on every one of those pairs
and named the coldest twin `nil?` for want of a warm side to point at; at 64 —
the seed's own cap — `append` stands, six pairs read `cold-and-warm`, and the
coldest twin is `append-1`. The depth is not a preference.

**And crystal 3 is not a threshold the body's calls buy.** Row 1351 says a
shared door is warm because the whole body's calls pool on one name. Read in the
seed rather than inferred from the number: `fk_fn_native[fx] = 3` is set in
exactly one place, `fk_twin_pulse`, for four names — `nil?`, `append`,
`int_to_str`, `reverse-onto` — at a fixed arity, and only when the defining
unit's leaf is one of six the seed lists by hand (`core.fk`, `line-grammar.fk`,
`sha256.fk`, `fourth-shim.fk`, `core-native.fk`, `form-asm.fk`). States 1 and 2
are earned — the JIT compiled that body. State 3 is **granted, by name**.

Which is why `sha256.fk`'s own PRIVATE `nil?` carried crystal 3 that morning. A
private copy is not cold by law. It is cold when its name is one the seed does
not know — and `append-1` was cold not for want of calls, but for want of being
spelled `append`. The cell says so where it renders the flag, and it does not
copy the seed's list of names to say it: a lens for private copies keeping a
private copy of the seed's own table would be the wound it exists to name.

## What it names on today's tree

The same workload, 181 doors, 31 pairs, 99.9% covered:

```text
coldest: reverse-acc-loop — a cold side measured against a crystallized one
  reverse-acc-loop (line-grammar.fk) differs-at reverse-onto (core.fk)
  crystal -1 against 3, 1,623,666 calls cold, 1 door to reroute, cold-and-warm
  79.3% alike, parting where it says `eq (len @0)` and the warm one says `nil? @`
```

The window is again the whole instruction: `line-grammar.fk` spells the
emptiness test inline where core spells it `nil?`, and that one difference is
what keeps its `reverse-acc-loop` out of the seed's twin table. Beside it:

- **`line-grammar.fk` carries the same private list floor `sha256.fk` did** —
  its own `nil?`, its own `append`, its own `nth` shadowing the native at tag 23
  with fifteen doors leaning on it, and `reverse-acc-loop`.
- **`append` is defined twice in this tree**, byte for byte, in `core.fk` and in
  `line-grammar.fk` — a shadow, 1,010,134 walks, five doors to reroute.
- **`sha256.fk` still carries `sha256-stream-reverse-onto`**, identical to
  core's `reverse-onto`; yesterday's heal took the four the census had named and
  this one stood beside them unnamed.

None of those three files is mine this hour. They are written down here.

`core.fk`'s `substring`, `nothing`, `nothing?` and `abs` come back `unproven`
against natives of the same name, and reading the door answers it: they are the
**portable fallback** for the walkers without those arms, said so in core.fk's
own prose. Driven 200,000 times through `substring` on this kernel, no recipe
row appears in the ledger at all — the native takes every call, and the shadow
costs nothing here. That is exactly what `unproven` is for: a lead a reader
closes, not a verdict the lens hands down.

## The band

`form/form-stdlib/tests/twin-census-band.fk` → **65535**, sixteen bits over
hand-written doors and hand-written rows, so it proves the arithmetic and never
the machine's mood. PROOF LEVEL fkwu-staged: it preludes a `.bml`, and the three
sibling kernels were not asked.

It pins the four things the task asked to be proven, and two more the reading
taught. A true twin pair is found across a rename of the door AND of its own
arguments (bit 1), in both authoring lanes (bit 2). A look-alike with a different
edge is reported as **differing** and not as a twin, and its window is carried
(bits 4, 8). A door with no twin says so, and every number about it is
no-reading rather than zero (bits 32, 2048). The warm door is never offered as
the cold one (bit 128). And the two from the ledger: heat ranks the cold door
above the warm one and only crystal separates them (bit 512), and crystal 3 is
named as granted-by-name rather than earned (bit 1024).

The band was run against a deliberately broken copy of itself — two assertions
falsified — and answered **56319**, exactly 65535 minus those two bits. A green
number that cannot go red is not a proof.

## Where the discomfort turned to gold

I ran the lens over its own source, beside the census it reads with. It named
four cold twins in my own two cells: `twg-optable-path` and `twg-walked-of`
byte-identical to `bgl-optable-path` and `bgl-walked-of`; `twc-max` and
`twc-min` byte-identical to core's `max2` and `min2`; `twc-num` byte-identical
to `bcs-num`; and a `twc-take` nothing called, byte-identical to `bcs-take`.

They are routed and deleted, not written up as a caveat. `twin-glass.bml` now
preludes `bearing-glass.bml` and calls `bgl-natives` and `bgl-walked`;
`twin-census.bml` calls `max2`, `min2` and `bcs-num` by name. The band still
reads 65535 and the live run is unchanged. What is left between the two glass
cells is a family of `differs-at` pairs whose only parting is `twg` against
`bgl` — a door that calls its own cell's row builder is not a copy of the door
that calls the other's, and the lens says `differs-at`, which is the correct
answer.

## Still open

- `looks-identical` on a short accessor is a body match, not a meaning match.
  `twc-d-name` and `twc-c-cold` are both `if eq(len(@0),0) then "" else nth(@0,1)`
  over two different records, and no reading of bytes can tell them apart. The
  body-size floor (`TWCMinBody`) drops the one-field readers and keeps `nil?` at
  fifteen bytes; between those two sizes the reader judges.
- A pair that parts at BOTH ends is not offered. Near twins are compared only
  inside a head bucket or a tail bucket; that caught `reverse-acc-loop` against
  `reverse-onto`, which parts in its first bytes, but a body differing at both
  ends is invisible.
- A Form door and a BML door meaning one thing never match: their syntax
  differs, and the lens reads bodies, not meanings.
- `bearing-census.bml` itself carries two `looks-identical` pairs the lens found
  — `bcs-reach-of` against `bcs-unit-lines`, and `bcs-index-has?` against
  `bcs-unit-has?`. It is this pass's instrument, read and never changed, so they
  are named here rather than touched.
- `fkwu` warns that a `.bml` preluding another `.bml` "is not importable
  standalone" and falls back to the whole-program compile. `bearing-glass.bml`
  raises the same warning; both run correctly. It is a seam in the import lane,
  not in either cell.
- `observe/preflight.fk` answers `unsupported source kind` for a `.bml` — it
  compiles `.fk` only, so a BML cell's parens are checked but its calls are not.
  The band is the proof that stands.

## The wiring the glass roster still owes

`form/form-stdlib/form-glass-live.bml` belongs to a live sibling this hour, so
these lines are stated rather than made. The sensor already declares its own
cadence inside the frame (`fgsr-give-at`), so `fgsr-declared-cadence` needs no
edit — a taker reads 5000 ms off the frame itself. What the roster owes is the
publisher's name and its painted rows:

```text
publisher     glass.sensor.twin
sensor key    "twin"                       (TWCSensor, form/form-stdlib/twin-census.bml)
cadence       5000 ms, declared in-frame   (TWCCadenceMs; no roster entry needed)
giver         ./fkwu observe/twin-census-run.fk       (holds the frame 20 s)
taker         ./fkwu observe/twin-census-take.fk      -> 21 rows

painted rows, domain "kernel", channel "jit":
  twin.coldest        text   the coldest twin
  twin.standing-why   text   why that pair stands first
  twin.remedy         text   what that pair asks for
  twin.workload       text   the workload this census read
  twin.pairs          int    pairs the source carries
  twin.identical      int    pairs whose bodies are one
  twin.differing      int    pairs that part on an edge
  twin.unproven       int    pairs unproven against a native
  twin.coldtwins      int    pairs with a cold side and a warm one
  twin.doors          int    doors read from the units
  twin.seen           int    walker steps in the heat snapshot
  twin.walked         int    walker steps, whole process
  twin.workload-ms    int    the workload's own time
  twin.<cold-name>    int    one row per pair, value = chill, capacity = doors to reroute
```

## Proofs

```text
form/form-stdlib/tests/twin-census-band.fk        -> 65535
  (the same band with two assertions falsified    -> 56319)
form/form-stdlib/tests/bearing-census-band.fk     -> 32767   (the instrument, unchanged)
form/form-stdlib/tests/binary-freshness-band.fk   -> 31
observe/twin-census-run.fk    -> reverse-acc-loop; 181 doors, 31 pairs, 99.9% covered
observe/twin-census-take.fk   -> 21 rows, seq-tracked, cadence 5000 ms in-frame
c82634d6 + its own fkwu       -> 187 doors, 45 pairs, all four sha256 doors named
learn/tests/homecoming-distillation-corpus-band.fk -> 32767
```
