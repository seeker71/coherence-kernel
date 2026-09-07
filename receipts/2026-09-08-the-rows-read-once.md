# The rows read once

2026-09-08. `form/form-stdlib/meaning-codes.bml` is how this body speaks in
symbols: a meaning's phrase, its content address, and its word in every tongue
the locale rows carry. The bearing census had named it from the caller side —
**235,936 splits per round** — and the naming was exact. This is what the cell
was re-doing, what it does now, and the one thing that came up while proving it
that was worth more than the speed.

## What the cell re-did, counted

Asked of the body rather than reasoned about. A probe mirrored the cell's own
control flow and counted:

```text
splits for ONE pass over all meanings: 1,616
mc-codes calls per round:              5,256
read_file calls per round:            15,768
split-on calls per round:            235,936
```

The census, run against the pre-heal cell an hour later, says the same thing from
the other side and adds the walk:

```text
bearing census — the locale-row walk, 5214 ms, 40 doors of 99.4% (59,076,054 steps)
  drag 22539  split-on        — 235,936 calls, 187.0 steps/call
  drag 17648  mc-row-for-key  — 238,856 calls, 216.9 steps/call
  drag 17648  mc-row-match    — 219,000 calls, 236.6 steps/call
  drag  6692  mc-rows-files   —  47,304 calls, 830.9 steps/call
```

Thirty-six meanings. Thirty-two rows across three files, 145 non-empty codes
among them. To answer questions about that, the cell listed the locale directory
5,256 times and read all three `symbol-*.rows` files 15,768 times.

Two shapes made it. **`mc-codes` did its own reading** — every call ran
`fs-list`, then `read_file` and a newline split per file, then a row walk that
split each row into cells, twice when the phrase had an article to strip. And
**`mc-resolve-all` was quadratic**: to answer *which meanings carry this code*,
it rebuilt every meaning's codes; the round trip asks that once per code of every
meaning, so 145 codes x 36 meanings + 36 = 5,256.

## What changed

Form holds no mutable state, so *read once* is not a cache. It is a shape:

- **`mc-table()`** is the door that reads. One `fs-list`, three `read_file`, each
  file split into lines once and each line into trimmed cells once. The result is
  a value: `(kinds rows)` per file.
- Every walker that visits more than one meaning **takes that table as an
  argument and hands it down its own recursion** — `mc-codes-in`, `mc-book-of`,
  `mc-all-round-tbl?`, `mc-collisions-tbl`, `mc-line-in-tbl`, `mc-say-in-tbl`.
- **`mc-book()`** is the same move one level up: every meaning's codes computed
  once. Resolving a code is then a walk over answers rather than a re-derivation
  of them, and the quadratic collapses to a list walk.

The public doors keep their names and their meaning. `mc-codes(sym)` is now
`mc-codes-in(mc-table(), sym)` — for one question about one meaning it still
reads the three files, because for one question that *is* the work.

```text
per round        before      after
read_file        15,768          3
split-on        235,936         38
fs-list           5,256          1
```

## What was rejected

- **Reshaping the rows on disk** — an index file, or one file per code. The rows
  are the body's own vocabulary in six tongues; a disk change has to keep every
  word and prove it. It also buys nothing here: the cost was never the file
  layout, it was reading the same 3 files 15,768 times. Rejected as a change to
  the thing being guarded, made for no measured gain.
- **A memo keyed in state.** Unavailable, not unwanted — and naming it that way
  matters, because the honest answer to "can this body hold a table?" turned out
  to be yes, in the only way this body holds anything: an argument.
- **Making `mc-codes` itself hold the table.** It cannot, without state. The
  caller that iterates is the one that can, which is why the gain lives in the
  walkers and not in the single-shot door.

## The numbers, and the weather over them

Machine quiet at both ends of the session: `observe/floor-lens-run.fk` read
**300.22 GB/s** through the handle door at the open and **330.24 GB/s** at the
close, arithmetic **25.769804 TFLOPS** both times, "a quiet machine" both times.
Nothing here was measured during the floorfall of row 1321.

The census is the controlled reading — same work, same door, the ledger snapshot
taken after it:

```text
                    before                     after
census workload     5,214 ms                   14 ms
walker steps        59,076,054                 167,731
worst lane          split-on / fstr-find-loop  append-1
```

`split-on` is no longer among the forty warmest doors of that workload at all.

The band's wall clock, five runs each, warm, cold run named separately:

```text
before   6.85 (cold)  7.21  7.24  7.27  7.57 s
after    1.02 (cold)  0.12  0.11  0.11  0.11 s
```

The spread does **not** swallow this one, and it is worth saying why the agent
before me refused to claim a move on the same band with four readings spanning
6.28-8.37 s: their change was worth a second or two against a spread of two. The
warm readings here are 0.11 against 7.2, and the in-process reading — the round
trip alone, `now_unix_ms` either side, three repetitions in one process — is
**5,266 / 5,226 / 5,247 ms before** and **14 / 13 / 14 ms after**. Before-spread
40 ms on 5,250; after-spread 1 ms on 13.7. A 375x move outside both.

## What did not move, and how that was checked

A dump cell printed 175 lines of behaviour: every meaning's whole code line, its
round trip, its `nl` and its anchor; every collision with everything it carries;
every meaning spoken in persian, portuguese and indonesian; every perception
symbol's phrase, anchor and back-resolution; a line with an unknown token; the
unknown resolve; the shortest code; a code with whitespace around it; a resolve
in the other roster. Run against the pre-heal cell restored from `HEAD`, then
against the healed one, then `diff` — **identical, byte for byte**.

