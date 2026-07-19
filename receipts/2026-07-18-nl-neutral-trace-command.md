# One command, one legible NL → neutral → NL trace — second pass: embodied

The ask that shaped the first pass: the concept-query trace should be a single
command whose response carries enough pointers that any follow-up inquiry is
obvious — native first, oracle only as fallback or comparison. The ask that
shaped this second pass: less demo, more real; fewer gaps deferred; the trace
should teach how this kernel is unique, and the visit should leave the place
better than it was found.

## The command

```
./fkwu --src cognition/tests/nl-neutral-trace-live.fk
```

Default lanes: German `Wasser` and Indonesian `air`, plus the three-tongue
pivot exemplar. A caller's own question is one staged line
`<asserted-locale> <target-locale> <query text>` at
`.coherence-network/nl-neutral-trace-query.txt`; an oracle's answer staged at
`.coherence-network/nl-neutral-trace-oracle-answer.txt` is scored against the
native answer. (Direct-source Form reads no argv and the op table carries no
env native — the staged files are the Form-native doors, written and read by
the same organs.)

## Closed this visit (was deferred, now embodied)

- **The rag lane is real.** The index is written to the healed home
  (`.coherence-network/rag-index/index.jsonl`) and the answer comes back
  through the production `ra-grounded-hit-at` path — `file_size`, `read_file`,
  stream ranking over disk bytes. Identity is content-addressed: `key`,
  `persisted_source_sha256`, and `answer_key` are the SHA-256 of the exact
  committed row bytes. The `@10.3.99.<row>` coordinate is a declared,
  documented file-substrate scheme, reproducible by any visitor from the
  bytes — no longer demo coordinates on an in-memory string.
- **German is seated on the pivot.** `cognition/nl-pivot-de.fk` adds the
  tongue exactly as `nl-translate.fk` teaches — one grammar, one column, the
  pivot untouched: `die quelle ist nativ` ↔ `the source is native` ↔
  `sumber adalah asli`, all four de-pairings live. Articles der|die|das cross
  an alt pattern on encode and are restored from the de column on decode —
  gender is column dress, never pivot content. The sovereignty witness is
  node identity: the same meaning from three surfaces interns to the SAME
  cell, checked by `node_eq`, not by claim.
- **Per-rule BMF counters are measured.** `nnt-rule-trace` runs every named
  rule of a grammar against the live input and counts ok/fail — the report
  shows `s:ok prop:ok (ok 2, fail 0)` for the exemplars and the honest
  `(ok 0, fail 2)` when a bare label meets the property grammars.
- **The comparison ledger is memory between visits.**
  `.coherence-network/nl-neutral-trace-ledger.txt` grows one row per lane per
  run (witnessed 1 → 2 → 3 across two runs); when an oracle answer is staged
  the row carries its agreement score (witnessed 0/100 for `Wasser` vs
  native `Feuer`, 100/100 for `Feuer` vs `Feuer`).
- **Four of the five loud prelude lines are healed** by preluding the shims
  the repo already wrote (`string-byte-fold-src-shim.fk`,
  `record-src-shim.fk`) and by releasing `json.fk`/`cache.fk` from the chain
  (they were never needed by this path). Exactly ONE loud line remains —
  `form-ontology-loader.fk`'s `walk_recipe_here`, which that file keeps loud
  on purpose ("recorded, not shimmed"). The trace names it so the nonzero
  exit puzzles nobody.

## Two carrier boundaries, learned and left as shape

Fixing the ledger surfaced two real `--src` boundaries, now documented at the
site in `nl-neutral-trace.fk` and honored as code shape:

1. a `(do (let ...) ...)` nested inside a **let value** loses its bindings;
2. a host-read string returned as a let-bound **name through an if branch**
   loses its value category — the same seam `rag-ask.fk` documents at
   `ra-grounded-hit-at`. A direct call in branch position survives.

Both were found by minimal probes (`a=0 b=0 c=38`, then `d=38`), not by
guessing. Note for a later probe: `ra-query-text` and `ra-read-index` in the
stdlib carry the `(nil? raw)`-then-branch shape on their staged-file lanes;
they are proven on the sibling kernels, but the fkwu `--src` staged-query
path deserves the same minimal probe before it is trusted blindly.

