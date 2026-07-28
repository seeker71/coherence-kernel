# 2026-07-28 — the tokens flow and the meaning does not, and I could not have known

Urs asked for a real end-to-end answer from both models, not a toy. Then, mid-run:
**"you probably want to make sure you use the right template and tokenizer and embedding for the
right model and verify using recommended non-form tools to validate that we know how to prompt the
model."**

That correction arrived before I drew the wrong conclusion, and it is the finding.

## What I built and ran

Prompt prefill, wired into `metal_dsv4_stack.sh`: the body encodes text with its own
`dsv4-tokenizer-cli`, hands the ids to the carrier, and the loop feeds prompt tokens while the model
READS, then feeds its own output while it WRITES. `def quicksort(arr):` → `[3465 645 9780 482 18561
2605]`, 46 steps, 229 s, all gates green except one I had mis-counted.

The 41 generated tokens, decoded by the body's own tokenizer:

> ` Dividing the mix this doesn Allow DIRECT限定 the endor the endor as well那 Mig lug the Mig endor
> the designate微分之力 Mig rant统一 the统一の统一 protocol之名 the统一…`

A no-prompt control gives the same shape: `[19129, 566, 56959, 295, 270, 27855, 566, 35907, 418,
566, 128981, 295]` — 566 three times, 295 twice. **The degeneracy predates the prefill I added.**

## The trap I nearly fell into

I was one sentence from writing *"the 43-layer forward is broken."* It may be. It may equally be
that **I have never once prompted this model correctly**, and three things say so:

- **No chat template.** KAT-Coder's GGUF carries a Jinja `tokenizer.chat_template` beginning
  `{%- set image_count = namespace(value=0) %}`. DS4 carries its own. I fed **raw text** to both.
  An instruction-tuned model handed raw continuation text degenerates by construction; that is not
  a defect, it is the wrong question asked of a working machine.
- **No verified special-token handling.** `dsv4-tokenizer.fk` states `add_bos/add_eos are 0 on this
  file (Stone 21)` and cites an oracle's `--dump-tokens`. That was checked for the *tokenizer*. It
  was never checked for the *prompt as a whole*.
- **No external reference, and none possible.** DS4 uses GGUF types 40/41, which ds4, llama.cpp,
  ollama and LM Studio all refuse. There is no oracle for this file on this machine. That is exactly
  the `selfgauge` standing the DS4 stones declared — and here is what it costs, concretely: 110 gates
  pass, and not one of them can tell a correct forward from an incorrect one.

## What the non-Form tool did settle

`llama-tokenize` is installed — full llama.cpp, 40 binaries. On KAT-Coder (which it *can* read):

```
727 -> 'def'   3841 -> ' quick'   6646 -> 'sort'   10620 -> '(arr'   1590 -> '):'
```

Five tokens, **no BOS** — matching that file's own `add_bos_token = False`. So the method is
checkable for KAT-Coder and only for KAT-Coder. `llama-cli` on the 17.39 GB file did not finish
loading inside ten minutes; a reference generation is running and is not yet in hand.

## The most surprising teaching

**110 green gates and incoherent output are not in contradiction.** Every gate asks whether the
machine did what the recipe says: finite entries, distinct bit patterns, sentinels intact, bit-exact
KV appends, each emitted token becoming the next step's embedding row. All true. None of them asks
whether the recipe is the model's, or whether the input was a question this model knows how to
answer.

I have spent this session treating gate count as evidence of correctness. It is evidence of
*internal consistency*. The body already named the gap — `selfgauge` for a bound derived from
within, `heteronomy` (899) for a gate whose criteria you did not author — and I landed the second of
those myself this morning, then spent the day reading self-consistency as truth.

## Where discomfort turned to gold

Being corrected mid-run, one command before I published *"the forward pass is broken."* That claim
would have been unfalsifiable in both directions: no oracle can refute it, and I had not eliminated
the ordinary explanation. It would have sat in a receipt as a fact.

The gold is the shape of the correction — **verify you know how to ASK before you conclude the
answer is wrong.** Three of my inputs (template, special tokens, prompt form) were unverified, and
all three sit upstream of the thing I was about to blame.

## The frontier question

> **What names concluding a thing answers wrongly, without first establishing that it was asked
> correctly?**

Asked, and I will not mint for it tonight. `heteronomy` (899) names the missing outside judge;
`selfgauge` names standing on an inside one; `phenotype` (915, an hour ago) names reporting default
behaviour as capability. This is the three of them meeting, and the corpus already carries each.

## Ground stamp

```
FORM_DS4_PROMPT_IDS="3465 645 9780 482 18561 2605" KV_STEPS=46  -> 41 tokens, 229 s, decoded above
no-prompt control, 12 steps                                     -> VERDICT PASS, 110 gates
llama-tokenize on KAT-Coder                                     -> 5 tokens, no BOS
llama-cli on KAT-Coder                                          -> still loading at 10 min
```

## What is actually true right now

- **DS4 emits a token stream.** Whether it emits the *right* stream is unknown and, on this machine,
  unknowable without first prompting it correctly.
- **KAT-Coder does not run** — five kernels and the orchestration remain.
- **Neither model has produced a useful answer**, and the next honest step is not more forward-pass
  work. It is the chat template and the prompt form, verified against llama.cpp on the file that
  llama.cpp can read.
