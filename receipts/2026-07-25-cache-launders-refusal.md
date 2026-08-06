# 2026-07-25 — the cache was overturning the kernel's own refusal, and one band in five was living on it

I stopped, and the reason was bad: a PR check-in said "re-arm silently if nothing changed", and I let a
monitoring cadence stand in for the work. The watch being quiet does not empty the list. Three things
were on it, observed and written down. This is the biggest.

## The thing I had recorded and not chased

Yesterday's note: *"loader-dependent bands print a value only on the SECOND run — the first cold compile
carries errors and prints nothing. The numb-green shape again, wider than one cell. Observed, not chased."*

Chased. Measured on `bmf-choice-receipt-band.fk`, stdout and stderr separated so the finding could not be
an artifact of my own shell:

| | exit | stdout | stderr |
|---|---|---|---|
| run 1, cold | 1 | **0 bytes** | 1891 lines, ending `1888 error(s)` |
| run 2, cached | 1 | **`56360959`** | 1 line, `cached image was compiled with errors` |

Same file. Same errors. A number that exists only on the second run, and 1888 errors collapsed to one
warning line beside it.

## Why — and it is not a subtlety, it is a door left open

`runtime/fkwu-uni.c:8255`. An `[unbound-name]` in value position sets `fk_src_unrunnable = 1`. The
fresh-compile door then reaches

```c
if (fk_src_truncated || fk_src_unrunnable) {
    return 1;
}
```

and returns **without printing the root value** — the kernel declining to compute an answer over a
program whose names have silently become 0. That refusal is deliberate and correct.

But `fk_src_compile_current_unit` has already written the `.fkb` and its `.sym` lens by then. So the
next run takes the cached-image path, which checks the recorded error *count*, warns, and **runs the
image anyway**.

The `.sym` lens carries `compile-errors`, and the comment above its reader shows the right instinct
already: *"an image without its error record is an incomplete cache, not a clean one — otherwise
deleting the lens would launder a degraded image back to exit 0."* They guarded the **exit code**.
Nobody carried the **refusal**. So the cache did not merely replay a degraded image — it overturned a
decision the kernel had already made, silently, on the second run of an unchanged file.

The error count could not have stood in for it: an unresolved *call* recovers to `nothing` and does run,
so `errors > 0 and runnable` is an ordinary, honest state. The latch is a different fact and needed its
own line.

## The repair

`unrunnable` is now recorded in the `.sym` lens beside `compile-errors`, and the cached path refuses
identically to the fresh one — same door, same answer, whichever side of the cache the caller is standing
on. A lens without the field reads as `-1` and is treated exactly as a missing error record already is:
an incomplete cache to rebuild, never a clean one. Guessing "probably runnable" there would restore the
laundering the field exists to stop.

Witnessed, three runs of the same file, caches cleared before the first:

```
run1 exit=1 stdout=[]  1888 error(s)
run2 exit=1 stdout=[]  cached image was REFUSED at compile ... refusing to run it from cache
run3 exit=1 stdout=[]  (same)
```

`56360959` no longer appears on any run.

## How much this was hiding

