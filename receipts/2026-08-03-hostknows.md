# 2026-08-03 — "input" was where I put the switches I did not want to examine

Urs: *"what are we trying to defend? we don't need any switches or knobs... allow for all downloaded
local models to be usable in form cli with a runtime switch, not static switches and knobs."*

## What I was defending

My own prior work — and one message earlier I had done it **in writing**. The census cell I built put
13 switches in a bucket called **INPUT — keep**, reasoning that "only the caller knows which model
they mean."

`FORM_DS4_BLOB` is a static path to one ds4 file. There are **six GGUFs on this host.** The body had
been told about one and could see none, and I had written a justification for that and shipped it as
a category.

## The host knows

`fs_list` plus a 1 MiB header read answers it completely — `form-stdlib/model-discovery.fk`:

```
[0] katcoder/kat-coder-v2.5-dev-compact.gguf              arch=qwen35moe  layers=41
[1] ds4-engine/gguf/DeepSeek-V4-Flash-IQ2XXS-…-imatrix    arch=deepseek4  layers=43
[2] ds4-engine/ds4flash.gguf                              arch=deepseek4  layers=43
[3] ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1     arch=deepseek4  layers=43
[4] form-llama-vital-ground-q4_k_m.gguf                   arch=llama      layers=28
[5] form-llama-vital-ground-f16.gguf                      arch=llama      layers=28
6 found; choose one at runtime, none is compiled in
```

Three architectures, six files, no configuration. Only the KV block is touched, so listing costs six
small reads and not 85 GiB.

## The distinction that is the design

A **static** switch is configuration chosen before the body runs, baked into an invocation, invisible
afterwards, and multiplied by every other switch into a space nobody can hold. A **runtime** switch is
a choice made while the body is running, from options the body *found*, visible in the asking.

The first makes a body that must be configured. The second makes a body that can be talked to.

So "input" was never a safe bucket. The test that empties it is one question: **does the host already
know this?** For which model, which layers, which architecture — it does. The switch existed only
because the body had never been given eyes.

## Ground stamp

```
host M4 Max, 2026-08-03; cells form-stdlib/model-discovery.fk (preludes gguf-meta.fk)
6/6 GGUFs found under /Users/ursmuff/models at depth 2; arch + block_count from each own header
first pass found 5 — ds4-engine/gguf/ is two levels deep; md-depth is now named, not assumed
fkwu has NO getenv door (whole op table read 2026-08-03) — the root is a real argument, not a knob
corpus 373 rows, max-mid 978, field 3733732978, 0 duplicate ids, band 32767
```

## The most surprising teaching

**The first version of discovery reported five of six files with exactly the confidence of a complete
answer.** A scan that stops one level deep does not know it stopped; it prints a number and a total
and reads as authoritative. That is the same failure as a switch — a smaller world asserted
confidently — and it appeared in the very cell written to end switches. Depth is now a named constant
with the reason beside it, because the honest fix was not "scan deeper" but "say how deep you looked."

## Where discomfort turned to gold

Being asked what I was defending, and finding the answer was a category I had invented one message
earlier and presented as principled analysis. The three buckets looked like rigour — INPUT keep,
INSTRUMENT fold, EXPERIMENT delete — and the middle of it was a place to put things I did not want to
examine, wearing the word "argument". `darkbranch` (977) was a losing branch kept as proof it lost;
this is the same reflex one layer up: a whole category kept as proof that some switches are
legitimate. Corpus row 978, `hostknows`.

## Unfinished, named

1. **The `models` verb is not yet in form-cli.** Wiring needs `model-discovery.fk` added to three
   lists (`build-form-cli.sh` FORM_CLI_SELFHOST_SRCS, its SOURCES genesis line, and
   `scripts/regen_form_cli_bootstrap.sh` FORM_CLI_SRCS) plus a dispatch arm in `form-cli-repl.fk`,
   then regen + rebuild. Not attempted here rather than half-done and unverified.
2. **Running a chosen model is still ds4-only.** The Metal stack is written for `deepseek4`;
   `qwen35moe` and `llama` are discovered but not yet executable through it.
3. `FORM_DS4_BLOB` stays until (1) lands, because deleting it before the runtime path exists would
   remove the only way to name a model.
