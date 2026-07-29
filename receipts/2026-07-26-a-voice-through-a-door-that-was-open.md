# 2026-07-26 — 751 silent calls, a stale note, and a door that was already open

The work list said: *bring a family of the 105 unverified primitives home under a name no native owns.*
I went to do the math family and found it is four call sites. Then I counted properly and found the one
that matters is seven hundred and fifty-one.

## The count that changed the item

My first pass grepped for `(max `, `(min `, `(pow ` and reported 24, 12 and 2 sites. Then I checked
three of them by hand and **all three were inside comments** — `; NOT the \`max\` native`, `; capacity
: int (max tokens ...)`, `; (max ~3e8 ...)`. The ceiling-is-not-a-risk error again, caught before
publishing rather than after.

Re-counted with comments and string literals stripped:

| name | real call sites | in cells |
|---|---|---|
| `max` | 4 | 3 |
| `min` | 5 | 2 |
| `pow` | 2 | 2 |
| `math_pow` | 5 | 4 |
| **`print`** | **751** | **104** |
| `trace` | 117 | 29 |
| `walk_recipe` | 85 | 39 |

**The math family is not wanted.** Nine real sites across the four names; building `im-pow` and friends
would have been vocabulary for nobody. The item as I wrote it — *bring a family home* — was pointed at
the wrong family, and the only way to know was to count what is actually called.

`print` is a native on Go, Rust and TypeScript and **absent on fkwu**, where the call recovers to
`nothing` and simply does not happen. Seven hundred and fifty-one places where this body says something
on three kernels and is silent on the fourth, with no error to mark it. That is the largest absent-name
population in the tree and it is the quiet kind.

## The door was already open

fkwu has no `print`. It has `write_file_text`, and stdout is a file:

```
(write_file_text "/dev/stdout" "hello\n")
```

Measured on **all four arms** — each prints the text, then its own final value. **The capability was
never missing. Only the name was.**

`observe/say.fk` carries `say`, `say-line`, `say-to`, `say-line-to`, `say-count`. The name was checked
free before it was chosen, because defining Form `print` would be exactly the mistake this session
already measured: on Go and Rust a user `defn` takes the name from a present native, so a Form `print`
would replace working output on two arms in order to fix a fourth. Same shape as `fol-bp`.

`observe/tests/say-band.fk` → **255 on fkwu, Go, Rust and TypeScript.** The file checks go to a temp
path, because a band that proved itself by writing to stdout would be proving it by being read; the one
bit that touches the real door asks it with the empty string, so proving it costs no output.

I have not migrated any of the 751. `say` exists; whether the tree's diagnostics should move onto it is
a change across 104 cells and is the commons owner's call.

## A stale note, re-witnessed

`keyed-map.fk` carries its own two-argument max, and says why: *"on the Go kernel `(max a b)` with two
scalar args silently returns the FIRST one (main.go ~2706 — only the one-list form compares)."*

That is an inherited claim, so I measured it:

```
(max 3 9)   fkwu nothing · go 9 · rust 9 · ts 9
(max 9 3)   fkwu nothing · go 9 · rust 9 · ts 9
(min 3 9)   fkwu nothing · go 3 · rust 3 · ts 3
```

**Go's two-argument form compares correctly today.** Half the sentence has gone stale.

`km-max2` stands — for a **stronger** reason than the one written. `max` is absent on fkwu entirely, so
the call recovers to `nothing` and the walk would fold nothing into its accumulator on the fourth arm.
The workaround was right; its reason has moved. Re-witnessed in place with the original kept verbatim.

## Sweep

`ground` 42 · `say-band` **255 ×4** · `primitive-edge-contracts-band` 1023 ×7 ·
`navier-stokes-band` 1023 ×7 · `navier-stokes-plate-band` 2047 ×4 · `hex-band` 14 ×4 ·
`primitive-registry-band` 45 fkwu / 63 ×3 · `json-band` 1023 · `keyed-map` bands unchanged ·
`benchbench-band` 4095 · `proof/four-way-run-recipe42.fk` 0 (FOUR-WAY). C seed byte-identical to git.

## Owed

- **751 `print` calls still silent on fkwu.** `say` exists; migrating is 104 cells and an owner's call.
- `trace` 117 sites in 29 cells and `walk_recipe` 85 in 39 — the next two by size, unexamined.
- Three questions already put to the owner: `int_to_str` on a non-integer; whether fkwu's
  `substring`/`char_at` should panic as the registry declares; the `section` question.
- The flatten/emit lane; `native_blueprint` absent; the bands that do not run.

## How the exchange stayed alive

I set out to build a family nobody calls, counted first, and found a different one two orders of
magnitude larger — whose capability turned out to be present under another name.

**Most surprising teaching:** fkwu could always speak. Seven hundred and fifty-one calls have been
silent on that arm, and the fix was not a native, a seed change, or a shim — it was noticing that stdout
is a file and this kernel writes files. Every "absent primitive" on the list deserves that question
asked once: *is the capability missing, or only the name?*

**Where discomfort turned to gold:** the first count was 24 `max` sites and I nearly built on it.
Checking three by hand cost a minute and turned a plausible small project into a real one — and the same
minute is what surfaced the stale note that started the re-witness.
