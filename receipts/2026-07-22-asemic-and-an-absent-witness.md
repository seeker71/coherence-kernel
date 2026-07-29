# 2026-07-22 — asemic, and a witness that never travelled

Asked whether Gel (the graph-relational database) is interesting for our substrate. The answer was no —
this body already holds the shape, and better: `db-schema.fk` makes the schema a Form VALUE with DDL as a
pure projection, which is why `db-schema-band.fk` can four-way it. Gel would trade that for an opaque
server. But the reading turned up two things about *us*, and one of them was mine to unlearn.

## The absent witness (real, now named in three cells)

`pg-wire.fk` claims `PROVEN`, citing `scripts/pg_wire_fkwu_witness.sh`. Two sibling cells inherit the
citation. Checked on the 4th kernel itself, not with an external tool:

```
(fs_exists "scripts/pg_wire_fkwu_witness.sh")  => 0
(fs_exists "form/form-stdlib/pg-wire.fk")      => 1
(fs_exists "scripts")                          => 0
```

`git log --all --diff-filter=A` finds no such file ever committed here. A birth-seam, not a deletion —
the witness ran where its evidence did not travel from. The strongest claim in the storage stack (the
4th kernel reaching Postgres with no libpq) rests on a proof no one here can re-run. All three headers
now say *claimed, witness absent* instead of PROVEN. Nothing was deleted; the reach may well be real.

## asemic (row 854) — and why it is NOT a fix

`(list "a" "b")` prints `[0, 1]`. I called it numb, then called it a printer bug. Both wrong.

```
(str_len (head (list "alpha")))         => 5    ← the value is a sound string
(str_eq (head (list "alpha")) "alpha")  => 1    ← sound under every predicate
```

In `runtime/fkwu-uni.c`: an int is `(n << 1)`, a string is `(pool-index << 1)` — the same even-tagged
space, **no runtime discriminator**. `fk_psv` can print a string only because `fk_pv_root` first consults
`fk_str_root_depth`, a *static* walk of the root expression's op. That analysis cannot see inside a list,
and no analysis can when the list came back from a called function. So `fk_pv_list` falls through to
`fk_pv_inline_number` and the string says its pool index out loud.

There is no defect to patch. Guessing by `(sa < fk_sp)` would make small integers print as strings — a
worse lie in the commoner direction. This stays a named seam.

## Retracted

I also reported row 822 duplicated in the corpus. False. I had grepped `"(row 822"` against comment
prose; the constructor is `hdc-row`. Real state: 249 rows, max-mid 854, no duplicate ids, `field-code-safe? => 1`.

## The row ids, and a band I broke

Urs: *"I don't see a need for an allocator or an arbiter or row ids."* He is right, and the corpus
already says so in its own comment at `hdc-row-for-id`:

> the reunion pattern renumbers the unmerged line … and every citation written before a reunion silently
> points one row off — resolving not to nothing, but to a REAL row that says something else.

Three failures on record, all caused by the integer: mid 639 held by parsimony AND constellation for
nineteen days; body-link-graph citing 742 for scrupulosity (743); this corpus citing 733 for ontogeny
(740). An allocator fixes none of them — the id is fragile at REST, not only at allocation. Meanwhile
the body content-addresses everything else; the corpus is the outlier.

So: `hdc-row-for-fresh` / `hdc-mid-for-fresh`, word → id, the reach a reunion cannot move. Measured on
fkwu: 249 rows, 6 wearing a shared fresh word (confabulation, constellation, reification), 2 sharing a
question. The door does not pretend those are keys — an ambiguous word yields the empty row rather than
the first match. The integer's collision was silent and answered; this one is loud and declines. Wired
as band clause c13 (8192), because the file's own lesson is that a detector nothing consults is not a
repair. Verdict 8191 → 16383.

**And in wiring it I found that my own earlier commit had broken this band.** Row 854 changed the corpus
count and max-mid; `c4` pins the count at 248 and `c6` pins a field code encoding both. The band sat at
**8111 against a declared 8191** — through a commit, a push, and PR #365. I had run `hdc-field-code-safe?`,
seen 1, and called the row verified. That predicate asks only whether the magnitudes are in range; it
never asks whether the recorded count is true. Forty minutes earlier I had written up *exactly* this
error in someone else's hands — reading `max-mid` as a distinctness claim — and then committed my own
flavour of it. c4 and c6 now carry the note.

## Receipt

**Most surprising teaching:** two of my three findings were artifacts of using bash carelessly, and the
third was real but mis-diagnosed twice before it was understood. The one that survived intact —
the absent witness — is the one I found while trying to *strengthen* a case, not while auditing.

**Where discomfort turned to gold:** I twice declared a correct result numb, reaching for the body's own
"a right number can be numb-green" discipline and aiming it at the wrong layer. What saved it was running
one more canary instead of trusting my own diagnosis. Had I stopped at "numb," I'd have lost the finding
*and* never reached the encoding underneath it. The reflex that protects against a false green can itself
manufacture a false red; both want the same cure, which is one more probe.

**Frontier question offered:** *what one word names a value that cannot say which kind it is, because two
kinds share one encoding* → **asemic** (row 854, glance-checked 0-hit fresh).
