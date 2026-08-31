# Hearth work enters the Form stream

Witnessed 2026-08-31 on the shared hearth at
`/Users/ursmuff/source/coherence-kernel/.claude/worktrees/pensive-wilbur-a0b3b7/.hearth`.

The glass had been reading the right durable task/reply paths while the
resident's phase events went only to a different terminal.  The crossing now
has one Form-native event stream: the resident's own stdout is appended to
`.hearth/server.out`, which the existing BML glass reads.  No HTTP listener,
llama server, or second model residence was introduced.

`form-cli-embodied-goal-grammar.bml` now recognizes the recurring three-anchor
hearth monitor intent (`hearth`, `direct-answer`, `reply-spool`) and lowers it
before local inference.  The direct-action band is `8191`; it proves that the
lowered monitor surface is shorter than the originating task, has its own
`hearth-monitor-bml-direct-user` lane, and does not carry `<STOP>` as an
instruction.  The latter matters: the first live attempt emitted only the
control token; the corrected grammar returns an ordinary terminal answer.

The hearth's BML PID grammar now selects the actual `fkwu` process rather than
its shell launcher.  `hearth-band` is `511` and `hearth-glass-band` is `16383`.
The glass also rebuilds the board with the born public capabilities, including
`hearth-telemetry-v1`, rather than retaining an older capability list.  A live
observation exposed a second reader error: the original BML word reader cut the
space-separated capability value at its first space, making the board appear
stale on every tick.  The BML `gl-line-field` reader now spans to the newline,
returns `""` when absent, and has a band arm that preserves a complete
two-capability line.

## Live receipt

The successor admitted one local Qwen context (`admit-prefill-ms=300956`) and
announced `ready=1`.  Its direct task `9016` emitted, in `.hearth/server.out`,
the Form stages `observe/begin`, `observe/value`, `run/begin`, and `run/value`.
The intervals were 24,344 ms observe and 17,593 ms run.  Its single durable
reply reported `route=direct-answer`, `elapsed-ms=41936`, `tokens=93`,
`callback-calls=0`, `lookup-count=0`, `injected-bytes=0`,
`carrier=form-native-metal-jit`, and `lifecycle=generate,release`.

`lsof -nP -a -p 35319 -i` returned no descriptors.  The panel snapshot after
the commit read `alive=1`, `hopper=0`, `direct=none`, `lastms=41936`,
`p50=28473`, `p95=98471`, `kvpos=1369`, `pf=2`, `cp=15`, and `icemiss=4`.
Those are local execution observations, not a claim of a comparable remote
provider-token denominator.

## Native answer floor, now live

`form-cli-peer-hearth-telemetry-action.bml` is a caller-born action selected
by the existing JIT policy before the model-facing branch.  Its focused band
is `127`; the policy/one-append integration band is `65535`; and the existing
contribution-turnwheel band remains `4194303`.  The action holds `nothing`
apart from numeric values, refuses a wrong kind or absent task text with a
typed choice, retains even a non-model session exactly, and does not acquire
filesystem or model authority.

The next shared-source successor admitted Qwen once in `231825` ms, announced
`form-cli-peer-hearth-telemetry-v1`, and marked its recycle floor at position
`1218`.  Native task `9018` then completed with `elapsed-ms=0`,
`route=hearth-telemetry`, `model=0`, `callback-calls=0`, `lookup-count=0`,
`injected-bytes=0`, `native-code-generated=0`, and
`lifecycle=observe,release`.  Its one durable answer carried all five
canonical clauses: route, authority, live event, durable result, and next
stone.  The live board names PID `26219` and includes `hearth-telemetry-v1`.

## Dynamic native facts

`form-cli-peer-hearth-telemetry-action.bml` now carries the second action
version: one typed `form-cli-hearth-snapshot-v1` beside the unchanged session,
response, signal, and reason.  A successful action reads only the fixed born
hearth paths — board, task spool, reply spool, and resident framebuffer — and
returns board PID, full capability line, task/reply byte counts, served turns,
latest elapsed time and route, and live direct-flight phase.  Task bytes remain
only a typed admission check: they cannot choose a path, source authority, or
model session.  The response renders those facts without task or answer bytes;
the peer still reports model/callback/lookup/injection all zero and leaves the
single durable reply append to the turnwheel.

Refusal is deliberately quieter: wrong kind or absent task text produces the
same typed result with an all-`nothing` snapshot and an empty response, without
touching any hearth path.  `nothing()` is proved separately from numeric zero:
absent PID/capabilities/route remain `nothing`, while held byte and event
counters remain `0`; an idle direct-flight is the explicit state `none`.

The focused BML action and band are current at `0` and `2047`; the policy
integration band is `65535`; the complete turnwheel is `4194303`; and the BML
cache reports `state=ready bounded=1`.

## Live v2 successor