## Third pass: the whole concept body behind every query

The same command now serves, for ANY of the 10,000 anchors in any of the 13
seats: the concept's WordNet 3.1 description; its full attributed sense set
(34,244 senses, polysemy opened, never hidden — count plus further glosses);
the complete 13-seat label row (all scripts, absent seats shown as `-`); the
local oracle dictionary row (the 120,000-cell machine table), compared against
the body's label and recorded in the ledger (`dict-agrees-with-body` /
`dict-DIFFERS-from-body` / `dict-no-dictionary-row`); and the hypernym chain —
parent concepts with their own labels in both seats and their own
descriptions, joined INSIDE the WordNet 3.1 projection by primary-synset scan
over the 35-byte index records.

Two real seams surfaced and are named in the trace instead of papered over:

- **The version seam.** cs10 relations are WordNet 3.1; the OMW anchor column
  is PWN 3.0. Joining relation targets to the anchor column can never match —
  the first parent join was written that way and hung on a Form-implemented
  `str_find` scanning 1.4 MB for a synset that structurally is not there. The
  correct join never leaves the 3.1 projection. Both versions are printed
  where they appear.
- **The specificity seam.** Direct hypernyms of frequency-ranked lexemes are
  often fine-grained synsets that are not the primary sense of any top-10k
  word: `water`'s parent (`n14643012`) is honestly "not among the 10k
  anchors", while `why` → `n09201896` → row 483 `reason` (de `Ursache`, id
  `alasan`) joins fully, parent description included. Offline analysis
  (analysis only, nothing committed depends on it): 1,916 of the 7,371 mapped
  anchors have an in-table direct parent.

One performance law joined the carrier-boundary notes: `len` is a full list
walk, so calling it per step of a 10k scan turns the scan into minutes —
`nil?` on the tail is the shape (measured: the walk went from timeout to
instant).

## Witnessed on 2026-07-18 (live fkwu, Linux x86-64 checkout)

- Band `cognition/tests/nl-neutral-trace-band.fk` → **1048575** (twenty bits;
  the six new bits witness: the row-377 gloss at `n14869913`; its 10 attributed
  senses; the machine dictionary agreeing on `Wasser`; the positive parent
  join `why → reason/Ursache`; the honest parent miss for `water`; and the
  13-seat row rendering `zh` and `ar`). The first fourteen bits remain (from
  the earlier pass:
  both lanes on row 377; both generation directions; walk counters including
  cut = pruned seats; the en/id `air` homograph fork; the PERSISTED index
  answering both queries from disk at `@10.3.99.377`; a valid measured choice
  receipt with 0 silences; the id↔en pivot round trip; `n0`/ice; de→en
  decode; en→de and id→de restoring the article; three-tongue pivot node
  identity; per-rule counters ok 2 / fail 0).
- Live command ≈ 10 s wall under `tools/ftimeout`, 0 timeouts.
- Staged follow-up `id de api` → row 454 → `Feuer`, rag cell `@10.3.99.454`.

This organ opens and writes files, so it is fkwu-witnessed with its own band,
not claimed four-way. The de seed rows (quelle/nativ/kern/leib) are
agent-authored, ASCII-safe by the byte-classed alpha-run boundary, awaiting
native-speaker review — named in the trace itself.

## What the trace now teaches (in its own closing frame)

Trust is measured: every claim is a counter, a byte interval, a sha256 key,
or a node identity recomputable from committed bytes. Sovereignty is
native-first: the whole answer is computed by fkwu from this repo's cells;
the oracle lane is a scored fallback whose answers are compared and
remembered, never obeyed. Vitality is the loop: the ledger grows, forks
surface, every stage names its own gap and next step. Unique to this kernel:
meaning lives at a content-addressed cell, tongues are columns over neutral
symbols the body owns, and honesty is structural.

## Fourth pass: the 10k sweep has landed

```
./fkwu --src cognition/tests/nl-neutral-trace-sweep-live.fk
```

sweeps ALL 10,000 concept rows into the persisted index (~9.3 MB, one
content-addressed `nodeid-rag-v2` row per concept, key = sha256 of the exact
committed TSV row bytes), printing progress every 500 rows through the native
`print_str` door and recording the row count in a sibling meta cell.

