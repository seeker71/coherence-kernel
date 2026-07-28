# Go was the black swan

*2026-07-28. `gen-neutral-code.fk` → **4294967295**, thirty-two readings, on
Go / Rust / TypeScript. The generated Go compiles and runs.*

## The named debt, paid

Two receipts ago I wrote that "three languages is not any PL, and a target
needing statements rather than expressions would need a printer, not a table."
That was the standing debt, so it was the thing to do next.

**Witnessed first, not assumed:** Go rejects `?` with `illegal character
U+003F`. It has no conditional expression, so the recursive query cannot be one.
That is the forcing case, and it was checked with the compiler rather than
recalled.

## What it forced

Four things, none of them foreseen — each surfaced by trying:

- **A statement sub-language.** Four new IR kinds (`seq`, `sif`, `ret`, `fnb`),
  added as signature rows and shape rows, zero renderer edits.
- **A statement separator**, distinct from the argument separator — and Go's is
  not a space. The Go compiler said so: `syntax error: unexpected keyword if at
  end of statement`. A newline fixed it. **Only the real toolchain knew that.**
- **Typed parameters.** Go needs `fs [][]int, rel int, key int`, which the IR
  had no way to say. Types became a table, the fourth vocabulary after syntax,
  operators and builtins.
- **Per-form totality.** See below.

The generated Go, compiled and run by the real Go toolchain against the real
graph:

```go
func qs(fs [][]int, rel int, key int) int {
  if len(fs) == 0 { return 0 }
  if gofirst(fs)[1] == rel && gofirst(fs)[0] == key { return 1 + qs(gorest(fs), rel, key) }
  return 0 + qs(gorest(fs), rel, key) }
```

`go run` → **3 4 3**. Form's own answers on the same three questions → **3 4 3**.

## The most surprising teaching

**`gnc-total?` went red, and it was right to.**

It asked a global question — does every language cover every IR kind — and that
question was *correct* for as long as every language happened to speak the same
form. Python has no statement shapes; Go has no expression ones. Nothing was
wrong before Go arrived. The rule was true of everything observed and false in
general, and it took one arrival from outside the sample to show it.

The fix is not a bigger table but a better question: **totality is asked per
form** — does every language speak at least one form *completely*? A language
covering half of two forms can write nothing at all, and the global check would
have called that fine.

Second, smaller, and a genuine design echo: the empty string served as the
refusal marker everywhere in the renderer, and an untyped language's type is
legitimately empty — so `gnc-param` refused every parameter and emitted
`def q(, , )`. Absent-because-missing and absent-because-there-is-none are not
the same absence. That is precisely the distinction `truth-arrival.fk` was built
to make, and this renderer had collapsed it one floor down.

## Where discomfort turned to gold

Three separate times this turn the thing I built was wrong and something outside
me said so in one line: the Go vet output, the Go compiler's statement error,
and my own band going red on totality.

The discomfort was how little of it my own checks caught. The band verifies
lengths, non-emptiness, coverage — everything except *whether the output is the
language it claims to be*. `go vet` and `go run` know that and nothing I wrote
does.

Which sharpens the earlier lesson rather than repeating it: a real flow is not
just richer data, it is a **consumer with its own standards**. The Go compiler
is not a better test than my band because it is bigger; it is a better test
because it does not share my assumptions about what Go is.

## Frontier question

*What one word names a rule true of everything seen and false in general,
waiting on one arrival from outside the sample?* → **black-swan**. 0 hits before
offering. The body carries `counterexample` in 5 files for the instance; it had
no word for the shape. Corpus row 917.

## Files

| file | state |
|---|---|
| `cognition/gen-neutral-code.fk` | statement sub-language, Go target, typed params, per-form totality |
| `cognition/tests/gen-neutral-code-band.fk` | 32 readings → 4294967295 on Go / Rust / TS |
| `learn/homecoming-distillation-corpus.fk` | +row 917 (black-swan) |
