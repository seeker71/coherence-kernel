# Witnessed only — and the walker the excuse had been describing

*2026-07-28. `gen-neutral-code.fk` → **536870911**, twenty-nine readings, on
Go / Rust / TypeScript.*

## The directive, applied to the message that preceded it

> "never make assumptions. fact check and only hold core axioms and witnessed
> facts as current ground truth."

Applied first to my own last message, which contained two unwitnessed claims:

**"Rust agrees with Go."** I had inferred it from Rust's *runtime output* in the
repro. Reading `form-kernel-rust/src/main.rs:1896` afterward: it does agree,
with a near-identical comment to Go's. So the claim was **true and the grounds
were not the grounds** — right by luck, which looks exactly like right by
knowledge until someone asks.

**"giles-light-hubs sits at 3041xxxxxx."** Derived from the `lnsi-node` formula
in my head. Witnessed now by running it: `3041001001`, and the Steiner graph's
largest is `3045006008`. Both above 2³¹, so the body-wide claim holds — on
measured values rather than arithmetic I did silently.

Saved as a standing memory row, with the specific rules the session earned:
a divergence between the kernels is evidence of a **bug**, never of a design; a
comment near a line is not evidence about that line; and **a limitation must be
witnessed before it is declared in a band**, because declaring it makes it
structural and stops anyone looking again.

## Then continued

The panel's remaining ranked item: `gnc-total?` asks whether every *declared*
kind has a shape, and cannot ask whether the operators and builtins a program
actually **reaches for** exist. Every kind covered, and a call to a builtin the
language never names — that is the hole a program falls through.

Closed with a generic IR walker that collects required vocabulary from the
program itself: 3 operators and 3 builtins in the real query, all spelled by all
three languages, and a program calling an unnamed builtin is now **refused by
the check** rather than discovered at render time.

## The most surprising teaching

**The walker is the one this cell said could not be written.**

Two turns ago it carried a comment: *"a part is sometimes a child node,
sometimes an operator name, sometimes an integer — a generic walker has to know
which is which."* That sentence was the reason I counted tables instead. It was
also, exactly, the specification: once the signature table existed, the walker
took nine lines and worked first time.

**The excuse and the capability were the same missing row-set.** Not a lesson
about carelessness — the comment was accurate. The failure was in what I did
with an accurate diagnosis: I ended it in *therefore not*, and an accurate
diagnosis ending in *therefore not* is indistinguishable from a design document
nobody read.

## Where discomfort turned to gold

Fact-checking my own previous message and finding a claim that was **true but
unjustified** was worse than finding one that was wrong. A wrong claim gets
corrected and leaves a scar you can navigate by. A lucky one leaves nothing —
the receipt reads clean, the number was right, and the habit that produced it is
completely intact.

That is why the directive matters more than any single correction: it targets
the process rather than the output, and the process is invisible precisely when
it happens to work.

## Frontier question

*What one word names a belief that is true while the reason for holding it does
not hold?* → **gettier**. 0 hits before offering. The body carries `defeasible`
in 7 files for a claim open to revision; it had no word for a claim that is true
while its justification is broken — which is also what the band scoring 253 was
doing when it passed one TypeScript question because truncation happened to
leave 0 where 0 was expected. Corpus row 915.

## Files

| file | state |
|---|---|
| `cognition/gen-neutral-code.fk` | generic IR walker; required-vocabulary coverage checks |
| `cognition/tests/gen-neutral-code-band.fk` | 29 readings → 536870911 on Go / Rust / TS |
| `learn/homecoming-distillation-corpus.fk` | +row 915 (gettier) |
| memory | +`feedback-witnessed-facts-only` (standing) |
