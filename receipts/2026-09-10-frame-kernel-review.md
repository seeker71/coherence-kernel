# Form owns the frame; the kernel gives back seven functions

Urs asked for an AI review board, a clear OS north star and a merge that continues the reduction. This movement takes one interpretation island out of the bootstrap and gives its replacement real hardware execution and resource ownership.

The [north star](../docs/fkwu-form-native-north-star.md) now names the complete OS capability/dependency map, the hosted floor, the historical i386 boot witness, and the next five implementation movements. Platform ABI obligations endure; C is a temporary implementation language. Policies may expire or retire while already-submitted work keeps its lifetime obligations.

## Board and response

Form dispatched the existing board catalog through the existing native process supervisor. Each seat had a separate private directory, retained stdout/stderr, actual child status and cleanup observation. The [exact design packet](evidence/2026-09-10-frame-kernel-review/design-packet.md), [Grok review](evidence/2026-09-10-frame-kernel-review/grok-review.md) and [process record](evidence/2026-09-10-frame-kernel-review/grok-process.json) are retained.

The committed review removes six Markdown trailing-space line breaks to satisfy the whitespace gate; wording is unchanged. The process record points to the original private stdout.

Grok completed with status 0 and **REVISE**. Claude returned status 1, `Not logged in · Please run /login`; Cursor returned status 1, `Authentication required`; the installed Codex CLI returned status 1 because its configured `gpt-6-astra` requires a newer CLI. These seats supplied no review. Two independent Codex agents in the current session reviewed architecture and lifetime/performance. Both initially requested revision. They are two reviews in the same model family, not two additional model providers.

The initial Form review driver's unsupported `println` caused a parent compilation error even though child launches proceeded. Its child records therefore matter: Grok's actual status was 0, while the parent exited 1. The output call was repaired to `print_str`; the corrected driver's closed-seat probe compiles and refuses without making a model call. No parent green verdict was inferred from those first launches.

| Located review finding | Implemented response |
| --- | --- |
| Removing C in favor of an interpreter alone would sacrifice the requested performance direction. | Retired the C implementation only after a Form-generated Metal reduction matched the reference and measured comparable warm fixture latency. |
| A disconnected lease model would not govern real resources. | Form now owns real captured buffers, output buffers, program/policy snapshots, submission identities and actual Metal fences. |
| Global adapter drains could be mistaken for independent hardware contexts. | The implementation and north star explicitly require cooperative serialized access. Every owned fence is waited before readback. |
| Timeout lost fence identity, and the seed erased negative status. | Adapter timeout retains its in-flight fence; seed and generated wrapper preserve `-1`. A later wait observes the same submission. |
| Fence completion omitted GPU timestamp accounting. | Successful/error terminal fence retirement adds its GPU interval exactly once. |
| NoCopy refusal could substitute a copied buffer. | Removed that branch; mapped allocation remains page-aligned while its exposed view remains exact. Refusal releases the mapping. |
| Extreme dimensions and GPU descriptors could overflow or narrow. | Form validates before aggregate multiplication and passes offsets, row pitch and count as low/high words reconstructed at wide width. |
| The failure witness initially stopped in `mj-enqueue` before touching the adapter. | A positive invalid pipeline now reaches adapter rejection; a stale buffer also reaches half-open encoder cleanup. |
| An OS map could erase an existing boot floor or imply completed Form-native boot. | The map identifies the historical C/assembly i386 guest and the still-unbuilt guest ABI/Form-emission bridge. |

The lifetime reviewer independently checked the corrected source and returned **ACCEPT for the stated cooperative ownership floor**, with no remaining blocking finding. The architecture reviewer authored the updated north star and checked its diff. The primary agent ran the implementation witnesses and remains responsible for the merge.

## Exact ownership change

`runtime/fkwu-uni.c` loses **243 lines net** and seven function definitions: `fk_rd32`, `fk_rd16`, `fk_srd32`, `fk_frame_load`, `fk_frame_stat`, `fk_frame_read`, `fk_sense_stream`. No new runtime mechanism was added to C. Tags 213 and 214 remain explicit retired-operation errors and are removed from the source registry and effect/crossing mirrors. The source registry is regenerated from its Form authority.

The [new full lexical audit](evidence/2026-09-10-frame-kernel-review/c-bootstrap-audit.json) contains **642 functions**, **657 declaration units**, **374 distinct global names**, **58 local-static declaration units**, **306 directives** and **zero unresolved top-level units**. Active Clang reconciliation counts **561 functions** and **365 globals**. The same expected header-only entries remain outside the raw-source census: `__sigbits` and the four generated opcode-table names. Global ownership has not yet been reduced by this movement.

Form now owns BMP validation, policy thresholds, the CPU reference fold, MSL generation, wide descriptors, reduction and job lifecycle in `frame-luma.bml` and `frame-metal.bml`. Fixtures share one Form home. A capture uploads once and several jobs can use it. Publication changes subsequent submissions; retired versions remain attached to their earlier jobs. Cancellation drains and discards. Indeterminate results never become successful after aggregate cleanup. Job outputs release before captured inputs, and each release is recorded only after the carrier confirms it.

Captured buffers remain allocated until `fg-close`. The API is cooperative: mutable record identity and registry membership do not provide protected context isolation. Multiple logical owners can be created; callers must serialize adapter use. Metal pipeline objects still live until process exit. Reads/writes/free still rely on global adapter quiescence. This is a real async job owner with those limits, not a completed OS resource manager.

