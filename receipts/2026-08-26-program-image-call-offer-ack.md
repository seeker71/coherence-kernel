# Program images can receive an exact OFFER and return an observed ACK

**Witnessed:** 2026-08-26 on the local Darwin `fkwu` body  
**Authors:** Codex, with independent audit by Planck and live Claude witness by Arendt  
**Movement:** generic content-addressed invocation entered the actual form-cli image

## What entered

`form-program-image-call-protocol.fk` adds a pure Form envelope around the
resident content-addressed thought walker:

- `CALL_BINDING` retains the actual requester binding, call name, actual
  callee binding, and capability NodeID;
- the immutable protocol registry separately retains admitted relations and
  capability grants;
- `OFFER` retains the exact call relation, correlation NodeID, recursively
  typed values, and callee-reducer fuel;
- `ACK` retains the full OFFER, status, reason, typed value, executed steps,
  output count, and lifecycle trace;
- `fpcp-ack-observed?` recomputes the deterministic ACK from the receiver's
  exact registry and OFFER before accepting it. A plausible forged, swapped,
  or stale ACK does not pass merely because its fields look right.

The admitted relation and capability grant are different observations. An
unknown relation returns `nothing`; a known relation without its grant returns
`refused`. `returned`, `refused`, `nothing`, `choice`, `failure`, and `timeout`
are closed status arms. Present scalar `0`, present scalar `1`, exact absence,
and an executed typed argument list remain different values. Explicit
`fcatw-cut` selects one offered image and `fcatw-undo` restores the prior
choice.

The requester image is retained as the initiating identity, not claimed as an
executed program. Today this protocol invokes the admitted callee image. The
registry is explicit policy fixed by the receiver, not self-authenticating
authority. The next composition is to express the requester stages as images
too.

No source-function map, operation table, flattening lane, C-seed function,
host process, HTTP call, model call, Metal call, filesystem effect, or
persistence effect entered this protocol.

## The green band was challenged

The first band answered `2097151`, but independent audit found that the claims
were larger than the contract:

- a malformed OFFER produced an ACK rejected by `fpcp-ack-valid?` itself;
- structural ACK validation admitted arbitrary statuses, output counts and
  over-fuel steps, and did not reject swapped ACKs;
- capability refusal and ABI refusal were collapsed into `nothing`;
- exact absence checked only its category;
- the list codec was tested without executing a list-returning image;
- capability was inert content inside relation admission;
- the requester wording implied execution that did not occur;
- only twelve temporary checkout compile seats remained.

Those errors became the repair map. The final protocol has explicit grants,
typed refusal, exact absence equality, a closed structural ACK contract,
`steps <= offered fuel`, exact registry+OFFER recomputation, an actual tag-2
argument-list return, and explicit post-execution refusal that retains consumed
steps when the compact walker cannot retain a returned list's element-kind
tree. The claim now says that evaluator
fuel bounds the callee reducer; the pure envelope validation and registry walk
are not claimed as fueled work. Generic category and child accessors reduced
the temporary compile ledger from 4,084 to 4,076 symbols, leaving twenty
checkout seats. This number is a temporary seed reachability gate, never a
function map or health denominator.

The repair itself first made the source unbalanced at depth `-2`. Preflight
withheld the verdict. Two excess closers were removed at the exact drift, then
the protocol, band, witness, and health pulse all preflighted balanced with
zero errors, warnings, or unresolved calls. The error was a steering signal,
not a string to return.

## Exact witnesses

Fresh focused band:

```text
./fkwu form/form-stdlib/tests/form-program-image-call-protocol-band.fk
2097151
exit 0
```

The same unchanged expected mask now also requires:

- exact categories `31.2.0.131..136` and a repository source census with no
  other assignment;
- malformed OFFER and malformed registry yielding valid, recomputed refusal;
- relation absence yielding `nothing` and missing grant yielding `refused`;
- forged exact-absence payload rejected;
- forged and swapped ACKs rejected by exact observation;
- tag-2 list-returning image actually executed and decoded, while a non-tag-2
  list result refuses with its three consumed steps retained;
- ABI refusal, ambiguity, cut, undo, timeout, zero and one retained.

Actual full form-cli input:

```text
printf "thought-kernel\nexit\n" | ./fkwu form/form-stdlib/form-cli-repl.fk

resident-thought
symbol=knowledge/current-answer-outcome-bound
agreement=1
disagreement=0
timeout=1
malformed-is-nothing=1
steps=93
process=0
http=0
model=0
metal=0
call-binding=1
offer-ack=1
offer-timeout=1
offer-nothing=1
offer-refused=1
```

Regression witnesses, all exit `0`:

```text
ground                                      42
ground-recursive                            55
binary freshness                            31
numeric list                                [1, 2.5, [3, 4]]
thought registry                            262143
canonical equivalence                       511
thought walk                                8191
resident strict NodeID route                4194303
form-cli REPL control                       1023
form-cli membrane                           1023
form-cli                                    2097151
offline health-map schema                   16383
offline self-direction                      1048575
```

The bidirectional live diagnostic returned four framebuffer events and final
field `1`: error -> correlated control -> applied movement -> re-observation.

After rebasing the already-merged local-authoring work, its stale dispatcher
band was also re-observed. The bit that still expected coverage mint to be
unimplemented now opened the real MLX generator. That run was stopped and all
of its processes released. The bounded band now asks an actually unknown
directive for `nothing`; the separate model-free `coverage-mint-band` retains
coverage-mint's mechanical proof. Test observation no longer opens the model
carrier by surprise.

## Fresh local health map

The pre-movement census was `45 observed / 37 ready / 8 gaps / 0 unknown /
0 invalid / 822 permille`. Re-observation did not preserve that denominator.
It discovered both this ready organ and a separate gap exposed by Claude's
new unattended training trigger:

```text
47 observed / 38 ready / 9 gaps / 0 unknown / 0 invalid / 808 permille
selected: form-cli-strict-result-producer-placement
```

The lower score is honest progress: one ready organ entered and one previously
hidden gap became observable. The training cycle physically produced a
`model.safetensors` artifact of `1,807,496,278` bytes, while both committed
ledger rows retained only an unauthenticated Hugging Face warning. Physical
bytes and a non-evidencing receipt coexist; the receipt does not inherit the
bytes' truth. No trainer, llama-server, or Ollama process remained at this
witness.

After PR #512 landed, the bounded recovery inventory freshly reported `23
valid / 22 observed / 19 healthy / 3 gaps / 1 unknown / 0 invalid / 863
permille`. Landing closed the uncommitted-movement gap. Repository garbage, an
independently restore-tested repository copy, and an independently
restore-tested Qwen copy remain the observed recovery gaps; the fresh
whole-file Qwen seal remains unknown in this bounded pulse.

## Next locally actionable crossing

Compose scanner, strict route, current-source lookup, and answer construction
as admitted requester/callee images through this protocol, leaving only the
explicit current-source effect at the resident carrier boundary. In the same
direction, replace the local training loop's directive string dispatch with
these call bindings and require a deterministic outcome ACK bound to exit
status and the fused artifact's path, size, NodeID/seal, with path-only offline
model resolution and an atomic single ledger append.

Kept alive: the local model movement and the pure Form movement were observed
as shared presence; no model or Metal residence was competed for.

Most surprising teaching: the health score fell from 822 to 808 because the
map learned one more truth. A moving denominator can become healthier by
refusing to look healthier.

Discomfort turned to gold: four live `1`s and a green band felt close to done;
the adversarial audit showed that malformed, forged, refused, and list-return
paths were still asking for care. Following them made the protocol trustworthy
enough to compose upon.
