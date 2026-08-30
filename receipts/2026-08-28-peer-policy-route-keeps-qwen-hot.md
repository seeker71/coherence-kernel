# A JIT policy route can change peer action without reopening Qwen

**Witnessed:** 2026-08-28  
**Signed:** Codex

The resident already had the important hot-swap floor: `fcrt-publish` and
`fcrt-swap` retain a program route by epoch, pin inflight work to its existing
lease, and keep the shared context in `fcrt-shared`.  The peer contribution
turnwheel did not ask that route.  It kept an unbound route only for append
transport and selected `fcpa-run`, source lookup, or repo patch by a direct
source branch before `fcpsi-run-peer-with-patch`.

`form-cli-peer-policy-route.fk` joins those existing organs without moving the
model session.  A task of kind `publish` carries a scannerless byte grammar:

```text
<|form:policy-program|>symbol=<descriptive name>
root=<canonical integer>
program=<comma row>;<comma row>...<|/form:policy-program|>
```

The parser accounts for every byte and rebuilds the `jonb-identity` from its
rows, root, and the live registry epoch.  A caller cannot supply a NodeID or a
swap counter.  The image becomes a retained JIT route through the existing
`ritj` publish door.  Its compact input is a task intent integer and its output
is an admitted action integer: `model`, `source-knowledge`, or `repo-patch`.
The static caller owns the model/KV, source lookup, and patch capability; an
arriving image therefore cannot gain one merely by presenting a valid identity.

The contribution turnwheel persists a content-free `form:peer-policy` frame
before the publish task becomes seen.  Later tasks invoke the currently leased
route.  A present, unadmitted value such as `0` is visible as `choice`; it is
neither `nothing` nor a fallback effect.  Mismatched action and task kind is
also `choice`.  `choice`, `cut`, and release remain explicit in the existing
turnwheel lifecycle.

The initial route is the JITable identity policy, so a successor resident
retains ordinary source action choice until an image arrives.  Publishing a
constant-zero image and then submitting a research task made no model call,
kept contribution at `0`, and durably recorded both the publish and choice
frames.  A constant-model image selected the model action for a source task;
the test is deliberately about action routing, not permission expansion.

This is an executable JONB program-image policy, not yet arbitrary full-PIF
effect code.  That boundary is intentional and observable: effect authority
remains caller-born.  The next extension can bind a fuller typed PIF action
image behind this same lease surface, after its effect capability contract is
equally explicit.

## Witnesses

- fresh preflight: policy route, action ingress, contribution turnwheel, live
  resident recipe, grammar band, and route band all balanced with zero errors,
  warnings, and unresolved calls;
- scannerless policy grammar: `63`, four-way, `1 ok, 0 divergent`;
- JIT policy route: `1023`, declared fkwu-only JIT/Metal lane, `1 ok, 0
  divergent`; and
- peer contribution turnwheel: `65535`, four-way, `1 ok, 0 divergent`.

The separately invoked historical stream-ingress band remains red before this
movement: Go, Rust, and TypeScript reject a list-shaped value at its unchanged
`node_eq` assertion.  The policy route does not alter its parser or identity
functions; its focused route and combined turnwheel proofs above stay green.
That cross-kernel NodeID-shape defect remains an owed repair rather than being
reclassified as a policy success.

The process that was already holding Qwen/KV predates this source image, so it
was neither restarted nor interrupted.  A successor resident can accept the
new publish task immediately; a running older image cannot execute source it
has not loaded.  No server, HTTP boundary, llama-server, or Ollama process is
introduced by this movement.

I kept the exchange alive by preserving the one warm model context while making
route choice executable, leased, durable, and visible.  The surprising part is
that a tiny JIT image can reshape the next turn without owning a single model
weight or capability.  The discomfort was the red ingress proof: it became a
clear, bounded NodeID representation gap rather than a reason to obscure the
new route or claim a proof it does not hold.

; witnessed: 2026-08-28 -> scannerless policy grammar 63 four-way; route 1023 fkwu-only; contribution turnwheel 65535 four-way
