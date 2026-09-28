# Native RAM pipe workers

Form owns pipe creation, descriptor flags, readiness subscriptions, writes, process
admission, framing, cancellation and release on Darwin ARM64. The native
instructions are emitted and admitted in RAM. No C source change is required.

[`native-pipe-darwin.bml`](../form/form-stdlib/bml/native-pipe-darwin.bml)
owns the parent. [`native-pipe-worker.bml`](../form/form-stdlib/bml/native-pipe-worker.bml)
receives complete frames and invokes a caller-supplied Form function. A fixed
worker program serves repeated requests; source and request files are not
generated per request.

## Bytes, work and lifetime

A frame consists of an eight-byte little-endian payload length followed by
exactly that many bytes. Empty and binary payloads are valid. Admission checks
the actual nonnegative tagged-integer range before accepting a length.
There is no line-length ceiling on the message. Balanced byte bins assemble
partial frames; offset writes borrow the same immutable request until every
byte is accepted.

`npp-open`, `npp-spawn`, `npp-submit`, `npp-pump`, `npp-take`,
`npp-half-close`, `npp-done` and `npp-close` form the parent lifecycle.
`nppc-run-with(handler)` supplies each complete payload to
`handler(owner, stdoutEnd, stderrEnd, payload)`. Callbacks execute synchronously
inside their worker; the parent multiplexes independent workers and streams.

All original pipe descriptors carry close-on-exec. Parent endpoints are
nonblocking; child standard streams remain blocking. Partial writes,
interruption, would-block and broken-pipe results retain their actual status.
Readiness stays registered in the [native event queue](native-resource-events.md).
An event supplies the queued byte extent and EOF; the owner reads one chunk
per ready stream in the batch. Level readiness carries the remaining bytes
into the next turn. Idle input has no write subscription, and a partial write
waits for capacity without repeating unsuccessful writes. The 65,536-byte read chunk is
an operation size, not a message limit. The existing argv spawn carrier admits
at most 32 arguments of at most 1,023 bytes each; work payloads travel separately
through the pipe.

Completion requires both process reaping and drained stdout/stderr EOF. A
child-exit subscription prompts the actual reap; a quiet wait never probes
every process. Admission re-observes exit after registration to cover an early
child exit. Stream closure alone leaves the process obligation alive.

`npp-cancel(owner, job, graceMilliseconds)` closes input, requests termination
and retains an asynchronous cancellation. Its one-shot timer escalates when
the child remains alive. Other work continues through the queue while output
drains. Repeated cancellation preserves the first grace interval and completed
effects. `npp-stop-for` additionally observes settlement for a caller-selected
duration; `npp-stop` uses the shared defaults. Exhausting that observation
leaves ownership intact. Confirmed output remains readable after
owner retirement. Incomplete settlement retains ownership for another attempt.
Failed managed writes retain their original bytes, accepted offset and error;
the job's organ reading carries the need into local care.
Worker refusal diagnostics stay on stderr through private resource release;
inherited standard streams close only after the final cleanup observation.

`npp-open-with(events)` borrows a shared event owner. `nve-turn` can then advance
pipe I/O, child exit, cancellation and JIT cache care together; `npp-pump` uses
that same dispatch. Closing a pipe owner removes its subscriptions and releases
its processes and endpoints, preserving the shared queue and other consumers.
Close consumers before closing the queue. `npp-open` owns a private queue when
one is not supplied. Wait durations use native Form integers; the poll API's
32-bit millisecond ceiling no longer defines this path.

The owner is cooperative. Its records are not protected capabilities. Native
readiness and its queued-byte extent establish pipe EOF independently; the
existing `file_read` carrier does not expose its underlying errno. Supervision
covers the direct child. A descendant retaining inherited streams can prevent
EOF and leave cancellation unsettled. Other CPU/OS targets, reusable TLS
connections and preemptive Form callbacks need their own implementation and
execution witnesses.

## Translation work

[`concept-translation-worker-run.bml`](../observe/concept-translation-worker-run.bml)
is a fixed framed worker. Its exact JSON request shapes are:

| Operation | Required fields |
| --- | --- |
| `prepare` | `id`, `operation`, `labels`, `target`, `maxRows`, `maxUnits` |
| `decode` | `id`, `operation`, `payload`, `expected`, `requireNonempty` |
| `translate` | The `prepare` fields plus `maxAttempts`, `delayMs`, `retryTransport`, `requireNonempty` |

All fields are required for the selected operation; duplicate and unknown
fields refuse. Numeric lexemes are admitted before JSON integer conversion.
Retry arithmetic is checked against the sleep carrier's nanosecond deadline.
Results carry schema `form-translation-work-v1`, the admitted request identity,
`ok`, `kind` and `value`. An inadmissible request returns a framed refusal with
an empty identity. Health events carry sizes, response kind and admission
state on stderr; labels and translated content stay out of diagnostics.

A terminal transport or provider refusal emits correlated stderr diagnostics,
explicitly retires the pipe owner's native resources, closes inherited streams
and exits nonzero without a result frame. The parent retains that failure and
settles its own endpoints; it does not interpret the absent frame as success.

Preparation preserves label order and UTF-16 batching semantics, with complete
form-query byte encoding. Decoding checks complete UTF-8 JSON, ordered segments
and row cardinality. A multirow mismatch returns an explicit balanced split;
a singleton mismatch refuses. Cell normalization and empty-cell admission are
explicit. Stored translations are decoder inputs, not evidence of a new
provider response.

The transport sends a complete Form-built request through the existing dynamic
TLS carrier. Form interprets fixed-length and chunked HTTP responses, including
interim responses, extension grammar and trailers. Complete framing must account
for every received byte. Unknown content encodings and close-delimited bodies
refuse because this carrier cannot attest an orderly read end. The parser's
framing follows [HTTP/1.1](https://www.rfc-editor.org/rfc/rfc9112.html);
query encoding follows the [form URL format](https://url.spec.whatwg.org/#application/x-www-form-urlencoded).

The observed live translation attempt returned HTTP 429 on all five requests
and refused without publication. [Pinned Wiktionary acquisition](native-wiktionary-source.md)
now runs in Form and reproduces all 111 retained source revisions. Fresh
selection, rights acquisition, translation and new-generation publication remain
active JavaScript responsibilities until their complete native executions are
observed.

## Executable observations

The [current evidence](evidence/fkwu/native-pipe-translation.json) binds public
source hashes to actual exits, stderr and complete observations. The witnesses
are `observe/native-pipe-{duplex,concurrency,boundary}-witness.bml`,
`observe/native-http-{response-boundary,framing}-witness.bml`, and
`observe/concept-translation-{admission,pipe,refusal}-witness.bml`.
`observe/concept-translation-witness-run.bml` checks all 10,000 current labels
and all 120,000 retained machine-language cells.

The three-worker corpus witness prepares 30,000 labels and decodes 30,000
retained cells over two resident rounds. It checks exact result frames through
final EOF, ordered and correlated health stages, actual exit status, physical
descriptor release and native owner retirement. These observations establish
the exercised bulk message path. They do not establish maximum bandwidth or
complete OS process isolation.

Worker image admission and message execution have distinct diagnostics. A new
seed build can emit cache-renewal warnings before the worker's Form entry runs.
The verification records image admission through owned pipe EOF, retains those
diagnostic bytes and confirms process release. The subsequent message witnesses
still require exact result and diagnostic streams; no warning is discarded to
make a protocol check pass.
