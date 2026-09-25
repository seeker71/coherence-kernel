# Form waits for the answer and checks which document was reviewed

The previous movement made progress: `784b3929a` landed the native handoff,
retained the 471-word failed result and continued its original checkpoint.
This movement verified that owner PID 17711 is still live; its public decode
checkpoints advanced. It has not returned a terminal answer.

The [comparison door](../observe/form-cli-code-feedback-compare.bml) now accepts
an optional owner PID. It waits for that owner to exit and for the complete
public execution record before comparing the returned answer with the preceding
answer. It admits no model or provider. The
[actual request](artifacts/2026-09-26-native-identity-usefulness-care/comparison-request.json)
is running against the existing owner. A timeout in the observing agent is not
a restart. Missing terminal evidence remains an explicit error.

The same door now compares a retained coding review's goal and document
identities with the actual returned goal and documents. Replaying the completed
434-to-471-word comparison established **goal_matches=1, documents_match=0,
current_review=0**. The earlier acceptance remains available and is labelled as
a retained review whose applicability to the returned document is unestablished.
The old comparison and answer bytes remained unchanged. This establishes
attribution of review evidence, not improved answer quality.

The [preceding full coordinating turn](artifacts/2026-09-26-native-identity-usefulness-care/handoff-coordinator-cost.json)
cost **8,617,979 rented tokens**, including **8,200,320 cached input tokens**:
8,504,565 input, 86,994 output and 26,420 unattributed tokens across 60 model
calls. Full-turn and tool-event reconciliation both returned 1. Its separate
provider subprocess and this open turn are additional. The cost remains far
from the requested minimum; native ownership of waiting and comparison removes
repeated orchestration work, while its effect on total session cost needs the
next completed observation.

Compile-only validation and the completed real comparison passed. The live
comparison has reached its owner-wait boundary. Drift gates passed **8191/8191**.
The verified review-scope teaching was retained as `f3bd5c27…`; the learning
worker reported standing or launching, so no completed weight update is claimed.
The native authoring guide reports zero Python implementations and zero unread
files, with two existing invocation candidates. The counsel panel last returned
**0 orphans**, with **11/12** serving lanes unobserved. Native answer quality,
felt resonance, dependable completion and whole-session throughput remain open.

— Codex
