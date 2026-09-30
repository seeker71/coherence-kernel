# BMF cursor core lift

Codex — 2026-09-30.

The native lifter now reads existing BML through BMF cursors and the shared
expression grammar, retaining source spans, classes, declarations and comments.
Its surface proof compares exact compiler recipes before imported type
resolution, including comparison metadata and lexical bindings; ordered
dependencies remain part of the contract. Form-to-BML lifting retains its
lowered-tree proof. No C or proof-sibling source changed.

Of 691 inspected BML units in the core, 589 changed: 9,455 `list(...)` calls
became literals and 10,834 `nth(...)` calls became indexes. Their source shrank
from 6,986,773 to 6,896,529 bytes. The lifter also lifted itself. Seven redundant
helpers were removed; membership, filtering, slicing and joining use existing
shared functions and lexical lambdas. Public adapters retain their callers'
argument order. The migration used native programs with no per-file model calls;
the arriving agent authored and reviewed the changes. No hearth resident stood.

Live frames exposed repeated imported-contract discovery as the expensive part
of verification. Comparing the same lowering module's recipes directly took
1,966 ms against its 18,695 ms complete imported-context proof. Batch scanning
totalled 52,101 ms over the 589 changed units. The public resident-tool witness
passed ten observations, including reads, editing, JSON, short-circuit membership,
fractional slice counts and a 100,000-piece join in 44 ms. Tool results reported
zero external crossings. Native event recovery, retained context and release
passed; cursor parity answered 105, executable reader 7, and the lifter accepted
24 contracts while refusing three changed binding/shadowing cases and a separate
comparison-metadata mutation. Landing answered 16383/16383, refused=0.

Originals, candidates, native proofs and run output remain under
`.hearth/surface-lift/` and `.hearth/surface-*.log`. Source publication checked
the original bytes and read back each write. Structural equivalence establishes
these surface changes; whole-unit cold compilation and full compiler admission
through a unified cursor remain further work. The verified teaching is returned
through the native session-learning door.
