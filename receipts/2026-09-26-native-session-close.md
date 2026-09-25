# A CLI session closes while another session keeps learning

The real `models /Users/ursmuff/models/qwen38-27b` → `quit` session returned its
listing, then spent **624,107 ms** in shared learner settlement. Its recorded
wait exit was `-1`: that supervisor belonged to another process.

Normal CLI close now reads the supervisor's actual parent through the existing
native `host_process` primitive. It waits for its own supervisor, retries an
interrupted wait while that child remains owned, and preserves the existing
supervisor-failure evidence. A session with no owned supervisor records its
continuity and closes. Explicit `session drain` and one-shot embodiment retain
their shared-drain behavior. The C seed is unchanged.

## Same real command, observed again

The [native observation cell](artifacts/2026-09-26-native-session-close/observe-cli-close.bml)
retains the original public events and executes the same command sequence.

| Observation | Before | After |
| --- | ---: | ---: |
| Measured interval | Quit's settlement wait | Entire CLI command |
| Elapsed ms | 624,107 | 188 |
| CLI exit | 0 | 0 |
| Supervisor owned by CLI | No | No |

In the new observation, worker **27054** had parent **27049**, while the CLI
was **27177**. The same worker was alive before and after CLI completion.
Its actual work was the verified ownership teaching submitted through the
session embodiment door. The listing still returned both local Qwen artifacts;
stderr was empty. The timing boundaries differ as named above, and this is one
actual execution on each side, not a general performance benchmark.
The whitespace check flagged the raw listing's trailing blank line. Its 390
bytes are retained exactly in a JSON string, verified after decoding.

The existing failed-worker band now exercises normal session settlement:
**63/63**, including actual child exit 1, retained pending example, stderr and
lock release. Session-memory checks passed **1023/1023**; preflights and CLI
compilation were clean. Drift gates passed **8191/8191**. The counsel panel read
**0 orphans**, with **11/12** serving lanes unobserved because no hearth stood.

The useful distinction is ownership: retaining one's own work does not require
holding every session open for shared work. This closes an observed completion
delay. It does not establish better native answer quality or whole-session
parity. The preceding 443-word native answer's roughly 35-minute completion
and rented coordination cost remain material gaps.

— Codex
