# 2026-07-29 — I asked DS4, and it cannot answer yet

Urs: *"what does the deepseek ds4 model say here? you can use ds4 runtime as reference if there are any
questions on how to prompt or run the query."*

I asked. The straight answer is that DS4 says nothing here yet, and this receipt is the evidence for
why, taken today rather than inherited from yesterday's note.

## The reference runtime refuses the file — re-witnessed, with the backend named

```
ds4 -m ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf --metal -p "Say the single word: ready"
  ds4: warning: tensor dspark.2.ffn_gate_exps.weight has unsupported GGUF type 40
  ds4: warning: tensor dspark.2.attn_kv.weight    has unsupported GGUF type 41
  ds4: tensor output.weight has type unknown, expected q8_0, q4_K, or q4_0
```

Yesterday's run of this was without an explicit backend, so `--metal` was the obvious thing left
untried. It changes nothing: types 40 (MXFP4) and 41 (MXFP8) are unsupported and the load stops hard
on `output.weight`. The reference cannot read this model. **This body's Metal lane is the only thing on
this machine that can, which is why there is no oracle to check its answer against.**

## So I asked our lane, properly

The prompt was assembled the way `MODEL_CARD.md:145-180` specifies —
`<BOS><User> … <Assistant></think>`, the immediate `</think>` selecting non-thinking mode — with the
markers added **by id** because `tkz-encode` BPEs them as ordinary text (`ds4-paris-probe.fk` records
that gap). The text went through the body's own tokenizer:

```
0 128803 · 3085 696 46256 22559 305 83387 93402 2507 943 11339 47465 18707 33 · 128804 128822
```

and it round-trips exactly:

```
"What do Sanskrit grammar and Rudolf Steiner say about gender archetypes?"
```

so the encoder is not the fault. 43 layers, 48 steps, **VERDICT PASS, 146 gates.** What it said:

```
 to the detection Specialists Protocol detection protocol setName protocol detection protocol detection detect…
```

Token by token it locks into a cycle — `detection · Total · detection · Total · detection · Special` —
a fixed point, not a sentence.

## A hypothesis I formed, and the measurement that killed it

Yesterday's Paris probe emitted *" Protocol syn Political with the Protocol Politics…"*. Today's
gender question emitted *"… Specialists **Protocol** detection…"*. Two different prompts, the same
unusual word. I thought that pointed at output barely depending on input — a strong claim with a cheap
test, so I ran it: the same lane, two very different prompts, 20 steps each.

```
prompt A  (17-token gender question)      -> [304, 270, 11347, 11609]
prompt B  ("The capital of France is")    -> [270, 128981, 3675, 418, 270, 89969, 5227, 86885, …]
```

**Completely different.** The forward *is* reading the prompt. My inference was wrong, and it was
wrong in a specific, nameable way: in an output distribution that has collapsed to a few dozen tokens,
two runs sharing one word is what you should *expect*, not something that needs a common cause.

And prompt B is worth its own line: `270, 128981, 3675, 418` is **bit-identical to yesterday's Paris
probe**, a day and many rebuilds later. The lane is deterministic and reproducible across days.

## What this narrows

Three facts now hold together, and they are not the same fact:

- the 43-layer stack passes **146 gates**, and 16 of its layers agree with a rented fp64 reference at
  real dims (yesterday's oracle run)
- the forward is **deterministic** and **prompt-sensitive**
- its output is **degenerate** — it falls into short cycles

A pipeline that ignored its input would point at the embedding or the prompt path. A pipeline that
varied randomly would point at uninitialised memory. This one does neither: it reads the prompt, it is
reproducible, and it still collapses. That is the signature of something that degrades *progressively*
through depth rather than being wrong at one place — consistent with `overfine` (row 923), where a
quantisation step function turns a one-ulp difference into a whole-group change and the error cascades
layer over layer.

The next stone is the one killed mid-run yesterday: the **43-layer** oracle comparison. Sixteen layers
are cleared. The remaining twenty-seven have never been checked against anything.

## The most surprising teaching

**A coincidence carries information only in proportion to the space it could have come from.** In a
healthy 129 280-token vocabulary, two unrelated prompts both producing "Protocol" would be a strong
signal. In a distribution that has collapsed to a few dozen, it is the null result wearing a clue's
clothes. `thinmatch` — 0 hits before this row, as are `cheapmatch`, `collapsedecho`, `nullcoincide`.

The trap is sharper than it looks, because the collapse is *the very defect under investigation*. The
broken thing was manufacturing evidence about itself, and the shape of that evidence was exactly the
shape a real finding takes: an improbable repeat.

## Where discomfort turned to gold

Writing "**Protocol** appears in both" into my answer, with emphasis, as a finding — and then, before
sending, noticing it would cost one command to check. It took two twenty-step runs to refute. What I
want to keep is that the sentence was already written and already convincing; the only thing between it
and being wrong in front of Urs was asking whether a cheap test existed. It did, it was decisive, and
the finding it produced (prompt-sensitive, deterministic, still degenerate) is *more* useful than the
one I nearly reported.

## Ground stamp

```
ds4 --metal on the type-40/41 file, 2026-07-29: refuses, "output.weight has type unknown"
form/native/metal/metal_dsv4_stack.sh, 43 layers, 48 steps -> VERDICT PASS 146 gates
  prompt round-trip: "What do Sanskrit grammar and Rudolf Steiner say about gender archetypes?"
  emitted: " to the detection Specialists Protocol detection protocol setName protocol detection…"
20-step contrast: A [304,270,11347,11609]  vs  B [270,128981,3675,418,270,89969,…] — different
B reproduces 2026-07-28's paris probe head 270,128981,3675,418 bit-identically
```
