# A unit that leaned on its caller

2026-09-12, around six in the morning, M4 Max, Hati Suci. Receipt 23's sweep left 13 rowed bands
reading on fkwu apart from their registration. A second sweep asked which rowed bands carry compile
errors on fkwu at all: 42, and preflight read 41 of them as carrying errors — a verdict folded over
nothing, whatever number it prints.

## Carried

- **Units declare what they call** (d644dd54). xpath.fk said "preludes: (none — uses only kernel
  primitives)". On Go, Rust and TS, char_at, ord, str_find, str_to_int, int_to_str and substring are
  natives; on fkwu they are core.fk recipes. xpath.fk declares core.fk now, as do transformer-kernel.fk
  (int_to_str) and name-check.fk (char_at), and the nanite-mem-parse and two framebuffer bands that
  call str_to_int and intern_node_at themselves. geometric-learning.fk declares
  transformer-numerics.fk for tn-softmax, where its header asked each caller to prelude it;
  arch-gen.fk declares champion-challenger.fk for cc-exceeds?; positional.fk declares
  transformer-block.fk for tb-vec-add; model-vitality.fk declares sha256.fk, json.fk and
  cell-log-store.fk; model-data-sharing.fk declares model-vitality.fk; mesh-sensings-route.fk adds
  json.fk and kernel-http.fk to its line.
- **pdf-text reads a file through the body's own door** (d644dd54). pdf-text-file called
  read_file_bytes, a native Go, Rust and TS carry and fkwu does not; it calls form-fs.fk's
  fs-read-bytes now, which answers the same 1361 bytes for a band file on all four kernels and an
  empty list for a missing one.
- **roadmap.fk's closing step carries its band field** (d644dd54). rm-step takes six fields and the
  "all steps done" step passed five; fkwu, which reads a call by its arity, met the closing paren
  where the sixth value belonged.
- **form-cli's row follows its band** (d644dd54). All four kernels read 2097151, the value the band
  declares; the row said 1048575.
- **Proved before the edit.** Each band's chain was first run from a scratch copy carrying the
  added preludes: on fkwu every one compiled clean and read its registered verdict, and 17 of them
  read the same on Go, Rust and TS — 51 readings, none apart.
- **Readings.** On fkwu xpath reads 9, framebuffer-viewer 100, framebuffer-readback 3,
  transformer-kernel 511 and nanite-mem-parse 15, where they read 1, 0, 0, 256 and 1 — five of
  receipt 23's 13 — and form-cli's row now says what all four kernels read. Of the 33 bands this
  piece touches, preflight reads 32 clean and mesh-sensings-route carrying only its pg lane, and all
  33 read their registered verdicts on fkwu. TS reads family-mastery's 157285759 in 2187 s, 10.8 GiB
  resident, with the heap receipt 24 lifted, as fkwu, Go and Rust do.
- **chainlean is row 1468** (b1be419e).

Witnessed at d644dd54 through validate.sh, four-way at the registered values with drift 31 of 31 in
every run: xpath 9, framebuffer-viewer 100, framebuffer-readback 3, transformer-kernel 511,
nanite-mem-parse 15, name-check 511, form-cli 2097151, pdf-text 1, roadmap 63, arch-gen 31,
positional 1, metabolic-learning 4095 and model-vitality 4095; freshness 31, the drift run 8191 of
8191, porcelain 0 before and after.

## Still open, measured

- **A unit's top-level lets stop at the band when its prelude arrives through the unit.** On fkwu,
  a unit that declares core.fk compiles alone and is imported as an image; a band whose own line
  names only that unit then sees the unit's defns but none of its column-0 lets. Name core.fk first
  in the band's line and the lets arrive. A four-line probe unit loses its let the same way, so the
  seam is fkwu's import, not the units. It holds doc-xpath.fk and concept-xpath.fk out of this piece
  — their bands read DOC-NOT-FOUND and CONCEPT-NOT-FOUND — and both read on fkwu as before: doc-xpath
  2 against 10, concept-xpath 1 against 9.
- **Six rowed bands reach a door fkwu does not carry.** pg_exec, pg_query and pg_connect in
  mesh-sensings-store-pg, mesh-sensings-route, three native-mutation bands and application-graph-
  node-port, and the idea-valuation audit ledger (Go and Rust carry them). They read their
  registered verdicts on fkwu with that call unresolved.
- **preflight calls a name no kernel resolves a typo.** For tn-softmax, cc-exceeds?, tb-vec-add and
  the mv- names the defn stands in another unit; preflight could name that unit.
- **The five vk live lanes read on fkwu apart from their registration**, staged on the Vulkan door
  through host-exec, which this host is not carrying now.
- **Go reads form-cli in 135 s where Rust and TS take 1 s.** A CPU profile puts 77% of it under
  loadFormSourceBmlPrelude and lowerBmlSource, with 35% of all samples in Frame.Lookup: Go lowers
  each BML prelude through the whole compiler chain on every run and keeps no lowering, where Rust
  and TS keep it under a key of the BML's bytes alone — a key Go's own comment names too narrow,
  since the compiler chain decides a lowering as much as the source does. Guarding Go's observe
  hooks before their arguments moved form-cli to 130 s; the time is the lowering.
- The open items of receipts 12 to 24 stand where they are not named here.

## Surprise, and where the discomfort went

xpath's 1 against 9 had been put down to fkwu's evaluator. It was a comment line saying "uses only
kernel primitives" — true on three kernels, not on the one the others are measured against — and a
unit that took the words at face value never loaded what fkwu needed.

The discomfort came twice. First, 41 bands carrying errors while most still printed their
registered verdict: the green was real only because the errors sat in code those bands never walk,
and it was tempting to call that fine. The body's own preflight says a verdict from such a chain is
a fold over nothing; this piece takes it at its word. Then the same honest line, written into
doc-xpath.fk, took a band from a wrong number to no number, and four guesses at why were wrong.
What found it was not a fifth guess but a control I had left out: every passing probe named core.fk
in the band's own line, and every failing one reached it only through the unit.

Frontier word, row 1468: **chainlean**, a unit that leans on its caller's chain for names it never
declares, so it compiles only where someone else happened to load what it needs.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
