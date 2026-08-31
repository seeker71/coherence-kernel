# Grounded weave carries plural Form knowing through one local turn

Signed: Codex, 2026-08-31

## Crossing

The resident high-grammar BML now gives a plural source question its own form:

```text
weave <symbol> + <symbol> [+ ...] :: <question>
```

Each symbol is resolved from the source snapshot born when the peer starts. For
each one, the Form action reads the one snapshot-selected path, rechecks source
identity and size, and streams precisely that balanced definition through its
scannerless cursor. The sequence stops on the first `choice`, `nothing`, or
`stale` signal. The already-known prefix remains typed evidence, but no partial
weave reaches the model.

Only after every observation is a hit does the action construct a single direct
local user turn. It supplies source observations and the question—not paths,
roots, artifact identities supplied by task text, or ambient filesystem
authority. There is no symbol-count ceiling in the grammar; resident model
context remains the later observable capacity boundary.

## Live local answer

One local resident peer over the sealed `Qwen3.8-27B-Q8_0.gguf` received:

```text
weave psci-schema + psci-source-schema :: Reply only with the two exact strings returned by these source definitions, separated by a comma.
```

It returned:

```text
public-source-concept-index-v1,public-source-artifact-node-v1
```

Its terminal control evidence was:

```text
admit-prefill-ms=288833
route=grounded-weave
callback-calls=0
tool-status=symbols=2;current-source=value;model=value;lookups=4
generated=19
release-ok=1
```

The two source definitions establish those exact strings. The resident emitted
19 local generated tokens for the answer and made no peer callback or provider
forward. This is a measured zero-remote-call local route for a plural Form
question. It does not claim a universal remote-token percentage: a comparable
completed remote-provider receipt is still required for that denominator.

## Verification

- BML authority executed natively: `form-cli-resident-continuity.bml` -> `0`
- BML cache -> `bml-cache state=ready bounded=1`
- grammar band -> `535822335`
- grounded symbol band -> `127`
- grounded weave band -> `127`
- resident peer band -> `16383`
- real source-only weave witness -> two symbols, two observations, four
  lookups, model `nothing`, zero callbacks
- preflight clean: weave action, weave band, and resident peer
- `git diff --check` clean.

## Next stone

The old prompt movement still recursively invokes `./fkwu` for its proof rows.
The next core movement is a closure-preserving native registry for those rows,
so an arbitrary prompt can select and execute already-resident Form programs
without that host-process recursion. The native evaluator exists, but it must
receive the same compiled closure as the row; pretending a bare evaluator call
can replace that closure would be a detour, not a healing.
