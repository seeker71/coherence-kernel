# A lens that sees reach

2026-09-08. Last night this body asked itself a question it could not answer
(corpus row 1358, `limbkept`):

> *Can a cost lens tell a wall from a name that fell off a standing arm?*

No. A lens that weighs each door sees **weight**, never **reach**. The bearing
census named `fstr-find-loop` at 35 million calls with the remedy *mint a
native*, and there was nothing to mint: `str_find`'s tag-30 arm had stood
complete in `runtime/fkwu-uni.c` since before the name existed, and so had its
serializer arm. Only the row had gone. Four such verdicts in three days were all
answered by a routing to something that already stood.

`form/form-stdlib/mirror-census.bml` is the lens that would have said so in one
reading. It takes a name and answers which of its mirrors still stand.

## Eleven mirrors, because a native does not stand on one thing

| mirror | file | keyed by |
| --- | --- | --- |
| manifest | `native-op-manifest.fk` | name → arity, tag, class |
| flt-ops | `flatten/form-flatten.fk` | name |
| row | `fkwu-optable.h`, `fk_optab` | name → arity, tag |
| arm | `runtime/fkwu-uni.c` | **tag** |
| fkc | `fkc-table-serialize.fk` | **tag** |
| rw | `fkwu-optable.h`, `fk_rwtab` | name |
| go rust ts | the three proof siblings | name |
| jit | `jit.go` emitter arms | name |
| abi | `jit.go`'s value-ABI list | name |

Three of those were not in the plan and are the reading's spine.

**`fk_rwtab` is a second table in the same header, and its third number is a
program length, not a tag.** Read as one table, `substring` reads as a native at
tag 14, an arm answers at 14, and the lens vouches for a route that does not
exist. Found by building the lens, which is the only way anyone finds it: a table
you have never had to tell apart looks like one table. Each is read inside its own
window now, and `rewrite` is a verdict of its own — reached, no arm owed.

**`jit.go` decides twice about every name**, in its emitter arms and in
`jitRecipeNeedsValueABI`. Both surfaces are read; their disagreement is
`jitsplit`. Today they agree exactly, 18 names each.

**The arm is keyed by a tag, so a name with no tag anywhere cannot be looked up.**
That is not a hole to paper over. It is the shape of the wound.

## The witness: the tree that had it

A detached worktree at `0609d921~1`. On that tree `str_find` is in **no**
name-carrying mirror the body keeps for natives — the manifest row, the flt-ops
row and the op table row had all gone together on 2026-07-01. What still said the
word was three sibling registrations. So the honest verdict is not *an arm
stands*; the lens cannot see the arm without a tag. It is:

```text
str_find -> seedgap  str_find  tag unread arity unread
  [ -manifest -flt-ops -row ?arm ?fkc -rw +go +rust +ts -jit -abi ]  wounds: seedgap
```

**seedgap**: all three siblings hold this native, the seed has no route at all.
*Go look in the seed before minting.* And the panel's other half, the arms nobody
names — tags the walker dispatches on that no row and no manifest row claims:

```text
is tag 30 an orphan arm?  1
```

Cross the two and `if (t == 30)` is found in one reading. That is the whole proof.

On today's tree `str_find` reads `whole` across all eleven mirrors, seedgap fell
**49 → 48** and the orphan arms **41 → 40**. The one name that left is `str_find`.

The lens could not have run on the tree it read. `str_find` is one of the natives
it searches with, so on the pre-restoration kernel the census cannot execute at
all: **the wound disables the tool that finds it.** A body cannot always read a
wound with the body that has it. `mcs-live-at(root)` exists for that reason, not
for convenience, and `/tmp/mirror-census-root` is its door.

## What it says about the body today

362 names, 11 mirrors, 151 disagreeing, 490 ms warm (`mirror.read-ms`, glass row).

```text
armhush 3  seedgap 48  tagclash 3  rowhush 0  fkcgap 0
siblinggap 4  jitsplit 0  siblinglone 59  manifestgap 41
```

**Three armhush, and each is also a tagclash.** `string_bytes` 205,
`string_byte_fold` 206, `form_table_text` 207 stand in the manifest with a walker
arm and a serializer arm and all three siblings — and no `fk_optab` row. Their
declared tags belong to `sense_mic_count`, `sense_cam_count` and `sense_mic_name`.
A `str_find`-style restore driven off the manifest would route each into a
stranger's arm, at the wrong arity. That is why tagclash is ranked immediately
under armhush: a wrong coordinate misleads the repair itself.

**`str_to_int` is `str_find` again, still open.** Tag 31 in `runtime/fkwu-uni.c`
is a complete arm — leading whitespace skipped, sign read, digits accumulated —
with no `fk_optab` row and no manifest row. It is in the orphan-arm list. The name
reads `seedgap`: native in all three siblings, walked as a recipe at
`core.fk:233`. The receipt of 2026-09-08 said `str_find` left `flt-ops` "with
`substring`, `int_to_str` and `str_to_int`"; `substring` came back as a rewrite,
`str_find` as a native, and this one did not come back at all. `int_to_str`
(`core.fk:202`) reads `seedgap` too and **no orphan arm was found for it** — the
lens said `arm ?`, I looked at tags 12, 13, 14, 16 and 44, and there is nothing
there. The discipline held: it said *I have not looked*, not *it is missing*.

**Four siblinggap**, seed natives the four-way proof cannot close over:
`host-exec` (go alone), `http_get` and `jit_compile_value` (rust and ts, not go),
`host_file_append_bytes` (go and ts, not rust).

