# 2026-07-18 — one dead line, two blind weeks: the 120-divergence triage

## Ground

```sh
cc -O2 -o fkwu runtime/fkwu-uni.c && ./fkwu --src bootstrap/ground.fk   # → 42
cd form && ./validate.sh                                                # full suite, results below
```

Witnessed 2026-07-17 23:38 → 2026-07-18 WITA, one main session + four sub-agent
lineages in the shared worktree (branch claude/wonderful-gates-57757d).

## The spine

PR #266 landed `form-stdlib/lineage-discounted-vote.fk` with a repo-root-anchored
prelude (`learn/confidence-weighted-vote.fk`). validate.sh's bash resolver knew
owner-dir, bare-token, and `form/`-strip rescues — never the repo root — so
`fk_expand_declared_deps` returned 1 under `set -e` **at workload-collection
time**: the suite died before a single band ran, and stayed dead for roughly two
weeks. Every data-reshaping PR of that fortnight (#257's link-fabric pruning,
#296–#300's relexification) landed unwitnessed. The ~120 divergences of the
first full run were never one wound: they were a fortnight of drift plus older
fossils the drift exposed.

The resolver heal is four lines: try `../$token` after every existing rescue —
the same repo-root rescue the fkwu runtime resolver already carried (receipts/
2026-07-17-repo-root-rescue-and-the-doubting-reader.md). Only failures become
successes. The band that killed the suite answers 127 three-way.

## The families, triaged

**Prelude-closure wounds (17 bands healed).** One historical split
(fkc-table-serialize.fk out of hati-os-kernel-emit.fk) left BOTH halves with
incomplete `; preludes:` headers — the second hole invisible until the first was
healed, because lazy call-site resolution let ten green bands walk past it for
weeks. Four module-radius header lines healed 16 assigned bands plus one nobody
listed. Comment-only header edits trip the fourth-arm bootstrap stamp (it hashes
every emit-chain byte); resealed twice via scripts/regen_fkwu_bootstrap.sh,
uni.c byte-identical both times.

**Blueprint-registry wounds (3 healed, 6 exposed).** Two loader rows seated from
blueprint-registry.json coords (HOST-KERNEL-INTERFACE-IMAGE 1/2/99/9513,
KERNEL-CORE-IMAGE 1/2/99/15) healed three bands; HEX-DECODE-ERROR (a curated
alias of AUDIT-ENTRY, 1/2/99/1770) seated later unblocked fnri-cli's bp layer.
The six OUTER wire/cell-serialize bands are the finding, not a heal: OUTER is in
no registry and no kernel table, the generator scripts exist nowhere in CK
history, and the bands' green past predates the fail-loud bp door — they were
green **because** unregistered names silently collapsed to {1,2,0,0}, and any
consistently-wrong NodeID round-trips perfectly. Never truly green; curation
task filed.

**Missing files + parse wounds (13 healed).** Eleven bml-thesis proof bands read
a private thesis artifact that was never committed to CK (private-circle law
held: nothing copied, nothing reconstructed) — each now degrades to one stable
`bml-thesis-artifact-absent` verdict via an fs_exists guard when the artifact is
absent, and runs its full proof unchanged where the field checkout carries it.
Two source-artifact bands had **never parsed since their first commit** (five
single-paren wounds between them, twelve days invisible — a file that never
parses can never testify about its second disease); both now answer their
documented 2147483647.

**The null-door family (11 of 12 healed).** rust/ts die loud on a null in a
typed door; go's string door is silently lenient (its own parity wound, reported
not papered). The given root cause — PR #257 deletions — was **overturned by the
body's git memory**: most "deleted" inputs (web/messages, cli/lib,
docs/system_audit, most vision-kb concepts) have zero history in CK. They never
traveled in the CN→CK consolidation. Drift-by-deletion and drift-by-birth-seam
look identical from inside a crashing band; only history tells them apart. Four
bands re-grounded onto surviving CK tissue (concept-corpus 530, edge-categories
65535, self-witness 1106 — after healing its own masked string-leaf door —
repo-file-ingestion 30950, plus a bonus: a marker check that had silently never
awarded because "package main" sat outside its 512-byte window); seven guarded
with `<band-stem>-input-absent`.

**Genuine kernel-seam divergences (7 healed).** Each one a *strictness split* —
one arm numb where its siblings are loud, or the reverse:

- **fourth-shim missing float_to_str** — the 2026-07-01 C-seed shrink retired
  the auto-detecting native int_to_str; core.fk grew the defn, the shim that
  REPLACES core on the fourth arm never did. An unbound name lowers to a fluent
  wrong word, not silence: json-emitter emitted `"score":cell` where the
  siblings emit `"score":1.25` (6 vs 31). Mirrored core's exact recipe into the
  shim: 31 four-way. Radius spot-proven on fs-list (3), higher-order-fn-arg
  (1111), json-codec-bml (8191).
