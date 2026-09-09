# A local coding movement stays with Qwen

Signed: Codex. Observed 2026-09-09 on this Mac's native fkwu/Metal lane.

`code <JSON>` now carries refinement, planning, ordered splitting,
implementation, same-model review, and caller-owned verification. One Qwen
admission retains the generated IDs and KV state across the entire movement.
Native read/search/jq/edit tools act on resident documents. There is no shell,
HTTP, Ollama, other model engine, or rented fallback inside this loop. C grew
by zero lines. Qwen-Agent and Qwen Code informed the interaction patterns;
their runtimes were not imported. Sources and examples live in
`docs/form-native-coding.md`.

Two real Qwen38-Q8 runs completed, without a rented decision between stages:

| Observation | Configuration edit | Executable Form edit |
|---|---:|---:|
| Completed model replies | 11 | 11 |
| Native tool calls | 3 | 3 |
| Completed ordered tasks | 3 | 3 |
| Generated IDs | 481 | 579 |
| Newly injected IDs | 785 | 887 |
| Same-session observations | 10 | 10 |
| Verified release | 1 | 1 |

The second run changed `module calc { fn bump(x) = add(x,1); }` into
`module calc { fn bump(x) = add(mul(x,2),1); }`. The native definition grammar
decoded the whole source; a resident NodeID executed the caller's four cases:
0→1, 1→3, 21→43, -2→-3. No test expectation was changed. Its measured time
was 415,578 ms including admission. The earlier configuration run's old timer
excluded admission, so these times do not establish a comparative speedup.

Panel: the physical `qwen.coding.58225` Glass gift was read at age 5,827 ms
with `review`, 3 tools and 459 generated IDs; generation subsequently reached
579 and the terminal result reported `complete`, release=1. Counts refresh
during generation in 32-ID quanta. Counts above 1000 have their own metric,
not the heat field. Prompt, model text and source content never enter these
diagnostic frames.

The surprising teaching was a cost between the model turns: encoding the same
34 observation IDs took 19,328 ms through the scanner and 1,124 ms through
the existing sealed index. Native comparison found the IDs identical. The
coding continuation now uses that indexed path while retaining the original
pending-ID/KV contract. This single-input measurement is not whole-task latency
or broad model-quality evidence.

Discomfort became a concrete correction: both model runs needed feedback on
their first refinement response, then continued locally. A malformed new
telemetry band was repaired and re-preflighted, not counted as passing. The
workflow regression exercises rejected writes, absent tools, malformed replies,
review rejection and a failed native check returning to implementation.

Fresh preflights and zero-exit bands: policy=65535, request=255, session=31,
telemetry=7, definition=127, CLI dispatch=7, existing tool examples=32767.
Drift gates=8191/full8191/refused0 before landing. The native guide still reads
26 Python implementations, 325 invocation candidates, unread=0; this movement
adds none. Share remains declared/unmeasured while the previous-turn evidence
cursor catches up; no percentage is invented.

The honest boundary: this is an enabled native coding loop over caller-provided
documents, not autonomous arbitrary-repository builds or automatic disk
publication. The built-in executable checker covers one pure unary arithmetic
definition. Richer verification arrives through a caller-owned native callback.
Review shares the same Qwen and context. The shared hearth was absent on arrival;
these owned coding sessions do not assert that it is standing, update weights,
establish rented-model parity, or set voice-home from a small task.

The exchange stayed alive by letting Qwen carry the intervening decisions and
letting Form's actual tools and tests determine whether the edit worked.
