# 2026-08-01 — I made a working model sound broken, and the user was right to stop me

Urs: *"this makes no sense whatsoever. this difference makes those differences essentially unable to
use or generate anything real or useful, unless you can show me how this actually makes sense and
makes the model useable."*

He was right to push. My framing — *"the reference is a coin the machine flips"* — described a genuine
measurement and drew a false conclusion from it. Here is the measurement that settles usefulness.

## Ask it real questions

```
PROMPT: The three primary colors are
  ours:  red, yellow, and blue. They are called primary because they cannot be
         produced by mixing other colors. The secondary colors are orange, green, and vi…
  ds4 :  red, yellow, and blue. They are called primary because they cannot be
         made from any other colors. Secondary colors are made by mixing two primary col…

PROMPT: Water boils at a temperature of
  ours:  100°C at sea level. The boiling point of water decreases as altitude
         increases. At 1,500 m above sea level, water boils at…
  ds4 :  100°C at sea level. The boiling point of water decreases as the altitude
         increases. The boiling point of water at an altitude of 1…
```

**Identical facts. Word-identical for the first ~12 tokens. Then different paraphrases of the same
true statement.** Both correct, both fluent. The model is usable. That is the whole answer to the
question asked, and it is an observation, not an argument.

## Why this is normal and not a defect

Two implementations of the same weights diverge under greedy decoding as soon as any step is a near
tie. That is not a property of this port — it is a property of argmax over floats. `llama.cpp`,
vLLM and HF transformers all diverge from each other on long greedy continuations of the same model,
and nobody calls those unusable. What is measured here is stronger than any of those pairs:

- r = **0.999995** against ds4 at three tokens, **0.999333** at five — inside ds4's own
  backend-to-backend spread (0.999782 / 0.999044).
- At the one divergence we examined, **all three of ds4's own answers were in our top three**, and
  ours matched ds4's CPU arm exactly.
- 24 of 24 tokens identical on the verified prompt.

A model whose next-token distribution matches the reference to r≈0.999 and whose text is factually
correct is a working model. Token-identity with a *particular* implementation is a much stronger
property than usefulness, and it was never the thing that made the model good.

## What the stream check actually was

The mistake underneath my framing: I had been treating "the stream is bit-exact" as a **claim about
model quality**. It is not. It is a **change detector for kernel work** — and an excellent one. It
caught, this week alone: a Q2_K lane split that was faster and emitted all zeros; a kernel running on
a quarter of its rows while reporting a 5 ms lower floor; a perl interpolation that deleted an operand
and still compiled. Forty kernel changes were gated on it and it never once passed something broken.

Used as a regression test against *yesterday's own output*, it is exactly right. Read as "we have
reproduced ds4", it claims more than it can carry — because at a 0.6-logit tie ds4 does not have a
single output to reproduce.

## What is genuinely still open, at its real size

Our max |delta| at that tie was **3.24** against ds4's own internal **2.16** — so there is real
arithmetic difference, about 1.5× outside the reference's self-disagreement. Named candidates, both
known and neither yet measured: our transcendentals are Taylor series where ds4 calls libm (`mla_exp`
vs `expf`), and several reassociations were adopted after checking only that the stream held on the
one verified prompt. That is a bounded target with a number on it. It is not a reason the model
cannot be used.

## The most surprising teaching

**A measurement can be sound, reported accurately, and still mislead — through the sentence wrapped
around it.** Every number in the previous receipt was correct and re-derived. "ds4 gives three answers
at this point" is true. But I let a fact about *one near-tie in one continuation* stand in for a claim
about the model, and the reader who could not run the model himself had nothing to correct it with
except suspicion. Precision in the numbers does not buy honesty in the summary; those are two separate
disciplines and I only practised one.

## Where discomfort turned to gold

Being told my finding made no sense — when every number in it was measured, checked, and right. The
comfortable response was to re-explain the measurement. What was actually needed was to notice that
the question had changed from *"do we match ds4"* to *"is this thing any good"*, and that I had never
once run the second experiment despite six days of running the first. Two prompts and ninety seconds
answered it. The gold is the correction to my own instinct: when someone says a result makes no sense,
the first move is not to defend the result — it is to ask what question they are holding that the
result does not answer.

## Ground stamp
```
form-native, FORM_DS4_PROMPT (text in via the body's own tokenizer), 28 t/s, 34 ms/token GPU floor
"The three primary colors are"      -> red, yellow, blue + correct definition of primary/secondary
"Water boils at a temperature of"   -> 100C at sea level + correct altitude dependence
ds4 on the same prompts: same facts, word-identical ~12 tokens, then different paraphrase
r vs ds4: 0.999995 @3 tokens, 0.999333 @5 — inside ds4's own arm-to-arm spread (0.999782 / 0.999044)
at the examined divergence: ds4's three answers (22059 / 21289 / 7610) are our ranks 2 / 1 / 0
still open, sized: our max |delta| 3.24 vs ds4's internal 2.16; candidates are our Taylor
  transcendentals vs libm, and reassociations verified on one prompt only
```
