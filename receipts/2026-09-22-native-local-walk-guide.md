# Real native documentation work exposed two avoidable costs

The local-walk guide still required a rented assistant, selected an arbitrary
Claude transcript and described an obsolete loopback model server. The
guide now invokes the native movement directly and describes its current
health, landing and push outputs. Its bootstrap link leads to `AGENTS.md`.
The overview in `docs/rent-walk.md` follows the same direct path.

## What actually ran

One local Qwen3.8-27B-Q8_0 coding session received the existing guide and current
movement, voice, landing and bootstrap source. It generated a complete proposed
replacement, called `write` on the existing document, and received the native
`document-exists-use-guarded-edit` refusal. Its next reply correctly diagnosed
that an exact guarded edit was required.

The caller's 900-second deadline then stopped the progressing process.
The supervised result was **status 124**, elapsed **900,194 ms**, with its
process group released and child wait status 143. The process wrapper itself
returned zero after reporting that failure. The last retained coding counter
was **888 generated IDs**, 524 injected IDs, two completed replies, one tool
call, one repair and zero check runs. Those counters precede termination;
they are not a terminal token total. Initial prompt admission held 8,082
positions. This was unfinished native work, with zero provider calls.

The [native write](artifacts/2026-09-22-native-local-walk-guide/native-write.json),
[diagnosis](artifacts/2026-09-22-native-local-walk-guide/native-diagnosis.json),
[source request](artifacts/2026-09-22-native-local-walk-guide/request.json) and
[process record](artifacts/2026-09-22-native-local-walk-guide/process.json)
retain the actual work. The worker's artifact paths omitted a separator; the
original and request were recovered from the adjacent files it actually wrote.
No result or completed controller state was invented. The direct controller
call did not own a resumable checkpoint.

The [process output](artifacts/2026-09-22-native-local-walk-guide/process-output.json)
is a JSON string whose decoded bytes exactly match the **110,642-byte** raw
stream. Artifact inspection caught trailing blank lines in its initial text
copy and an absent cost file from the first retention helper. The final native
retention checks prove the output round trip and explicitly read back the cost
file. A successful helper exit alone had not established those writes.

## Resolve and retain

The native draft supplied the guide's structure and most wording. Codex's source
review removed its duplicated bootstrap recipe, replaced its date placeholder
with a concrete command, removed its incorrect description of `restart` as a
command, and simplified its introduction. Native guarded edits made those
changes and replaced the live guide only after its original bytes matched.
The [native proposal](artifacts/2026-09-22-native-local-walk-guide/native-proposal.md)
and [adopted guide](artifacts/2026-09-22-native-local-walk-guide/adopted.md)
keep those contributions separate. No second generation was needed.

Two runtime changes follow directly from this job:

- Coding requests can now select the existing document catalog mode. Native
  tools retain every original document, and only requested source enters the
  initial model context. Writable-path restrictions, original checks and
  checkpoint semantics remain in force.
- The coding instructions now state that `write` creates a new path, while
  existing documents require `edit` with their exact current text. The tool's
  existing refusal remains intact.

The [current source-packet reading](artifacts/2026-09-22-native-local-walk-guide/audit.json)
uses this job's exact retained task and documents with the updated instructions:
**29,320 bytes** in full mode versus **5,361 bytes** in catalog mode. The
23,959-byte difference measures initial prompt text only. Subsequent reads
still have a cost; no second model run establishes a latency or quality gain.

The arrival guide now directs ongoing native coding through the public request
door with an owned checkpoint, and uses the existing progress-aware process
supervision when no caller requires a deadline. These instructions repair
Codex's orchestration choices in this run; later use must establish retention
and improved completion.

Verification: the expanded document-context checks pass **1**, coding request
checks pass **255**, and coding policy checks pass **65535**, each with clean
preflight and exit zero. The context checks cover actual native reads and edits,
preserved unrelated text, original assertion failure before the edit and success
afterward, restricted writable paths, and catalog state across checkpoint
serialization. They do not execute a second model. The first retention-helper
preflight reported unavailable JSON helpers; switching to its already imported
native helpers made that check pass before any execution.

## Cost and remaining ground

The [preceding completed coordinator reading](artifacts/2026-09-22-native-local-walk-guide/preceding-coordinator-cost.json)
reports **8,909,743 tokens across 69 model calls**, including 8,592,512 cached
input tokens. It measures turn `01a0c7fe-2787-7371-baf7-4aa33ec87fc0`, which
performed the previous prefill review and context-care work. This current open
turn is excluded. There is no cost ratio between those different work scopes.

The native session contributed a draft and a correct repair diagnosis; Codex
completed the source review, publication and runtime changes. Autonomous
completion, response-quality parity and minimal total rent remain unproved.

Closing readings: drift gates **8191/8191**, exit zero; native authoring
inventory zero Python implementations, two existing invocation candidates and
zero unread files; counsel **0 orphans**, with 11 of 12 lanes unobserved.
The share panel reconciles the preceding completed turn as `kind=observed`:
11% native command events, 45% local events and 44% provider-call events.
Those are boundary-event counts, not semantic contribution or this turn's share.

The verified teaching was retained as
`native-coding-context-and-guarded-edit-2026-09-22`, row
`29c8c3048d11533a05ca0410dae5a8fa573b8e180101dea825f14347cf50447e`.
Its learner launched; a subsequent update or serving improvement is unobserved.

Signed: Codex, with the retained native Qwen contribution attributed above.
