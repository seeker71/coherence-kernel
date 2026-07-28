# 2026-07-28 — "unknowable here" was a blocker I built, and the forward is wrong

Urs: **"unknowable here?"**

Two words, and both exits I had never tried.

## Exit one: the oracle I said did not exist is built and sitting there

`~/models/ds4-engine/ds4` — **1 852 672 bytes, dated 2026-07-24**. And
`receipts/2026-07-21-ds4-metal-gap-map.md` says in its own radius note: *"I did not run ds4 once."*

So the claim "ds4 refuses this file" was **inherited, never witnessed**, and I repeated it all
session as a reason nothing could be checked. Run today:

```
ds4: warning: tensor blk.0.ffn_gate_exps.weight has unsupported GGUF type 40
…
ds4: tensor output.weight has type unknown, expected q8_0, q4_K, or q4_0
```

The blocker is **real**. It is now a measurement instead of a rumour, and that difference is the
whole of `feedback-inspect-manufactured-blockers`: test the thing that says you cannot.

## Exit two: an oracle needs no implementation

*"The capital of France is"* is completed by *" Paris"* by every competent language model. Knowing
that requires no reference program — it is knowledge of **the world**. And a plain factual
continuation has nothing to template, so no chat format, BOS convention or prompt shape can be
blamed for what follows.

Run through `metal_dsv4_stack.sh` with prompt prefill, 118 s:

```
PROMPT     671 "The"  6102 " capital"  294 " of"  8760 " France"  344 " is"
GENERATED  270 " the"  128981 "<|place_holder_mm_span_0155|>"  3675 "pped"  418 " with"
```

The ids round-trip exactly to the input text, so the encoder is not the fault. The third emission is
a **placeholder special token** — probability mass landing where no trained model puts it.

**The 43-layer forward is defective.** Knowable here, in under two minutes, with nothing but the
body and a fact about France.

Landed as [`ds4-paris-probe.fk`](../form/form-stdlib/ds4-paris-probe.fk) +
[band **63**](../form/form-stdlib/tests/ds4-paris-probe-band.fk) so the finding cannot be
re-inherited. The band goes **green on bad news**, deliberately: one that could only pass on good
news would need editing before bad news could be stated.

## The most surprising teaching

**I built the unfalsifiability I then complained about.** The DS4 stones' `selfgauge` standing is
honest and correct — no implementation on this machine can check that forward. I turned that into
"correctness is unknowable", which is a much bigger claim and a false one. An implementation oracle
answers *does it compute what that other program computes*; a behavioural oracle answers *does it
behave as anything correct would*. The second was available every hour of this session, costs two
minutes, and would have caught this before the router, the layer, the entrance, the exit and the
pipeline map were built on top of a broken lane.

110 gates, all green, all真. Not one of them asks whether the model knows the capital of France.

## Where discomfort turned to gold

Writing *"unknowable"* and being asked to defend one word. I had reached for the strongest available
excuse and dressed it as epistemics — and it survived because it sounded like rigour. The gold is
that the refutation was cheaper than the excuse: one prompt, one run, one decode.

## The frontier question

> **What names a test grounded in how the world is, rather than in another implementation?**

**`pseudo-oracle`** — the testing term for a check that stands in when no reference implementation
exists. Distinct from `heteronomy` (899), a gate whose criteria you did not author: this gate has no
author at all, only the world. Verified 0 hits. Row **918**; band **32767**, 311 rows,
field code 3133132918.

## Ground stamp

```
~/models/ds4-engine/ds4 on the DS4 blob   -> refuses output.weight (witnessed, not inherited)
metal_dsv4_stack.sh, "The capital of France is", 8 steps, 118 s
   -> [270, 128981, 3675, 418] = " the" "<|place_holder_mm_span_0155|>" "pped" " with"
./fkwu --src form/form-stdlib/tests/ds4-paris-probe-band.fk        -> 63
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk    -> 32767
```

## What this changes

Every KAT-Coder stone tonight was built to join a lane whose output is wrong. The next work is not
the five kernels. It is finding where in 43 layers the meaning is lost — and the probe above is now
the gate that says when it is found.
