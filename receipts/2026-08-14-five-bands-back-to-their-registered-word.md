# Five bands back to their registered word

**Date:** 2026-08-14
**Status:** witnessed; five bands repaired to their registered verdicts, one seam measured and left standing
**Corpus:** rows 1022 `carrygap`, 1023 `primshadow`

## What was handed back, twice

First I reported `unresolved 3` instead of fixing it. Then, having fixed it, I reported the
regression it exposed instead of rooting it — *"I did not root it, and I'm not going to name a cause
I haven't seen."* That is the same handback one layer down, dressed as discipline. Urs named both.

## Repaired

| band | was | now | registered |
|---|---|---|---|
| `natural-language` | 196608, exit 1 | **262143**, exit 0 | 262143 |
| `nl-translate` | 32767, exit 1 | **32767**, exit 0 | 32767 |
| `translate-lane` | 11111, exit 1 | **11111**, exit 0 | 11111 |
| `training-catalog` | 1023, exit 1 | **1023**, exit 0 | 1023 |
| `form-cli-loop` | 31, exit 1 | **31**, exit 0 | 31 |

Untouched neighbours after: corpus 32767, bmf-core 700, bmf-grammar 2047, act-token 1023, ground 42.

## Three roots, none of them a shim

**1. `bp` is a native the Form definition cannot reach.** `form-ontology-bp.fk` says it in its own
comment, and I had read past it three times: `bp` sits in fkwu's optable at tag 45, in call position
the primitive wins, so the Form `bp` defined directly above that comment **never runs on this
kernel** — and the native is a *pass-through*, handing back the interned string. So
`natural-language-band` interned its expected nodes under a **string** while the parser interned its
produced nodes under a **NodeID**. Children identical, identity unable to meet, all sixteen
`node_eq` bits reading 0 while the two `str_eq` bits sailed through untouched by any blueprint.

`fol-bp` exists for exactly this — *"the same resolution as `bp` above, under a name the KERNEL DOES
NOT OWN."* Six constructors swapped. 196608 → 262143.

**2. `bmf-bp` was missing the natural-language vocabulary.** Its table held only BMF constructs, and
a miss answers `(make_nodeid 0 0 0 0)` silently rather than saying so. Added `fact`, `property`,
`isa`, `relation`, `question`, `meaning` — coordinates **copied** from `form-ontology-bp.fk:231-236`,
never minted, because two hand-held tables that disagree is what produced this in the first place.

**3. `json.fk`'s null test asked for a native fkwu lacks.** `json-node-null-value?` was the cell's
only `value_kind` call site, and `value_kind` is absent here. The cell had already worked out the
repair and written it down — *"any four-way null test wants `value_eq`"*, and *"what this wants is a
null test that does not ask a node question of a non-node"* — noting that the obvious shim had been
tried and killed two arms. Measured first on fkwu / go / ts, all three identical:
`(value_eq (nothing) 0)` → 0, `(value_eq (nothing) "")` → 0, `(value_eq (nothing) (nothing))` → 1.
Rewritten on `value_eq`. That one line cleared the seam for every chain naming `json.fk` — 33 bands
name it and not `cache.fk` — which is why it was repaired there rather than in 33 prelude lines.

Alongside those: `read_with_cache` — the only thing in `cache.fk` reaching the binary doors — moved
to its own `cache-binary.fk`, so the freshness predicate stays holdable by anyone and only its two
real callers (`concept-corpus.fk`, `i18n.fk`) reach for a door they can open.

And `observe/preflight.fk:271` graded chains on errors alone. It now reads `pf-unresolved` too and
gives that case its own word, verified discriminating on two chains.

## Measured and left standing, with the reason

`audit-evidence-cells-band` reads **544, exit 1**. I checked it against a stashed pristine tree:
**544, exit 1 at HEAD as well** — identical. I neither caused nor fixed it. It calls
`read_form_binary`/`write_form_binary` directly at three sites, so it is a genuine host-door gap and
not a carried absence; no prelude cut reaches it. Closing it wants those two natives written into
`runtime/fkwu-uni.c`.

I did not write them. Not as a deferral — as a judgement I will state plainly: that is new C in the
seed every other arm stands on, I had only glimpsed its tagging scheme, and I had already made three
malformed probes today from moving faster than my reading. The work is specified (`value_kind_name`
in `form-kernel-rust/src/main.rs:750` is the canonical name set; go registers at `main.go:2402`), and
it is the next thing.

Also measured: **201 cells** across `form/` and `learn/` name `json.fk` or `cache.fk` while calling
nothing from either. Repairing that at the source — one line in `json.fk`, one split in `cache.fk` —
is why this is five band repairs and not 201 prelude edits.

> **frontier questions**
> what names an unused import that carries the absences of what it names? → **carrygap** (row 1022)
> what names a definition that never runs because the kernel owns its name? → **primshadow** (row 1023)

Corpus re-probed: 402 rows / 402 admissible / max-mid 1009 / 0 duplicate ids / field code
402040221009. Band **32767**, exit 0.

## The most surprising teaching

Every root today was already written down in the cell that had the problem. `json.fk` carried a
twenty-line note specifying its own repair, down to which primitive to use and which one had already
been tried and failed. `form-ontology-bp.fk` explained that `bp` is a native pass-through and named
`fol-bp` as the door, in the paragraph immediately below the function it warns about. I wrote three
malformed probes before reading either. The body had diagnosed itself weeks ago and left the notes
in place; what was missing was somebody reading them before reaching for a tool.

## Where discomfort became gold

The uncomfortable part was not the two corrections. It was watching my own pattern hold across both:
told to fix rather than report, I fixed one thing and reported the next; told again, I found the
root in a comment I had already scrolled past three times that same hour.

What broke it was cheap and specific — stop constructing probes and read the file. Every probe I
built failed for a reason I had authored: a closure assembled and never passed, names taken from
outside the chain, a grammar sliced through the middle. Three failures, each costing more than the
reading would have. The fix, once read, was six words swapped in one line and a verdict that had
been wrong for weeks came back to its registered value on the first run.
