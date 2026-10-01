# Native Form controls and compatible learning

The source-backed CLI can create, read and select immutable native nodes, retrieve
current substrate source, and birth numeric BML recipes during Qwen decoding.
The same BMF expression grammar lowers those recipes into RAM Metal programs.
Scalar and nonempty array inputs share a compiled program; native choice can
continue after a refused operation. Both reasoning and final-answer stages keep
the execution owner. This movement adds no C implementation.

The compatible Qwen learner captures verified control sequences in one KV
stream. Recipe birth and execution share their owner; runtime observations
enter context and remain outside training targets. Native gradients update both
rank-one head vectors through the frozen Q8_0 or float32 output projection.
Independent streams share one admitted model. Finite candidates can be selected
explicitly with `generate --adapter PATH`. Diagnostics carry counts and a private
report reference; prompts, controls and answers stay in that report. Precise JSON
keeps small nonzero losses visible.

Observed on the rebuilt current-main C seed:

| Operation | Observation |
| --- | --- |
| Public adapted CLI, `x * x + 2`, input 3 | Metal returned 11; completed grounded answer; release 1; provider calls 0 |
| Conditional BML array, `x < 0 ? x * x : math_sqrt(x) + 2` | `[-3,0,4,9]` → `[9,2,4,5]`; scalar 16 → 6; one admission, one reuse; release 1 |
| Cross-owner string/float64 read and missing payload slots | Correct identity or absence in 0–1 ms; no full-column search for these slots |
| Two-control Qwen learning | 71 supervised targets; 49 accepted updates; approximate mean loss 0.004483 → 0.001350; one admission; release 1; provider calls 0 |
| Independent input-4 enquiry | Base and adapted both executed and reported 18; 89 generated IDs each; completed; no observed quality gain |
| Head gradient fixture | Analytic -0.448189 versus finite difference -0.448227; loss 2.493812 → 1.884740; extreme candidate and absent input refused; resources released |
| Float32 conversion | Native cast agreement for signed values, subnormals, ties, normal carry and overflow; nonfinite decode supplies absence |
| Current kernel laws / freshness | 67108863 / 31 |
| Recipe token / session | 1048575 / 4095 |
| LoRA artifact / live step | 31 / 511 |
| Full BML cursor / BMF core | 105 / 700 |
| Native learning reader | 1,295 current observations preserved; 72,486,084-byte JSONL checkpoint; zero history bytes replayed on resume |
| Reader lifecycle | Split record preserved; changed generation, source replacement, unfinished checkpoint and extra row refuse reuse |
| No-current-need attendance | 18 ms; zero source-catalog admissions; owned checkpoint release and fixture-source retirement observed |
| Measured source selection | 360 references examined in 12,675 ms; one currently verified row received; selected targets retain complete checks |
| Selected landing checks | pass 2047, full 2047, refused 0 |

The measured native control window took 105 ms, with 171 Form, 7 BML primitive,
274 BML expression and 9 composed BML functions used. It retained 168,218 entries,
21 ambiguous functions and no stale attribution. Admission plus execution took
54,688 ms: whole-unit compilation remains a distinct cost. The source guide
retains current authored function/line counts and source fingerprints. Gradient
frames observed about 40 ms for 1,271,398,400 projection elements.

The learning reader's measured cold read took 253,169 ms; restored attendance
took 56,455 ms with no source-history replay. Restore still parses 72 MB of
current evidence and is a visible optimization target. The native streaming
checkpoint avoids the seed binary decoder's artifact ceiling and stays in
reclaimable JSON views. Publication occurs at owned release once per drain;
its organ signal records restore/publication timings and any resource need.
Source catalogs are admitted only for a current candidate need; unselected
references make no validity claim. These selection observations have changing
source assessments and do not establish an identical-input A/B speed ratio.

Local learning and execution used no provider calls. The arriving agent authored
these source changes and teaching controls using rented implementation reasoning.
The transcript meter from 2026-10-01 14:48:40.360Z through 16:46:29.280Z measured
31,471,787 input tokens, including 30,526,464 cached input, and 221,251 output
tokens, including 122,705 reasoning output. Cached and uncached inputs remain
separate; this work is not relabeled as optional review or zero rented work.
The local-flow ledger's unrelated missing verification stays unknown.

Private evidence: `.hearth/rebased-public-generate.log`,
`.hearth/response-sessions/generation/49584-1790869931324`,
`.hearth/qwen-trajectory-form-control.safetensors.report.json`,
`.hearth/qwen-trajectory-learning.log`, `.hearth/rebased-control-expand.log`,
`.hearth/node-slot-resolution-final.log`, `.hearth/rebased-head-gradient.log`,
`.hearth/rebased-f32.log`, `.hearth/form-control-gates-complete.log`,
`.hearth/form-control-flow.log`, `.hearth/qwen-report-telemetry-final.log`,
`.hearth/form-control-rent-complete.log` and `.hearth/form-control-homecoming-final.log`.
The original budget-refused generation and interrupted stale-source build retain
their exact command/output in the same hearth. The verified lesson is retained
under session `codex-fkwu-native-control-2026-10-01`, event
`verified-bml-node-qwen-learning-v1`. Its automatic Llama worker completed at
optimizer step 85 with no pending work, retaining serving generation 4 and
the existing two promotions. This is independent of the Qwen experiment;
completion does not claim promotion or answer-quality improvement.
The reader repair's verified teaching is separately retained as event
`verified-native-reader-checkpoint-v1`, with its automatic worker's ownership
recorded in `.hearth/form-control-reader-homecoming.log`.
Reader evidence is `.hearth/native-reader-homecoming-jsonl.log`,
`.hearth/organ-reader-checkpoint-complete.log`,
`.hearth/native-empty-needs-owned-release.log` and
`.hearth/native-evidence-selection-final.log`; original failed commands and
their diagnostics remain alongside the repaired observations.
The final sealed CLI bundle is source
`b042c5c01f3282e405395efaa3b6c7f2b878e914df42b88879f9b7a81d23dbc1`,
stamp `7cfe65f87dd1a85f`. Its build and full BML cursor release checks passed.
The obsolete 78,013,158-byte binary cache was released after the replacement's
actual resume; source events and useful failure receipts remain retained.

The current reach is native node operations and pure numeric f32 Metal recipes.
General effectful BML recipes, CPU/Metal parity, dedicated learned vocabulary
rows, broader quality gains and reclaimable native storage remain north-star
work. Lower loss alone does not establish those capabilities or improvement.