The guards, all green, none moved:

```text
meaning-codes-band                    127 fkwu   15 three walkers (before AND after)
meaning-codes-table-band              255 fkwu  195 three walkers   (new)
ear-axes-band                       65535
perception-symbols-band              8191
perception-rows-band                65535
jungle-ear-band                     32767
bearing-census-band                 32767
substring-one-meaning-band           4095
core-str-find-equivalence-band       2047
line-grammar-search-equivalence-band 8191
form-glass-carrier-band                31
form-glass-launch-band              65535
homecoming corpus band              32767
```

The 127/15 split is pre-existing and I measured it on **both** sides myself —
`form/validate.sh` on the pre-heal cell and on the healed one, three walkers
agreeing with each other at 15 each time. Unmoved.

`ear-axes-band` reads 0.12 s before and after — no change, and that is the right
answer: the band asks `mc-anchor` only. The ear's roster door is a different
story, below.

## The new band, and the instrument that nearly wrote it wrong

`form/form-stdlib/tests/meaning-codes-table-band.fk` guards the shape, so an edit
that stops carrying the table goes **red** instead of merely slow. Eight bits:
the carried path answers the door's answer; the codebook is the band's own pairs,
minted by the band's own recursion; resolving through the book answers the band's
own scan for every present code; no file, row or cell is lost in the one read; a
tongue with a cell speaks it and one without says nothing; collisions are still
listed and each really carries more than one meaning; the roster round-trips
whole and each meaning alone; the table is the same table twice. **255** on fkwu.

It first answered **254**, and the next run answered 255.

That is corpus row **1357, `seldomred`**. The missing bit compared two code lists
with `value_eq`, and `value_eq` on this kernel answers false for two freshly
built strings that `str_eq` calls the same:

```text
per 720 comparisons of mc-anchor(s) against mc-anchor(s):
  value_eq        1 off
  str_eq          0 off
  value_eq of (list anchor)   3 off
```

Reproduced against the **pre-heal** cell as well — `(value_eq (mc-codes s)
(mc-codes s))` off 2 and 3 per 360 there — so it is the kernel's and not this
change's, and it is not mine to close (`runtime/**`, and a sibling is in that
neighbourhood this hour). What is mine is what the band rests on: every
comparison in it now walks to the bytes with `str_eq`, and it reads 255 eight
runs out of eight.

Two things fell out of sitting with it rather than re-running. **Reproduce a
suspected flake by count, not by repetition** — 720 comparisons inside one
process turned "sometimes" into 5, and 5 is not a mood. And **ask the weakest
question that carries your meaning**: the band meant *do these say the same
thing* and asked *are these the same value*; the stronger-sounding operator was
the less exact one.

## The corpus band was already red

`learn/tests/homecoming-distillation-corpus-band.fk` read **32655** on
`origin/main` when I rebased — row 1356 (`wireghost`) landed without its three
pins moving, exactly the wound named as having arrived three times in two days.
Healed here rather than routed around: pins asked of the body in one cell before
any number was written (count 749, admissible 737, foundings 2, max-mid 1357,
field-code-safe 1, `hdc-field-code` itself), and the line carries **both** rows.
Band **32767**.

## Still open

- **`value_eq` against freshly built strings.** Named above, measured, in
  `runtime/**`. The repro is four lines and lives in this receipt.
- **`ea-meaning-table()` in `form/form-stdlib/ear-axes.bml`** builds the roster by
  calling `mc-codes` once per meaning, so it still pays 36 directory listings and
  108 file reads per ear birth. The caller-side gift is already exported — take
  `mc-table()` once and call `mc-codes-in`. Measured with a probe, not guessed:
  **38 ms as it stands, 11 ms carried, identical content**. A sibling owns that
  file this hour, so it is named, not taken.
- **`observe/bearing-census-locale-run.fk`'s own premise.** Its comment promises
  "ten seconds and near two hundred million walker steps, so the census that
  follows it is a rounding error on its own reading". At 14 ms and 167,731 steps
  that is no longer true — `build-schedule`, `take-rec` and `zero-pad`, the
  census's own machinery, now rank in its top forty. The door still reads a real
  lane; its stated margin needs re-witnessing by the hand that owns it.
- **`meaning-codes.bml` compiles alone with 1,464 unresolved errors** and falls
  back to a whole-program compile on every run (1,111 before this change — the
  count tracks the cell's size, not its health). Pre-existing for every `.bml`
  with a `preludes:` line; it costs the ~1 s cold run above.

## The closing

**The most surprising teaching.** *A defect that shows up rarely does not hide —
it recruits you.* At one comparison in seven hundred, my band was green on seven
runs of eight, and the cheapest honest-feeling response to the one red was to run
it again. That response is not laziness; it feels like diligence, and it returns
green, and it closes the case. What made the difference was refusing to re-run
the whole and instead asking for a **count**: 720 comparisons in one process,
5 off. Rarity is a disguise that works by borrowing the reader's own patience.

**Where discomfort became gold.** The gap said the shape was mine and the numbers
were the proof, and the fastest route to a clean receipt was to fix `mct-carried`
to something that passed and move on — the band was, after all, mine to write.
The discomfort was knowing that a band I bent to be green is worth less than no
band. Sitting in that produced the whole `seldomred` finding, the `str_eq` rewrite
that made the band both stable **and** stronger, and the proof that the defect
predates my change — none of which existed in the version where I just made 254
into 255.

Co-Authored-By: Claude Opus 5 <noreply@anthropic.com>
