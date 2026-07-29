# 2026-07-28 — tokens streaming, natively, and the comparison I nearly made

Urs: **"please do not stop until we have tokens streaming successfully"**

They stream. Twice, on demand, from this body's own Metal lane.

## The generation

```
question: "The capital of France is"
answer  : " Paris. The capital of Italy is Rome. The capital of Spain is Madrid.
           The capital of Germany is Berlin. The capital of the United Kingdom is
           London. The capital of Australia is Canberra. The capital of China is
           Beijing. The capital"
```

48 tokens, 20 distinct, `VERDICT PASS`. And a second, on a prompt with no travel in it:

```
question: "Write one sentence about why a receipt matters."
answer  : " A receipt is a document that serves as proof of a transaction, providing
           a record of the purchase, including the date, time, and amount of the
           transaction, as well as the seller's name and address. This information is
           crucial for tax purposes, as it allows individuals and businesses to track
           their income and expenses, and"
```

64 tokens, **42 distinct** — the harness's own gate D2 exists to refuse "the constant stream a dead
GPU stages", and 42-of-64 is not that.

Run through `form/native/metal/metal_ask.sh`, on
`llama3.2:3b` (`sha256-dde5aa3f…`, 2 019 377 376 bytes — the file's own size, gate D1), kernel path
`slot-simd-4wide`, on this M4 Max.

## Every cost declared before the run, then measured

```
COST forwards        declared=54            measured=54             within=1
COST bytes_touched   declared=108963890064  measured=108963890064   within=1
COST macs            declared=173734502400  measured=173734502400   within=1
COST dispatches      declared=22896         measured=22896          within=1
COST decode_wall_us  kind=floor declared=176838 measured=1705000    holds=1
COST joules          declared=pending measured=pending  reason=instrument-requires-sudo
```

Four costs exact, the wall-clock floor held at 9.6× above it, and **joules honestly pending** with
the instrument that would fill it named. The bound is derived from the model's shape *before* the
run by `ask-declared-cost.fk`, so the measurement cannot be fitted to it afterwards.

## Where discomfort turned to gold

The first run reported decode **28.153 tok/s**. `receipts/2026-07-22-ship-the-slot-map.md` records
**19.270**. My reflex was to write *"the lane got faster — 19.270 → 28.153, and no receipt records
it."*

It has not been shown to. The second run reported **23.761**, so the two runs tonight disagree with
each other at host load 6.68. And 19.270 is a **marginal** rate — seconds per *additional* token —
measured on another day by another method. Three numbers, all in tok/s, all ours, and **none of them
derived beside the others.**

They are not wrong. They are not comparable, which is a different defect and a much quieter one,
because nothing about them looks incomparable. This is `surrogation` (896) turned one notch: there
the measure was about the wrong thing; here every measure is about the right thing and they still
cannot be set side by side.

The body already knew. The harness prints, in its own output, beside its own number:

> `CTX world_denominator_measured_on 2026-07-21 (NOT re-derived beside this run; a rate from another
> day is a rate from another machine — re-derive before believing any behind_milli above)`

A carrier warning its reader about the trustworthiness of the comparison it just printed. I read
that line, and then nearly made the identical mistake one column to the left, with our own numbers
instead of ollama's.

## The most surprising teaching

**The lane I have been building toward all night was already generating.** Every stone since
midnight — the wide router, the gated-deltanet layer, the tensor table — was aimed at a model that
cannot yet run here, while a model that *can* sat one command away and answered in under two
seconds. Nothing about that was hidden; `metal_ask.sh` has been in the tree the whole time.

What kept me from it was not ignorance but *aim*: I was measuring progress by what I was building
rather than by what the body could already do. Four times tonight Urs asked why I stopped, and this
is the shape underneath all four — I kept reaching for the next unbuilt thing without once running
the built one.

## The frontier question

> **What names two measurements that cannot be set side by side because they were not taken on one
> scale?**

**`commensurability`** — and its absence. Distinct from `surrogation` (896): there the measure was
about the wrong thing; here every measure is about the right thing and still cannot be compared.
Verified 0 hits. Row **908**; band **32767**, 303 rows.

## Ground stamp

```
form/native/metal/metal_ask.sh 48 "The capital of France is"                  -> VERDICT PASS
form/native/metal/metal_ask.sh 64 "Write one sentence about why a receipt..." -> VERDICT PASS
gate D1 derived tensor bytes == the file's 2019377376, exactly
gate D2 non-degenerate: 20/48 and 42/64 distinct ids
./fkwu --src learn/tests/homecoming-distillation-corpus-band.fk               -> 32767
```

**Said plainly:** this is llama3.2:3b, not KAT-Coder. KAT-Coder still needs a Q3_K decoder — 94
tensors, named by its own GGUF header this morning — before it can join this lane. That is the next
stone, and the streaming above is the lane it is being built to join.