**41 manifestgap** — `fk_optab` rows with no declared row behind them. The
generated surface has outrun the authority that is supposed to produce it.

Worth healing first, in order: `str_to_int` (a native the body pays a recipe for
on every integer parse), then the three armhush/tagclash manifest rows, then the
four siblinggap. All three families live in `runtime/**` or the kernel siblings,
which are not this hour's to touch.

## The band, and its red

`form/form-stdlib/tests/mirror-census-band.fk` → **65535**, sixteen claims over
hand-written mirror rows. Pure: it reads no file, so it grades the judgement and
never the tree's mood. It tells apart a complete name (b1), an arm with no row
(b2) and a name with no native anywhere (b14); it proves an alias is not a clash
(b5), that a binary arm owes no serializer arm (b8), that `seed_only` reads
seedgap with `arm` **unread and not zero** (b10, b15), and that tag 31 is an arm
nobody names while tag 30 is not (b16).

It goes red. Inverting one condition in `mcs-armhush?` — `eq(mcs-m-row, 0)` to
`eq(mcs-m-row, 1)` — gives **62204**. The missing 3331 is b1+b2+b9+b11+b12: one
falsified rule and five claims fall, because a name wrongly called armhush stops
reading whole and stops reaching its own verdict.

## What the lens refuses to judge

It does not rank a native as *the JIT ought to carry this*. The JIT's surface is a
deliberate subset and **nothing in this tree declares which natives it ought to
carry**, so such a ranking would be the lens's opinion wearing a measurement's
clothes. I wrote that rule first, ran it, got 22 findings including `_plus` — and
arithmetic reaches the JIT structurally, by op code, never by name. The columns
are reported and left to the reader; only the two surfaces disagreeing is a wound.
Naming what a lens cannot judge is part of the lens. `str_find` was missed for ten
weeks by a reading that was sure.

## An unresolved thing, named rather than handed over

A cold-cache first run of any cell preluding `mirror-census.bml` warns once:

```text
mirror-census.bml: unit is not importable standalone (1334 unresolved error(s)
compiled alone) -- image rejected, falling back to the whole-program compile
```

The verdict is unaffected (65535 cold and warm) and the second run is silent. The
cost is the imported-image path on that one run. I did not step around it:

- it reproduces on a **truncated copy of `bearing-census.bml` itself** (1209
  errors), so it is a property of the lane, not of this cell;
- the artifact signature is exact: after a cold dep run the proven cells leave
  only `.lowfk`, while this one leaves `.lowfk` **and** `.fkb` **and** a `.sym`
  recording `compile-errors 1334`. An image is attempted for this unit and not for
  those, and that attempt is what fails;
- the error count scales with the unit's total text, comments included, so nothing
  is being lowered in that attempt — while the `.lowfk` compiles clean as `.fk`
  with **0** unresolved calls;
- ruled out by measurement, each tested on the proven `bearing-census.bml` body:
  banner length and shape, total file size (padded to 40 KB, clean), line length
  (828 chars, clean), an 11-parameter def, a def whose body is a bare string
  literal, `ref`, `thought`, a self-referential path string, and class count.

The mechanism is `fk_src_sym_recorded_errors` and the dep-import path at
`runtime/fkwu-uni.c:19280-19315` — `runtime/**`, which this hour's ownership
forbids. It is located, bounded and reproducible; it is owed a hand that may
touch the seed.

## Wiring lines for the parent

`form/form-stdlib/form-glass-live.bml` carries thirteen sensors through one
assembly site and is not mine this hour. To carry the fourteenth, the mirror
sensor gives under `glass.sensor.mirror` with its cadence inside the frame, so
only the roster and the painted rows are owed:

```text
roster        add "mirror" to the sensor list (the cadence travels in-frame,
              15000 ms, declared by MCSCadenceMs — no cadence row is owed)
prelude       form/form-stdlib/mirror-glass.bml
painted rows  mirror.worst          text   the native the body cannot reach
              mirror.worst-verdict  text   armhush | seedgap | ... | whole
              mirror.armhush        int    natives standing with no route
              mirror.seedgap        int    all three siblings hold it, the seed does not
              mirror.orphan-arms    int    walker arms nobody names
              mirror.disagreeing    int    names whose mirrors disagree
```

Proven both ways already: `observe/mirror-census-run.fk` gives
(`glass.sensor.mirror seq 2`) and `observe/mirror-census-take.fk` reads **18 rows**
back, seq 4, cadence 15000 ms, no wire and no parser between.

## The closing

**The most surprising teaching.** I set out to build a lens that finds an arm with
no row, and on the one tree that had that wound the lens **cannot** find it —
because the arm is keyed by a tag and every mirror carrying the name had gone.
The answer was not to force the reading. It was to name the shape from the other
side (`seedgap`: three siblings, no seed route) and publish the arms-nobody-names
list beside it, so the reader crosses two honest half-readings instead of trusting
one confident whole one. The lens is stronger for the thing it cannot do.

**Where discomfort turned to gold.** The warning above. It appeared on every run
and I wanted to call it cosmetic — the band was green, the answer right. Instead I
spent fourteen probes on it, and it stayed unsolved. What it bought was not a fix:
it was a *falsifiable boundary*. Nine causes eliminated by measurement, the
artifact signature isolated, and a reproduction **on the proven sibling's own
bytes** — which turned "the new cell is malformed" into "the lane refuses an image
for some units", a fact about the body that outlives my cell. The discomfort of
not closing it is the honest state; the gold is that the next hand starts nine
steps in and knows it is not their file.
