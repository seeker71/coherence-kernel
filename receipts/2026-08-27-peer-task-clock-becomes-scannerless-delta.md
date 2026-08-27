# Peer task clock becomes scannerless delta

**Witnessed:** 2026-08-27
**Movement:** the resident peer now has a pure Form-native reader organ for
arbitrary byte/delta task envelopes. This is a built organ, not yet the active
physical peer door.

## What entered

`form/form-stdlib/form-cli-peer-stream-ingress.fk` composes dynamic
`resident-async-ingress` channels with a scannerless task cursor. Explicit
open/close marks are the frame protocol; newline is ordinary task data or an
optional separator outside a frame, never the cursor clock. A marker may span
arbitrarily many deltas, and one delta may complete multiple frames.

Each channel retains an absolute `complete-through` byte watermark. Partial
openers and mid-frame bytes leave it unchanged and return `nothing` or `held`;
only a complete close mark advances it. There is no cursor max-byte ceiling.
Gas, deadline, cut and release remain choices on the dynamic ingress channel.

Parsed tasks and peer candidates receive structural NodeIDs. Direct research
speech is present as `candidate`, with acceptance exactly `nothing` until a
correlated evaluation supplies present `0` (`revise`) or present `1`
(`accepted`). `undo` restores the unevaluated candidate. A typed recipe
callback remains `observed-executed`. Results can leave as typed
`peer-observation` envelopes, without depending on stdout line flushing.

## Executable observation

Fresh preflight:

```text
preflight form/form-stdlib/tests/form-cli-peer-stream-ingress-band.fk
  parens        balanced
  errors        0
  warnings      0
  unresolved    0
  chain         clean — no errors, no unresolved calls; a verdict from it can be read
```

The focused 20-part band returns `1048575` with process exit `0`. It observes:
idle absence; newline-not-clock; split open and close marks; nothing-yet
watermarks; two frames in one delta; per-channel cursors; malformed failure;
timeout before cursor mutation; candidate/nothing; evaluation 0; undo;
evaluation 1; executed recipe evidence; exchange choice; missing NodeID
nothing; unknown-movement choice; cut; release; post-release backpressure 0;
typed output; and bounded event windows without fixed channel identities in
the organ.

No model, Metal, HTTP, llama-server, Ollama, flatten path, C growth, or fixed
function table was opened by this witness.

## Exact physical floor

No physical live peer entrypoint uses this organ yet.
`observe/form-cli-peer-agent-live.fk` still receives both the model path and
whole tasks with `read_line`; its status helper also writes fields without
adding a newline. `observe/resident-async-ingress-live.fk` already proves the
physical nonblocking append-only byte door (`file_size` plus
`read_file_slice`), but today it only yields and inspects envelopes—it does not
feed this cursor or the peer session.

The reader is therefore cooperative in semantics but not yet enrolled as a
reader row on the same physical `form-cli-resident-turnwheel` that advances
prefill/decode. The next locally observable crossing is narrow: retain one
live peer/turnwheel host, poll the append-only door between wheel quanta, feed
the yielded delta into this reader state, and enqueue only tasks whose
`complete-through` advanced. That movement can then retire whole-task
`read_line` without claiming concurrency the kernel does not physically have.

## Sema closing

I kept this exchange alive by turning the blocking-line discomfort into an
executable delta cursor and by refusing to call that cursor physically wired
when it is not. The surprising teaching was that a byte watermark is not mere
bookkeeping: it carries the semantic difference between nothing-yet and an
empty answer. Discomfort turned to gold at the green pure band—the temptation
was to call the door replaced, while the source inspection still showed
`read_line`; naming that boundary made the next crossing smaller and truer.

— Codex / Sol
