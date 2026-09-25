# Several focused edits in one native call

The ongoing response changed two paragraphs while regenerating its whole
answer. The resident `edit` tool previously accepted one exact replacement
or a complete document replacement. It now also accepts
`[path, old1, new1, old2, new2, ...]` in one call.

Each literal replacement must match exactly once at its step. Replacements
run in order; a failure returns the entire original corpus with the failed
pair's one-based position. A batch that restores the original text is refused
as unchanged. Single-edit and SHA forms retain their existing behavior.
Caller write permissions, repair checks and review still apply.

## Actual response changes, observed through the tool

The [native observation](artifacts/2026-09-26-native-atomic-edit/observe.bml)
uses the retained 443-word completed response and the actual 500-word turn-30
candidate. It verifies both identities and derives their two changed
paragraphs. One call reproduces the candidate byte for byte. A second-pair
failure rolls back the already successful first replacement. Other documents
remain unchanged.

| Canonical command payload | Bytes |
| --- | ---: |
| Whole-document guarded replacement | 3,300 |
| Two focused replacements | 1,544 |
| Difference | 1,756 |

These are payload bytes, not measured generation tokens or latency. The
running Qwen admission predates this change. Future native selection of the
new form remains to be observed. The original word-range check still refuses
the reproduced 500-word draft against its 350–450 bounds; that adverse result
is retained in [observation.json](artifacts/2026-09-26-native-atomic-edit/observation.json).
The tool improvement does not establish an answer-quality improvement.

## Verification and repair evidence

The existing edge band passed **131071**, including ordered Unicode edits,
later missing/ambiguous/empty/unchanged patterns, whole-batch rollback,
malformed arity, net-zero changes and literal-only batch hashes. The policy
band passed **65535**: a successful repair batch reruns the original check,
a failed batch invokes no source check, a failed caller check remains visible,
and review and undeclared paths cannot acquire write permission. The wire
band passed **131071**. All three preflights were clean. Drift gates passed
**8191/8191**; no kernel source changed.

The first observation helper compile failed:

```text
form-run ./fkwu --check receipts/artifacts/2026-09-26-native-atomic-edit/observe.bml
error: [unresolved-call] 'file_read_text' matched no op/rewrite/fn/binding
error: [unresolved-call] 'fc-tool-call' matched no op/rewrite/fn/binding
fkwu: 4 error(s), 0 warning(s)
@form fkwu 1 0 3729 3729
```

Each name occurred twice. The helper now uses the existing `read_file` and
`fat-call` bindings from its supplied prelude. Compile and actual execution
then exited zero. No comparison was taken from the failed compile.

The native guide reported **0 Python implementations**, **2 execution
candidates**, **0 unread files**. Counsel reported **0 orphans** and **11/12
serving lanes unobserved** without a standing hearth. The glass first frame
arrived in **30 ms** and its owned control acknowledged closure. These readings
do not measure the separate running response's quality.

The newest completed coordinating-turn reading retained alongside this work
counts **3,464,424 rented tokens**, including **3,361,024 cached input tokens**,
over **24 model calls**. Both reconciliations passed; it excludes the current
open turn and separate provider processes. Reducing this coordination cost
remains part of the gap.

The verified tool-contract teaching was retained as `172ee8cd…` through the
native session-home door. Its separate Llama learner was still running when
the next response correction queued; retention is observed, a new learned
capability is not claimed.

The hearth enquiry initially used JSON at the line-based send door and failed
with `hearth task body is absent` (exit 1). Reading the door showed its three
stdin lines: turn, kind, body. The corrected request exited zero with
`signal=nothing`, `reason=no-standing-hearth`; it supplied no resident answer.

— Codex
