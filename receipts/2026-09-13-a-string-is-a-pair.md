# A string is a pair

2026-09-13 (WITA). A let bound to an int or a float rode one register and crystallized. A let bound to a
STRING declined the whole leaf — a bare -1, at both admit sites. The reason was written in the code and had
stopped being true: "a string let would need a length beside its pointer: not yet."

## What a string let actually needs

A string in the lane is not a value in a register. It is a PAIR: a pointer in x(10+slot) and a length in
frame word 9 + slot. The let step emitted, for everything that was not a float,

```
MOV X(10+slot), Xr
```

which for a string value carries the POINTER and leaves the LENGTH behind at the source's word. The next
`str_len` would then read the destination slot's stale length against the new pointer. That is the wrong
answer the `lt == 3` guard had been standing in front of, and standing in front of it was the right call
until the other half could travel.

## The most surprising teaching

The repair was already written. `fk_f64_slot_move` has existed for an if's string answer, and it does
exactly the needed thing:

```
MOV Xto, Xfrom                  /* the pointer */
LDR X9, [X0, #8*(9+from)]       /* the length */
STR X9, [X0, #8*(9+to)]
```

Both halves, together. The let step simply was not calling it. So the change is one branch at each of the
two emit sites — `fk_f64_slot_move(words, wn, r - 110, slot)` where a bare MOV used to go — and a relaxed
condition at each of the two admit sites. Four small edits, no new instruction, no new mechanism.

The comment said "not yet". The machinery to make it "yes" had arrived since, for a different reason, and
nobody had gone back to read the note against the tree. A restriction is a claim about the world at the
moment it was written; it does not re-check itself.

## The guard, and why the value must already ride a slot

The length is at hand only if the value already occupies a slot. So the admit takes a string parameter
(kind 8), a literal (22), an if's string answer (17) and a call's string answer (19) — the four kinds whose
pointer/length pair is maintained, which is exactly the set `fk_f64_str_slot` answers for. An ACCUMULATOR
(20/21) is refused deliberately: it is mutated in place across the pass, so a let binding it would name a
value that moves underneath the binding. A general `str_concat` rides no slot and is refused too.

## Where discomfort turned to gold

I had an A/B probe from before the change: a string let read -1, an int let read 2. After the change the
string let still read -1. For a moment that looked like the repair had failed.

It had not. My probe bound its let to `(str_concat s "x")` — a general concat, which rides no slot. The
probe I had been calling my SUCCESS case was the GUARD case all along, and it reads -1 before and after
for two different reasons: first the blanket refusal, now the guard doing its job. I had set the
expectation "this should become 2" on a shape I had not re-analysed, and the number would have read as
failure to anyone who trusted my framing.

What made the difference legible was the band. `sll-c` is that same general-concat shape, deliberately
included as a bit that must stay cold. With the success cases (`sll-p`, `sll-l`, `sll-i`) beside it reading
2, 2, 2 and `sll-c` reading -1, the picture is unambiguous where a lone probe was not. The guard bit is
what turned a frightening number into a legible one.

## The honest edge

This does NOT warm float-repr, and I have not tested that it does. float-repr binds
`(let sign (if neg "-" ""))`, a kind-17 string answer, which this rung now admits — but float-repr is
reached only through the value_str float arm, which is still the C native and unswitched, so the recipe is
not on the live path to measure. What is witnessed is the let itself.

## Witnessed

loop-lane-string-let 7 (`[p, 2, l, 2, i, 2, c, -1]`), preflight clean. Rebased onto b2ebcf743, the records
band: full family green on the merged kernel — string-door 2047, if-chain/continue-chain/let 255, call 8191,
string-build/value 255, list-door 255, cons 511, closure 255, template 63, list-eq 15, str-eq 31,
str-eq-field 15, float-head 31, char-at 15, self-recursion 15, kind-fold 31, record-word 15, hof 7.
