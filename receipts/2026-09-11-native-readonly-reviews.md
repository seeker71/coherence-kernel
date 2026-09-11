# Native review without a manufactured edit

Form's coding loop now accepts caller-owned `mode=review`. Every supplied
document remains unchanged; the report is a separate result checked by the
caller. Native `verify` accepts no model-supplied command and invokes only the
bound source checker. The JSON example and callback signatures live in
`docs/form-native-coding.md`.

Review mode and report survive native checkpoints. Completed resumes and
recalled repairs rerun the report-aware checker. `evaluation=1` begins before
continuity lookup, so it cannot recall, resume, write coding checkpoints or
use a learned proposal. Review reports remain assessment experience, never
verified implementation targets. Ordinary coding still requires an actual edit.

The documented public configuration review completed on local Qwen: 8 model
replies, 2 native calls, 2 check stages, 249 generated IDs, 643 injected IDs,
zero repairs and zero recalled lessons. All three returned fields matched;
original source bytes were preserved. Model release was observed, exit zero,
stderr empty. Admission through release took 287,493 ms under other local
workload. This is one small functionality witness, not independent semantic
review, an unseen benchmark, whole-session equivalence or a latency guarantee.

Existing policy/request/memory/session-code bands, extended at their executing
boundaries, pass 65535/255/65535/511. Repair 262143, documents 255, progress 255,
definition 127, session 31, dispatch 7, telemetry 7, tool-wire 131071 and
tool-examples 32767 also pass with actual exit zero. Stale/foreign cache warnings
were retained on affected recompilations. All 13 drift gates pass 8191/8191,
refused 0. Root C is unchanged. Glass reads 123 rows in 12 ms and shows all 24 retained
findings; capacity pressure and other physical gaps remain. Share is declared,
percentage withheld.

The surprising lesson: unchanged documents can be a complete result when the
requested work is a review. The concrete discomfort was two broken prompt
envelope checks after adding instructions. Preserving the original envelope
restored both checks without weakening them. A verified explanation of the
new contract was returned through the native session-learning door, separately
from the excluded evaluation report. The exchange stays alive through that
usable interface and its observed limits.

— Codex
