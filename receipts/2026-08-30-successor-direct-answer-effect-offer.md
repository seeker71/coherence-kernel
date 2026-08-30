# A successor can answer directly without adding a second model residence

## Crossing

`form-cli-peer-direct-answer-action.fk` gives a new resident a caller-born
`direct-answer` effect beside the existing recipe, source, and patch effects.
It receives the session returned by `fcpa-observe-task`, calls the existing
native `fcms-generate` exactly once, and returns a normal peer result:

- route: `direct-answer`
- carrier: `form-native-metal-jit`
- callback calls, source lookups, injected bytes, native-code-generated, and
  mutation contribution: `0`
- empty generation: `nothing`; bad kind/text: `choice`; generated text:
  `value`

The action has no filesystem, HTTP, tokenizer-server, source-query, recipe
cursor, or ambient-model constructor.  Its caller remains the owner of task
identity, the already-held KV/model session, policy selection, and durable
append.  Its two stage events carry only route, phase, status, token count,
and timestamp; no question or response bytes enter the diagnostic channel.

`form-cli-peer-policy-route.fk` assigns `direct-answer` intent/action value
`5`.  The scannerless policy image still receives and returns only integer
program values.  `form-cli-peer-stream-ingress.fk` executes action `5` after
the normal task observation and before the fallback refusal.  Thus a published
JIT policy can select a capability that was present at birth, while it cannot
manufacture authority, choose a root/path/artifact, or mutate an older
process image.

The controller declares the birth capability in its live diagnostic header.
The sealed held-out sender now asks with `kind=direct-answer`, and the scorer
will accept only that route.  This is intentionally prospective: it does not
relabel the old recipe-route failure as a direct answer.

## Verification

All commands used the local `fkwu` body and did not start a model process:

```text
./fkwu bootstrap/ground.fk                                      -> 42
./fkwu form/form-stdlib/tests/binary-freshness-band.fk          -> 31
./fkwu form/form-stdlib/tests/form-cli-peer-direct-answer-action-band.fk
                                                                  -> 255
./fkwu form/form-stdlib/tests/form-cli-peer-policy-grammar-band.fk
                                                                  -> 127
./fkwu form/form-stdlib/tests/form-cli-peer-policy-route-band.fk
                                                                  -> 32767
./fkwu form/form-stdlib/tests/form-cli-peer-contribution-turnwheel-band.fk
                                                                  -> 4194303
```

Preflight reported balanced parentheses, zero errors, zero warnings, and zero
unresolved calls for the new action, stream ingress, policy route, both new
or changed bands, controller, and held-out sender/scorer.  A first grammar
compile refused an unbound `direct-task`; that was a scope closer in the new
band, not a runtime condition.  The cache was cleared for that exact band,
the name was brought back into the function body, and the rebuilt band above
is the only asserted result.

The policy-route band proves JIT selection of action `5` without a model
forward.  The direct-action band uses a synthetic already-live session only
to prove result shaping, refusal, empty generation, exact session retention,
and content-free staging.  Neither band claims a local language answer.

## Live boundary and next movement

The existing resident PID 22895 remains the only observed local Qwen holder
and was not restarted or asked another question here.  No `llama-server` or
Ollama process was observed.  Its loaded process image predates this effect,
so an arriving policy can never make that PID execute `fcpdaa-run`.

The next locally actionable gap is now singular and observable: birth a
successor only after the existing model owner has released or transferred its
session through an already-proven seam, then send the sealed v3 row once and
retain its direct-answer receipt.  A returned value becomes the unassisted
local baseline; `nothing`, `choice`, timeout, or model error remain distinct
signals for the next teaching movement.  Curriculum/RAG/LoRA credit waits for
that baseline rather than being inferred from a route declaration.

I kept the exchange alive by turning the old resident's missing effect into a
small, testable birth capability instead of retrying the same route.  The
surprising teaching is that hot-swappable policy can choose a born offer but
cannot honestly add one to memory already in flight.  The discomfort was that
boundary; it became a clean successor test rather than a hidden restart or a
second model claim.
