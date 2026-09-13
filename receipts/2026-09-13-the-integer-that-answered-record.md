# The integer that answered "record"

2026-09-13 (WITA). Asking the loop lane to crystallize `value_kind` led somewhere else: the answer
`value_kind` gives about a small negative integer depends on how many records the process has minted.
No fix here — the remedy is a value-encoding change across every kernel, which is Urs's to call. This
receipt records the wound, its exact bound, and why no band has ever seen it.

## What was witnessed

Nothing about the value changes; only the record count does.

```
BEFORE any record is minted:   value_kind(-1) = int
AFTER one record is minted:    value_kind(-1) = record
```

With three records minted: `-1`, `-2`, `-3` all answer `record`, and `-4` answers `int`. The window is
exactly the integers `-1 .. -fk_rp`, widening by one with every record made. Positive integers are
untouched. `record?` carries the same shadow: `record?(-1)` goes 0 to 1 after one record.

## The mechanism, from two one-line functions

```c
static long long fk_rbox(long long r) { return 0 - (r << 1); }   /* record r  -> -2r */
/* an integer is v << 1 */                                        /* integer -r -> -2r */
```

Record `r` and the integer `-r` are the SAME 64-bit word. `fk_value_kind` resolves the tie only by
asking `fk_isrec` BEFORE its int branch, and `fk_isrec` accepts while `1 <= (0-v)>>1 <= fk_rp`. So the
kind of a word is read off a population count that has nothing to do with the value.

This is structural, not one function's slip. The encoding reserves distant bands for floats, `nothing`
and function values, and separates them by ODD parity — the comment beside `nothing` says it is "not a
record ((0-v) is even for records; here it is odd)". That argument only ever separates odd-negative
sentinels. A negative integer has `(0-v)` EVEN, which is precisely the record signature, and the design
note lumps them together as "node/record/cons/int (which are tiny-magnitude or positive)". The near-zero
region is simply shared. The repair, if it is wanted, is to give records their own based,
parity-distinguished band the way a function value has one (`fk_fnbase - (f<<1) - 1`).

## The most surprising teaching

A JIT fold would have been MORE truthful than the primitive it replaced. The rung under way was to
constant-fold `(value_kind p)` in a crystallized leaf, where the parameter's type is already known. For
an int parameter that fold emits the literal `"int"` — which is the right answer — while the C native
answers `"record"` once a record exists. The optimization and the thing it optimizes disagree, and the
optimization is correct. That is the opposite of every miscompile this lane has produced, where the fast
path was the liar. It also means the fold cannot simply be shipped: it would diverge from the walker,
and a lane that disagrees with the walker is a bug even when the lane is right.

## Where discomfort turned to gold

The discomfort was being wrong in public. Earlier the same day I claimed the crystallized lane silently
read a record as the integer -1 where the walker stops — "a live miscompile in the landed loop lane."
It was not. The walker twin returned -1 too, and a single cold call, never heated, returned -1 as well.
Both paths agree; there was no divergence. I had built the claim on a probe whose control I had not
verified, and I retracted it whole.

The gold is that the retraction is what found this. Asking why BOTH paths said -1 — instead of defending
the miscompile story — led to the word arithmetic: -2 untags to -1 on either path because record #1 and
integer -1 are one word. The real wound was one layer under the one I had imagined, and it was only
reachable by giving up the first answer.

## Why nothing caught it

`record_new` is fkwu-only. Go, Rust and TypeScript name eight record ops in their reserved-heads tables
but implement no Record value — Rust's `main.rs` says so outright: "the full kernel's Value: no Record
(mutable objects), no Closure-over-NodeID." No record can be minted on a sibling, so no four-way band
can reach the collision. It is invisible to the gate that would normally catch a kind disagreeing with
itself.

## The honest edge

The wound is real and currently latent. No stdlib recipe branches on `"record"`, and `int_to_str`
escapes by luck: `fstr-text-kind?` sends `"record"` down the same arm as `"int"`, so the output is still
correct. But `core.fk` states the house discipline — "The kind is asked of value_kind, never of
arithmetic" — which makes a lying `value_kind` worse, not better: it is the sanctioned way to ask.

No band is offered with this row. A band pinning today's behaviour would freeze the wound; a band
asserting the truthful answer would stand red until the encoding changes. The row and this receipt hold
the teaching until that call is made.

Row 1525 kindshadow.
