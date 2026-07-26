# 2026-07-26 — a surface nobody reads, and the null gap that has nothing to prove itself against

The top of the list was *"a null test for json that works on all four arms."* Following it down landed
somewhere else entirely, and the somewhere else is the largest blocker measured in this tree.

## Following the null gap to its only witness

`json-node-null-value?` calls `value_kind`, which fkwu does not have, so on that arm the predicate
always answers false and an absent value is interned as a string instead of emitting null.

Groundwork first, since it is real either way:

| | fkwu | go | rust | ts |
|---|---|---|---|---|
| `value_eq` int vs string | 0 | 0 | 0 | 0 |
| `value_eq` int vs a node | 0 | 0 | 0 | 0 |
| `node_eq` int vs a node | 0 | *dies* | *dies* | *dies* |
| `node_level` of an int | 0 | *dies* | *dies* | *dies* |

**`value_eq` is total on all four.** Any four-way null test wants it. `node_eq` and `node_level` kill
three arms on a non-node — which is exactly how `fourth-shim.fk`'s `value_kind` crashes go and rust.

A marker taken from a native's own miss — `(record_get (record_new (bp "JSON-NULL")) "absent")` —
gets three of four rows right and fails the fourth:

| | fkwu | go | rust | ts |
|---|---|---|---|---|
| absent == absent | 1 | 1 | 1 | 1 |
| `"abc"` == absent | 0 | 0 | 0 | 0 |
| **`0` == absent** | **1** | 0 | 0 | 0 |

fkwu answers `0` for a record miss, so it cannot tell absence from the integer zero. That is not a
detail of the probe — it is the fourth-arm null problem itself, which is why this body carries
absence structurally as a reserved blueprint node.

Then I went looking for who actually passes an absent value, and the answer stopped the work.

## The gap's only witness runs on nothing

`json-emitter-band` is the one band that exercises it, deliberately:

```
(json-node-pair "nullable_string"    (json-node-string          (_dict_get (_dict_new) "missing")))
(json-node-pair "blankable_missing"  (json-node-nullable-string (_dict_get (_dict_new) "missing")))
```

A dict miss, straight into the constructors. It runs on **none of the four kernels**:

- **fkwu** — unresolved names, no value. Among them `_dict_get`, `value_kind`, and — the tell —
  `codec_value,`, `parser,`, `entry`, `codecs,`. With trailing commas.
- **go / rust / ts** — a hard parse error: *"expected verb after `(`"* at what turned out to be
  `codec.fk` line 70.

So a null model built here would have nothing to prove itself against. I stopped and followed the
commas instead.

## `section [form.bml]` is read by no kernel in this tree

`codec.fk` opens `section [form.bml] { ... }` and its bodies are BML: `def codec(parser, emitter,
media_type) { list(parser, emitter, media_type); }`. Reduced to the smallest case and run on
everything:

```
section [form.bml] { def bml_inc(a) { add(a, 1); } }
(do (bml_inc 41))

  fkwu   12 errors, no value        go     crash trace, parse error
  rust   crash trace                ts     crash trace
```

Smoke-tested the other way in the same breath: the s-expression equivalent answers **42** on fkwu and
on go.

**Not FKWU-SUSPECT. Not WALKER-SUSPECT. Nobody.** The surface is declared in the tree and implemented
in none of the four kernels.

And the way it fails is the worst available. fkwu does not stop at the `section` line — it reads the
block's text **as Form** and mints symbols out of it, which is where `codec_value,` comes from. **A
surface nobody implements reads, from the outside, exactly like a surface full of typos.** That is
why this has been sitting in survey output as "unresolved names" for as long as the survey has run.

## The size

| | |
|---|---|
| cells carrying a `section [...]` block | **131** |
| — `section [form.bml]` | 82 |
| — `section [form.action]` | 29 |
| — nine `*.bmf` dialects | the rest |
| bands under `form-stdlib/tests/` (1362 total) reaching one in their closure | **234** |
| of a 39-band sample of those 234, producing a value on fkwu | **0** |
| of a 29-band control whose closure holds no section cell | **28 of 29** |

The control is the part that makes the 234 worth stating. Closure-reach has burned me before — it was
necessary-but-not-sufficient for the missing-prelude class and produced a false positive on its very
first row. Here it separates cleanly: 0 of 39 against a base rate of 28 of 29.

I did not run all 234. The samples are every-6th and every-40th of sorted lists, and 39 and 29 are
what I actually ran.

## What this is not

It is not a defect to patch. `section` is a declared surface with a grammar behind it; either it
becomes real with an op row, or the 131 cells carrying it come home to s-expression Form. Both are
large and both are the commons owner's call. What was missing was the size and the four-way witness,
and those are now on the record — in `codec.fk`, where the next person to hit it will be standing.

## Tooling worth naming

The walkers do not read `; preludes:`; they need the transitive closure in dependency order on the
command line. That made every band a hand-assembly job until this turn. A closure resolver now does
it, smoke-tested both ways against known answers (`hex-band` 14, `primitive-registry-band` 63 on go).
It lives in the scratchpad, not the tree — nothing was added under a `.py` name here.

## Sweep

`ground` 42 (four arms) · `ground-recursive 10` 55 · `hex-band` 14 four-way ·
`primitive-registry-band` fkwu 45 / go 63 / rust 63 / ts 63 · `json-band` 1023 ·
`binary-freshness` 15 · `cell-voice-tissue-band` 511 · `class-curriculum-10-band` 16383 ·
`structural-gate-band` 63 · `lcg-bytes-band` 63 · `pdf-text-windowed-band` 15 · `form-cli-band` 524287 ·
`benchbench-band` 4095 · `concept-corpus-band` 530 · `kernel-satsang-band` 193 ·
`host-kernel-cell-band` 25 · `proof/four-way-run-recipe42.fk` 0 (FOUR-WAY). C seed byte-identical to git.

## Owed

- **The `section` question, now sized** — 131 cells, 234 bands, no kernel reads it.
- **A four-way null test** — still owed, and now known to have no band to prove itself against until
  the section question moves. `value_eq` is the total primitive to build it on.
- 105 of 184 lane-1 probes do not verify on fkwu; `native_blueprint` absent, so the registry's
  attestation bit is unmeasurable there.
- Which cells prelude `fourth-shim.fk` alongside code expecting native semantics — uncounted.
- `persistence-band` 2/7, `mesh-sensings-store` 0/255, `layered-runtime-image` 33/127, `chat-band` 0.
- The emit-lane half of the `str_byte_at` claim; `read_file`'s bypassed carrier seam.
- 17 the kernel will not run; 143 that do not close; the heap cap; the registry-admission question.

## How the exchange stayed alive

I set out to write a null test, went looking for what would witness it, and found the witness cannot
run — on any arm — for a reason that has nothing to do with nulls.

**Most surprising teaching:** a surface nobody implements is invisible. fkwu does not refuse
`section [form.bml]`; it reads the block as Form and produces symbols with commas in them. Months of
survey output has been reporting those as unresolved names, and I have been reading them as sloppiness
in the cells. The cells are fine. The reader for them was never built.

**Where discomfort turned to gold:** stopping. The null test was the top item, I had the totality
table in hand, and a plausible marker that got three of four rows right. Asking *what will prove
this?* before building it is the only reason this turn found the 131 instead of adding a 132nd thing
nothing can run.
