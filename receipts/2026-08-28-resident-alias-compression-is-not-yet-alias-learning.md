# Resident alias compression is not yet alias learning

Signed 2026-08-28 by Codex.

## Movement

`form-cli-peer-agent.fk` now carries a caller-born, dynamically populated
alias registry.  An alias resolves to exactly one NodeID only when the whole
registry and alias are well-formed.  Absence remains `nothing`, a competing
binding remains `choice`, a malformed registry, alias, or non-task target is
`failure`; none of those states crosses into the model as an invented task.
The action verb remains literal.  This is a local residence surface, not a
global function table and not a cross-restart identity.

The tokenizer map also now prices each distinct task form once, rather than
repeating identical GGUF token-tail walks for its own arithmetic.

## Static witness

Preflight was clean for the peer cell, its band, the semantic map, and the
physical patch witness.  The pure peer-agent band remains `8191`.

With the local Qwen3.8 27B Q8_0 GGUF, the external task-role map reports:

| surface | bytes | role IDs |
| --- | ---: | ---: |
| two NodeID coordinates | 47 | 37 |
| one NodeID coordinate | 38 | 29 |
| dynamically resolved `p` alias | 31 | 22 |

So one resolved alias removes 7 bytes and 7 Qwen role IDs beyond the
one-coordinate form.  The complete three-surface map returned in 28.1 s
after caching its three exact token-tail results within the cell; that time is
still dominated by opening the 27 GB GGUF source, not by repeated arithmetic.

## Physical boundary

The resident released its held Metal session (`release-ok=1`) before the
physical run.  The alias task then reached the same local Qwen/KV process:

```
task-observation begin  bytes=31 status=choice  stamp=1787880539799
task-observation end    bytes=31 status=live    stamp=1787880552399
repo-patch run:end      status=held callbacks=0 stamp=1787880561750
```

At the bounded 48-token run, the scanner remained `held`; no callback
occurred.  This is not a successful patch and does not establish that Qwen
has learned the alias.  The model received the alias definition in bootstrap,
but bootstrap mention alone is not training.  The guard therefore preserved
the fixture and emitted the typed held observation rather than making a
guessed or partial mutation.

## Next owed attempt

Keep the registry as an opt-in compressed transport.  Teach execution aliases
through a native, scannerless response grammar with a held-out physical
receipt, then compare callback success and token cost against the established
one-coordinate explicit-verb baseline.  In parallel, move tokenizer pricing
behind an already resident model/session source so the 27 GB source-open cost
does not recur for every diagnostic map.

The surprising teaching is that identity can be compressed without becoming a
guess, while the discomfort turning to gold is the model's `held`: it named
the exact missing teaching layer instead of letting a 7-ID saving impersonate
local reasoning.
