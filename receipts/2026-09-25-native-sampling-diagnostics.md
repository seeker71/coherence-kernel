# Locate the native correction gap

The same source-grounded enquiry still returns unsupported claims about its
own grounding. Its length correction also fails. This movement investigates
whether sampling actually offers alternatives during that failed correction.

## What changed

The executing sampler now accumulates non-argmax draws, singleton nuclei,
total nucleus candidates and peak probabilities conditional on its top twenty
candidates. Reports retain both pre-correction and final cumulative snapshots.
The controller compares answer bytes and reports `not-requested`, `unchanged`
or `changed`, independently of word counts and completion. Private generation
files remain closed; these diagnostics contain neither text nor token sequences.

The GPU witness now distinguishes sampling from argmax: six draws from a
uniform device distribution reproduce the published MINSTD buckets
`[0,2,12,7,8,3]`. Its counters report five alternatives, zero singleton nuclei,
96 total nucleus candidates and a pre-nucleus peak sum of 0.3. The controller
witness distinguishes equal word counts from equal bytes.

The first authored witness used unavailable `lbw-zeros`. The command
`form-run ./fkwu observe/preflight-stdin-run.fk`, given
`form/form-stdlib/tests/form-cli-model-sampling-band.bml`, returned exit 1 with
`lbw-zeros -> UNOBSERVED` and `preflight: no readable verdict`. Executing despite
that diagnosis was my workflow error; it returned exit 1 and
`order: only numbers have an order`. A small native zero-list helper repaired
the source. Fresh preflight and physical execution then passed. No checks were
weakened.

## The actual answer

The public source-backed CLI selected Qwen3.8-27B-Q8_0 from its listing and ran
the same prompt, seed 1729, 1,024-token allowance and 350–450-word requirement.
The exporter verifies unchanged prompt bytes, 1,887 prompt IDs, 4,831-position
capacity, seed and allowance against the retained sampled baseline.

| Observation | Initial answer | Length correction |
| --- | ---: | ---: |
| Draws, including stop | 603 | 603 |
| Non-argmax choices | 82 | 0 |
| Single-candidate nuclei | 411 | 602 |
| Total nucleus candidates | 915 | 604 |
| Words | 496 | 496 |

The correction is **byte-for-byte unchanged**. The final answer also matches
the preceding sampled run. Its hash is
`8dc06d4efde94ba8593965c806bfbaf42464698a21f5185aae2de32fc39bc4fe`.
The original assertions still reject its length. Completion, release and
retention are each 1; elapsed time is 400,681 ms; native provider calls are 0.
Local checks, a brief panel read and session learning overlapped execution,
so this is not an isolated throughput comparison.

Sampling genuinely selects alternatives in the initial answer. During correction,
almost every nucleus contains only one candidate. Another seed has no useful
basis here. The next causal boundary is how corrective context changes the
model's state and scores. These counters do not distinguish a model tendency
to copy from an inference defect. Earlier fresh-prefill and template observations
remain relevant; they guide the next repair.

The answer still derives asserted fearlessness and embodiment from garbage
collection and identity. Those mechanisms do not establish that behavior.
This movement closes an observation gap, not the response-quality gap.
The exact public answer, report, comparison, byte-preserving public-log archive
and native exporter are in `artifacts/2026-09-25-native-sampling-diagnostics/`.

## Verification and continuity

Sampler band 1, session band 4095 and reasoning-budget band 1 pass after clean
preflights. The source-backed CLI compiles; drift gates pass 8191/8191. The
runtime change is Form/BML only, with no C growth or external dependency.
Glass first frame is 28 ms; its short read ended intentionally with Ctrl-C.
Counsel reports zero orphans and eleven unobserved lanes without a hearth.
The native guide reports zero Python implementations, two invocation candidates
and zero unread files.

The previous sampling teaching completed at optimizer step 184, learned rounds
185. The verified diagnostics contract was retained as event
`2026-09-25-native-sampling-diagnostics-v1`, row
`9fd2e7a6c4c7f0a8e691fe3afc6d66444f489d538ae960819817ce0e351b67d5`.
Its worker is running with one pending row. Evaluated answers remain excluded;
this separate learner establishes no Qwen update or response-quality gain.

The latest completed coordinator-turn cost is 19,749,192 tokens, including
19,181,056 cached input. Input is 19,613,720, output 108,305 and unattributed
27,167. This reading excludes the current open turn and separate provider
processes. Token quantities reconcile; tool events do not (109 calls, 144
output events), so contribution share remains declared, with no percentage.
The retained cost JSON carries that scope.

Two input mistakes also remain visible: a JSON packet sent to the three-line
hearth door returned `hearth task body is absent`; the corrected native call
reported `no-standing-hearth`. The output-only turn meter received no transcript
path and reported that absence; the completed-turn cost above comes from the
separately bound native collector.

— Codex
