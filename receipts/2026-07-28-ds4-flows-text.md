# 2026-07-28 — DS4 flows text; KAT-Coder does not. The honest answer to a direct question.

Urs: **"all gaps closed now and we have now flowing tokens for both models?"**

**No.** One model, not two. And the one that flows was flowing before I noticed.

## DS4 — yes, and it is text

```
gate 99  BOUNDED AUTOREGRESSIVE TOKEN FEEDBACK (4 steps, no EOS claim):
         [19129, 566, 56959, 295]
         each emitted token became the next step's exact embedding row
         and every hashed router's token
VERDICT PASS  102 gates
```

Decoded by the body's own Form-native tokenizer:

| token | text |
|---|---|
| 671 (prompt) | `The` |
| 19129 | `﻿using` |
| 566 | ` this` |
| 56959 | `.CharField` |
| 295 | ` in` |

**`"The"` → `"using this .CharField in"`** — 43 heterogeneous layers, growing per-layer KV arenas,
every per-layer decision read from the file's own tensor table, over the 85 GiB blob. Code-flavoured
output from a code model.

## KAT-Coder — no

Five kernels and the orchestration remain, all named in
[the pipeline map](2026-07-28-the-whole-pipeline-at-once.md): F32 matvec, the deltanet gates in MSL,
plain SwiGLU, partial RoPE, GQA 16/2. Entrance and exit run; three of forty blocks' stages are wired.
Nothing has changed about that since the map was written, and saying otherwise would be the
fabrication this lane exists to refuse.

## Where discomfort turned to gold

`metal_dsv4_stack.sh` carries that autoregressive loop behind `FORM_DS4_KV_SEQUENCE`, **which
defaults to 0.**

I ran that harness **twice tonight**. I read 96 gates, then 102. I reported *"DS4 emits a token"* —
twice, in a receipt and in a summary — when what it emits is a token **stream**, and the code to do
it was sitting in the file I had open, one environment variable away.

I had observed the **phenotype** — what the system does under its defaults — and reported it as the
**genotype**, what the system *is*. Those are different questions, and only one of them is answered
by running the thing the ordinary way. Every "not yet" I wrote about DS4's autoregression tonight
was false, and it was false because I never asked the harness what else it could do.

## The most surprising teaching

**A default is an editorial decision about what you will see.** `KV_SEQUENCE=0` is a sensible
default — the sequence path costs 102 s and the two-position path proves the stack — and it also
meant the single most interesting capability in the file was invisible to anyone who just ran it.
The body writes exhaustive gate text for everything it does; it has no gate for what it *declines to
do by default*, and that silence read as absence.

## The frontier question

> **What names what a thing does under its defaults, as distinct from what it can do?**

**`phenotype`** — against *genotype*. Distinct from `autoepistemic` (897, not-found read as
not-there): this was found, run, and passed, and its capability still went unread because a default
hid it. Verified 0 hits. Row **914**; band **32767**, 309 rows, field code 3093092914.

## Ground stamp

```
FORM_DS4_KV_SEQUENCE=1 FORM_DS4_KV_CAP=8 FORM_DS4_KV_STEPS=4 metal_dsv4_stack.sh
   -> VERDICT PASS, 102 gates, 102 s, tokens [19129, 566, 56959, 295]
dsv4-tokenizer-cli decode -> "using" / " this" / ".CharField" / " in"
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk -> 32767
```

## What is actually left

For KAT-Coder: five kernels, then blocks 0–39, then the timed pass against the 3300-dispatch
prediction. For DS4: nothing — it flows.
