The Form owner in `form/form-stdlib/bml/native-fp64-matrix.bml` emits an ordered
binary64 MXFP8 matrix program and admits it directly in RAM on Darwin ARM64.
The architecture probe runs before raw image admission. No compiler process,
C change, disk executable, Metal admission or boxed intermediate is required.

`nfm-open()` creates a process-bound owner. `nfm-vector(values)` validates finite
numbers and packs one immutable binary64 input. `nfm-output(owner, rows)` owns an
anonymous writable map. `nfm-mx8-run(owner, data, scales, vector, rows, region)`
reuses these objects and returns the completed row count, or `nothing` when
admission refuses. The payload contains row-major E4M3 bytes; the separate E8M0
plane contains one scale per 32 payload bytes. All 256 byte codes retain their
arithmetic interpretation. Columns must be a positive multiple of 32. Extents
and row counts must be exact integers and the output span must belong to the
same active owner.

Each product and addition executes as a separate fp64 instruction. The loop
reduces 32 products in input order, multiplies that partial sum by its scale,
then adds groups in order. It uses no fused multiply-add. Packing and output
decoding happen outside that loop. Raw output preserves IEEE binary64 results;
`nfm-mx8` additionally refuses nonfinite decoded results after releasing its map.

The caller holds immutable input strings for the whole synchronous call. Mutable
output belongs only to the owned map. `npd-region-close(region)` physically
unmaps it; `nfm-close(owner)` refuses while any map remains live, then releases
or tombstones the program and closes its memory owner. A closed request cannot
readmit code. Ownership records are cooperative, process-local contracts.
Pointers have no independent lifetime, protected address space or asynchronous
lease. Reuse the admitted vector and map for repeated calls; the convenience
`nfm-mx8` allocates a fresh output map and decodes a Form list for each call.

Run `form-run ./fkwu observe/native-fp64-matrix-run.bml`. The executing organ
reports correlated arithmetic, residency and release observations. An independent
scalar implementation covers every E4M3 and E8M0 byte in a 32-by-256 matrix and
compares raw native output bytes before decoding. Across 256 repetitions, the
float pool, boxed-value count and owned-region count stay unchanged. The same
owner also has an observed real 1,024-by-4,096 matrix: all 8,192 output bytes
match the independent ordered reference. Its isolated 256-call observation
held one region and zero additional floats; sampled RSS changed from 169,232
to 169,248 KiB. These dimensions and measurements describe observations, not
admission ceilings or promised throughput.

The north star is one Form-owned numerical program that retains tensors and
activations in owned native memory across the whole computation. Additional
quantized formats, attention reductions, explicit concurrent leases and dynamic
program selection can then extend the same lifetime and evidence boundaries.
The [real-model DSV4 oracle](native-dsv4-oracle.md) extends this owner with mapped
GGUF input, F16/MXFP4/IQ2 matrix programs, MLA, hyperconnections, expert routing
and per-layer sequence state. Its independent consumer observations and current
acceptance boundary are recorded with that organ.
