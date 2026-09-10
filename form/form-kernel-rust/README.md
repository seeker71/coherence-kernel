# form-kernel-rust

Rust is a Form proof sibling. It executes recipe trees and binary artifacts for independent comparison; the running body is `fkwu`. This directory's HTTP server is a local proof surface for route selection, Form handler execution, forwarded HTTP framing, and worker ownership.

From the repository root:

```sh
form-run cargo build --release --locked --manifest-path form/form-kernel-rust/Cargo.toml
form-run ./fkwu form/form-kernel-rust/router-native-run.bml
```

The observation runs entirely through Form/BML. Form starts the actual Rust proof server, owns native socket cursors, and serves the upstream fixtures itself. It contacts only `127.0.0.1`. The fixture's current `socket_listen` carrier binds a wildcard address; the Rust listeners explicitly bind loopback. No external API is contacted.

The final JSON reports the actual exit status, exact correlation between emitted health rows and the shared process reader, retained stdout/stderr byte counts, child releases, and process-group cleanup. `accepted: 1` requires every check and both ownership observations to agree. The printed private evidence directory retains each child log, request/connection count, observation, resource lease, and supervisor event. Failed expectations emit an offered `request-evidence` response and retain the exact observation before continuing toward cleanup. A failed execution exits nonzero.

| Executing boundary | Native observation |
| --- | --- |
| Route selection | Native hello, query decoding, input-dependent comma counts, float arithmetic, local 404, method-specific GET/POST selection, and forwarded path/query identity |
| Forwarded headers | Request authorization, cookie, accept, custom and content headers; rewritten host/framing; removed hop headers; response cookie/cache/custom/content headers; exact invitation fields on success and error |
| Request bodies | Form sum, JSON byte length, a 12,000-byte native argument crossing the 8KiB read buffer, explicit 20,000-byte native refusal, byte-identical forwarding above 1MiB, and declared request-shape refusal |
| Response framing | Every byte of 16MiB content-length, 2,304,000-byte chunked, 1,280,000-byte close-framed, and in-data chunk-terminator specimens; malformed and truncated peer responses are refused |
| Client connections | Eight distinct sequential requests, two pipelined requests, explicit close, HTTP/1.0 close, five-second idle release, an independently served neighbor, and thirty-request reuse/fresh timing |
| Upstream connections | Counted reuse versus close-each over 65 requests per arm, distinct framed results, and silent close every three requests with each request served exactly once |
| Async progress | An actually accepted upstream request held across the current 30-second deadline; twenty native neighbors complete while it is held; one 504, one upstream request/connection, and subsequent native service |
| Concurrent worker execution | Fifty requests enqueued on distinct owned sockets before any result is read; fifty distinct expected results; the same 600 requests of 4,000 commas in twelve waves against one and eight workers, preserving the 1.3× throughput requirement |
| Failure and release | Missing route admission exits nonzero with retained diagnostics; every admitted child is waited and absent afterward; the supervisor independently observes the process group released |

The Form client retains complete response bytes for exact comparison. This verifies HTTP streaming behavior and byte identity; it does not measure the server's peak resident memory. Socket reads currently block. The native process supervisor imposes a 180-second exercise deadline and releases the owned process group on completion or refusal. Ports selected for Rust have a close-then-bind interval; listener admission failure is visible rather than retried silently.

Timing uses whole monotonic milliseconds and includes the Form client's work. The concurrent samples run from send to ordered response drain, so their percentiles include that drain order. One-versus-eight worker throughput uses the complete identical workload wall time. Other latency comparisons are observations, not promises of a universal speedup.

The current Rust proof carrier admits at most 16KiB of native-handler input, holds a 64MiB request/response shape, and uses fixed 5-second connect, 30-second upstream read/write, 30-second whole-request, and 5-second idle deadlines. Environment variables named `COH_*` do not alter these current constants. The local refusal checks observe immediate connection refusal and an accepted request reaching its read deadline. They do not exercise an expiring TCP connect, a whole-request delivery timeout, or a blocked upstream write. The native observations do not claim unbounded input or a smaller response shape. Removing these carrier constraints belongs to the runtime's native ownership work.

`api/app/main.py` and `deploy/kernel-router/production-routes.fk` are absent from this checkout. Consequently this local proof makes no claim about FastAPI health/quote endpoints, promoted application-route value equality, production response identity, live attention/status projections, or application latency. Those claims require an actual application and its route authority. The legacy wire values `fanout-python` and `X-Form-Python-Fallback` remain protocol fields emitted by Rust; the upstream used here is Form.

The reusable implementation lives in [`router-proof-io.bml`](../form-stdlib/bml/router-proof-io.bml), [`router-proof-fixture.bml`](../form-stdlib/bml/router-proof-fixture.bml), [`router-proof-cases.bml`](../form-stdlib/bml/router-proof-cases.bml), [`router-proof-lifecycle.bml`](../form-stdlib/bml/router-proof-lifecycle.bml), and [`router-proof-runner.bml`](../form-stdlib/bml/router-proof-runner.bml). The three entrypoints in this directory carry process roles. Compiler checks can read the library without starting a listener:

```sh
printf '%s\n' form/form-stdlib/bml/router-proof-lifecycle.bml | form-run ./fkwu observe/preflight-stdin-run.fk
printf '%s\n' form/form-stdlib/bml/router-proof-runner.bml | form-run ./fkwu observe/preflight-stdin-run.fk
```

Recipe examples remain available as independent proof inputs:

```sh
form-run form/form-kernel-rust/target/release/form-kernel-rust form/form-samples/fact.fk
form-run form/form-kernel-rust/target/release/form-kernel-rust --expr '(add 2 (mul 3 4))'
```

Runtime direction: [`../kernel-roadmap.md`](../kernel-roadmap.md). Proof sibling: [`../form-kernel-go/`](../form-kernel-go/). Category identities: [`../category-contract.json`](../category-contract.json).
