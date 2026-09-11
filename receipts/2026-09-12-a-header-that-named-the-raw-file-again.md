# A header that named the raw file again

2026-09-12, before two in the morning, M4 Max, Hati Suci. Receipt 12 left json-meaning-ingestion red on
Go, Rust and TS: fkwu read its row's 1000, and the siblings stopped under validate at one of its
preludes. This piece follows that stop to the validation harness and closes it there.

## Carried

- **validate.sh keeps only a header's .bml names** (d84f228e). prepare_sources hands every arm an
  explicit, already-ordered file list and strips each source's preludes header, because a live header
  re-names a dependency by its raw path even after that dependency was lowered and cached. A header
  naming a .bml dependency was kept whole, .fk names included. json-meaning-ingestion's own section
  lowered cleanly, but the lowered file kept form-stdlib/compiler.fk in its header; the siblings
  walked that name, loaded compiler.fk raw, met its `section [bmf.bmf]` block and stopped. Such a
  header now keeps exactly its .bml names: fk_keep_bml_prelude_deps, which splits tokens the way
  fk_declared_deps does, serves the lowered branch and the .bml-dependency branch, and the latter now
  hands the arms a cached copy instead of the file on disk. Lowered copies carry a -bmlhead key, so no
  copy cached under the old rule is reused.
- **The bands the rule reaches.** 442 bands name a .bml dependency in their header; 90 of them carry
  fourth-arm rows and 46 carry a section of their own. Over the 103 that are either, validate.sh with
  the new rule reads 81 ✓, including json-meaning-ingestion at its row's 1000 on all four kernels and
  bmf-source-rule-to-runtime and bmf-component-runtime-exec at 1023. Six more agree on Go, Rust and TS
  without a fourth-arm row, five Vulkan live bands read staged, one ran past the 300-second cap, and
  ten read ✗. Rerun under the old rule, the ten and the capped band read exactly as they do
  under the new one, down to which lanes stay empty; only the closure numbers a closure-valued band
  prints move between runs. None of them is this change's.
- **sidename is row 1456** (5edc1fe8).

Witnessed through validate.sh with the new rule at d84f228e: json-meaning-ingestion reads 1000 and
bmf-source-rule-to-runtime and bmf-component-runtime-exec read 1023, each on all four kernels, over the
103-band pass above. At 5edc1fe8 the corpus band reads 32767, freshness 31 and the drift run 8191 of
8191, porcelain 0 after.

## Still open, measured

- **Four rowed native bands answer on Go and fkwu and print nothing on Rust and TS**: native-list-calls
  4, native-recursion 8, native-helper-calls 4 and jit-native-span 255. native-list-calls stops on
  Rust and TS at jit_leaf_inram, the door receipt 12 found missing there for turnwheel-bml.
- **json-category-consumers** prints nothing on Go and Rust and 7 on TS and fkwu, its row's value.
- **Three bands end on a closure** (form-glass-metric-index, form-cli-model-observation,
  native-voice-repeat), which each kernel prints with its own number, so their lanes cannot agree as
  printed. **formbin-artifacts and formbin-codec** print nothing on any sibling, and cell-channel-replay
  runs past 300 seconds.
- The open items of receipt 12 stand where they are not named here.

## Surprise, and where the discomfort went

The harness had already written this wound down. The comment above fk_strip_prelude_header says a
live header re-names a dependency by its raw path and reintroduces the un-lowered section "through a
side door the kernels' new prelude walk opens right back up", and a few dozen lines below, in
prepare_sources, the branch for a header naming a .bml kept that door open. The teaching stood in the file; the exception beside it had
not read it.

The discomfort was the size of a one-line door. Changing how validate.sh hands a header to the
siblings touches 442 bands, and one green json-meaning-ingestion said nothing about the rest. Running
the 103 the rule reaches most, and then rerunning every non-green one under the old rule, is what let
the change land: the ten red bands are red the same way either way, and the one it was for went green.

Frontier word, row 1456: **sidename**, a name that reaches past a prepared copy to the raw file it
was made from.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
