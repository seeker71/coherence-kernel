# Native Linux dispatch

The existing CPU-image JIT door admits explicit `linux-x86_64` images on the
production Linux ISA. Untagged Darwin ARM64 images and foreign ABI tags remain
unavailable there, rather than being executed as the wrong ISA. The seed's only
movement is page allocation/admission for that existing door, plus two upstream
Linux bootstrap repairs: its early allocation uses the already declared
`fk_size_t`, and the ARM64 admission tombstone reset stays in its own carrier.
No C socket, poll, fork, dispatch, protocol or application implementation was added.

`native-linux.bml` owns one reusable Form-emitted syscall image. Syscall number,
six scalar/pointer positions and borrowed/writable spans are data, so another
operation does not create another native image or host wrapper. Signed errors,
zero, EOF and absent admission remain distinct. The host's ELF header establishes
the ISA before execution; the PID observation checks actual execution. The
syscall ABI and number data follow Linux syscall(2) and its x86 syscall table.

`native-socket-dispatch.bml` owns readiness, accepted streams, private forked
request contexts, response backpressure, completion events and release. Its
caller supplies the response cell. The listener closes its copy of a handed-off
stream; a child releases the inherited listener, other peers and pidfds before
calling that cell. A pidfd wakes the parent to reap completion. No periodic poke,
replica, host server, generated executable, request file or text-serialization
bridge serves dispatch. Use private Form nodes (`FK_FIELD_OFF 1`) for the
copy-on-write request contexts; durable state remains the application's own
publication/checkpoint protocol, not inherited mutable shared state.

## Observed

The focused Linux RAM observation returned the actual PID, refused untagged and
foreign ISA images, transferred `whole-native-packet`, distinguished ready/idle
polling and errno -11, and closed all three owned sockets. Linux source was built
with the production GCC 12.5.0/bookworm image; the shared page boundary also builds
on this Mac. No broad kernel suite or bootstrap comparisons ran.

The first Linux build named two existing upstream errors precisely:
`fk_sboard_count: size_t undeclared` before the platform headers, and
`fk_cell_run_door: fk_inram_node_released undeclared` outside its ARM64 carrier.
The source repairs above restored the same build rather than changing its flags.

An initial application attempt used asynchronous file-backed tasks. A three-way
archive/document/health observation showed health 1234 ms, document 3088 ms and
archive 17689 ms: no archive serialization, but fresh process/source admission
remained a membrane cost. That attempt was retired before landing. The Form-JIT
fork replacement retains the admitted cells and held values instead: the same
private archive wave returned health in 73 ms (48 ms serving), document in
4013 ms and the full archive in 19127 ms. The heavy reads share physical capacity;
the result establishes independent progress, not a claim that every latency is
green. Exact private captures remain under the application's native I/O trace.

The archive holds 6339890 rehearsal samples; it is neither truncated nor replaced.
Production has not been patched in place. The application dependency lands and
deploys after this kernel dependency. OpenAI Codex supplied rented reasoning;
Form supplied the native lookup, execution and observations. The exact model and
task-token count are not exposed; no native generation share is inferred.
