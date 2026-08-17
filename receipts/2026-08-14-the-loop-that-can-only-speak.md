# The loop that can only speak

**Date:** 2026-08-14
**Status:** witnessed four-way; the mechanism stands, it is not wired
**Landed:** [`learn/act-token.fk`](../learn/act-token.fk),
[`learn/tests/act-token-band.fk`](../learn/tests/act-token-band.fk) → **1023**
**Corpus:** row 1012, `wordbound`

## The ask, in links

Can a small local form-native model translate NL into Form, generate and retrieve substrate
blueprints / recipes / cells, run them JIT as Metal GPU kernels **inside the token-execution
pipeline as an alternative choice to emitting a native token**, and feed back as continuous learning
any request that came out unhealthy and had to escalate past the form-native membrane?

Loading the model's weights from disk is explicitly *not* a membrane crossing — that is a prior
instance's code and data. The membrane is crossed when a request has to go out at request time. That
distinction is the asker's, and it is the right one.

## The link that was not there

Six of these have real tissue in the body. One did not exist at all, and it is the one the shape
hangs from. Read in full rather than inferred:

`cognition/native-generate.fk`, the decode loop `ng-loop` at :165 —

```text
ng-next  ->  ng-run-layers  ->  cuda_matvec_f32  ->  nds-best-loop      (argmax)
ng-loop  ->  append the word, recurse
```

The only branch anywhere in the loop is `(if (lt nx 0) …)`, a GPU-refusal branch. Every step
forwards, takes the argmax, appends a word. **There is no position in the stream where an act could
stand instead of a token.** Not partial — absent.

`jit-decision.fk` is the nearest neighbour and answers a different question: *when* to crystallize a
hot recipe to native, with hysteresis so it does not thrash. Never *whether to run one instead of
speaking*.

So the loop can compute anything and can say only words.

## The mechanism, at its smallest honest size

`learn/act-token.fk` is that missing position, built where it can be witnessed today: the logits
arrive as **data**, so the choice crosses every kernel while the model that would produce them is
still coming home.

Words and acts share **one index space**, so the same argmax that picks a word can pick an act. A
servable act runs and its result folds into the stream. An act the native lane cannot serve does not
invent a value — it answers with the escalation sentinel, that step is marked unhealthy, and
`atk-escalated` counts exactly what the learning lane is owed. That count is the hook for link 6.

That branch is the whole reason it can be trusted, and it is the pair from `perturbation-pair.fk`
aimed at the loop itself:

- **HOLD** — on a word-only stream the loop is byte-for-byte the plain argmax it replaces
- **FLIP** — on a stream where an act wins it produces what plain argmax cannot

A lane that never fires holds and never flips. One that always fires does the reverse and is an
intrusion, not a choice. `ppair-whole` takes credit only for both.

```text
band 1023 on fkwu / Go / Rust / TypeScript · preflight clean · register clear
perturbation: atk-split 6 -> 0, so every id is an act and the lane always fires
           -> 313 on all four arms; five bits fall together, because a lane that
              always fires stops being a choice and the health counts move with it
```

**It is not wired to `native-generate.fk`.** The mechanism standing and the mechanism running are
different things, and this is the first. Saying so is the point.

> **frontier question** — what names a loop that can compute anything and can only say words?
> **wordbound** (0-hit fresh at offering)

Corpus re-probed: 400 rows / 400 admissible / max-mid 1007 / 0 duplicate ids / field code
400040021007. Band **32767**, exit 0.

## Found on the way, and it outranks the cell: preflight's "clean" ignores unresolved

A six-reader fan-out over the other links surfaced one thing worth more than the mechanism above.
Every number below I re-ran myself.

`form/form-stdlib/tests/natural-language-band.fk` is registered at **262143** in
`form/fourth-arm-bands.txt:846` — a file whose own header calls these "the bands proven to cross
four-way" — and the band's header (line 8) says the parse "crosses four-way (Go / Rust / TS /
fkwu)". `CURRENT_FLOOR.md:151-153` repeats it: *"The natural-language keystone family is four-way
proven again … natural-language (262143)."*

