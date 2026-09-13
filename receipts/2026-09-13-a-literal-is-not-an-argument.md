# A literal is not an argument

2026-09-13 (WITA). `str_eq` of a string parameter against an inline literal — `(str_eq k "float")` — declined
at -1026 and walked. That single decline was holding `fstr-text-kind?` cold, and `fstr-text-kind?` sits on the
measured path to `to_string`. It is warm now, and the fix was not in the compare.

## The decline, and what it was costing

Guard 1 landed str_eq as a compare of two interned frame words, and said so in its own comment: "First cut:
string PARAMETERS only (kind 8) ... a literal operand keeps walking (its slot word is unfilled)." A literal
does get a hidden slot, and `fk_f64_str_slot` already returns it — but nothing ever wrote the literal's WORD to
frame word 24 + slot, so the compare had nothing to read.

`fstr-text-kind?` is nothing but three such compares:

```
(defn fstr-text-kind? (k) (if (str_eq k "string") 1 (if (str_eq k "list") 1 (if (str_eq k "float") 1 0))))
```

so it declined at -1026, and `int_to_str`, which calls it, sat cold at 0.

## The most surprising teaching

The repair belongs in the PROLOGUE, not in the compare. Teaching kind 35 to special-case a literal would have
meant a second code path in the emit — `mov64` on one side, `LDR` on the other — and every later arm that reads
a word at 24 + slot would have needed the same special case again.

Instead the literal's prologue now writes its interned word into frame word 24 + slot, beside the pointer and
length it already wrote. Two instructions per literal. After that the literal READS EXACTLY LIKE A PARAMETER:
the admit simply stops rejecting kind 22, the emit is untouched, and the field-string guard is untouched. A
hidden slot is only ever allocated where `types[s] == 0`, so 24 + s can never collide with a live parameter's
word. The cheapest place to fix a read is often where the value is written.

The guard needed no new case because a literal can never be a field string, and provably so rather than
probably: the kind-24 admit refuses unless `si < FK_STR_BASE`, so a literal's word is always local, always
above T, and the `B.LE` simply never fires for it.

## The coverage that was not coverage

Both str_eq bands looked like they covered literals. Neither did. `loop-lane-str-eq` bit 8 reads
`(defn seqlit (a) (seq 5 a "lit" 0))` — the literal is an ARGUMENT, and the callee receives it as a PARAMETER,
so it exercised the parameter path under a literal's name. `loop-lane-str-eq-field` binds `(let ls "...")` and
passes `ls` in, which is the same thing again. The shape changed in transit and the gap hid behind two green
bits.

So both bands gained a bit that uses a literal where it actually differs — as an OPERAND in the body.
loop-lane-str-eq is 31 (bit 16: crystallizes, equal and differing answer as the walker).
loop-lane-str-eq-field is 15 (bit 8: the carrier-read FIELD string against an inline literal of the same bytes
answers equal, warm — the field side trips the guard, the walker does the byte walk). That second one is the
one that mattered: it is the multi-process crossing, and without it the new path would have been reasoned about
and not witnessed, which is the exact wound guard 1 shipped and had to repair.

## Where discomfort turned to gold

Right after the change, `loop-lane-char-at` fell from 15 to 11. Bit 4 is a timing bit — the warm run beating
its walker twin — and I had just added two instructions to every leaf's prologue. Two comfortable stories were
available: "it is only the host load" (flattering) and "I slowed it down" (self-critical). Both were guesses.

So I built origin/main's runtime as a control and ran the two binaries interleaved, clearing the band's cache
before every single run so neither could ride a warm image. Three rounds: control 15, mine 15, control 15, mine
15, control 15, mine 15 — with identical timings, warm 3 ms against the walker's 5 in both. The 11 was a flake
at load 3.41; load had since fallen to 2.41. The discomfort of not knowing was worth more than either story,
and the control is what ended it.

It also killed a change I had already designed: gating the prologue write so only str_eq consumers pay for it.
The control shows the cost is not measurable. Adding a mechanism for an unmeasurable gain is complexity without
evidence, so it is not here.

## The honest edge

This does NOT move `to_string`. `int_to_str` still reads 0: `value_kind` at tag 201 sits behind it, and that is
blocked on the kindshadow collision (row 1525) — `value_kind` cannot be folded while its own answer depends on
how many records exist. One link of the chain is warm; the chain is not.

Witnessed: loop-lane-str-eq 31, loop-lane-str-eq-field 15 (carrier), both preflight clean; full loop-lane
family green (char-at 15, self-recursion 15, call 8191, template 63, cons 511, closure 255, list-door 255,
list-eq 15, string-door 2047, string-build/value 255, if-chain/continue-chain/let 255, float-head 31, hof 7);
corpus 32767.
