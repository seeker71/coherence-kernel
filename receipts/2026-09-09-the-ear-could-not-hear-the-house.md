# The ear could not hear the house it lives in

2026-09-09, Bali. Urs asked two things in one breath: *can we ensure we understand Indonesian as
well?* and *and we understand persian for live transcripts*.

Persian, grounded: **yes**. Indonesian: **no** — and the reason is the same shape as everything
else this day held.

## What was there

`form/form-stdlib/ear-native.fk`, one line:

```
; the tongue: the argmax over the language window, unless it lies outside the three the glass
; offers and one of them is within 3 logits of it
(defn enw-three () (list 50259 50300 50267))
```

Read from the asset the ear itself loads (`mlx_whisper/tokenizer.py`, whisper's own `LANGUAGES`
order, token = 50259 + index): 50259 is `en`, 50300 is `fa`, 50267 is `pt`. **Indonesian is
50275, and it is not there.**

The comment was already right. It says *the three the glass offers* — and the glass offers four:
`tln-defaults()` is `take(4, tln-rows())`, which is `en pt fa id`. A sentence sitting directly above
the code, describing it correctly, naming a number that disagreed with the list beside it, for as
long as both have existed.

Nothing could catch it, because a hand-written shortlist has nothing to disagree with. It is not
derived from anything, so no drift is possible — only silence.

## What it did to the room

`enw-lang-pick` returns the argmax unless it lies outside the shortlist *and* a listed tongue is
within 3 logits, in which case the listed one wins. So Indonesian, whenever it came close to
Portuguese or English — which for whisper-tiny is often — was heard as Portuguese or English.

The live spool from this house, `.hearth/ear.spool`, 3.2 MB:

```
3692 lang=en    253 lang=pt    96 lang=fa    15 de   12 ru   11 ro   7 la   4 ms
```

**Zero `id`.** In Bali. And it could not have been otherwise.

I went looking for proof that the 253 Portuguese frames were Indonesian misfiled, and they are
not — they read as real Portuguese, the repeated-phantom pattern of a quiet room
(`O que pode ser ver o vídeo?` twenty-odd times). The four `ms` frames carry Persian script. The
tail is detection noise, not misfiling. So the spool proves only the second half: no Indonesian
was ever recorded, and none ever could be.

## The heal

The heard-token now lives in the **same catalog row** as the code and the name, in
`form/form-stdlib/bml/transcript-languages.bml`:

```
list("en", "English", 50259), list("pt", "Portuguese", 50267),
list("fa", "Persian", 50300), list("id", "Indonesian", 50275), …
```

and `enw-offered()` is `tln-default-toks()`. One row, one truth. All 32 catalog tongues fall at
offset 0..89, inside the 99 logits the ear already reads, so the window needed nothing.

`form/form-stdlib/tests/ear-heard-tongues-band.fk` = **63**, and **59** with Indonesian dropped
from the defaults again — witnessed on the exact wound. `ear-native-band` still 32767,
`form-glass-meaning-ui-band` still 8191.

## What the mouth said when asked to test it

The plan was to have the body speak Indonesian and hear itself. The mouth answered:

```
the pass refused this voice
```

The model is present — 63 MB, mapped, `vs-has-voice?` = 1. And `voice-pass.bml` already holds
`vv-open-why`, a door that names exactly where an export parts from the one the pass was built on.
`vs-native-speak` was calling `vv-open-ok?`, discarding the why, and printing a bare refusal. The
healing was already in the body, one door away, thrown out at the last line. It now says:

```
unhealed native voice for id: no layernorm epsilon in this export
```

## And the census I ran was the liar

Asked to name which of the 29 mouths open, my first cell opened all 29 in one process and reported
**"the model file did not map"** for 25 of them. That is not the models. `es` opens cleanly on its
own; the process had exhausted itself and the lens blamed what it was looking at.

One model per process, the honest count is **15 of 29** — not the 25 the tree claimed:

```
opens:    en fa de fr es ru it ar tr ro sv vi cs hu sw
unhealed: the feed-forward padding did not resolve   bn he ja ko th zh
          no layernorm epsilon in this export        hi id nl pt
          the spline's own constants did not resolve el pl uk ur
```

The Indonesian export is not missing a value. It carries **0 `Constant` nodes where English carries
2579**, and 399 initializers against 460: it was exported with constant folding on, so the scalars
the pass reads by node name are inlined and must be read another way. Same 50 op kinds, 2729 nodes
against 2755 — the same graph, a different dialect of writing it down. Named, not attempted.

## Receipt

**Most surprising teaching.** The comment was correct. `; the three the glass offers` sat above a
list of three while the glass offered four, and it read as a description rather than as a claim.
Prose next to code is the one witness nobody checks, and here it was *right* — it had been quietly
naming the defect for as long as the defect existed.

**Where discomfort turned to gold.** Running the mouth census and getting 25 "did not map" results
that would have made a devastating paragraph. It was too good a story. Opening one model alone took
one minute and turned my own instrument into the accused: 25 of those 25 were my process running
out, not the body's models. The true number, 15 of 29, is a smaller finding and the only one that
is real — and the discipline that caught it is the same one from this morning's row 1380, applied
to myself instead of to a receipt I inherited.

**Frontier question, offered as corpus row 1416 `heardshort`:** *why a hand-written shortlist cannot
be caught being wrong.* Because it is derived from nothing, so nothing can disagree with it. Every
other kind of error has a second party — a compiler, a reference, a mirror, a count. A list typed by
hand has only the comment beside it, and a comment is not asked.