Surveyed **120 bands** in `form/form-stdlib/tests/` (the first 120 by name — an alphabetical prefix, not
a random sample, so it is a count and not an estimate for the tree's 1239):

| | |
|---|---|
| **REFUSED at compile** (`unrunnable 1`) | **25** |
| runnable, 0 errors | 73 |
| runnable, some errors (unresolved calls, axiom-5, honest) | 21 |
| no lens written | 1 |

**One band in five was printing a "green" number the kernel had refused to compute.** The recorded error
counts behind those 25 run from 1 to 2684: `bmf-generic-language-scanner-band` 2684,
`binary-symbol-lens-band` 2110, `bml-action-runtime-band` 1900, `bmf-choice-receipt-band` 1888, down
through `adler32-band` 88 — a core codec — and `boot-band` 9, to `biography-band` 1.

## And the cause is one bug, which this branch has now met three times

The unbound names are `BIOGRAPHY`, `PARETO-CANDIDATE`, `AUTO-EXPERIMENT`, … — ALL-CAPS constants in
value position. Every one is a **top-level `let`**, invisible inside a `defn` body on the `--src` lane.

That is the same defect as `HEX-DECODE-ERROR` this morning and `FOL-BP-BOOTSTRAP-TABLE` this afternoon.
Three encounters, three different rooms, one bug — and `MANIFEST.md` has stated the rule the whole time
(*"top-level `let` is invisible inside `defn` bodies — use zero-arg `defn`s for constants"*). What kept
it invisible at scale is exactly what this receipt repairs: the cache handed back a number, so nobody
had to look.

Counted: **36 cells** carry a column-0 ALL-CAPS top-level `let`, **304 such bindings**, and that grep
sees only column 0 — the `    (let BIOGRAPHY …)` form nested one level inside a `(do` is not in the count.

## Three cells closed, as proof the idiom does it

| cell / band | before | after |
|---|---|---|
| `tests/biography-band.fk` (`BIOGRAPHY`) | refused; silent on cold run | **5** on a cold run — and its header says *"Band verdict locks at 5"* |
| `pareto.fk` (`PARETO-CANDIDATE`) | refused | `auto-fitness-band` **computes 1900** cold |
| `autoresearch-loop.fk` (`AUTO-EXPERIMENT`) | refused | `autoresearch-loop-band` **computes 530** cold |

`biography-band` is the clean witness: a declared verdict, met on first compile for what is likely the
first time. The other two headers state no expected number, so the honest claim is narrower — they now
*compute* where they were refused. Whether 1900 and 530 are right is a question those headers do not
answer, and I am not going to answer it for them.

`bp` was deliberately **kept** in the two `bp`-based cells rather than swapped to this afternoon's
`fol-bp`. Neither `PARETO-CANDIDATE` nor `AUTO-EXPERIMENT` is in the reviewed bootstrap registry, so
`fol-bp` would rightly refuse them. Repairing the scope is mine to do; admitting a name to the registry
is not, and a rename would have answered that question silently.

## Regression sweep, cold, after all of it

`ground` 42 · `ground-recursive 10` 55 · `binary-freshness` 15 · `native-vs-rented` 11111 ·
`hex-band` **14** · `biography-band` **5** · `form-cli-band` 524287 · `form-cli-ask-band` 262143 ·
`form-cli-membrane-band` 1023 · `form-cli-surface-inquiry-band` 65535 · `membrane-lane-band` 31 ·
`membrane-lane-live-band` 31 · `ask-lane-floor-band` 31 · `dsv4-decode-loop-band` 1023 ·
`benchbench-band` 4095 · `frontier-ingest-benchbenchbench-band` 127 · `pdf-text-windowed-band` 15 ·
`homecoming-distillation-corpus-band` 32767 · `file-byte-window-band` 2147483647.
form-cli still live from the seed: `ping` → `pong`, `version` → `form-cli 0.5`.

Not one moved. Every band that was green stayed green — because they were green on the *cold* run
already. The repair costs an honest band nothing, which is how you can tell what it takes away.

## The C seed grew — shrink receipt

+~60 lines: one reader mirroring the existing `compile-errors` reader, one extra `.sym` header field, one
refusal branch. No runtime meaning: the refusal already existed and is unchanged, the latch already
existed and is unchanged. What was missing was that the decision did not travel with the artifact.
**Shrink condition:** this leaves when the image format is owned by Form (`program-image-fkb-*` cells)
rather than by C string-scanning of a header — at that point the lens fields are Form data and both
readers delete.

## Owed, observed, still open

- **22 of the 25 surveyed bands remain refused.** Each is a real diagnosis, not a sweep — the counts run
  to 2684 and some of those names will be genuinely absent rather than merely out of scope.
- **1119 bands in that directory were not surveyed**, plus every other `tests/` directory. The 25/120 is
  a count of what was looked at, and nothing more.
- **304 column-0 ALL-CAPS top-level `let`s across 36 cells**, plus the nested form the grep cannot see.
  The idiom that fixes them is proven four times now; the work is per-cell witnessing, not cleverness.
- The three natives fkwu lacks (`native_blueprint`, `seeded_bytes`, `walk_recipe_here`) are untouched.

## How the exchange stayed alive

I answered "and you stopped why?" by looking at my own list instead of defending the stop. The list had
the answer on it, in my handwriting, from an hour earlier.

**Most surprising teaching:** the kernel had been right all along. It refused these programs, deliberately,
with a latch and a comment explaining itself — and then its own cache handed back the answer it had just
declined to give. The bug was not a missing check. It was a correct check whose verdict did not travel
as far as the artifact it produced.

**Where discomfort turned to gold:** measuring before believing. My first instinct was that the cold run
was "just noisy" and the second run was the real one — the folklore the whole tree had absorbed. Splitting
stdout from stderr took one command and turned a shrug into a door left open, 25 bands wide.
