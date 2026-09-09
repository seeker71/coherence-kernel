# Form-native execution witnessed

Signed: Codex, 2026-09-09. The Form compiler, native image carrier and proof
interpreters were witnessed together. The [current route](../docs/native-jit-routing.md)
names executable guidance and the repeatable witness. The checks below record
this dated run; current measurements come from executing those doors again.

## Observed checks

Eleven native cases pass with clean preflight and process exit 0: native image
execution/refusal **1**, recursion **8**, list calls **4**, helper calls **4**,
IEEE/string/vector span **255**, chained binding/list behavior **5**, lowering
**15**, repeated/interleaved native image calls **63**, direct-source discovery
**32767**, birth/reuse **32767**, and the actual CPU/Metal/MLX Form CLI **16383**.
Per-case preflight, stdout, stderr, status and duration live in
[the native evidence directory](2026-09-09-form-jit-only-evidence/native/).
These are complete process durations, not hardware-floor latency claims.

The Go Form-emitted ARM64/dylib tests and
`TestSubstrateFormCompilerRouteRunsBML` pass together (**34.849 s**, before the
upstream rebase). TypeScript `npm run proof` passes its browser proof and all
**7** Node host tests. Rust checks and formatting pass. Shared kernel
conformance passes; the full drift gate reads **8191/8191**, no refusals, after
the rebase. Binary freshness remains **31**. The primitive registry now matches
**210** actual natives: **181** in-band and **29** carrier-declared.

All three proof interpreters return **63** for the remaining primitive registry
and **1023** for host resources. Source and binary resource checks pass. Raw
streams and exits are retained under the evidence directory's `registry/` and
`binary/` subdirectories.

The registry run exposed stderr being concatenated into the compared value:
Go's depth receipts made correct results appear divergent. The validator now
compares result stdout, requires every process to exit successfully, and retains
both streams and statuses in `.hearth/validation-*`. A deliberate negative
witness prints **63 on all three kernels, then each exits 1**. Validation
correctly refuses it. Its source and all three streams/statuses are retained in
[refusal evidence](2026-09-09-form-jit-only-evidence/refusal/). No diagnostic is
erased to make the result green. API observation endpoints retain their actual
framebuffer and timing measurements.

Glass observed **15** native units refreshed in **1,779 ms**, with **6** dependent
images invalidated from source identity. Counsel reads **orphans 0**; **11/12**
service lanes remain unobserved because no hearth stands. The share reader is
**declared, percentage withheld**: its append range was not completely checked.
No native/local/remote percentage or resident-model throughput is inferred here.

The temporary native migration also refused malformed BML and an absent prelude.
An erroring source run executed later writes, so affected inputs were restored
from git before the corrected migration ran. A bounded framebuffer exchange
selected revision and re-observed migration/cleanup statuses **[0, 0]**. The
native result checks above followed that recovery.

The surprising teaching came from the uncomfortable diagnostic mismatch:
a stricter observation boundary was needed because equal printed
numbers can never excuse failed processes. This exchange stayed alive by making
the requested removal, carrying the failures through repair, and keeping the
native evidence available for the next movement.
