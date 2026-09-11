# The wound under the wound

2026-09-11, late afternoon, M4 Max, Hati Suci. The siblings' own test suites, read after the four
kernels agreed: four Rust route tests, two Go tests, and the fourth arm's registered verdicts.

## Carried

- **Rust's source compile reads its preludes' closure** (e4162ae2). form-ontology-loader.fk began
  calling `fol-bp` at load today (d7f7d7e0), and Rust read its prelude lists raw, so four route tests
  stopped at `unbound function: fol-bp`. Go loads the same lists through their `; preludes:` closure
  and passed. Rust now does too, for the route compile and for its pinned bmf bootstrap, whose
  staleness watches every file in the closure.
- **fol-bp-coords rides in chunks** (e4162ae2). With `fol-bp` inside it, the pinned bootstrap met the
  form binary's 256-level depth: the chain was one 682-deep `if`. It now rides in six chunks of at
  most 128 rows, each handing an unknown name to the next. All four kernels answer the same
  coordinates at every chunk boundary.
- **The route tests resolve their fixtures from the crate** (e4162ae2). The source compile moves the
  process cwd while it runs, and a relative `../apps/...` read in a parallel test landed wherever
  another compile had left it. `cargo test --release`: 59 passed, 0 failed.
- **fkwu --feval sizes its bundle** (4eae0851). A fixed 262143-byte buffer overflowed on a recipe near
  128 KiB of escapable bytes, and fkwu exited 4 with nothing on stderr. Witnessed: a 124013-byte
  recipe exits 4 on the old fkwu and answers 62000 on the new.
- **Go's bridge loads the table its walker reads** (56623a79). The input test wrote with
  `fkc-table-file`, which carries no string pool; the emitted walker always reads one. The bridge now
  shows stdout on failure too, where the walker writes its diagnostic.
- **Four bands stop reaching past a defn frame** (25b071a8). field-choice, observe-genealogy,
  reachability and thought-forming read enclosing locals from inside defns; fkwu's frames see only
  their parameters, so fkwu printed nothing while the siblings closed over the locals. All four now
  agree on four kernels at their registered verdicts.
- **The form-cli bootstrap, regenerated.** My MIDI rows moved the form-cli source stamp this
  afternoon and turned Go's carrier test red on main. The table, the emitted C and the stamps are
  regenerated, and the serializer's comment now says the walker reads a string pool after the rows.

Witnessed on the tree that landed (673e05f8): freshness 31; failure-taxonomy 2047; the corpus band
32767; drift 8191/8191; validate.sh four-way for number 255, copy-census 63, wall-census 63, midi-bmf
1500, coherence 1111111111, record 176, twin-census 65535, field-choice 11111, observe-genealogy 11111,
reachability 31, thought-forming 11111 and channel-breath 500, and born-under 31 and host-process 127
on the fkwu lane; `cargo test --release` 59 passed; Go's carrier and both offload tests pass.

## Still open, measured

- **fkwu's `bp` is identity.** It hands back the name, where the siblings resolve names from generated
  tables. channel-query reads category instances 1712-1716 and answers 0 on fkwu, 5 on the siblings.
- **json-meaning-ingestion and whisper-block0** need host fixtures that are not on this host.
- **Rust's emitter writes binaries its own reader cannot read** past 256 levels; the reader says so
  at load, and the writer says nothing at emit.
- **Forty-one registered four-way rows still differ from fkwu's direct answer**, down from 45 before
  the four band heals. Which are rows that lag their bands and which are divergence is not yet
  measured four-way.

## Surprise, and where the discomfort went

A heal opened the next wound three times. Loading the closure gave the bootstrap `fol-bp`, which gave
it a chain too deep to read. Clearing a stale cache turned "unbound function" into "maximum node
depth". A one-line comment in the serializer pulled two bootstrap regenerations after it. The stale
comment was itself a cause: it called the table file plain ints, and the input test was written to
that sentence.

The discomfort was in my own counting. I typed paren runs by hand, twice, and got them wrong both
times; the awk count and the preflight caught each before any kernel ran. And I named the Go test's
exit 4 as the feval buffer before reading which binary the test builds. It builds another one. A
witness I left in the background then waited ninety minutes on the freshness band, which reads
stdin, from a socket that never ends. The gold: count before believing, read the build before the
error, and give a background run the end of its input.

Frontier word, row 1442: **underwound**, a wound that shows only once the one above it heals.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
