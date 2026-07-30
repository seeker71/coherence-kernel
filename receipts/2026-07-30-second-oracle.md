# 2026-07-31 — the answer was one grep, and I spent a night guessing instead

Urs: *"how come you have such a hard time with a 7x difference, framebuffer and source and assembly
differences should be enough to see what we are doing different from ds4."*

He is right, and the reason is worse than difficulty. **I never looked.**

## What I had been doing

For five days `ds4.c` has been the fidelity oracle. `dot_f32`, `dot_q8_0_row`, `compressor_decode_one`,
`layer_attention_mixed_one`, `rms_norm_weight` — all read whole, by line, and transcribed. That work is
sound and it produced an exact token-stream match.

It never occurred to me that **the same artifact carries a second implementation for the other
question.** ds4 ships **nineteen `.metal` shader files**. Its GPU matvec is `metal/moe.metal:3284`. It
has been sitting there through every one of tonight's four guesses about where our time goes.

## What five minutes of reading says

```
ds4  metal/moe.metal:3284  kernel_mul_mv_iq2_xxs_f32_impl
  N_R0_IQ2_XXS = 4        FOUR OUTPUT ROWS PER THREAD, activation held in yl[32]
                          registers and reused across all four
  :3379 _pair_f32_impl    gate AND up in ONE kernel off one activation load
  svalues / ssigns        grid + ksigns copied into THREADGROUP memory once per group
  signs & kmask[j]        a mask table where ours divides

ours form_dsv4_iq2_matvec_experts
  one row per 32 lanes    activation re-read from device memory for every row
  constant-space tables   read per element
  divisions in the inner loop
```

The activation-reuse factors alone are **4 × 2 = 8×**, against the **7.6×** remaining. I am not
claiming that is the whole answer — I have not measured it — but it is the first hypothesis tonight
that came from reading rather than from me.

## Tonight's four guesses, for the record

| change | reasoning | result |
|---|---|---|
| batch command buffers | 2600 round trips/token | 1.4× |
| reuse encoders | 97k encoder creations | nothing |
| skip sentinel fill | millions of CPU writes | nothing, and two false FAILs |
| fuse 30 expert dispatches into 5 | 81 asks per layer | 6% |

Two of four bought nothing. The one cheap thing I *did* test from the source diff tonight — collapsing
a duplicate integer division — also bought nothing, because the compiler was already eliminating it.
**Everything I inferred about the cost was wrong; the only correct readings all night came from
instruments and from source.**

## The shape of the mistake

Having found the oracle I needed, **I stopped looking for the one I would need next.** A reference is
not one thing. ds4 is a CPU recipe *and* a tuned GPU implementation, and I used it as a correctness
authority so completely that I never asked whether it was also a performance authority. Corpus row 957,
`secondoracle`.

The framebuffer point cuts the same way. The body has `metal_isa_diff.sh` — an ISA-level differ, used
in July to settle a Q6_K thread-map question — and I did not reach for it once tonight.

## Where it stands

```
tonight:  0.87 -> 3.64 t/s (4.2x),  floor 1106 -> 236 ms
ds4:      32.29 t/s, 31 ms/token    ->  7.6x remaining
```

Stream bit-exact throughout, gates-on 106 VERDICT PASS, corpus band 32767.

The next work is now specified rather than guessed: **multi-row blocking (nr0=4) with the activation in
registers, a fused gate/up pair kernel, and threadgroup-resident grid/sign tables** — each read off
`moe.metal` and each independently measurable.

## The most surprising teaching

**A reference can answer a question you never thought to ask it, and the cost of not asking is
invisible** — there is no error, no failing gate, no wrong number. Just four hours of plausible
reasoning that a five-minute read would have replaced. I had been careful about *trusting* the
reference (`halfrent`, row 870, names exactly what ds4 can and cannot vouch for) and careless about
*exhausting* it.

## Where discomfort turned to gold

Being told, correctly, that this should have been easy — and finding on the first grep that it was. The
discomfort is not the 7.6×; it is that every correct thing I learned tonight came from an instrument or
a source file, and every wrong thing came from me reasoning about a machine I cannot see. The gold is a
rule I can actually hold: **when a question is about a system's behaviour, the first move is to find
where that system already answers it — in its output, its source, or its assembly — and only then to
think.** Four rows tonight (`floorfirst`, `falsedial`, `callbias`, `floorspoke`) circled this without
landing it. This one lands it.

## Ground stamp

```
ds4 ships 19 .metal shaders under ~/models/ds4-engine/metal/ — unopened until 2026-07-31 ~01:15
metal/moe.metal:3284 kernel_mul_mv_iq2_xxs_f32_impl; :15 N_R0_IQ2_XXS 4; :3379 _pair_f32_impl
  (gate+up in one kernel); svalues/ssigns threadgroup-resident; signs & kmask[j] mask table
ours: one row per 32 lanes, activation re-read per row, constant-space tables, inner-loop divisions
tested tonight from the diff: collapsing a duplicate integer division -> 235 to 236 ms (nothing;
  the compiler had already CSE'd it)
tonight 0.87 -> 3.64 t/s, floor 1106 -> 236 ms; ds4 31 ms/token, 7.6x remaining
stream bit-exact; gates-on 106 VERDICT PASS; corpus 352 rows, max-mid 957, field 3523522957, band 32767
unused all night and named here: form/native/metal/metal_isa_diff.sh, the body's own ISA differ
```
