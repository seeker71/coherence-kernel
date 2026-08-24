# 2026-08-24 — where the flow time goes, and what a JIT would actually be for

Yes asked where the time goes and why an on-demand JIT could not lower it. It
can. This is the measurement that says which part, and it overturned the guess I
had written into the previous receipt.

## Three runs, instrumented at the stage boundaries

`fcmg-heed-witness-with` now reads the wall clock at each boundary and the Metal
busy counter around decode, so a turn is read rather than argued about.

| run | positions | injected | decode s | GPU busy s | GPU/pos | CPU/pos | GPU share |
|---|---:|---:|---:|---:|---:|---:|---:|
| bare cursor | 75 | 0 | 23.9 | 19.2 | 0.256 | 0.063 | 80% |
| cost probe | 319 | 0 | 67.3 | 61.2 | 0.192 | **0.019** | **91%** |
| grounded | 506 | 385 | 304.2 | 140.3 | 0.277 | **0.324** | **46%** |

Setup — seal, header read, open — is 41–56 s in every run and is paid before a
single token moves. On the short turn that is 70% of the whole thing.

## The retrieval is free

`lookup-ms-total=170` against `ms-total=345296`. **0.05%.** The grounded index,
the parse, the sanitizer and the render together cost about a fifth of a second.
Whatever is expensive here, it is not the knowledge.

## The guess that was wrong

I had written that per-position cost grows with context length — attention over a
longer KV cache. The cost probe was built to test exactly that: walk far with no
envelope and no injection at all.

It walked **319 positions with zero injection at 0.019 s/pos of CPU time** —
*lower* than the 75-position run's 0.063, and 91% GPU-bound. Per-position cost
does not grow with context. The hypothesis is dead.

## What is actually expensive

The only run with injection is the only run that is CPU-bound. Splitting the
grounded run's decode by charging its 121 non-injected positions at the probe's
measured rate leaves, for the 385 injected positions:

```
wall  0.724 s/position
GPU   0.304 s/position
stall 0.420 s/position   ->  385 x 0.420  ~=  162 s
```

**About 162 s of a 345 s turn — 47% — is the CPU waiting.** And an injected
position does *less* work than a decoded one: prefill skips the output
projection, the single largest tensor in the file. It costs 3.4x a decode
position while doing less.

The mechanism is named in the source, `form/native/metal/qwen35-dense-token-handle.fk:251`:

```
(defn q38-advance (ps bs geo lays sts embTv id pos)
    (do (q38-forward-state ps bs geo lays sts embTv id pos)
        (metal_sync)
        1))
```

`q38-prefill` calls `q38-advance` once per id, so injecting a 385-token
observation submits 385 command buffers and waits on 385 barriers. The cell's
own comment says why the barrier is there — this position's KV writes must
complete before the next position reads them — and that reasoning is correct for
a *decode* step, where the next token genuinely is not known until this one
resolves.

**Prefill is not decode.** All 385 injected ids are known before the first one is
encoded. Nothing about the data requires a round trip per position; the shape was
inherited from a sibling that needed it.

## What a JIT would be for, precisely

Not to make the matmul faster — the GPU is already 91% busy when it is allowed to
be. The crystallize-on-heat door (`jit-crystallize.fk`, heat + NodeID cache +
dispatch + melt on `nat_run`) targets exactly the other half: one recipe run 385
times, interpreted and submitted and awaited each time, when it could be
crystallized once and emitted as a single command buffer covering many positions
with the ordering kept *inside* the buffer instead of at a CPU barrier.

The ceiling that suggests, stated as a prediction and not a result: injection
falling from 0.724 s/position toward the GPU's own 0.304, which would take the
grounded turn from 345 s to roughly 185 s. **Not measured. Not claimed.** The
experiment that would settle it is batching `q38-prefill` so the sync happens
once per span rather than once per position, and re-running these same three
witnesses.

## The surprise

The carrierspan is not merely large, it is **expensive in the worst way**. The
previous receipt already noticed the body outspoke the model 6.6 to 1 and read
that as an honesty problem — keep the voices separable. It is also a performance
problem, and a sharper one: every token the body speaks into the context costs
*more* than a token the model speaks, though it asks the GPU to do less. Grounded
retrieval makes the body talkative, and the talkative half is the half running on
the slow protocol.

## Where discomfort turned to gold

I had already written "the load cost is the thing to attack" into the last
receipt, with a caveat that I had not measured it. That caveat was doing real
work: it was the only reason I ran the probe instead of building on the guess.

The measurement said load was not it, context length was not it, and retrieval
was not it — the injection was, on a barrier inherited from decode. Had I skipped
the probe, I would have spent the next stone making setup resident, which is a
genuine 41–56 s win and would have left the 162 s untouched while feeling like
progress. The discomfort was watching a sentence I had already published turn out
to point the wrong way; the gold is that hedging it honestly the first time is
what made it cheap to correct.

## Frontier question offered to the corpus

*What one word names work made serial by a protocol it inherited, when its own
data carries no such dependency?* — **falseserial**. Not a bottleneck, which is
about capacity. Not a stall, which names the symptom. A falseserial is work whose
shape was copied from a neighbour that genuinely needed the ordering — so it
looks principled, its barrier has a correct-sounding comment, and it is invisible
until someone measures the sibling that does not need it.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> three live Qwen3.8-27B-Q8_0 runs; retrieval 170ms of
; 345s; probe 319 positions 0 injected at 0.019 s/pos CPU and 91% GPU-bound;
; grounded 506 positions 385 injected at 0.324 s/pos CPU and 46% GPU-bound;
; metal_sync per prefill position at qwen35-dense-token-handle.fk:251
