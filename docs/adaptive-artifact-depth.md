# Adaptive artifact traversal

The Go FORMBIN2 proof reader sends its artifact to a resident native process for
that decode: `./fkwu observe/formbin-depth-native-run.fk`. Form owns traversal,
continuations and attention in `form/form-stdlib/bml/formbin-depth.bml`. Go carries
bytes and postorder node rows, interns physical NodeIDs and reports its measured
time and memory activity. The body runs on fkwu throughout this path.

Depth is an observation. A linked continuation holds open composites, including
their remaining children. Both descent and completion can yield between slices.
A deeper valid artifact advances the attention watermark; it is never rejected
because it crossed a depth number. Existing byte, node and child-count format
checks still reject malformed or oversized input. Legacy FORMBIN1/raw decoders
and the other proof readers have not been migrated by this change.

The initial native policy offers 256 operations, a depth watermark of 32 and a
50 ms attention target. These are starting observations, not depth limits or a
total-work budget. A slow slice halves the next quantum and yields; a slice
below one quarter of the target doubles it; otherwise it continues. Actual GC
activity requests attention, but only measured delay requests smaller slices.
The continuation survives each choice. There is no aggregate duration timeout.

Each decode prints its actual `formbin-depth-*.jsonl` evidence path to stderr.
Every slice row carries cursor, node count, current/peak depth, native PID, native
slice milliseconds, carrier elapsed time, control microseconds, offered and
selected actions, next quantum/watermark, and whether the action was applied.
An initial lifecycle row makes startup visible before admission. The first slice
includes setup and temporary-input preparation time; a final lifecycle row
records completion or the actual error. The framebuffer
exchange correlates the outgoing observation with native control; JSON rows
retain every round even when attributed framebuffer nodes share an identity.

Memory and GC fields prefixed `carrier_` measure the Go process only. They do
not claim native heap coverage or system memory headroom. `elapsed_ms` covers
waiting for native rows and materializing them; `native_slice_ms` measures the
native traversal itself, so those intervals overlap and must not be added.
`control_us` includes the pipe exchange. `round_before_trace_us` excludes the
JSON write. The witness's `elapsed_ms` covers the entire child execution,
including evaluation after decoding; it is not decoder time. No model or LoRA
is involved in this decoder, and these timings do not establish a hardware floor.

The native process and its temporary input belong to the invocation. On return,
the carrier closes/reaps its process and removes the temporary input. Native
compiler/image freshness remains fkwu's responsibility. No persistent lowered
source or separate compiler-cache invalidation rule is introduced.

`heal guide|form binary: maximum node depth exceeded` names this implementation,
`observe/formbin-depth-witness-run.fk` and this reference. The next repair can
follow the real trace: find the last cursor/mode, compare elapsed time and
selected quantum in consecutive rows, preserve the continuation, then witness
the chosen path. GC count alone is insufficient evidence for shrinking work.

The pure band is `form/form-stdlib/tests/formbin-depth-band.fk` (2047). The native
witness authors depth 4096, exercises the real Go reader, requires output 42,
copies its complete trace, and checks strings, float64 and int64 roundtrips.
The shared malformed corpus now uses a genuinely truncated child-count field
in place of a structurally valid depth-257 artifact.

The integrated server compiler loads and hashes its declared transitive source
closure. The API's `api-presence-supported?` closing parenthesis is repaired so
subsequent handlers remain top-level definitions. The original route regression
now exercises all three repairs together.
