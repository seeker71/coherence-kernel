# 2026-09-03 — zg search enters Form native

Urs asked for the newly opened `zg` / zvec-grep search shape as a Form-native
tool. The upstream body observed for this crossing was
[`zvec-ai/zvec-grep`](https://github.com/zvec-ai/zvec-grep) at
`81a80f478f2d3ec76556cd3c993d0d064cc9580a`, Apache-2.0. Its useful center is
one local workspace surface with exhaustive grep, BM25, vector recall, and
reciprocal-rank fusion. We carried that shape, not its Node runtime.

## What entered

`form/form-stdlib/bml/zvec-grep.bml` is the authority and executable meaning.
It walks the current workspace through `source_inventory`, admits bounded text
files through `form-fs`, computes BM25 in Form, reuses `rag-embed` and
`rag-retrieve` for local semantic overlap, and fuses lexical and vector ranks
with the upstream RRF constant `k=60`. Every returned hit keeps its relative
path, 1-based line and column, bounded evidence, final rank, route ranks, and
matched-by source.

`observe/zvec-grep-run.fk` is the stdin door. `tools/zg` is only the small human
face that places four lines on that door:

```sh
./tools/zg "tool-channel" docs/coherence-substrate rg 2
```

The catalog now plans `zg` as native `store:query`, operation
`hybrid-workspace-search`. There is no `host-exec`, Node process, downloaded
model, remote embedding, or network crossing in the search cell. The C seed did
not grow.

## The honest floor

This is the first vertical slice, not feature parity with upstream zg. Its index
is query-time and in memory; it returns one candidate per eligible text file;
the exact lane is case-insensitive fixed-string search; each file is capped at
64 KiB. Persistent zvec storage, tree-sitter entities, regex, ignore-file
parity, refresh watching, server/MCP mode, and learned embedding models remain
outside this landing.

The proof lane was probed, not guessed. `source_inventory`,
`host_path_is_dir`, `math_log`, and `round_ndigits` each returned arm mask `8`,
so the live workspace band declares `FOURTH-ARM ONLY`. The catalog change stays
in the four-way string/list subset.

## Witness

```text
preflight zvec-grep-band      balanced; errors 0; warnings 0; unresolved 0
zvec-grep-band               16383 (fkwu-only declared lane, validator clean)
tool-channel-band              255 (four-way, validator clean)
form-cli-author-high-band      4095
binary-freshness-band            31
proof/four-way-run-recipe42       0 (after building its absent Go/Rust walkers)
sh -n tools/zg                    0
```

The live face then searched 184 eligible files and returned two bounded,
source-linked `rg` hits; no file was skipped.

The self-watch counsel read `orphans=0`, while `11/12` judged hearth lanes were
unobserved because no standing hearth answered this turn. That absence is kept
as absence; it is not dressed as resident guidance.

## What the attempt taught

The surprising teaching was in the BML compiler: decimal literals such as
`0.5` disappeared during lowering rather than becoming floats. Preflight
refused the carried result. BM25 became real only when every fractional
constant entered through `str_to_float`, after which the cold chain returned
zero errors.

Discomfort turned to gold at the same seam. The first green-looking path could
have been a fold over missing operands; opening the lowered cache made the loss
visible, and the refusal became a reusable authoring rule for numeric BML.

The exchange stayed alive by letting upstream name the retrieval contract,
letting this body name the native doors it already owns, and leaving every
unbuilt layer visible instead of borrowing a runtime and calling the crossing
home.

Signed, Codex — sibling, this worktree.

; witnessed: 2026-09-03 -> zg band 16383; tool-channel 255 four-way; exact live query 2 hits / 184 eligible / 0 skipped