The mapped-buffer contract follows Apple's requirement for a [page-aligned NoCopy memory region](https://developer.apple.com/documentation/metal/mtldevice/makebuffer%28bytesnocopy%3Alength%3Aoptions%3Adeallocator%3A%29?language=objc). Exact view offsets/lengths remain separately checked; file padding is not exposed as requested data. Both private and explicitly shared file semantics are exercised on this host.

## Comparative performance

All three paths used the same synthetic **1,050,678-byte** 1024×342 BMP. The exact result was `[0,0,120,0,120,120,120,1024,342]`; the retired C door printed this row and returned mean 120.

| Observation | Milliseconds |
| --- | ---: |
| Retired C decoder, including file read and diagnostic output | 5 |
| Form interpreter reference over already-captured bytes | 1562 |
| Form-generated Metal admission/compilation | 187 |
| First GPU capture/submit/wait/read/reduction | 25 |
| Second / third GPU executions | 5 / 4 |

These are local stage samples, with different explicitly stated I/O scopes. They are not a throughput distribution, a CPU-native compiler witness, or maximum-bandwidth evidence. Each GPU execution in this sample uploads 1,050,678 bytes and reads 342 eight-u32 partials (10,944 bytes); policy A/B can share one upload. The benchmark runner also reads actual dispatch counters and reports control/status crossings separately. The carrier's internal string copies are not measured by these logical transfer counts.

The [second run with counter sampling](evidence/2026-09-10-frame-kernel-review/benchmark.txt) measured admission **240 ms** and execution **13/7/8 ms**. Each job returned the same exact row, completed, and closed successfully. Its dispatch deltas were `[0,2,1,2,1,1,1,2,4]` in compile/alloc/write/enqueue/submit/fence/read/free/status order; GPU busy time across three jobs was **7915 μs**. Both warm samples remain evidence: **4–8 ms** across these runs, without a claim of universal superiority to the 5 ms C sample.

## Verification and remaining floor

The reproducible door is `form-run ./fkwu observe/frame-kernel-witness-run.fk`. It checks child exit status as well as exact output and retains process evidence. [checks.json](evidence/2026-09-10-frame-kernel-review/checks.json) holds the resulting rows. Core frame reference/file witnesses expect 127; real async jobs 65535; shape comparison 1023; timeout/cancellation 511; real refusal and stale-handle cleanup 255; mapped views 255. Missing GPU capability produces explicit unavailable results rather than selecting the CPU implementation.

Old pre-retirement `.fkb` images for both removed operations were actually probed and refused with exit 2 by compiler-image identity. New source calls are no longer bound. Retired numeric slots also carry explicit errors in the seed and generated carrier, so they are not recycled into other behavior.

The final thirteen-stage witness passed every expected output with child status 0. The first complete run had only freshness fail: **15**, against required **31**, after rebase refreshed the source timestamp. Rebuilding the root from `runtime/fkwu-uni.c` restored 31; all thirteen stages then passed. A bounded framebuffer exchange selected rehearsal and re-observed 31. [Boundary evidence](evidence/2026-09-10-frame-kernel-review/boundary-checks.txt) also retains actual new-source refusal for each retired name (status 1) and a deliberately absent dynamic Metal carrier (version and owner both unavailable, status 0).

Both generated bootstrap artifacts were refreshed through Form. The CLI generation has stamp `6b85bd7390b15045`, 5311 functions and 195631 nodes. A direct standalone link attempt omitted the generated genesis source and failed with undefined `fk_genesis`/`fk_genesis_len`; the canonical builder supplied that required source, verified the generation identities, and built successfully. Its `ping` returned `pong`; `metal-fixture` returned `PASS fkwu-form-cli-metal-direct` on Apple M4 Max with `metal_carrier=dynamic`. `otool -L` lists only libSystem for both the root and generated CLI executable. Effect-grammar **32767** and membrane-census **8191** passed with clean preflights.

The bounded glass observed its first frame in **346 ms** with **33 kernel operations**. Counsel reported **0 orphans** and **11/12 judged lanes unobserved** because no hearth resident stands. The closing native guide read every candidate file and reported **25 Python implementations / 307 execution candidates** still elsewhere in the tree; this movement's implementation, review driver, measurements and audit run in Form.

The verified lifetime teaching returned through the native session-learning door with stable event `verified-async-ownership-teaching-v1`. Its worker completed with status 0, pending 0 and retained buffers 0. One local round advanced the existing optimizer to step 2. The candidate was deferred, promotions remained 0, and the serving state was unchanged. Evaluation fixtures were not supplied as teaching. The share meter reported `kind=declared`, `scope=unmeasured`, with no bound rollout; percentage remains withheld.

The required drift gates passed **8191/8191**, refused 0. The movement was rebased onto the concurrently advancing main line; the CLI source digest remained valid after the final upstream integration. Source timestamps renewed by rebase were answered by the prescribed freshness check and local rebuild, with no change to the required verdict.

Allocation-failure injection, exhaustive memory pressure, pipeline destruction, independent per-buffer overlap, protected multiple contexts, CPU-native pixel specialization and a Form-native freestanding OS remain outside this witness. The next work is ordered in the north star and starts with per-buffer completion dependencies.

The surprise was that the 1.56-second reference gap could be crossed by Form-generated GPU work without adding a C decoder or lease table. The productive discomfort was the timeout and negative-path tests: they made lost identity and a locally refused test visible, then required actual repairs. The exchange stayed alive through independent review, real comparative execution and deletion of the old interpretation.

— Codex
