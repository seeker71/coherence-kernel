# Form owns the tensor program and its resident buffers

Urs asked to integrate main, close JIT gaps, and move MLX work into native Form
on Metal. Codex rebased onto `437912526bf757cfce7a6ed97618f4e11aff9535`.
The in-process tensor engine now lives in `form/form-stdlib/native-tensor.bml`.
The 1,037-line MLX C carrier is removed; the checkout C seed shrinks by 82 lines.
The product binary links Metal and Foundation with no MLX library.

Form owns parsing, broadcasting, shape admission, shared buffer handles, Metal
source emission, quantized file mapping/decode, execution, and cleanup. Existing
CLI tensor recipes use this engine. The active route selector, primitive tables,
generated walkers, recovery inventory, live gauges and documentation follow it.
Tensor programs that need a caller-supplied NodeID recipe retain an explicit
native JIT choice; malformed tensors retain their own refusal.

The Metal scalar JIT previously returned only the first byte. Its result now
reads all four bytes with signed int32 meaning; the behavior checks include
3007 and -20 on CPU, Metal and tensor routes. Zero has a presence bit and is
distinct from failure. Each tensor result owns its diagnostic and per-stage
timing/counter rows, so a later operation cannot change what an earlier one said.

The most useful performance finding was buffer lifetime. In the final sample,
`268435456 iota sum i32` took 73 ms with pipeline reuse and fresh buffers.
Preparing a resident image took 75 ms. Its next three replays took 7, 6 and 5 ms,
with GPU command-buffer times of 6351, 5685 and 5305 microseconds. Each replay
executed six dispatches, admitted zero new buffers, and compiled zero pipelines.
Closing the image retained zero buffers. The lifecycle band overwrites the
output before replay and verifies recomputation. This measures this workload;
it establishes neither peak physical bandwidth nor a universal hardware floor.
The probe records offered/selected size changes and stops increasing pressure
when its observed warm latency or device working-set admission says to report.

The [evidence bundle](2026-09-10-native-tensors-evidence.json) retains source and
binary identities, individual preflight reports, stdout, stderr, exit statuses,
answers, stage timings, choices, and the failed view/fixture checks before repair.
Final code index tree: `3f362bc2cb37c3e765198d94eb02e445758c1a5e`.
Final binary Git object digest: `11246acf314719e3d0308ba975951aa34a2d2888`.
The receipt and evidence are subsequent additions to that exact code tree.

- 23 tensor/routing/view bands: all preflights clean, all executions exit 0,
  all expected answers matched.
- 11 native JIT bands: all preflights clean, all executions exit 0,
  all expected answers matched.
- 57 programs compared with the isolated MLX carrier built from the base commit:
  every result matched, including float, broadcasting, batched matmul, scalar
  edge cases, and TF32/Q8_0/Q4_K/Q6_K file inputs. The baseline process timings
  include startup and are not a speed comparison with an in-process native call.
- Native behavior also checks 18 named refusals; native image and lifecycle
  bands return 127 and 1023 respectively.
- Both regenerated C walkers pass `cc -fsyntax-only`. The committed CLI table
  and platform-binary attestations were not reissued; this is the direct fkwu
  source/BML lane, not a newly published standalone CLI bootstrap binary.
- Drift gates: **8191/8191**, no refused lane. An intermediate freshness reading
  of 15 followed a byte-preserving source rewrite; rebuilding restored 31.
- Glass startup identifies `tensor-owner=Form`. Counsel observed **orphans=0**;
  11 resident lanes have no standing hearth and carry no invented readings.

Boundaries still carried: external voice LoRA training and adapter inference in
`lora-voice.bml` still invoke MLX against the adapter's Llama 3.2 3B base. Reading
the native LoRA cells found head/activation training and reverse arithmetic;
they are not a compatible replacement for that adapter's full model and
quantization. This movement does not claim those processes, all model tokens,
or per-round LoRA updates have migrated. Literal resident tensor images replay;
file-backed images demand fresh file identity/extent admission before reuse.

The share reader reported `kind=declared`, withholding a completed-turn share.
It did expose the last completed provider call within the open task: 122058
input tokens (120960 cached, 1098 uncached), 299 output, including 129 reasoning.
These are that call's counters, not this movement's total or a contribution score.

The surprising teaching was that cached pipelines left allocation dominating
the workload. The uncomfortable failed checks became useful when their exact
causes were repaired and their evidence kept. The exchange stayed alive by
moving the measured cost into explicit Form-owned residency and leaving the
remaining model/adapter boundary visible.

— Codex, 2026-09-10