The first sweep exposed the next wall honestly: a single production-path
query over the 9.3 MB file needs **more than four minutes** (measured, JIT
made no difference) — the per-row admission scan calls the Form-implemented
`str_find` once per field over ~900-byte lines. The answer follows the
repo's own `T_flat` doctrine — *crystallized speed is a regenerable cache,
never a foundation*: the sweep also writes a **rank sidecar**
(`index-vec.tsv`, ~1 MB: row, byte interval, semantic codes). Queries scan
the sidecar with a byte cursor and then run the PRODUCTION codec and
cryptographic admission (`ra-rank-entry-of-line` + answer-key verification)
on the selected row only. Rank is a cache; truth stays in the index bytes.

Sweep-aware behavior, kept truthful:

- The main trace command preserves a swept index (rewrite skipped, stated in
  the report) and queries it through the fast lane; without a sweep it writes
  and production-scans the two-row focused index as before.
- Writing the focused index resets the row meta so a stale "swept" claim can
  never outlive the file it described; the band writes to its own scratch
  path so it can never clobber the swept home.
- A big meta-less index is named "interrupted sweep" instead of being
  line-counted into the stack wall (one frame per byte).
- Shapes that keep the sweep linear are documented in the organ: the offset
  tail carried one cons per row, appends through the native
  `file_append_bytes`, the vec computed once per row for both files.

## Fifth pass: the trace is a live stream

Every trace line is now EMITTED the moment its fact exists — nothing waits
for the end of the run. Boot events land within milliseconds of the body's
first evaluated expression; each locale seat's verdict streams as its
10k-row scan completes (watchably, ~1.5 s apart); the neutral cell, the
gloss, the parents, the dictionary verdict, the rag answer, the measured
receipt, and the ledger row each arrive as they are computed; the pivot
stage emits the interned pivot cell's REAL coordinates live
(`node_pkg`/`node_level`/`node_type`/`node_inst` of the actual node).

One runtime line made this true on every surface: `print_str` (op 115) now
flushes after each emitted line (`fflush(0)` — no `FILE` type needed in the
freestanding extern set), so the stream is live through a pipe exactly as on
a tty. Both bootstrap witnesses (`42`, `55`) hold on the rebuilt seed. What
the stream cannot cover is named in its own first line: the C seed's compile
phase runs before the body can speak.

The interruption drill also caught a truth hole: killing a sweep mid-run
left the previous sweep's row meta claiming "swept" over a truncated file.
The sweep now resets the meta FIRST, so an interrupted sweep always reads as
interrupted.

Witnessed arrival times (piped, timestamped per line): boot events at
+0.00–0.02 s after compile; seven seat verdicts streaming individually;
full two-lane run complete in ~12 s; band verdict unchanged at **1048575**.

## Sixth pass (2026-07-19): Spanish seated, the floor regrounded

After #328 and #333 merged, the branch was regrounded on `7667fad`
(bootstrap 42/55 re-measured) and the next stone walked: **Spanish on the
one pivot** (`cognition/nl-pivot-es.fk`) — one grammar (el|la|los|las
through an alt pattern), one column, pivot untouched. The first probe
surfaced a real agreement wound (`el nucleo es nativa`); the fix keeps
agreement in the column, never the pivot: the adjective row carries both
gender forms and the decoder picks by the subject's own article
(witnessed: `la fuente es nativa`, `el nucleo es nativo`). The sovereignty
witness now spans FOUR tongues: es/de/en/id intern the SAME pivot cell
(`node_eq`, band bit 2097152). Band: **4194303** (22 bits).
`CURRENT_FLOOR.md` carries the new reground section.

## Still open, in order

1. Seat the remaining 9 tongues on the pivot (`nl-pivot-de.fk` and
   `nl-pivot-es.fk` are the worked examples: one grammar + one column each).
2. Native-speaker review of mapped labels and the de seed rows.
3. A native (or JIT-crystallized) `str_find` so the production stream can
   scan the full index at sidecar speed and the rank cache becomes optional.
4. `walk_recipe_here`'s real fkwu counterpart (its file keeps it loud on
   purpose until the walk can be real).
5. argv/env natives so the staged files become optional.
