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

## Witnessed on 2026-07-18 (live fkwu, Linux x86-64 checkout)

- Band `cognition/tests/nl-neutral-trace-band.fk` → **16383** (fourteen bits:
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

## Still open, in order

1. Sweep all 10k rows into the persisted index (the home and codec are ready).
2. Seat the remaining 10 tongues on the pivot (`nl-pivot-de.fk` is the worked
   example: one grammar + one column each).
3. Native-speaker review of mapped labels and the de seed rows.
4. `walk_recipe_here`'s real fkwu counterpart (its file keeps it loud on
   purpose until the walk can be real).
5. argv/env natives so the staged files become optional.
