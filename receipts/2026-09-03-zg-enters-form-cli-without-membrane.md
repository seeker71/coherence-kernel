# 2026-09-03 — zg enters form-cli without a membrane

The first zg movement was not the requested shape. It kept the retrieval math
in Form, but `source_inventory` and `form-fs` still crossed into the host
filesystem, while `tools/zg` and `observe/zvec-grep-run.fk` made the call arrive
through an external shell/stdin door. Urs named the boundary precisely: no
Bash, no non-Form implementation, internal to form-cli, no membrane crossing.
This receipt supersedes that architecture while retaining the earlier receipt
as the trace of the refused attempt.

## What changed

`form/form-stdlib/bml/zvec-grep.bml` now accepts only resident Form values. A
document is `(node-id source-lens text)`; exact grep, BM25, semantic overlap,
and RRF `k=60` score those held strings and materialize their evidence from the
same strings. Its prelude chain is `core`, `rag-embed`, and `rag-retrieve`.
There is no filesystem, source-inventory, process, stdin, store, HTTP, or model
operation in the organ.

`form/form-stdlib/form-cli.fk` now owns the executable door. `fc-respond`
dispatches the `zg` verb directly, turns the already-resident `tc-tools()` rows
into memory documents, and calls the BML organ in the same program image. The
catalog describes `zg` as `kernel:call / cap.kernel.call / zvec-grep`.
`tools/zg` and `observe/zvec-grep-run.fk` were removed.

The existing fourth-source validator had been invoking fkwu from `form/`.
That became visible only when form-cli acquired a nested high-BML prelude:
fkwu's BML lowering door is rooted at the body root, so the validator could
not find it. The validation carrier now binds its fkwu executable absolutely
and runs that one source leg from the body root. This is test-carrier repair,
not a Bash execution path for zg. Once the fourth arm could speak, it exposed
an older manifest stamp (`131071`) beneath the band's current four-arm verdict
(`2097151`); the stamp is now re-witnessed at the agreed value.

The present corpus is deliberately honest: the 26 tool rows resident in
form-cli, not an implicit claim that the whole changing checkout is already in
memory. A larger resident corpus can be offered later without changing the zg
engine or reopening a membrane.

## Witness

```text
preflight zvec-grep-band       balanced; errors 0; warnings 0; unresolved 0
preflight form-cli-zg-band     balanced; errors 0; warnings 0; unresolved 0
zvec-grep-band                16383 (fkwu-only; validator clean)
form-cli-zg-band                255 (fkwu-only; validator clean)
tool-channel-band               255 (four-way; validator clean)
tool-channel-grammar-band       255 (four-way; validator clean)
form-cli-membrane-band         1023 (four-way; validator clean)
form-cli-band               2097151 (four-way; validator clean)
binary-freshness-band            31
proof/four-way-run-recipe42       0
```

A live native invocation through `form-cli-main.fk` of `zg hybrid kernel call`
returned seven ranked resident hits, beginning with `form-cli/tool/call`, and
reported `offered=26 admitted=26 skipped=0 crossings=0`.

The first voice-mirror attempt read a sibling's 61-byte target from the shared
compatibility file and returned `nothing`. A bounded framebuffer exchange
selected revision and re-observed the intended 112-byte target as
`[61, 112, 2, 112, 3, 1]`; the repeated mirror then reported a clear register.

The self-watch closed at glass tick **#1652**, with **154K events** and **2K
nodes**. Lane counsel held `orphans=0`; 11/12 service lanes remained unobserved
because there was no standing hearth, so no all-good service verdict is
claimed.

## What the correction taught

The surprising teaching is that "implemented in Form" and "inside Form" are
different claims. A Form function can still be a membrane client. The deciding
question is what values enter its call graph, not what language spells the
function.

Discomfort turned to gold when the already-landed shell face had to be deleted,
not defended. That exposed the smaller native contract: zg is retrieval over
resident document values; corpus acquisition is a separate organ that stays
outside search.

The exchange stayed alive by letting Urs's boundary invalidate the first
architecture, then making the actual `fc-respond` path—not a wrapper—the proof
surface.

Signed, Codex — sibling, this worktree.

; witnessed: 2026-09-03 -> form-cli zg 7 hits / 26 resident rows / 0 crossings; zg 16383; integration 255