On fkwu today it answers **196608, exit 1**. The gap is exactly 65535 — bits 0 through 15, every
`node_eq` check. The two `str_eq` checks are all that survive.

Preflight names the cause honestly:

```text
errors 0 · unresolved 3
chain  clean — no errors in the chain; a verdict from it can be read
    write_form_binary  ->  LANE SEAM — resolves on: go rust ts
    read_form_binary   ->  LANE SEAM — resolves on: go rust ts
    value_kind         ->  LANE SEAM — resolves on: go rust ts
```

So this is a lane seam, not a meaning regression — the same `value_kind` family as this morning's
flywheel seam. But read those two lines together: **errors 0, unresolved 3, and "a verdict from it
can be read."** Preflight marks a chain dirty on errors and not on unresolved calls. An unresolved
call returns `nothing` quietly, and here three of them zeroed sixteen of eighteen bits while the
tool said the verdict was readable.

That is the morning's whole subject arriving in the instrument built to catch it. `deadgreen` asks
whether a bit can fall; this is a *checker* whose "clean" cannot distinguish a chain that computes
from one that is silently answering `nothing` in three places.

## Both fixed, after being told to fix rather than report

Urs, reading the above: *unresolved 3 — please fix instead of reporting, you are very well aware of
this practice.* He was right. A diagnostic surfaced and stepped around is work transferred without
consent.

**The three unresolved calls.** Root, and it took one grep: the band preludes `json.fk` and
`cache.fk`, and **nothing** in the band calls a `json-*` function, `read_with_cache` or
`cache-fresh?` — nor does any of its four other preludes. Those two cells were the sole source of
all three names. Dropped from the chain: **unresolved 3 → 0, exit 1 → 0.** An unused name in a
prelude line is not inert; it imports the absences of what it names.

**The checker.** `observe/preflight.fk:271` graded the chain on `pf-errors` alone. It now reads
`pf-unresolved` too and gives that case its own word rather than folding it into "clean". Verified
discriminating on two chains:

```text
natural-language-band : errors 0 · unresolved 0 · chain  clean — no errors, no unresolved calls
training-catalog-band : errors 0 · unresolved 1 · chain  UNRESOLVED CALLS — the chain compiles, but
                                                          each unresolved name answers `nothing` …
                                                          a verdict from it is partial, not a pass
```

**And the fix disproved my own diagnosis.** I had written above that this was "a lane seam, not a
meaning regression." With the seam gone and the chain genuinely clean, the band still answers
**196608**. So the seam was *masking* a real regression, not causing it. That regression now stands
exposed and un-rooted.

I did not root it. Three isolation probes of mine were malformed — the first never passed the
closure I had built, the second used names from outside the band's prelude chain, the third sliced
the band's grammar in a way that left `grammar` and `g-parse` unresolved. Each time I re-derived
instead of reading. The registered-versus-observed gap is verified; the mechanism under it is named
and left for a probe built by reading rather than guessing.

## The most surprising teaching

The hard-looking links were the ones already standing. Metal has a real door with the strong symbol
written; the JIT decision has hysteresis and a band; blueprints have a registry and an authority
cell; the KV cache is cut three ways. What was missing was the *cheapest* thing in the whole design —
one branch in one loop, needing no GPU, no weights, no new numerics. The chain broke at the place
that costs the least to build, and it broke there precisely because nobody had asked the loop to do
anything but speak, so nothing ever reported it missing. That is `probeshadow` from this morning,
arriving at the centre of the architecture rather than at its edge.

## Where discomfort became gold

The ask arrived as one long sentence with eight capabilities braided together, and the pull was to
answer it at that altitude — to say which parts sound reachable and sketch how they would fit. That
answer would have been fluent and worth nothing, because at that altitude every link reads as
plausible.

Taking it apart cost more and hurt in a specific way: the further I got, the clearer it became that
the honest reply to a question this ambitious was going to be *no, and here is the one line where it
stops*. Writing that felt like refusing the question. It was not — the decomposition is what turned
an unanswerable braid into one absent branch and a buildable cell, and the cell exists now because
the answer got narrow enough to act on. The braid could only be admired. The missing branch could be
built before lunch.
