# Native node accessor

Form owns a reusable native getter over its packed blueprint runs. The
[accessor authority](../form/form-stdlib/bml/native-node-accessor.bml) emits and
admits one ARM64 image in RAM. Layout changes supply a new descriptor to the
same entry point. No C source or per-layout getter recompilation is required.

The native ABI is `x0 = descriptor`, `x1 = row`, `x2 = field`; it returns the
complete raw unsigned 64-bit field in `x0` and availability in `x1`. Zero is a
valid field value. An unavailable row, field or retired descriptor returns
availability zero. The getter uses caller scratch registers and no callbacks,
allocation or tagged value conversion. It preserves the callee-saved registers.

`nra-project` supplies an executing consumer of that ABI. Queries are raw
64-bit row/field pairs. One Form entry processes a batch through native indirect
calls and writes complete raw words to an owned output span. Only the completed
prefix count returns through the seed's tagged JIT result. The first unavailable
query stops before writing its result or any later result. Bounds use unsigned
comparison, including indices with bit 63 set. A linked, 16-byte-aligned native
frame preserves the caller and makes the nested calls visible to stack readers.

Each descriptor contains a 64-byte header and 16 bytes per field. The header
holds its active version, row count, bit stride, field count, data address,
allocated extent, field-table address and local generation. Fields hold bit
offset and width. This metadata is shared across the run; it adds no bytes to
individual cells. It contains no movable tagged Form references. The getter
accepts descriptors constructed by this cooperative Form owner. The stored
extent describes the validated allocation; it does not validate arbitrary
foreign descriptor memory.

`nra-acquire` pins a blueprint run. Closing the run prevents new readers while
existing views retain the original bytes. Releasing the final reader unmaps a
retired run. The blueprint owner retains its code and memory owner until its
runs and outputs release. A released accessor view first invalidates its native
descriptor and then releases its data lease. The descriptor remains mapped
until the accessor closes, allowing saved native readers to receive an
unavailable result while that owner still stands.

Closing the accessor requires all views and ordinary outputs to release. A
refusal preserves the owner and its descriptors. An admitted close blocks new
calls, releases code, then releases descriptors and the entry slot. A failed
release retains a closing owner for retry. Native callers must finish before
releasing their view; no caller may use an entry or descriptor after owner
retirement begins. Publication and retirement are serialized in one process;
these are not concurrent atomic reader leases. Released descriptor mappings
accumulate until owner close, and the seed retains Form record metadata.

Unease carries the owner's actual live-view count, requested descriptor and
output bytes, native-call count and code state. The
[execution witness](../observe/native-node-accessor-witness.bml) correlates a
retirement response, retains and reads both pinned views, then observes their
continued ownership. It also verifies 64 field widths, 67 layouts, 68 views,
full unsigned word extrema, indexed refusal, output guards, immutable aliases,
retirement and a refused close followed by a saved native descriptor call.
The same 424-byte image and getter entry serve every layout in that execution.
Actual exits and held source identities live in
[blueprint evidence](evidence/fkwu/native-blueprint-layout.json).

The current host is Darwin ARM64. Exported code pointers must fit below 2^56
because the existing Form pointer-reading door returns seven bytes. Payloads
and results retain all 64 bits. Host mappings round to pages; descriptor
requests and payload sizes do not measure physical residency. Generic field
lookup removes per-field Form crossings but does not establish a throughput
advantage over a layout-specialized native projection.

The primary shared field still uses C arrays and a fixed 2^26-cell capacity.
Its semantic identity word, tagged runtime handle and physical slot are
different objects. The next migration is a Form-owned identity column and
stable directory consumed through native access, preserving existing handles.
Before replacing allocation, native admission must be resident so it cannot
recursively depend on the allocator it replaces. Reference-bearing category,
children and value columns need a root/relocation bridge: the current collector
marks and rewrites those arrays and cannot see arbitrary raw RAM. Slot reuse
also needs handle generations and ownership of native side tables. Those
boundaries remain part of the [north star](fkwu-form-native-north-star.md).
