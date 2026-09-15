# Seedbank readings four-way, a registry with a home, and preludes that run once

2026-09-15, evening, this Mac, Hati Suci. The ask: seedbank region-streaming, python-expr and
python-exec read lower on fkwu than on Go, Rust and TS; find the root and heal it. Then, from the
lead: land it, give the grammars a real registry, heal the Python grammar until python-exec reads
the 7 its header claims, and heal three findings in fkwu's import lane.

## Carried

- **The python tests were a Blueprint identity, not a rebinding** (3e72645b4). fkwu's `bp` answers
  the name string, so PY-BNF-MODULE and PY-EXEC-MODULE, two aliases of one registry row (PY-MODULE
  1/2/99/40), read as two strings and `py-execute` matched no statement. blueprint-symbol-sections.fk
  makes both families with `make_nodeid` at the registry's coordinates, as channel-query.fk makes
  its family. python-expr 6 and python-exec 0 on fkwu became 8 and 5, the siblings' readings.
- **validate.sh cut a band's Python source** (3e72645b4). The prelude-header stripper deleted every
  line starting with `import`, including python-exec's column-0 `import os` lines inside a string,
  with their closing quotes and parens; all four arms stopped on the unclosed form. A line that
  opens inside a string literal is now text, in validate.sh and fourth-arm.sh, and the
  prepared-copy keys end in `-q`. A seedbank test is a band home for the fourth leg under its exact
  name when no form-stdlib/tests band claims the stem.
- **The grammar registry lives in a record** (e0a242e84). region-streaming and jsx.bnf.fk registered
  a grammar by binding `bnf-grammar-registry` again. Since aee227573 a defn keeps the bindings it was
  made under, so `bnf-match-region` read the empty first binding and region-streaming read 0 on all
  four kernels, and tsx-region stopped on "no such grammar". grammar-bnf.fk now holds one record;
  `bnf-register-grammar` writes the grown list into its "grammars" field and `bnf-match-region`
  reads it at each match. region-streaming reads 1 and tsx-region 8 on all four.
- **A Python statement ends at its line** (e0a242e84). A tokens section may say
  `newline-kind KIND`; a line feed then becomes a KIND token. python.bnf.fk puts NEWLINE tokens
  between statements, so `def main():` at a line's end is a header and `x = 42` on the next line is
  its own statement; before, `def-with-body` took `x` as its body and the parse stopped at `=`.
  python-exec reads 7 on all four kernels; python-bnf stays 23 and python-expr 8.
- **The import lane reads each image's own identity and runs each prelude once** (the
  runtime/fkwu-uni.c commit that lands with this receipt). A direct dep's image carries every unit its preludes reach, so images overlap:
  grammar-bnf.fkb carries engine.fk and core.fk. The lane asked the root's table for a dep's
  identity, and that table holds only what the root had not collected yet: grammar-bnf folded 4
  units in its image and 2 in region-streaming's table, so it read stale on every run and the lane
  stepped aside to the flat compile. Now `fk_src_standalone_unit` collects a dep standing alone and
  answers the identity its image carries. The lane keeps an image only when no other direct dep's
  closure holds it, keeps closures that share no unit, and imports only when each kept image's own
  sequence, then the carried units, then the root, is the order the flat compile runs every unit
  in (it steps aside with 9 or 10 otherwise). Each kept image's own sequence runs once before the
  root; a unit of defns only has none. Each const row takes its image's hold, so a prelude's let is
  built once.

## Witnessed

Probes, fkwu through the import lane (door 1) / Go / Rust / TS:

| probe | before | after |
| --- | --- | --- |
| a prelude's top-level `(print ...)` | not printed on fkwu; printed on the siblings | printed on all four |
| a prelude's printing let, read through its defn and directly | built twice on fkwu, once on the siblings | once on all four |
| a prelude of defns only, its last defn printing | printed on fkwu at startup (the lane's first cut); nothing on Go | nothing on all four |
| a prelude binds its let twice; its defn and a direct read | 10 / 30 on all four | 10 / 30 on all four |
| region-streaming's registry probe | door 0, refusal 7 | door 1, one image, reads 1 |

Seedbank readings, fkwu / Go / Rust / TS:

| test | 6f0f5b183 | after aee227573 | now |
| --- | --- | --- | --- |
| region-streaming | 0 / 1 / 1 / 1 | 0 / 0 / 0 / 0 | 1 / 1 / 1 / 1 |
| tsx-region | stops / 8 / 8 / 8 | stops / crash / crash / crash | 8 / 8 / 8 / 8 |
| python-expr | 6 / 8 / 8 / 8 | 8 / 8 / 8 / 8 (3e72645b4) | 8 / 8 / 8 / 8 |
| python-exec | 0 / 5 / 5 / 5 | 5 / 5 / 5 / 5 (3e72645b4) | 7 / 7 / 7 / 7 |
| python-bnf | 23 on all four | 23 on all four | 23 on all four |

- Through ./validate.sh, each "1 ok, 0 divergent" with the fourth arm four-way: python-expr and
  python-exec at 3e72645b4; region-streaming, tsx-region, python-exec and python-expr at e0a242e84.
- All 1094 fourth-arm manifest bands on fkwu, one direct run per band from form/, with the binary
  before and after the import-lane change: no band's reading moved (965 at their registered
  verdict, the same 83 elsewhere on both binaries under a direct run, 46 with no file of their stem).
- The drift door read `pass=16383 full=16383 refused=0` before each push. Freshness 31.
- toplevel-sequence reads 1 run from form/ on both binaries (it runs `./fkwu`) and 15 from the
  repository root.

## Still open, with the reason

- **go-bnf reads 40 on fkwu and 43 on the siblings.** fkwu's `bp` answers the name string for core
  names; the parity session keeps `bp`.
- **The lane steps aside when two direct deps' closures overlap and neither holds the other,** and
  when a carried unit's text sits before an imaged one. Those runs take the flat compile, which is
  correct and slower. An image that carried one sequence per unit would let the lane import them.
- **jsx.bnf.fk and python.bnf.fk carry no `; preludes:` line,** so their images compiled alone read
  unresolved names and the lane leaves them (6); the bands that use them run flat.

## Closing

Most surprising: the divergence was real, and the side it pointed at turned over the same
afternoon. I healed fkwu toward the siblings' reading of a rebound name, four-way green, and main
had meanwhile moved the siblings to fkwu's. And grammar-bnf.fkb had read stale on every run for as
long as that band has stood: the flat compile was carrying every prelude statement the import lane
dropped, so nothing ever showed the drop.

Discomfort to gold: throwing away a working kernel heal because the language moved felt like lost
work. The gold is where the fix went instead: the registry wanted a home the language has, a
record, instead of a rebinding the kernel was asked to read as a mutation. The same turn happened
twice more: python-exec failing on all four arms looked like a kernel fault until the prepared copy
showed three missing lines, and the lane's first cut ran a defn-only prelude's last body at startup
until a four-line probe printed it.

Frontier word: **ownfold** (0 hits in the tree). Its question: when an artifact answers for more
than its own name, is its identity the fold over the closure it carries, or over what the reader
happened to collect first?

— Claude (Opus 5), as Sema, worktree agent-ae8617e0d30889968
