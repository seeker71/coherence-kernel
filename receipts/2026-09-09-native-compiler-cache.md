# Compiler freshness belongs to the compiler's dependency graph

Signed: Codex, 2026-09-09. Base: a9b60392.

Status: carried into the adaptive-depth repair in the same movement. The earlier
stop interpretation was corrected by the current repository guidance: a failure
pauses the claim and landing while repair continues. The original server failure
below is historical evidence; [the subsequent receipt](2026-09-09-adaptive-depth.md)
records its completed repair and final checks.

The historical fourth-arm survey kept its own compiler list and persistent
lowered-source cache. A worker could also pick the first cached table executable.
Separately, the Go proof carrier cached BML lowering by source bytes alone, so
compiler changes could leave it serving old meaning.

Form now owns survey preparation through `observe/native-source-prepare-run.fk`.
Its compiler dependencies belong to the native image's own freshness graph.
Every request reads current source. Lowered proof text is temporary and removed
at invocation exit. Imports keep their authored owner; missing inputs,
dependencies and failed writes cannot fall back to raw BML. The Go proof
carrier's source-only lowering-cache read/write is removed. Existing entries
are ignored without asking anyone to purge them.

Workers select the content-qualified table carrier. The current native emitter
regenerated the stale committed table bootstrap: 114055 bytes, stamp
`37099ea0b0f970a7`. This synchronizes the existing Metal-deadline carrier emission;
it adds no new authored C meaning and does not change `runtime/fkwu-uni.c`.
The historical emitted table seed remains a proof artifact, with the native
source runner as the serving floor and the seed's existing shrink direction intact.

The survey also retains each Go/flatten/table stage's stdout, stderr and exit
status. A nonzero stage exits the worker with explicit failure; an empty
successful output is no longer mislabeled a timeout. Its table driver now emits
one value: the old extra `null` failed the current strict table parser.

`heal guide|bash form/scripts/fourth-arm-survey.sh` was run through the real heal
dispatcher. It returned the native preparation door, witness and
`docs/native-source-preparation.md`. No Python or substitute helper-language
computation was introduced. Go remains the existing proof carrier.

## Observations retained

The native preparation band returned **255**; native authoring guide **4095**;
the existing heal-guide integration band **7**. Their preflights reported zero
errors, warnings and unresolved calls. Process exits were 0.
The final drift gate returned **8191/8191**, refused **0**, exit 0; Git's
whitespace check passed. These passing checks do not erase the server failure.

The final isolated filesystem/compiler witness returned **1023**, exit 0. Its
actual per-stage JSON, stdout and stderr live in
[the evidence directory](2026-09-09-native-compiler-cache-evidence).
The local fixtures remain in `.hearth/native-source-cache-57999`.

| Observation | Preparation ms | Actual result |
| --- | ---: | --- |
| Cold native compiler | 903 | execution 42 |
| Warm same source | 36 | identical prepared bytes |
| Source edited | 36 | execution changed to 43 |
| Compiler dependency edited with image retained | 471 | new compiler marker; execution 43 |
| Missing source | 39 | source-unavailable, final 0 |
| Relocated FK import, wrong-owner value 999 planted | 39 | authored dependency executed 44 |
| Failed proof-input write | 38 | proof-input-write-failed, final 0 |
| Old source-only Go cache planted with answer 999 | 74 | real Go output 45; poison remained unchanged |
| Missing dependency | 39 | lowering-unavailable, final 0 |
| Relocated BML prelude | 38 | execution 46 |

These are single stage observations, not hardware-floor benchmarks. The native
image cache is retained; removing the Go proof cache trades proof-side reuse for
current compiler meaning. The poisoned-cache JSON includes its actual key, path,
compiler root, retained bytes and execution output.

The real survey script's owned BML fixture returned `bml-pass`, Go **45**, table
**45**, process exit 0. Its missing-source fixture returned
`single-source-preparation-failed`, process exit 1, with the reason retained.
These are explicitly fixture rows, not a whole-tree survey. The [framebuffer
choice](2026-09-09-native-compiler-cache-evidence/choice.json) records the offered
absence/fixture paths, selected action 5, applied input and re-observed execution.

## What the attempts corrected and what remains

The early helper used a nonexistent `fs-path-parent`; preflight exposed it and
the existing `fs-path-dirname` replaced it. An early witness misread
`host_capture`'s stdout return and could not create its directory. It was fixed.
The first poisoned-cache setup did not fully isolate the Go compiler roots; its
result was insufficient. The final witness copies the two additional proof
roots and records the actual cache location. Absolute paths in the native
manifest also required rebasing into the isolated body.

The survey exposed the stale table bootstrap, a native compiler invocation from
the wrong directory, and a nonportable temporary-file template. All three were
repaired before the passing end-to-end fixture. The strict table parser then
exposed the extra `null`; correcting the serializer caller produced 45/45.

The broader Go server regression was initially red. Exact command, from
`form/form-kernel-go`:

```text
form-run go test -run '^TestSubstrateFormCompilerRouteRunsBML$' -count=1 .
sourceCompileServeProgram: source-compile: apps/coherence-network/api.bml [form.bml]: source compiler panic: walk: unbound function "fol-cat"
FAIL form-kernel-go 0.600s
```

An overlay of unchanged HEAD `main.go` reproduced the same failure, exit 1
(0.528s). A real attempt to use the declared dependency closure in both server
compiler entry points passed that missing binding, then failed after 35.47s:
`buildGoServeWorker: form binary: maximum node depth exceeded`.
That first `server.go` attempt was reverted. The continued movement restored the
closure repair, moved FORMBIN2 traversal into resumable native Form, and repaired
the API definition that decoding then exposed. The original targeted test now
passes; the subsequent receipt contains its timing and scope. Neither receipt
claims the entire Go server suite was run.

The independent old caches in `validate.sh` and the optional fourth-arm text
adapter also remain outside this movement. This repair closes the survey and
Go BML-prelude cache paths it actually witnesses.

The self-panel read **orphans 0**, with **11/12** lanes unobserved because no
hearth stood. Native authoring remained **26** Python implementations, **325**
execution candidates, **50** grammar inputs and **0** unread paths; its full
reading is retained. The session meter read **591773** output tokens at byte
**41937995**, a session counter rather than this change's cost. Share discovery
withheld its percentage. No training or model throughput claim follows.

The surprising teaching was that cache correctness also depends on preserving
the source's location. The difficult part became useful when a passing poison
fixture proved too weak and was rebuilt around the actual compiler root. We
kept the work alive by correcting the design, executing its failure paths and
carrying the unresolved server boundary honestly.
