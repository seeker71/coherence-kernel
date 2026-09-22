# Prefill advances after observed completion

Urs asked for real work and comparisons of real steps. This movement repairs
the execution path implicated by the retained interruption. It launches no
response comparison, replacement enquiry, model sweep or subagent.

## Ground and change

The original process reached position 1,472 of 2,694 at 154,888 ms, then supplied
no further progress. Its supervisor ended at 902,264 ms with status 125,
`release-pending`, `cleanup-incomplete`. The
[original process evidence](artifacts/2026-09-22-response-session-recovery-original-process.json)
is unchanged. Its last position event followed `metal_sync`; it cannot distinguish
where the following slice stopped. A historical host-log query returned no rows.

Source inspection established three concrete gaps in that path:

| Boundary | Previous implementation | Current implementation |
| --- | --- | --- |
| GPU wait | No deadline supplied by sliced prefill | Preserve a caller deadline; otherwise install the existing 300,000 ms hearth deadline for this carrier lifetime, including release |
| Slice completion | Ignore `metal_sync`, advance and recurse | Require drained dispatches, a completed/released frame, and no pending, in-flight or shelved work; stop on refusal |
| Initial session | Cursor wrapper reports success after a failed prefill; refusal discards its owner | Propagate refusal and preserve context/state ownership for explicit release |

The prediction head also requires four returned bytes before decoding an ID.
An absent read becomes `-1`; token zero remains a valid prediction. Public
prefill results retain their two-field shape. A failed slice returns its last
completed position and does not dispatch the head or another slice.

The executing organ emits correlated completion evidence and a stop response.
Embedding, layers and synchronization each announce their entry. These events
carry stages and carrier counters, without prompt or response content.
The completion policy lives in BML. No C seed or carrier changes, new runtime,
model server or dependency were introduced.

The useful distinction is precise: **a completed process, a completed GPU slice,
and a useful answer are separate observations**. A positive position counter
must describe completed work.

## Verification and limits

- Rebased onto `origin/main` at `19b0a2112dad` before changing code.
- Model-session preflight: balanced, zero errors/warnings/unresolved calls.
  Existing session contract: **4095**, exit 0.
- Existing final-slice/head boundary check: preflight clean; **7**, exit 0.
- Landing gates: **8191/8191**, `refused=0`, exit 0; freshness **31**, ground **42**.
- Native authoring guide: **0** Python implementations, **2** invocation
  candidates, **0** unread files. The candidates remain in `voice-say.bml`.
- Glass startup panel: first frame **763 ms**. Its stale caches were rebuilt
  by the native bootstrap. This movement's temporary Glass processes were stopped
  after reading; existing Glass processes were left running. The explicit TERM
  produced supervisor status 143 and outer exit 1 with
  `fkwu: form_error: Glass supervisor ended with an error`; this is retained as
  an interrupted observer, not a successful observer shutdown. A subsequent
  `ps` query for this movement's seven Glass PIDs returned no rows (exit 1).
- Share reader: `kind=declared`, `share=withheld`, no completed evidence row.
  Its last-provider-call total was **101,897 tokens**, explicitly a single call,
  not this movement's total. The current open turn is not fully measured.
- Verified teaching retained through the session-home door as event
  `qwen-prefill-completion-boundary-2026-09-22`, row
  `8ac2357480ab9e932d503f4006407cb10c31517d15187657b1d6309b0f57726c`.
  It reports `worker-standing-or-launching`; a serving update is unobserved.

These checks establish compilation and existing contracts. The new GPU refusal
branch has not been exercised on another live generation, and the historical
stall's cause remains unknown. No runtime speedup, successful cleanup of that
stalled workload, or answer-quality improvement is claimed. The next actual
workload can now retain the missing execution boundary if it encounters a fault.

Signed: Codex, arriving agent working through native Form evidence.