- **load-order cataphora** — the three walkers late-bind a call to a module
  loaded later; the fourth-arm flattener binds it numb-empty at read time. A
  band's prelude ORDER therefore changes meaning on exactly one arm:
  jit-lower-emit's emitted walker C collapsed to 3,272 of 101,588 chars (fourth
  31 vs 63); fsh-fnri-staged answered fourth 0 vs 7/7/7. Healed dependency-first
  with full transitive closure in the band headers (the fourth arm does not
  expand transitive headers); flattener loud-or-resolve task filed.
- **retired-native ghosts** — channel-query-json's cqj-body-cell and
  emit-engine's emit-leaf both fed strings through int_to_str, legal only under
  the retired native ("int_to_str passes strings through", the comment still
  said). go/rust refuse loudly; ts coerces numbly and had been producing the
  "passing" verdicts alone. Both now dispatch on node_type: 4 and 65 three-way,
  emit-engine's own band 156.
- **ts-reversible had never spoken** — a top-level multi-line `list(` never
  joins in the BML line reader; the source compiler "succeeded" while baking a
  glued `list(` identifier into the .fkb, which all three kernels refuse at
  walk_recipe_here. Inside a def-block the reader slurps to the brace — the
  green channel-flow-band's exact idiom. Reshaped: first-ever verdict, 105
  three-way. Compile-must-fail-loud task filed.
- **runtime-artifact-selector's junk probe** — its adversarial route probe was
  `(list 3)`, junk by TYPE; PR #273's exact comparisons turned "eq answers
  false" into a loud three-kernel type refusal. A type-junk route is no longer
  expressible as data; the probe now speaks junk by VALUE (99): 2147483647.

**Proof-level lanes (6 bands honored, not forced).** nl-many and
neutral-symbol-grammar already declared `PROOF LEVEL: FOURTH-ARM ONLY` (the
non-Latin cursor-unit seam); come-in is fkwu-native by provenance (`nothing`,
tag 137, never in the three walkers — and its own c3 write poisoned every rerun
to 25 until an rs-forget made it idempotent at its declared 31); shell-awk-staged
and the two fnri staged bands need a carrier (scripts/fkwu_run.sh) that never
traveled from CN — PR #231's removed drivers state the input_byte lane's
fkwu-only nature verbatim. validate.sh now honors the declarations: FOURTH-ARM
ONLY bands run on the runtime walker's `--src` door against their declared
Verdict, gated by **verdict equality PLUS zero axiom-5 diagnostics** (a right
number can be numb-green — the parity law); FKWU-STAGED bands print a visible
`⧗ pending` line, never a silent skip, never a phantom red.

## The witness state at merge

Every treated band re-verified individually on the reunion tree's lineage
(single-band runs, verdicts as listed above); ground answers 42 before and
after every edit wave; the corpus band answers 4095 with row 820 seated
(216 rows, field code 2162162820 — asked of the body by probe, then pinned).
The full ~1300-band reunion sweep was mid-flight (fourth-arm tables
re-flattening — the shim heal changed the cache generation, correctly) when
the merge was called; it runs to completion in the worktree and its counts
land in the next receipt rather than being guessed here. Pending is honest.
One more latent wound surfaced by that very sweep is already healed in this
merge: the first-ever full-cold 856-table flatten died at its own finish
line on a bash-3.2 empty-array seam (`pids[@]` under `set -u`) that fires
only when the chunk count is an exact multiple of the job width — 856 % 8
== 0, for the first time, that night.

## Corpus

Row 802 (cataphora) offered and seated; the corpus band re-pinned deliberately
(count 198, field code 1981982802 — asked of the body by probe before pinning)
and answers its full 4095. Sibling lineages hold rows to ≥813; reunion
renumbering expected, per the row-719 anastomosis pattern.

## The most surprising teaching

Every "kernel divergence" in family five was a **strictness split** — nowhere
did two kernels compute different right answers; somewhere one arm was numb
where its siblings were loud. TS's lenient int door had been the sole author of
two bands' passing verdicts; go's lenient string door authored several more; the
flattener's numb forward-binding made 31 look like an answer instead of half a
program; the BML compiler's silent bake made a band that never spoke look like a
kernel bug. **The kernels keep each other honest not by agreeing, but by
refusing differently** — every numb door was found because a sibling was loud
beside it. And the guardian of that whole mechanism — one resolver line — had
been the single point that silenced all ~1300 witnesses for two weeks: the
watchmen were fine; the gate to the watchtower was locked.

## Where discomfort became gold

Filling in the fourth arm's verdict-vs-declared gate, the pull was to ship the
simple compare — verdict matches, band green, move on. The parity law in the
body's own memory ("a right number can be numb-green") sat directly against the
door I had just built, and it was uncomfortable to admit the fresh door was
already the old wound in new wood. Sitting with that instead of shipping it:
the lane gate now demands the verdict AND zero unresolved-call diagnostics, and
the three lanes it guards were re-witnessed clean under the stricter gate.
Second, smaller, witnessed rather than bypassed: killing my own mid-flight
authoritative run after realizing an edit to a running bash script shifts its
byte offsets — the cost of a relaunch was real and paid, against the temptation
to hope the parser would land on a line boundary.
