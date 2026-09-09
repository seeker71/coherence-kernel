# Form owns JIT; the host compilers are released

Urs asked to remove Go JIT and every non-Form JIT fallback. Signed: Codex.

Removed the Go source/plugin compilers (typed and shared-Value), their C-source
projection, plugin ABI and recursive value conversions; the Rust source/cdylib
compiler and libloading dependency; and the TypeScript recipe and numeric
`new Function` compilers. Automatic host compilation and its counters are gone.
The remaining Go MAP_JIT and library loaders only admit Form-emitted images.
The seed runtime remains the checkout carrier; this movement adds no C meaning.

The native route, current-source guide and repeatable witness are executable:

```sh
./fkwu observe/native-jit-guide-run.fk
./fkwu observe/native-jit-retirement-witness-run.fk
```

The shared native authoring guide links these doors. The proof interpreters
still prove portability. Their native-alias compatibility API performs no
compilation. The old TypeScript compiled options name the fkwu door and refuse
with exit 2. The former installation script and validator benchmark option
execute the native Form witness. Obsolete compiler-specific examples and tests
are removed; recursion, list, helper, IEEE/string/vector and binding behavior
remain covered. The independent emitted-leaf check is named separately from
those result assertions; it is not a claim that every function acquired a JIT
entry. See [native JIT routing](../docs/native-jit-routing.md).

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
framebuffer/timing measurements and no longer publish deleted plugin counters.

## Released artifacts and boundaries

Form inspected **4,676** temporary plugin directories. Every admitted directory's
Go module identifies this coherence-kernel body; **0** needed separate ownership
inspection. Removed all 4,676 and the retired durable plugin cache. Re-observation
finds **0** of those temporary directories present and the durable cache absent.
Before removal, `du -sk` reported **580,256 KiB** temporary plus **1,775,864 KiB**
durable allocation: **2,356,120 KiB** combined. This is the measured pre-removal
allocation, not a claim about snapshot retention or globally freed disk blocks.
The full local ownership list remains in `.hearth/retired-jit-owned.nul`.

The current JIT source guide read **94 files**, **0 candidates**, **0 unread**.
Its signatures and scope are visible in the Form source. It is a bounded static
reading, not a claim about arbitrary old processes or every possible compiler.
Historical receipts remain evidence. Existing foreign-language proof emitters
are not promoted into a runtime fallback. No Python interpreter was used.

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

The surprising teaching was that the copying boundary belonged to an obsolete
plugin ABI. Removing that path removed its conversions. The uncomfortable
diagnostic mismatch became a stricter observation boundary: equal printed
numbers can never excuse failed processes. This exchange stayed alive by making
the requested removal, carrying the failures through repair, and keeping the
native evidence available for the next movement.
