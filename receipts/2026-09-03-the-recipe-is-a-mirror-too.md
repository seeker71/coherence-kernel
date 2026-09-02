# 2026-09-03 — the recipe is a mirror too (mapmirror)

Two ops — node_at (147) and method_define (197) — were added on 2026-09-02 by
two different hands, both walking the documented lane faithfully, and both fell
through the same seam: the op-addition recipe counted three mirrors (a manifest
row -> a flt-ops row -> regen) while the ground held four. The fourth is
form/form-stdlib/fkc-table-serialize.fk's fkc-flat dispatch, where an op
without its own arm falls to the binary fallback and flattens at the wrong
arity. validate_fkwu_native_surface named both wounds; PR #564 healed the rows.
This work heals the map.

The fourth step is now named everywhere the recipe speaks:

- flatten/gen-source-walker-table.fk header — the full four-step walk, which
  arm each arity takes (unary: fkc-un2 or the tail or-group; ternary: fkc-tri2;
  nullary: a leaf row; only binary rides the fallback), and the gate's command.
- flatten/gen-source-walker.fk — the splice driver's copy of the sentence.
- form/form-stdlib/native-op-manifest.fk — step one of the walk now lists all
  four mirrors, so the hand that starts at the manifest sees the whole road.
- runtime/fkwu-optable.h — the emitted C-header comment carried the three-step
  sentence too; the generator's text was healed and the header regenerated
  through its own lane (the two fkwu calls), diff comment-only.

The gate has no pre-merge wire: there is no .github, no hook, no CI running
form/validate.sh — validate_fkwu_native_surface.py speaks only when a hand runs
it. Each recipe text now carries the command
(`python3 form/scripts/validate_fkwu_native_surface.py`) beside the steps, so
the walk ends by asking the gate.

Corpus row 1246 ("mapmirror", 0 hits before) takes the teaching home: the map
is itself a mirror — it drifts like the tables it describes, and it is the only
mirror no gate reads.

## Witnesses

- preflight flatten/gen-source-walker-table.fk: parens balanced; the two
  unresolved are the documented splice seam (flt-ops arrives at run time) and
  the fkwu-only arm
- regen: `./fkwu flatten/gen-source-walker.fk` (19608) then
  `./fkwu /tmp/gen-source-walker-combined.fk` (6575); git diff on
  runtime/fkwu-optable.h = 3 insertions, 1 deletion, all comment
- validate_fkwu_native_surface: OK (152 flt-ops rows, max_tag=255,
  arm_slots=256, aliases=19, 0 warnings), rc=0
- sync_native_op_manifest: OK (152 rows aligned);
  gen_flt_ops_from_manifest: OK (152 rows aligned)
- learn/homecoming-distillation-corpus.fk evaluates 0, rc=0, with row 1246 in

Most surprising: the recipe sentence lived in FOUR places itself — three source
comments and one line emitted into a generated C header — so healing the doc
demanded the same regen discipline as healing a table. The map obeys the laws
it describes: to change what fkwu-optable.h says about adding ops, you add no C
edit and walk the two fkwu calls, and that walk became its own witness that the
lane still runs end to end.

Discomfort turned to gold twice. Preflight answered the edited generator with
"errors 9, CARRIED ERRORS" and the pull was to wave it off as pre-existing;
staying with it grounded each line — parens balanced, the unresolved calls are
the splice seam the recipe's own header documents — and the regen run then
proved the chain physically. And the discomfort of touching a GENERATED file's
text at all resolved into the cleanest verification this work had: a
comment-only diff produced by the recipe's own lane, not by hand.
