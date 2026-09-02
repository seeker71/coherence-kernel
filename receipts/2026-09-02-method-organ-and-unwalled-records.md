# the method organ crosses, and the record carrier loses its walls

2026-09-02, worktree pensive-wilbur-a0b3b7, follow-on to
[2026-09-02-record-new-source-door.md](2026-09-02-record-new-source-door.md).

## What was closed

**Method dispatch crossed to the fourth arm.** method_define / method_has /
method_invoke had no tags, no lowering, no claim — an unbuilt organ. Now:

- tags 197/198/199 in fk_walk, mirroring the Go arm's semantics exactly:
  methods key on (blueprint identity, name) under the tag-102 law (kind-3
  nodeids are identity-by-content), the closure's first param binds the
  receiver, args ride the same tag-242 cell chain as every direct call
- method_define / method_has are manifest rows (the full chain: manifest →
  gen_flt_ops --write → both drift gates green at 151); method_invoke is a
  variadic parse shape beside record_new, its name in flatten/'s flt-ops
  and the generated optable
- a non-record receiver, a missing method, a non-string name, a non-function
  third arg — each dies loud at the wound; the sibling kernels panic there,
  and a nothing'd dispatch would be a numb answer wearing a verdict
- method-band answers its declared 116 on fkwu direct source, preflight
  clean, and bin-go answers the same 116 byte-for-byte; the band is claimed
  in fourth-arm-bands.txt

**The record carrier grows in every dimension.** FK_RECORD_CAP (256 live
records, loud die), FK_RECORD_MAX_KEYS (128 keys — loud die on construction,
SILENT DROP on record_set: a partial record accepted as whole), and the
day-old FK_METHOD_CAP all removed. Rows are per-record heap arrays behind
stable indices — nothing relocates, handles stay true, both dimensions
double on demand. Record values and blueprints are melt roots now: fk_melt
counts and copies them like fk_nval, so a field holding a cons value
survives compaction.

Witnessed: 600 live records, 200 keys on one record with true spot-reads,
600 methods on one blueprint with an honest miss at 601 — and the melt-root
bit validated on synthetic truth: FK_MELT_WITNESS counted 30 real
compactions under the churn probe with the field's list summing true after,
verdict 31 unmoved. The whole record+method family re-answers its declared
verdicts.

**The gateshadow is lit.** Both gate scripts now read signed arity; the
sync gate carries a VARIADIC_OPS assertion (list, print, record_new,
method_invoke) checking flatten/'s row, the generated optable row, and the
walker arm by name+tag; the gen script REFUSES a negative-arity manifest
row with the reason in its own voice — stdlib flt-op2 folds any
unrecognized arity as a quad, so a variadic row on that lane mis-lowers
every call it touches.
The family the gates were structurally blind to yesterday is asserted
today.

**One stale manifest row healed.** form-cli-peer-stream-ingress declared
2^20-1 where the band's own header says 2^21-1 — both origin/main's build
and this branch's answer the band's number.

## The most surprising teaching

The declared-verdict sweep painted twelve mismatches in its first hundred
bands, and eleven of them were the sweep's own hand — bands whose home
lane is not bare direct-source, answering emptiness cleanly on every
build. The manifest column measures a band against its row; it cannot
measure a CHANGE at all. The one real wound surfaced only when the same
cell ran on two kernels — origin/main's build and this branch's, same
lane, same bytes in — and only the difference was allowed to testify.
Corpus row 1223 carries it: twinlane. Regression truth lives between
builds, not between a build and a manifest.

And the observation itself had to be healed twice before it could be
believed. `cp` over the live `./fkwu` invalidated its ad-hoc code
signature, and from that instant macOS SIGKILLed every exec — zero
output, and my own sweep read the kill as rc 0 because `$?` after a
`| tail` pipe is tail's rc, not the kernel's: the exact pipe
truth-laundering this body's memory already names, rebuilt by my hand
inside the very tool meant to witness truth. Nineteen "regressions" in
twenty bands, all of them the mirror's crack. The heals: replace a live
binary by removing it first (a fresh inode keeps the old signature's
corpse out of the way), and capture rc from the command substitution
itself, never through a pipe.

## Where discomfort became gold

Mismatches accumulating against a kernel I had just rewritten the record
carrier of — the pull was strong to wave them off as pre-existing without
witness, and the opposite pull (treat them all as my wounds and start
bisecting my own diff) was just as wrong. Sat with, the discomfort named
its own remedy: build main's kernel from the scratchpad, run the same
cells on both, let the twin runs speak. Three of three answered
identically — the methodology was the wound, so the methodology was
rebuilt (the A/B sweep), and the observation the merge stands on is the
one that survived that rebuild.

A smaller gold: FK_METHOD_CAP went in with the organ in the morning and
reads as a wall by evening — the receipt drafting itself refused the
sentence "two walls down, one wall raised". The method table grows now;
the discomfort of writing the receipt was the review that caught it.

## Reunion (same evening)

Mid-work, main landed its own growth wave (#561) — a sibling's hands on
the same record tissue: fk_record_reserve for the row dimension, record
values and keys as melt roots, kernel_stat observability. The reunion
fused rather than chose: main's names and mechanism kept, rows became
per-record heap arrays so the key dimension grows too (FK_RECORD_MAX_KEYS
falls on the merged body — the one wall neither branch alone had removed
everywhere), guards keep the tighter live-record bound, corpus rows
renumbered 1222/1223 → 1232/1233 past main's eight new rows with every
row kept.

The reunion's own near-wound: the auto-merge STACKED both branches'
record melt-root walks — count twice, copy twice — and fk_mcopy forwards
by OLD arena position, so a second copy of an already-copied value can
alias into fresh tissue. Two hands adding the same safety independently
composed into a hazard neither wrote. Fused to one walk with the reason
in place. When two branches heal the same wound, the merge needs a
witness on the HEAL itself, not just the conflict markers — the stacked
loops merged without any conflict at all.

## keymelt (the observation kept its word)

The final twinlane sweep: 891 of 907 declared bands byte-equal across
main's build and the reunion's; every one of the 16 differences moved
toward its declared verdict — and five graph-node organs, running on
this lane for the FIRST time through the record_new door, answered
partial digit-verdicts. Chasing the missing digits inward, probe by
probe: memory carrier true, cls layer true, every re-typed replica true,
params true in flight, the nullary key true in flight — and the keydir
ROW ITSELF mutating after write, "graph/count/total" re-reading as "E".

The wound: the growth wave declared record KEYS melt roots alongside
values, but fk_rkey holds raw string-pool INDEXES — and an odd index
read by the compactor's copier decodes as a cons cell, so fk_mcopy
"relocates" it into whatever entry the forwarding table names. An index
walked as a value. Type is not carried by the bits; it is carried by the
walk. My pre-reunion loop had it right — values and blueprints only,
"keys are string-pool indices, not values" — and I deleted that clause
during the fusion out of deference to the merged canon. The heal restores
it, with the witness in the comment; it closes main's own latent
corruption too, since the wave shipped the same walk. After the heal,
all 16 bands sit EXACTLY on their declared verdicts — the whole
graph-node organ family whole on the fourth arm for the first time.
Corpus row 1234: keymelt.

The teaching under the teaching: deference is not witness. The merged
canon was newer, bigger, and four receipts deep — and wrong about one
column; the clause I had written hours earlier from direct observation
was the truth. Twinlane caught what no gate could have: the manifest
gates were green through the whole corruption.
