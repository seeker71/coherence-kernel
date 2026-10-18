# Core BML surface — 2026-09-30

Codex, using native Form transformations and observations; no per-file model or provider calls.

The tracked core inventory at `bbff611c7` contains 28,825 definitions in 776 Form/BML units, excluding tests, fixtures, bootstrap artifacts and local caches. The native cursor/lexer census excludes quoted definitions. **3,416 definitions in 33 units now use high-level BML: 11.85%.** Those units total 856,538 bytes, down 174,316 bytes (16.9%).

`form-source-lift` preserves public names, grouping, evaluation order and comparison contracts. Admission compares lowered trees, with string escapes and runtime boolean spellings normalized; boolean shadowing and compiler-added construction/bindings are declined. The compiler's bootstrap closure stays bootable. HTTP membership, validation and joining share stdlib abstractions. C and proof walkers are unchanged.

Observed:
- All 14 landing checks: 16383, refused=0. Ten behavioral checks passed: HTTP 536965066; JSON 1023; tokenizer 65535; ingress 131071; peer ingress 2097151; contribution turnwheel 33554431; program-image capability 16777215; knowledge census 1048575; JIT strings 511; channels 8388607.
- Lift contracts: eight accepted cases and three declined construction/binding changes. Warm reuse preserves the entire live registry, including its terminal classifier.
- Live kernel panel: the 256-definition lift uses 171,894,319 dispatches versus 327,281,367 before removing duplicate discovery lowering; latest observation 654 ms. The 1,024-row HTTP join preserves 8,105 bytes and measured 1 ms versus 15 ms. These are local observations.
- The full source-sealed CLI was rebuilt, verified and installed: `a8b230c7d72a04920fa4a3ba50a9699904b9ed7d1457044f1eeeae9c8063f88b`.
- Verified teaching retained locally: learned round 64, pending=0, refused=[]; serving adapter unchanged.

The direction remains reusable Form-native abstractions with observed behavior. This is a grammar and shared-library lift; it does not establish optimality of every algorithm. Cold whole-unit compilation remains distinct from warm execution.
