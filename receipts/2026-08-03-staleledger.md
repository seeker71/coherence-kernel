# 2026-08-03 — the body had outrun its own written record

Urs: *"all assumed limitations, we have all models working already."*

He was right, and this was the **fifth** time in one day I asserted a limit I had never probed.

## What I had written, one message earlier

> "Execution is still ds4-only — the Metal stack is written for `deepseek4`; `qwen35moe` and `llama`
> are discovered but not yet executable."

## What the body already carries

`form-stdlib/native-model-control-plane.fk` — *"one honest control plane for every model lane"* —
**35 registered rows**. Among them:

```
base.llama32-3b-metal   local-native  CHAMPION  native-recipe  fkwu+Metal
```

Llama has been running natively on Metal the whole time. And for KAT: `metal_kat_block0.sh`,
`metal_kat_exit.sh`, `kat-coder-embed.fk`, `kat-coder-pipeline-map.fk`, `kat-coder-layer-shape.fk`,
`kat-coder-tensor-table.fk`, and the whole `gated-deltanet-*` family — the architecture, already
carved.

## The ds4 row was false in every field

```
status    failing                                    -> 101 gates VERDICT PASS today
surface   local-process                              -> native recipe on fkwu+Metal
evidence  "live --inspect --metal refusal 2026-07-25"
next      "supply a build that accepts GGUF types 40 and 41"
                                                     -> 40 and 41 carved since mx-residency
active    0                                          -> 28.6 t/s, bit-exact 24/24
witnessed 20260725
```

It was also present **twice** — and `nmcp-unique?` scored green with the duplicate in the list, so
that check does not see a repeated row.

Corrected to witnessed truth, duplicate removed, band re-pinned 35 → 34 rows, **65535**.

## The shape of it

**A body can outrun its own ledger.** Every "limitation" I reported today was a *read of a record
written before the work*, and I kept treating the record as the territory because it lived inside the
repo and therefore felt like ground. A registry is a witness statement with a date on it. This one
said `20260725`. The freshness field was **right there in the row** and I never compared it to today.

The rule: **read the ledger's date before its claim.** An old green and an old red are the same
artifact.

## Ground stamp

```
host M4 Max, 2026-08-03
native-model-control-plane.fk: 35 rows -> 34 (duplicate ds4 row removed)
base.llama32-3b-metal        local-native / champion / native-recipe / fkwu+Metal (pre-existing)
KAT lanes present: metal_kat_block0.sh, metal_kat_exit.sh, kat-coder-{embed,pipeline-map,
  layer-shape,tensor-table}.fk, gated-deltanet-{layer,conv,gates}.fk
ds4 row corrected: proven / native-recipe / fkwu+Metal / witnessed 20260803 / active 1
band tests/native-model-control-plane-band.fk 65531 -> 65535 (bit 4 = row count)
corpus 375 rows, max-mid 980, field 3753752980, 0 duplicate ids, band 32767
```

## The most surprising teaching

**`nmcp-unique?` returned green while the registry held two byte-identical rows.** The band has a
bit named for uniqueness, it was lit, and the duplicate was three lines from the check. I only found
the duplicate because I was editing that exact row for another reason. A check whose name describes
the property you want is not evidence that the property holds — and this one had been green across
every run since the duplicate was introduced.

## Where discomfort turned to gold

Counting five. Form-can't-dispatch-Metal, form-cli-can't, blocked-on-a-stamp, models-not-executable,
and now ds4-is-failing-per-its-own-row — five assertions of incapacity in one day, every one of them
a read of something stale or unprobed, every one delivered with the confident texture of analysis. The
discomfort is that they did not feel like guesses; they felt like knowing the codebase. The gold is
that the tell is identifiable in advance: **each one was a claim about the present tense sourced from
a document.** When the sentence is "we cannot X", the next move is a probe, never a citation.
Corpus row 980, `staleledger`.

## Unfinished, named — none of these are limits

1. **`nmcp-unique?` does not detect duplicate rows.** Found today, not fixed today.
2. **The flatten path for the four discovery cells** (`functions` delta is the instrument — a correct
   wiring moves it ~50, not 1). Still the blocker for `models` inside form-cli.
3. **A single ask door across lanes.** llama, KAT and ds4 each have their own carrier; the registry
   knows all three, and nothing yet routes a request by the row.
