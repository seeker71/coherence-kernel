# Resident turnwheel BML is live

## Crossing

`form-cli-resident-turnwheel` now takes its terminal-row vocabulary from an
executable BML cell, not from a second handwritten cascade. The path is:

```
high BML authority ──seven-byte BMF cursor──> complete Form object
executable BML ──Form source compiler──> resident-turnwheel-xtal.fk
resident fcrt-terminal? ──> fcrtbml-terminal
```

The high authority names the scannerless cursor route, row frame, NodeID/JIT
route rule, epoch pinning, and carried-stop agreement. The executable sibling
lowers the same stage vocabulary into Form functions. `nothing`, `0`, and `1`
remain distinct; `choice`, `cut`, `timeout`, failure, refusal, and release are
terminal signals. `undo` stays a retained-state control operation, so it is
present in the language without being falsely treated as a completed stream.

## Evidence

The BML band returned `65535` directly on `fkwu`:

- its high BML file was fully consumed by `bml-grammar` over a seven-byte
  `cursor-file-window`;
- its executable BML compiled into `form-cli-resident-turnwheel-xtal.fk`;
- the lowered terminal/runnable/stop functions returned their named outcomes;
- `fcrt-terminal?` observed `cut` and `release` through that lowering while
  leaving `undo` and `live` nonterminal;
- the high source contains no `SourceScanner` path.

The existing resident proofs still returned their complete values: stage
`127`, carried model stop `127`, resident turnwheel `65535`, ingress join
`131071`, and peer append turnwheel `32767`. The stage proof emitted its two
real live frames for the arriving JIT program. No model server or HTTP route was
started for this crossing.

## What the repair taught

The first multi-line BML expression lowered to an empty method body. The
source compiler's accepted one-line expression form makes that boundary
observable: regenerated output now contains the complete nested Form
expression, and the band calls it. This is a source-language repair, not an
emitted-file patch.

The next local boundary remains model-mediated rows carrying the same BML stage
language while a real resident context is active; this crossing deliberately
does not claim a model run.

— Codex, 2026-08-30
