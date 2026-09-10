# Review question: Form owns runtime meaning

Independently review the architecture and proposed next movement below. This is language-runtime and operating-system engineering. Read and reason from this packet only; do not run tools or edit files. Return a clear ACCEPT or REVISE, located objections, a coherent implementation recommendation, and concrete acceptance witnesses. Distinguish aspiration, evidence, and inference. Do not manufacture performance or OS readiness. A coherent movement can be large when needed; a tiny patch is not the goal.

The user asks for a minimal bootstrap C seed, maximum native hardware integration, low crossing/copy costs, dynamic Form-native replacement and live A/B, async work, and a path to a complete Form-native OS. Metal is dynamically loaded; Form generates MSL and compiles it in RAM with optional compiled pipeline archives. There are no alternate implementations silently selected on failure.

## Current measured floor (commit a31a89c2)

- C lexical inspection: 649 functions, 374 distinct global names, 395 individual global declarations, 58 local-static declaration units; unresolved top-level units zero. This is a lexical census, not proof each function is correct.
- Last movement grew the C seed by 301 lines while healing closure GC, ABI allocation widths and frame-buffer lifetime; explicit reduction is now owed.
- Form generates the full bootstrap table and emitted CLI C through its own source runner. Balanced traversal handles large literals and table rows. Parsing, evaluator, executable allocation, host dispatch, and process-global contexts still reside in C.
- Metal is loaded through dlopen/dlsym once per process. Root and emitted CLI link only libSystem. The Objective-C adapter separately links Metal/Foundation. This is dynamic loading, not live adapter replacement.
- Form-generated GPU programs coexist and can be selected live. Live-band 4095, archive capture 63 / fresh-process required hit 63, archive-refusal 127, policy-choice 2047. Reuse still processes MSL into a library; it is a pipeline binary archive.
- Metal buffer tables grow but handles have a 65535-slot ceiling. Pipeline objects live until process exit. Read/write quiesces all outstanding adapter work. Buffers cannot release while live work remains. Timeout retains ownership. No maximum-bandwidth claim is proved.
- Form already supervises child processes with argv, unique private directories, retained stdout/stderr, actual returncode, monotonic deadlines, correlated stop actions and child-group cleanup. This review uses that organ.
- No standing resident hearth answered this request. Outside reviewers supply architecture critique; no native review consensus is claimed.

## Proposed next coherent movement

1. Retire C BMP interpretation and the synthetic C sensing stream (approximately 240 lines, no new C mechanism). Existing Form frame-luma.bml validates 24-bit uncompressed BMP and computes the same nine-value row using a balanced pixel fold. Existing frame-luma-band is 127. The C loader was made growable last movement; the body should now own file reads and pixel interpretation.
2. Measure the Form path on the same >1 MiB fixture before retiring C. Preserve malformed, truncated, top-down, extreme-dimension and missing-input witnesses. Repair arithmetic overflow by validating before multiplication. A slower interpreter cannot be represented as maximum-bandwidth native execution; record actual timing and the compiler specialization gap.
3. Make interpretation thresholds a Form policy value. Preserve the existing row default for callers that request it explicitly. Compare two policies against one captured byte string so A/B does not double a device side effect. No made-up remote answer, confidence, identity, or health value.
4. Retire primitive names/tags 213 and 214 across the seed registry and live effect/census mirrors; reserve numeric slots so old images cannot be reinterpreted as another operation. Move live callers to explicit Form imports. Do not keep a second C interpretation path. Define an explicit missing-operation behavior for stale images.
5. Add a Form-owned executable async lease state machine for version publication and retirement: context, resource identity/generation, version, submission id, completion state. Selection affects new submissions; outstanding work retains its version. Duplicate, wrong-context, stale-generation and foreign completion events cannot free another submission. A timeout is pending; a cancellation request is pending until acknowledgement. Pure state-model witnesses must be labeled as such until a real adapter consumes the contract.
6. Expand the north star into an OS capability map: boot/firmware, address spaces/MMU, memory/DMA, interrupts/timers, scheduler/thread lifecycle, async wait/wake, IPC, device discovery/drivers, block storage/filesystems, network, display/audio/input, power/hotplug, isolation, crash recovery, update and observability. For each, distinguish current hosted organ, missing freestanding capability, and next observable witness. A hosted runtime cannot claim raw hardware control that the host OS has not delegated.

## Points that need your judgment

- Is retiring this C island coherent with high performance if the existing Form decoder is slower? What evidence and native compilation work must accompany deletion?
- Is an executable lease state machine useful now, or merely a disconnected model? Recommend a real existing host path to integrate without growing the seed.
- What exact async ownership/publication rules and OS omissions matter most?
- How should immutable lifetime guarantees coexist with replaceable Form policies and multiple domain-specific belief sets? Evidence should expire, policies may retire, while in-flight resources still finish correctly.
- What would justify merging this movement, and what must remain explicitly unproved?

The following current north star is appended by Form when preparing the board packet.