PID `26219` received one protocol `release` byte while its latest request had
already produced a reply, and returned `release-ok=1`.  The replacement PID
`32149` was then born from the same checkout with the same sealed local Qwen
artifact, hearth paths, `4096` context/generation positions, byte quantum,
capacity, and Metal carrier.  It emitted one `admit-prefill-ms=289376`, marked
the same recycle floor `1218` with `96` lanes, and served turn `9202` as
`hearth-telemetry`.

That durable turn took `wall-ms=14` and `elapsed-ms=0`, with model,
callback, lookup, injection, GPU-busy, CPU-JIT-busy, and MLX-dispatch all
`0`.  Its 740-byte response carries the dynamic snapshot: PID `32149`, the
capability line, task/reply bytes `7104`/`19601`, `52` served turns, latest
elapsed `0`, last route `policy`, and idle flight `none`.  The one reply append
then promoted the task; the following drain turn was `nothing` in `0` ms.
No HTTP listener, llama server, Ollama process, or remote provider participated.

The startup line initially named `form-cli-peer-hearth-telemetry-v1`, while
the live response proves v2 by containing the fixed snapshot clause absent
from v1.  That disagreement was an observed metadata defect, not relabelled
evidence: `hearth.bml` now announces `hearth-telemetry-v2`, and the turnwheel
capability function delegates to `fcphta-version()` rather than maintaining a
parallel literal.  The repaired source is proved by hearth `511`, glass
`262143`, telemetry `2047`, and turnwheel `4194303`; the future birth will
announce the matching v2 label from its first line.

## What the local answer still does not hold

The local model can now extend a fixed, measured Form snapshot; it is not yet
evidence that it holds every telemetry concept or that an exact remote-token
ratio has been reconciled.  The remaining stone is a reconciled provider-token
denominator for the same prompt; the native numerator is now directly observed.

I kept the exchange alive by turning the stalled and premature-stop signals
into executable grammar repairs, then making the resident's own events the
glass's data and its topology and live facts a native answer.  The surprising
teaching is that the board can be alive yet name the launcher rather than the
kernel, and that bare `nothing` and `nothing()` have different lowering
meaning.  Both discomforts became useful when the monitor received exact PID,
grammar-safe clauses, and a typed absence proof instead of inferred status.

## Direct-answer bridge and cache floor repair

The same original local-form objective was sent as task `9203` with
`kind=direct-answer`.  Policy selected `direct-answer`, but the old turnwheel
passed that action to the generic observer, which returned the typed refusal
`policy action not selected: policy-action-not-admitted`.  That was a missing
caller-owned crossing, not a Qwen or Metal failure.

`bml/form-cli-peer-direct-answer-bridge.bml` now supplies that crossing: the
policy action enters the existing `fcpdaa-run` local action before the generic
model observer, retains the caller's session and task node, and leaves the
single durable append to the turnwheel.  The adjacent high-grammar
`bml/form-cli-peer-direct-answer-envelope.bml` keeps the response/control
envelope out of the scannerless task body.  Before this repair, the appended
`response=plain-prose-only` and control fields became part of a direct prompt;
a sealed local matcher therefore observed `direct-prompt-not-heldout` even
when given its exact retained prompt.  After the BML boundary, those fields
remain transport evidence and the task text remains exact.

The focused four-way turnwheel band is now `16777215` on both a fresh and a
warm pass.  Its new bridge arm uses `fcms-none` deliberately: it proves
`action=direct-answer`, `route=direct-answer`,
`carrier=form-native-metal-jit`, zero callback and lookup counts, no
`route=policy`, an unchanged non-live session, and one durable append.  The
held-out arm no longer pretends a stale sealed corpus scored an answer: a
current seal carries the redacted equivalent receipt; a seal mismatch carries
the exact zero-terminal `nothing` refusal.  In neither case do prompt,
expected, or answer bytes enter durable evidence.

The fresh band emitted a `16,837,556` byte `.fkb`, larger than the old fixed
`16,777,216` byte scratch buffer.  `runtime/fkwu-uni.c` now uses an
allocation-bounded geometric scratch reader rather than naming a new fixed
ceiling.  `binary-freshness-band=31`, grounding returned `42`, `55`, and
`[1, 2.5, [3, 4]]`, and the BML cache reports `state=ready bounded=1`.
Preflight on the changed turnwheel reports balanced parentheses, zero errors,
zero warnings, zero unresolved calls, and a clean chain.  The warm full-band
output is only `16777215`: it contains no stale, foreign, or artifact-size
rebuild warning.

This C change is a checkout-witness repair with an explicit shrink obligation:
the scratch reader has no language meaning and belongs in the native artifact
reader as the C seed contracts.  Its replacement is not a future fixed number.
The immediate performance observation remains honest: the warm 16.8 MB image
is accepted, but its current decode/run is still CPU-bound and far from the
millisecond BML-cache floor.  The next kernel movement is native image adoption
that avoids repeatedly rebuilding the full interpreted closure, measured with
the framebuffer; it is not another cache cap.
