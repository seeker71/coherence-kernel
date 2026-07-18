# Thirteen real-life text-to-program workflows execute across twelve carriers

**Verdict:** thirteen operational English sentences were detected against the
complete 10,000-anchor text surface with source and full-sense evidence.  Their
computed workflows were generated and recovered in all thirteen programming
language lenses.  Every permitted non-Python carrier executed every workflow:
156/156 outputs matched the Form evaluator.

## The real workflows

These are bounded operational examples, not concept labels printed through
different syntaxes:

| Concept | Observed-style input | Computed intervention |
|---|---|---|
| inventory 8180 | shelf counts `14,7,2`, reorder below 5 | `reorder:1` |
| water 377 | sample ppm `2,8,3`, unsafe above 5 | `unsafe:1` |
| route 2474 | stops `1,2,3,2` | `loop-found:1` |
| camera 959 | consent flags `1,1,0` | `recording-blocked:1` |
| schedule 2430 | appointments 09:00–10:00 and 09:30–10:30 | `conflict:1` |
| temperature 3187 | readings `3,7,9`, ceiling 8 | `temperature-alert:1` |
| smoke 1184 | sensor states `0,0,1` | `evacuate:1` |
| battery 4021 | charge `82,17,9`, floor 20 | `charge-two:2` |
| delivery 2746 | minutes `18,35,22`, promise 30 | `late-delivery:1` |
| payment 3999 | cents `1200,900,1600`, budget 3000 | `over-budget-cents:700` |
| medicine 1762 | dose log `1,0,1` | `missed-dose:1` |
| traffic 2129 | segment minutes `12,27,19`, ceiling 20 | `congested-segment:1` |
| energy 1244 | units `4,8,6,9`, cap 25 | `over-cap-units:2` |

The workflow engine has six real operations: count below, count above,
duplicate detection, missing/zero detection, summed excess, and interval
overlap.  Target stdout includes the source concept ID, selected action, and
computed metric.  No expected stdout is embedded in generated source.

## Text evidence before generation

`concept-real-life-workflows-13-text-live.fk` passes each exact surface and its
full sentence through the ambiguity-preserving detector.  It retains every
same-surface concept candidate, locates the intended anchor without discarding
the others, and attaches exact F/W/D/C/G provenance plus the complete pinned
WordNet 3.1 sense list and transparent context ranking.

The live audit tuple was:

```text
[workflows, occurred, targets-with-senses, candidates, total-senses,
 sourced, context-ranked, full-sentence-candidates, complete-evidence]
[13, 13, 13, 13, 74, 13, 13, 11, 13]
```

For example, “Warehouse inventory fell to two units, so reorder the low shelf
today” produced eleven complete sentence candidates.  The workflow does not
pretend that sentence matching resolved WordNet polysemy: each of the thirteen
target analyses remains explicitly `context-ranked-not-resolved`.

## Generation, recovery, and actual execution

`concept-real-life-workflows-13-generation.fk` generates one complete batch in
each lens:

```text
Python, JavaScript, TypeScript, Java, C, C++, C#, Go, Rust,
Ruby, PHP, Swift, Kotlin
```

All 169 workflow/language surfaces were nonempty and exactly recovered.  The
`FKR13` marker carries language and a complete workflow-data fingerprint, but
the marker alone is not trusted: recovery regenerates the source and requires
byte equality.  A one-byte suffix mutation is refused.

Python was generated and recovered only; it was not executed, per explicit
user policy.  Form wrote one 13-row batch for each of the twelve permitted
carriers under `/tmp/coherence-crl13`, compiled or interpreted it, and compared
all target stdout with the independent Form evaluation:

```text
[languages, available, written, recovered, output-lines, exact, output-hash]
[12, 12, 12, 12, 156, 12, 684301941]
```

One real JavaScript carrier run printed:

```text
8180:reorder:1
377:unsafe:1
2474:loop-found:1
959:recording-blocked:1
2430:conflict:1
3187:temperature-alert:1
1184:evacuate:1
4021:charge-two:2
2746:late-delivery:1
3999:over-budget-cents:700
1762:missed-dose:1
2129:congested-segment:1
1244:over-cap-units:2
```

## Gates

```text
presence/tests/concept-real-life-workflows-13-band.fk
  fkwu 4095   Go 4095   Rust 4095   TypeScript 4095

presence/tests/concept-real-life-workflows-13-text-live-band.fk
  fkwu 1023

presence/tests/concept-real-life-workflows-13-live-band.fk
  fkwu 511
```

The pure band checks all thirteen computed metrics, all 169 generated and
recovered surfaces, marker integrity, mutation refusal, and the full flattened
input structure.  The live text and carrier bands use the pinned local corpora
and actual host toolchains respectively.  No Python was used.  Generated C is
only a target program; `runtime/fkwu-uni.c` was not modified.

## Honest edge

These are real deterministic operational rules over concrete measurements,
not arbitrary natural-language-to-program synthesis.  Sentence context ranks
but does not claim to resolve the 74 WordNet senses.  The examples also do not
contact warehouse, clinic, payment, or sensor production systems; their inputs
are explicit observation fixtures, which is the honest floor for repeatable
cross-language execution.

; witnessed: 2026-07-18 -> text 1023; generation 4095 four-way; carriers 511
