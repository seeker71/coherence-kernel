# Core BML expansion — 2026-09-30

Codex, with native Form transformations and observations. The bulk lift used no per-file model or provider calls; model-generated implementation is not claimed.

At `9966ff0ec`, the tracked core census contains 28,852 definitions in 777 Form/BML units, excluding tests, fixtures and bootstrap artifacts. This movement lifts **8,436 definitions in 315 modules: 29.2%**. Their source falls from 3,090,940 to 2,787,165 bytes, a reduction of **303,775 bytes (9.8%)**. The compiler bootstrap, C seed and proof walkers retain their source.

The reusable lifter emits definitions, blocks, lazy conditionals and infix operators. Admission compares ordered dependencies and lowered structure, preserving loader imports outside executable sections, nested module scope and exact literal bytes. Closed semicolon construction is understood; shadowed literal constructors and added local bindings remain declined. Its witness accepts 14 cases and declines three binding changes. All 315 published modules pass the dependency/structure audit; all 33 modules from the preceding lift also satisfy the stronger scope comparison.

Model discovery shares `any?` and ordered `foldl` operations, with six observed checks for membership, traversal order, duplicate suppression and root exclusion. Its current documentation replaces historical commentary. The reserved-head generator emits the same BML arity table byte for byte; all 266 reserved names agree across the proof arms.

Live framebuffer observation: 128 membership queries over 1,024 paths returned identical values in 17 ms, versus 163 ms for the original traversal. Dispatch counts were 4,067,866 versus 2,755,354: dispatch count alone misses the repeated list-length work inside the old traversal. These are local observations, not general benchmark guarantees.

Verification: ground 42, binary freshness 31, native speech 32767, discovery checks 6/6, all twelve selected caller checks and all eight applicable landing checks 255/255. Choice-receipt learning returns 4294967295. The corrected contribution flow returns 33554431 from its cache in 1,385 ms. The full native CLI rebuild, source verification and installed bundle verification succeed with source identity `a8f7cffafdb03430d89d034f7ccd6ef0f6c6b53d27f4eea5ffbf8fc794ea57b5`.

The verified lifting method and loader-boundary repair completed local session learning through round 66 under `codex-core-lift-next-2026-09-30`, with nothing pending or refused; serving weights stayed unchanged. Retention and training do not establish improved model answers. The remaining source exclusions are the compiler bootstrap, CLI entry-block normalization, floating-literal spelling and an arrow-bearing identifier; their original source remains available. Cold whole-unit materialization remains distinct from warm native execution.
