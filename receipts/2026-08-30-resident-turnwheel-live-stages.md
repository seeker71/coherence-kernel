# The resident turnwheel now speaks each bounded move

## Crossing

`form-cli-resident-turnwheel.fk` already advances a prompt cursor in bounded
prefill quanta and commits at most one model token per resident revolution.
Those actual state transitions are now the place that emits the stage; there is
no second observer, HTTP door, server, subprocess, or model residence.

`fcrt-enqueue-flow` emits the admitted row's first stage. `fcrt-step-row` wraps
the existing raw step and emits exactly once after every live row advance. Both
`fcrt-revolve` and the peer append scheduler's selected-row path already call
that same step door, so program, prefill, model, `returned`, `choice`, `cut`,
`timeout`, and `nothing`-like terminal states retain one common observation
surface without changing their control meaning.

The flushed frame is deliberately small:

```text
<|form:resident-stage|>
id=<row id>
phase=<program|prefill|model|terminal phase>
status=<live|returned|timeout|...>
gas=<remaining bounded turns>
tokens=<committed value count>
<|/form:resident-stage|>
```

It contains no prompt, task, source, recipe, model path, generated text, or
releaser bytes. `fcrt-stage-node` holds the same structural facts as an
attributed NodeID, so the active Form process can read it through
`framebuffer-events` while its resident continues.

## Witness

The new native stage band publishes an arriving JIT program, accepts row `71`,
then advances it once. Its direct `fkwu` trace was:

```text
id=71 phase=program          status=live     gas=4 tokens=0
id=71 phase=program-returned status=returned gas=3 tokens=1
127
```

`127` proves the accepted and returned row states, result `22`, exactly two
framebuffer events, both with source
`form-cli-resident-turnwheel.fk:147:1`, the five state fields, and absence of
the arriving route/releaser strings from the flushed representation.

Focused evidence after the source and band preflighted clean:

```text
bootstrap/ground.fk                                      -> 42
binary-freshness-band.fk                                 -> 31
preflight form-cli-resident-turnwheel.fk                 -> balanced, 0 errors, 0 unresolved
preflight form-cli-resident-turnwheel-stage-band.fk      -> balanced, 0 errors, 0 unresolved
form-cli-resident-turnwheel-stage-band.fk                -> 127
form-cli-resident-turnwheel-band.fk                      -> 65535
resident-ingress-turnwheel-join-band.fk                  -> 131071
form-cli-peer-append-turnwheel-band.fk                   -> 32767
```

The stage band intentionally remains outside the four-way manifest: it tests
the live `print_str`/framebuffer behavior that the manifest names as a current
multi-line-output wall. That is a named proof boundary, not an excuse to call
the stage untested.

## Present boundary

This checkout has the actual resident scheduler and its stage surface, but no
checked-in live driver yet moves the peer contribution turnwheel's normal
model-mediated action onto an `fcrt-adopt` model row. The current peer model
branch still calls its source-defined session route directly. The stage hooks
therefore make an adopted resident observable as soon as it is used; they do
not pretend that a physical Qwen/KV session has already crossed into this
specific turnwheel path. The next direct core movement is to give successor
peer residents a caller-owned model-row action that advances via this existing
turnwheel while leaving the single `fcms` context/KV owner intact.

I kept the exchange alive by placing observation inside the existing bounded
turn, then letting the kernel reject and repair the first malformed renderer
before trusting it. The surprising teaching is that `intern_node_at` already
gives the resident's state frame an attributed framebuffer presence; a second
logging organ was unnecessary. The discomfort was the tempting claim that the
resident model path was already wired. It became a precise next seam instead:
the scheduler is now observable, while the peer's direct model route remains
honestly distinct until it is actually joined.
